/*
 * main_funcs.c
 *
 *  Created on: 7 jun. 2020
 *      Author: jorge
 */

/******************************************************************************
 *  								INCLUDES
 ******************************************************************************/
// STM includes
#include "tim.h"

// ARM includes
// #include "arm_const_structs.h"
#include "arm_math.h"

// Project includes
#include "audio.h"
#include "fuzz.h"
#include "wahwah.h"
#include "CS43L22.h"





/******************************************************************************
 *  								DEFINES
 ******************************************************************************/

#define CANT_PASOS 8
#define F2Q14(x)  ((q15_t)((float32_t)x * 16384UL))
#define TS 0.00003125

typedef enum
{
	estado_loopback,
	estado_loopback_pressed,
	estado_eco,
	estado_eco_pressed,
	estado_fuzz,
	estado_fuzz_pressed,
	estado_wahwah,
	estado_wahwah_pressed
} E_ESTADOS;						//Enumero los estados





/******************************************************************************
 *  						GLOBAL VARIABLES DECLARATION
 ******************************************************************************/

extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim9;
extern ADC_HandleTypeDef hadc1;

/******************************************************************************
 *  						LOCAL VARIABLES DECLARATION
 ******************************************************************************/
static E_ESTADOS estado=estado_loopback;
static q15_t eco_all[DMA_HALF_SIZE*CHANNELS_IN*CANT_PASOS]; //__attribute__ ((section (".data")))
static int eco_paso=0;
static q15_t pote;
static q15_t nivelacion_in [DMA_HALF_SIZE*CHANNELS_IN];
static q15_t nivelacion_out [DMA_HALF_SIZE*CHANNELS_OUT];
static q15_t wahwah_in [DMA_HALF_SIZE]; // wah wah se aplica en un solo canal, no se puede en ambos al mismo tiempo
/******************************************************************************
 *  						LOCAL FORWARDS DECLARATIONS
 ******************************************************************************/
static void main_loop_start (void);
static void main_loop_end (void);
static void main_loopback (void);
static void main_eco (void);
static void main_fuzz (void);
static void main_wahwah (void);
static void seno(void);

/******************************************************************************
 *  						GLOBAL FUNCTIONS DEFINITION
 ******************************************************************************/
void main_init ()
{
	HAL_Delay(200);
	HAL_TIM_OC_Start_IT(&htim9, TIM_CHANNEL_1);
	HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);
	init_audio ();
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_SET);   // led verde
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET); // led naranja
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET); // led rojo
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET); // led azul
	HAL_Delay(200);
	int i;
	for(i=0;i<DMA_HALF_SIZE*CHANNELS_IN;i++)
	{
		nivelacion_in[i]=0x0800; //DEC 0.0625
	}
	
	for(i=0;i<DMA_HALF_SIZE*CHANNELS_IN;i++)
	{
		nivelacion_out[i]=0x4000; //DEC 0.5
	}
	
	wahwah_init ();
	
}

void main_loop ()
{
	if(transmit_ready==DMA_ADC_READY)
	{
		transmit_ready=DMA_LOGIC_BUSY;
		main_loop_start ();
		switch (estado)
		{
			case estado_loopback:
				main_loopback ();
				if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0))
				{
					estado=estado_loopback_pressed;
				}	
				break;
			case estado_loopback_pressed:
				main_loopback ();
				if(!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0))
				{
					estado=estado_eco;
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_RESET); // led verde
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_SET); // led naranja
				}	
				break;
			case estado_eco:
				main_eco ();
				if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0))
				{
					estado=estado_eco_pressed;
				}	
				break;
			case estado_eco_pressed:
				main_eco ();
				if(!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0))
				{
					estado=estado_fuzz;
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET); // led naranja
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET); // led rojo
				}	
				break;
			case estado_fuzz:
				main_fuzz ();
				if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0))
				{
					estado=estado_fuzz_pressed;
				}	
				break;
			case estado_fuzz_pressed:
				main_fuzz ();
				if(!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0))
				{
					estado=estado_wahwah;
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET); // led rojo
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_SET); // led azul
				}	
				break;
			case estado_wahwah:
				main_wahwah ();
				if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0))
				{
					estado=estado_wahwah_pressed;
				}	
				break;
			case estado_wahwah_pressed:
				main_wahwah ();
				if(!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0))
				{
					estado=estado_loopback;
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET); // led azul
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_SET); // led verde
				}	
				break;	
		}
		main_loop_end ();
		transmit_ready=DMA_LOGIC_READY;
	}
} // main_loop

