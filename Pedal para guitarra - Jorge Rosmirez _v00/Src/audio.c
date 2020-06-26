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
// #include "arm_const_structs.h"
// #include "arm_math.h"

// Project includes
#include "audio.h"
#include "CS43L22.h"




/******************************************************************************
 *  								DEFINES
 ******************************************************************************/





/******************************************************************************
 *  						GLOBAL VARIABLES DECLARATION
 ******************************************************************************/
volatile DMA_FST_t transmit_ready = DMA_INIT;
volatile float * buffer_float = NULL;





/******************************************************************************
 *  						LOCAL FUNCTIONS FORWARD
 ******************************************************************************/






/******************************************************************************
 *  						LOCAL VARIABLES DECLARATION
 ******************************************************************************/
static q31_t   _dma_out   [DMA_HALF_SIZE * 2] = {0};
static q31_t   _dma_in    [DMA_HALF_SIZE * 2] = {0};
static float   _dma_float [DMA_HALF_SIZE]     = {0};





/******************************************************************************
 *  						GLOBAL FUNCTIONS DEFINITION
 ******************************************************************************/
void init_audio ()
{
	// Configura DAC
	CS43L22_init ();
	
	// Inicia Transmision de ADC y DAC (a traves de I2S) por DMA

	// DMA para mover del _dma_out al DAC
	HAL_I2S_Transmit_DMA (& hi2s3, (uint16_t *) _dma_out, DMA_HALF_SIZE * 2);
	HAL_Delay (1); // Must start I2S before ADC
	// DMA para mover del ADC al _dma_in
	HAL_ADC_Start_DMA (& hadc1, (uint32_t *) _dma_in,  DMA_HALF_SIZE * 2);
}


/******************************************************************************
 *  						INTERRUPT HANDLERS
 ******************************************************************************/
// not use __disable_irq() and __enable_irq() to do critical stuffs
// put transmit_ready = HAL_TIMEOUT at finalize using buffer_float

//**ADC**//

void HAL_ADC_ConvHalfCpltCallback (ADC_HandleTypeDef * hadc)
{
	if (transmit_ready == DMA_LOGIC_BUSY)
	{
	}
	else
	{
		arm_q31_to_float (& _dma_in [0], _dma_float, DMA_HALF_SIZE); //conversion de formato q31 a float
		buffer_float = _dma_float;

		transmit_ready = DMA_ADC_READY;		// hasta aca tengo la mitad del buffer lleno
	}
}

void HAL_ADC_ConvCpltCallback (ADC_HandleTypeDef * hadc)
{
	if (transmit_ready == DMA_LOGIC_BUSY)
	{
	}
	else
	{
		arm_q31_to_float (& _dma_in [DMA_HALF_SIZE], _dma_float, DMA_HALF_SIZE);
		buffer_float = _dma_float;

		transmit_ready = DMA_ADC_READY; // hasta aca se completó la otra mitad
	}
}



//*** DAC ***//

void HAL_I2S_TxHalfCpltCallback (I2S_HandleTypeDef * hi2s)
{
	if (transmit_ready == DMA_LOGIC_BUSY)
	{
		memset (& _dma_out [0], 0, DMA_HALF_SIZE * 2); // si el buffer esta ocupado completas todo con 0´s para que a la salida no tenga ruido
	}
	else
	{
		arm_float_to_q31 (_dma_float, & _dma_out [0], DMA_HALF_SIZE);
		buffer_float = NULL;

		transmit_ready = DMA_DAC_READY;
	}
}

void HAL_I2S_TxCpltCallback (I2S_HandleTypeDef * hi2s)
{
	if (transmit_ready == DMA_LOGIC_BUSY)
	{
		memset (& _dma_out [DMA_HALF_SIZE], 0, DMA_HALF_SIZE * 2);
	}
	else
	{
		
		arm_float_to_q31 (_dma_float, & _dma_out [DMA_HALF_SIZE], DMA_HALF_SIZE);
		buffer_float = NULL;
		
		transmit_ready = DMA_DAC_READY;
	}
}






/******************************************************************************
 *  						LOCAL FUNCTIONS DEFINITION
 ******************************************************************************/