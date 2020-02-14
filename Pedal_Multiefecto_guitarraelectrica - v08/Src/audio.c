/******************************************************************************
 *  								INCLUDES
 ******************************************************************************/
// STM includes
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "i2s.h"
#include "stm32f4xx_it.h"

// ARM includes
#include "arm_const_structs.h"
#include "arm_math.h"

// Project includes
#include "audio.h"





/******************************************************************************
 *  								DEFINES
 ******************************************************************************/





/******************************************************************************
 *  						GLOBAL VARIABLES DECLARATION
 ******************************************************************************/
volatile DMA_FST_t transmit_ready = DMA_INIT;
volatile float * buffer_float     = NULL;
volatile int pote                 = 0;





/******************************************************************************
 *  						LOCAL FUNCTIONS FORWARD
 ******************************************************************************/
static void init_CS43L22 (); // inicializacion del DAC





/******************************************************************************
 *  						LOCAL VARIABLES DECLARATION
 ******************************************************************************/
static q31_t   _dma_out   [DMA_HALF_SIZE * 2]            = {0};
static q31_t   _dma_in    [DMA_HALF_SIZE * 2 * CHANNELS] = {0};
static float   _dma_float [DMA_HALF_SIZE * CHANNELS]     = {0};





/******************************************************************************
 *  						GLOBAL FUNCTIONS DEFINITION
 ******************************************************************************/
void init_audio ()
{
	// Configura DAC
	init_CS43L22 ();

	// Inicia Transmision de ADC y DAC (a traves de I2S) por DMA

	// DMA para mover del _dma_out al DAC
	HAL_I2S_Transmit_DMA (& hi2s3, (uint16_t *) _dma_out, DMA_HALF_SIZE * 2);
	HAL_Delay (1); // Must start I2S before ADC
	// DMA para mover del ADC al _dma_in
	HAL_ADC_Start_DMA (& hadc1, (uint32_t *) _dma_in,  DMA_HALF_SIZE * 2 * CHANNELS);
}






/******************************************************************************
 *  						INTERRUPT HANDLERS
 ******************************************************************************/
// not use __disable_irq() and __enable_irq() to do critical stuffs
// put transmit_ready = HAL_TIMEOUT at finalize using buffer_float

void HAL_ADC_ConvHalfCpltCallback (ADC_HandleTypeDef * hadc)
{
	if (transmit_ready == DMA_LOGIC_BUSY)
	{
		HAL_GPIO_WritePin (LED_RED_GPIO_Port,  LED_RED_Pin, GPIO_PIN_SET);
	}
	else
	{
		//arm_q31_to_float (& _dma_in [0], _dma_float, DMA_HALF_SIZE);
		memcpy (_dma_float, & _dma_in [0], DMA_HALF_SIZE * sizeof (_dma_in [0]) * CHANNELS);
		/*
		for (int i=0; i<DMA_HALF_SIZE * sizeof (_dma_in [0]) * CHANNELS; i++)
		{
			_dma_float [i] = _dma_in [i * CHANNELS];
		}
		*/
		pote = _dma_in [1];
		buffer_float = _dma_float;

		transmit_ready = DMA_ADC_READY;
	}
}

void HAL_ADC_ConvCpltCallback (ADC_HandleTypeDef * hadc)
{
	if (transmit_ready == DMA_LOGIC_BUSY)
	{
		HAL_GPIO_WritePin (LED_RED_GPIO_Port,  LED_RED_Pin, GPIO_PIN_SET);
	}
	else
	{
		//arm_q31_to_float (& _dma_in [DMA_HALF_SIZE], _dma_float, DMA_HALF_SIZE);
		memcpy (_dma_float, & _dma_in [DMA_HALF_SIZE * CHANNELS], DMA_HALF_SIZE * sizeof (_dma_in [0]) * CHANNELS);
		/*
		for (int i=0; i<DMA_HALF_SIZE * sizeof (_dma_in [0]) * CHANNELS; i++)
		{
			_dma_float [i] = _dma_in [(DMA_HALF_SIZE + i) * CHANNELS];
		}
		*/
		pote = _dma_in [DMA_HALF_SIZE * CHANNELS + 1];
		buffer_float = _dma_float;

		transmit_ready = DMA_ADC_READY;
	}
}



//*** DAC ***//

void HAL_I2S_TxHalfCpltCallback (I2S_HandleTypeDef * hi2s)
{
	if (transmit_ready == DMA_LOGIC_BUSY)
	{
		memset (& _dma_out [0], TENSION_DC, DMA_HALF_SIZE * sizeof (_dma_out [0]));
		HAL_GPIO_WritePin (LED_RED_GPIO_Port,  LED_RED_Pin, GPIO_PIN_SET);
	}
	else
	{
		if (NULL != _dma_float)
		{
			//arm_float_to_q31 (_dma_float, & _dma_out [0], DMA_HALF_SIZE);
			memcpy (& _dma_out [0], _dma_float, DMA_HALF_SIZE * sizeof (_dma_out [0]));
			buffer_float = NULL;
		}

		transmit_ready = DMA_DAC_READY;
	}
}