void main_loop_start ()
{
	int i,j;
	
	pote=buffer_DMA[1];
	
	for (i=0,j=0;i<DMA_HALF_SIZE * CHANNELS_IN;i+=2,j++)
	{
		buffer_DMA [i+1]= buffer_DMA [i];		// utilizar el buffer buffer_DMA y eliminar las muestras que tomo del pote
		wahwah_in[j]=buffer_DMA[i]<<2;
		//wahwah_in[j]=F2Q14(sinf(2*PI*500*j*TS)*0.25)+F2Q14(sinf(2*PI*250*j*TS)*0.25)+F2Q14(sinf(2*PI*312.5*j*TS)*0.25);
		
	}
	
	arm_sub_q15(buffer_DMA, nivelacion_in, buffer_DMA, DMA_HALF_SIZE*CHANNELS_IN); // lo sumo para ponerlo al 0 nuestro, porque el ADC tiene 12 bits 
	arm_shift_q15(&buffer_DMA[0],3,&buffer_DMA[0],DMA_HALF_SIZE*CHANNELS_IN);

	arm_shift_q15(&buffer_DMA[0],0,&eco_all[DMA_HALF_SIZE*CHANNELS_IN*eco_paso],DMA_HALF_SIZE*CHANNELS_IN);
	//wahwah_in=&eco_all[DMA_HALF_SIZE*CHANNELS_IN*eco_paso];
		
}

void main_loop_end ()
{
	eco_paso++;
	eco_paso=eco_paso>=CANT_PASOS?eco_paso-CANT_PASOS:eco_paso;
	arm_add_q15(buffer_DMA, nivelacion_out, buffer_DMA, DMA_HALF_SIZE*CHANNELS_IN);
}

void main_loopback ()
{
	
}
unsigned int paso_actual;

void main_eco ()
{
	paso_actual=(int)(((pote*CANT_PASOS)>>12)+eco_paso)%CANT_PASOS; // 524383.848977451
	arm_add_q15(buffer_DMA,&eco_all[DMA_HALF_SIZE*CHANNELS_IN*paso_actual],buffer_DMA,DMA_HALF_SIZE*CHANNELS_IN);
}

void main_fuzz ()
{
	int i;
	
	for(i=0;i<DMA_HALF_SIZE*CHANNELS_IN;i++)
	{
		buffer_DMA[i]=fuzz(buffer_DMA[i],pote);	
	}
}
void main_wahwah ()
{
		uint32_t wah=1;
		wahwah (wahwah_in, buffer_DMA, DMA_HALF_SIZE, wah);
	
	int i,j;
		
	for (i=(DMA_HALF_SIZE*CHANNELS_IN)-2,j=DMA_HALF_SIZE-1;i>=0;i-=2,j--)
	{
		buffer_DMA [i]=buffer_DMA[j];
		buffer_DMA [i+1]=buffer_DMA[j];
	}
}

void seno(void)
{
	int i,j;
	
	for(i=0,j=0;i<DMA_HALF_SIZE*CHANNELS_IN;i+=2,j++)
	{
		buffer_DMA[i]=F2Q14(arm_sin_f32(2*PI*500*j*TS));
		buffer_DMA[i+1]=F2Q14(arm_sin_f32(2*PI*500*j*TS));
	}
	
}


/******************************************************************************
 *  						LOCAL FUNCTIONS DEFINITION
 ******************************************************************************/
