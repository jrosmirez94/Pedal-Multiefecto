/*
 * main_funcs.c
 *
 *  Created on: 8 feb. 2020
 *      Author: pablo
 */

/******************************************************************************
 *  								INCLUDES
 ******************************************************************************/
// STM includes
#include "tim.h"

// ARM includes
#include "arm_const_structs.h"
#include "arm_math.h"

// Project includes
#include "audio.h"
#include "fuzz.h"
#include "wahwah.h"





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
 *  						LOCAL VARIABLES DECLARATION
 ******************************************************************************/

static float all_in [(CANT_PASOS) * DMA_HALF_SIZE];
static int i;
static int delta_paso    = 0;
static int idx_paso      = 0;
static int idx_paso_prev = 0;
static E_ESTADOS estado  = estado_loopback;





/******************************************************************************
 *  						LOCAL FORWARDS DECLARATIONS
 ******************************************************************************/
static void main_loop_start ();
static void main_loop_end ();
static void main_loopback ();
static void main_eco ();
static void main_fuzz ();
static void main_wahwah ();





/******************************************************************************
 *  						GLOBAL FUNCTIONS DEFINITION
 ******************************************************************************/
void main_init ()
{
	HAL_Delay(200);

	// Inicia timer
	HAL_TIM_OC_Start_IT (& htim2, TIM_CHANNEL_1);
	HAL_TIM_OC_Start_IT (& htim3, TIM_CHANNEL_1);

	init_audio ();

	for (int p=0; p<CANT_PASOS; p++)
	{
		while (transmit_ready != DMA_ADC_READY);

		if (transmit_ready == DMA_ADC_READY)
		{
			transmit_ready = DMA_LOGIC_BUSY;

			for (int i=0; i<DMA_HALF_SIZE; i++)
			{
				all_in [p * DMA_HALF_SIZE + i] = buffer_float [i * CHANNELS];
			}

			transmit_ready = DMA_LOGIC_READY;

		} // if (transmit_ready == DMA_ADC_READY)
	}

	wahwah_init ();
}

void main_loop ()
{
	if (transmit_ready == DMA_ADC_READY)
	{
		transmit_ready = DMA_LOGIC_BUSY;

		main_loop_start ();

		switch (estado)
		{
			case estado_loopback:
			{
				main_loopback ();

				if (HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_0))
				{
					estado = estado_loopback_pressed;
				}
				break;
			} // case estado_loopback
			
			case estado_loopback_pressed:
			{
				main_loopback ();

				if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0))
				{
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);	//Para prender LED NARANJA

					estado = estado_eco;
				}
				break;
			} // case estado_loopback_pressed
			
			case estado_eco:
			{
				main_eco ();

				if (HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_0))
				{						
					estado = estado_eco_pressed;
				}
				break;
			} // case estado_eco

			case estado_eco_pressed:
			{
				main_eco ();

				if (!HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_0))
				{
					HAL_GPIO_WritePin (GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);	//Para apagar LED NARANJA
					HAL_GPIO_WritePin (GPIOD, GPIO_PIN_15, GPIO_PIN_SET);	//Para prender LED AZUL

					estado = estado_fuzz;
				}
				break;
			} // case estado_eco_pressed

			case estado_fuzz:
			{
				main_fuzz ();

				if (HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_0))
				{
					estado = estado_fuzz_pressed;
				}
				break;
			} // case estado_fuzz
			
			case estado_fuzz_pressed:
			{
				main_fuzz ();

				if (!HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_0))
				{				
					HAL_GPIO_WritePin (GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);	//Para apagar LED AZUL
					HAL_GPIO_WritePin (GPIOD, GPIO_PIN_14, GPIO_PIN_SET);	//Para prender LED ROJO

					estado = estado_wahwah;
				}
				break;
			} // case estado_fuzz_pressed

			case estado_wahwah:
			{
				main_wahwah ();

				if (HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_0))
				{
					estado = estado_wahwah_pressed;
				}
				break;
			} // case estado_wahwah

			case estado_wahwah_pressed:
			{
				main_wahwah ();

				if (!HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_0))
				{
					HAL_GPIO_WritePin (GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);	//Para apagar LED ROJO

					estado = estado_loopback;
				}
				break;
			} // case estado_wahwah_pressed
		} // switch (estado)

		main_loop_end ();

		transmit_ready = DMA_LOGIC_READY;

	} // if (transmit_ready == DMA_ADC_READY)
} // main_loop