void HAL_I2S_TxCpltCallback (I2S_HandleTypeDef * hi2s)
{
	if (transmit_ready == DMA_LOGIC_BUSY)
	{
		memset (& _dma_out [DMA_HALF_SIZE], TENSION_DC, DMA_HALF_SIZE * sizeof (_dma_out [0]));
		HAL_GPIO_WritePin (LED_RED_GPIO_Port,  LED_RED_Pin, GPIO_PIN_SET);
	}
	else
	{
		if (NULL != _dma_float)
		{
			//arm_float_to_q31 (_dma_float, & _dma_out [DMA_HALF_SIZE], DMA_HALF_SIZE);
			memcpy (& _dma_out [DMA_HALF_SIZE], _dma_float, DMA_HALF_SIZE * sizeof (_dma_out [0]));
			buffer_float = NULL;
		}

		transmit_ready = DMA_DAC_READY;
	}
}






/******************************************************************************
 *  						LOCAL FUNCTIONS DEFINITION
 ******************************************************************************/
/* void init_CS43L22 ()
 * Inicializa DAC (con clase D integrado).
 * Inicializaci�n por I2C.
 */
void init_CS43L22 ()
{
	uint8_t initV[2];



	//** CS43L22 datasheet: 4.9 Recommended Power-Up Sequence **//



		// Hold RESET low until the power supplies are stable.
			HAL_GPIO_WritePin (Audio_RST_GPIO_Port, Audio_RST_Pin, GPIO_PIN_RESET);

			HAL_Delay(500);

		// Bring RESET high.
			HAL_GPIO_WritePin (Audio_RST_GPIO_Port, Audio_RST_Pin, GPIO_PIN_SET);

		// The default state of the <Power Ctl. 1> register (0x02) is 0x01. Load the desired register settings while
		// keeping the <Power Ctl 1> register set to 0x01.

		// Load the required initialization settings listed in Section 4.11.
			// Write 0x99 to register 0x00.
				initV[0] = 0x00; // Registro
				initV[1] = 0x99; // Valor
				if(HAL_I2C_Master_Transmit (&hi2c1, 0x94, initV, 2, 50) != HAL_OK) //check if transfer failed.
				{
					HardFault_Handler ();
				}



			// Write 0x80 to register 0x47.
				initV[0] = 0x47; // Registro
				initV[1] = 0x80; // Valor
				if(HAL_I2C_Master_Transmit (&hi2c1, 0x94, initV, 2, 50) != HAL_OK)
				{
					HardFault_Handler ();
				}



			// Write 1 to bit 7 in register 0x32.
				initV[0] = 0x32; // Registro
				initV[1] = 0x80; // Valor
				if(HAL_I2C_Master_Transmit (&hi2c1, 0x94, initV, 2, 50) != HAL_OK)
				{
					HardFault_Handler ();
				}



			// Write 0 to bit 7 in register 0x32.
				initV[0] = 0x32; // Registro
				initV[1] = 0x00; // Valor
				if(HAL_I2C_Master_Transmit (&hi2c1, 0x94, initV, 2, 50) != HAL_OK)
				{
					HardFault_Handler ();
				}



			// Write 0x00 to register 0x00.
				initV[0] = 0x00; // Registro
				initV[1] = 0x00; // Valor
				if(HAL_I2C_Master_Transmit (&hi2c1, 0x94, initV, 2, 50) != HAL_OK)
				{
					HardFault_Handler ();
				}



	//** Finish required initialization **//



	//** Custom Configuration **//



		// Power Control 1 (Address 02h)
		// bit:		  7 	 6 		 5 	   4 		 3 		 2 		 1 		 0
		// name:	PDN7 	PDN6 	PDN5 	PDN4 	PDN3 	PDN2 	PDN1 	PDN0
		// value:	  1 	 0		 0		 1		 1		 1		 1		 0
		// State:	Power on
			initV[0] = 0x02; // Registro
			initV[1] = 0x9E; // Valor
			if( HAL_I2C_Master_Transmit (&hi2c1, 0x94, initV, 2, 50)!= HAL_OK)
			{
				HardFault_Handler ();
			}



		// Power Control 2 (0x04)
		// bit:		   7 	   	   6 			  5 		    4 		     3 		    	2 		    	1 		 			0
		// name:	PDN_HPB1 	PDN_HPB0 	PDN_HPA1 	PDN_HPA0 	PDN_SPKB1 	PDN_SPKB0 	PDN_SPKA1 	PDN_SPKA0
		// value:	   1	   	   0			  1		      0		   		 0		    	0		 		  	0		 	 			0
		// State:	Headphone channel is always ON. Speaker channel is always OFF.
			initV[0] = 0x04; // Registro
			initV[1] = 0xA0; // Valor
			if( HAL_I2C_Master_Transmit (&hi2c1, 0x94, initV, 2, 50)!= HAL_OK)
			{
				HardFault_Handler ();
			}



		// Interface Control 1 (0x06)
		// bit:		7 	 	   6 			5 		 4 		   	  3 		   2 		 1 	 	 0
		// name:	M/~S 	INV_SCLK 	Reserved 	DSP 		DACDIF1 	DACDIF0 	AWL1 	AWL0
		// value:	0		   0			0		 0		  	 0		   	   1		 1		 1
		// State:	Slave mode. SCLK Polarity Not Inverted. DSP Disabled. Format I2S. 16-bit data
			initV[0] = 0x06; // Registro
			initV[1] = 0x07; // Valor
			if( HAL_I2C_Master_Transmit (&hi2c1, 0x94, initV, 2, 50)!= HAL_OK)
			{
				HardFault_Handler ();
			}



	//** Final required start **//



		// Set the <Power Ctl 1> register (0x02) to 0x9E.
			initV[0] = 0x02; // Registro
			initV[1] = 0x9E; // Valor
			if( HAL_I2C_Master_Transmit (&hi2c1, 0x94, initV, 2, 50)!= HAL_OK)
			{
				HardFault_Handler ();
			}
}
