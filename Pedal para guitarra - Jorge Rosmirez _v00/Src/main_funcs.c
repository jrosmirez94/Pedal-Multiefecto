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

static uint32_t adc_buffer[256];

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
	HAL_ADC_Start_DMA(&hadc1, adc_buffer, 256);
}

void main_loop ()
{
	
	
} // main_loop

void main_loop_start ()
{
}

void main_loop_end ()
{
}

void main_loopback ()
{
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