void main_loop_start ()
{
	delta_paso = (((pote) * (CANT_PASOS - 1)) / 4100) + 1;
	idx_paso_prev = idx_paso - delta_paso + (CANT_PASOS - 1);
	idx_paso_prev %= (CANT_PASOS - 1);
}

void main_loop_end ()
{
	idx_paso ++;
	idx_paso %= (CANT_PASOS - 1);
}

void main_loopback ()
{
	for (int i=0; i<DMA_HALF_SIZE * CHANNELS; i+=CHANNELS)
	{
		buffer_float [i/CHANNELS] = buffer_float [i];
	}
}

void main_eco ()
{
	for (int i=0; i<DMA_HALF_SIZE * CHANNELS; i+=CHANNELS)
	{
		buffer_float [i/CHANNELS] = buffer_float [i];
	}
	arm_q31_to_float ((q31_t *) buffer_float, (float *) buffer_float, DMA_HALF_SIZE);

	for (i = 0; i < DMA_HALF_SIZE; i ++)
	{
		buffer_float [i] = all_in [idx_paso * DMA_HALF_SIZE + i] =
			(buffer_float [i] / 2) + (all_in [idx_paso_prev * DMA_HALF_SIZE + i] / 2);
	}
	arm_float_to_q31 ((float *) buffer_float, (q31_t *) buffer_float, DMA_HALF_SIZE);
}

void main_fuzz ()
{
	//*
	for (int i=0; i<DMA_HALF_SIZE * CHANNELS; i+=CHANNELS)
	{
		buffer_float [i/CHANNELS] = buffer_float [i];
	}
	arm_q31_to_float ((q31_t *) buffer_float, (float *) buffer_float, DMA_HALF_SIZE);

	float pote2 = pote / 100.0f, pote3 = pote / 1000.0f;
	float dist = 0.000001f / (pote2  + 1.0f);
	for (int i=0; i<DMA_HALF_SIZE; i++)
	{
		buffer_float [i] = (pote3 + 1.0f) * (fuzz (buffer_float [i] - TENSION_DC_FLOAT, dist) + TENSION_DC_FLOAT);
	}
	arm_float_to_q31 ((float *) buffer_float, (q31_t *) buffer_float, DMA_HALF_SIZE);
	//*/
}
static float aux_buf [DMA_HALF_SIZE];
void main_wahwah ()
{
	//*
	for (int i=0; i<DMA_HALF_SIZE * CHANNELS; i+=CHANNELS)
	{
		buffer_float [i/CHANNELS] = buffer_float [i];
	}
	arm_q31_to_float ((q31_t *) buffer_float, (float *) buffer_float, DMA_HALF_SIZE);

	float dist = 10 - ((pote+1)/4100.0f) * 9;
	if (dist < 1)  dist = 1;
	if (dist > 10) dist = 10;
	arm_offset_f32(buffer_float, -TENSION_DC_FLOAT, buffer_float, DMA_HALF_SIZE);
	wahwah (buffer_float, aux_buf, DMA_HALF_SIZE, dist);
	//arm_offset_f32(buffer_float, +TENSION_DC_FLOAT, buffer_float, DMA_HALF_SIZE);
	arm_scale_f32 (buffer_float, 0.5f, buffer_float, DMA_HALF_SIZE);
	arm_add_f32 (buffer_float, aux_buf, aux_buf, DMA_HALF_SIZE);

	arm_float_to_q31 ((float *) aux_buf, (q31_t *) buffer_float, DMA_HALF_SIZE);
	//*/
}





/******************************************************************************
 *  						LOCAL FUNCTIONS DEFINITION
 ******************************************************************************/
