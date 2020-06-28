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
#include "audio.h"
// ARM includes
// #include "arm_const_structs.h"
// #include "arm_math.h"

// Project includes
// #include "audio.h"
// #include "fuzz.h"
// #include "wahwah.h"
#include "CS43L22.h"





/******************************************************************************
 *  								DEFINES
 ******************************************************************************/

#define CANT_PASOS 10


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


/******************************************************************************
 *  						LOCAL FORWARDS DECLARATIONS
 ******************************************************************************/
static void main_loop_start (void);
static void main_loop_end (void);
static void main_loopback (void);
static void main_eco (void);
static void main_fuzz (void);
static void main_wahwah (void);

/******************************************************************************
 *  						GLOBAL FUNCTIONS DEFINITION
 ******************************************************************************/
void main_init ()
{
	HAL_TIM_OC_Start_IT(&htim9, TIM_CHANNEL_1);
	HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);
	HAL_Delay(100);
	init_audio ();
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_SET); // led verde
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET); // led naranja
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET); // led rojo
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET); // led azul
}

void main_loop ()
{
	if(transmit_ready==DMA_ADC_READY)
	{
		transmit_ready=DMA_LOGIC_BUSY;
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
		transmit_ready=DMA_LOGIC_READY;
	}	
} // main_loop

void main_loop_start ()
{
}

void main_loop_end ()
{
}

void main_loopback ()
{
	int i,j;
	
	for (i=0,j=0;i<DMA_HALF_SIZE *(CHANNELS_IN);i+=2,j++)
	{
		buffer_float [j]= buffer_float [i];		// utilizar el buffer float_dma y eliminar las muestras que tomo del pote
	}
}

void main_eco ()
{
}

void main_fuzz ()
{
}
void main_wahwah ()
{
}





/******************************************************************************
 *  						LOCAL FUNCTIONS DEFINITION
 ******************************************************************************/
