#ifndef AUDIO_H_
#define AUDIO_H_

/******************************************************************************
 *  								INCLUDES
 ******************************************************************************/
#include "arm_math.h"




/******************************************************************************
 *  								DEFINES
 ******************************************************************************/
// tamanio de la mitad del buffer DMA para hacer ping pong
#define DMA_HALF_SIZE 2048
#define CHANNELS_IN 2
#define CHANNELS_OUT 2



/******************************************************************************
 *  								ENUMS
 ******************************************************************************/
typedef enum
{
	DMA_INIT,					// el ADC esta listo para completar el buffer
	DMA_ADC_READY,		// el ADC tiene completo el buffer y esta listo para procesar
	DMA_LOGIC_BUSY,		// comenzo el procesamiento y no deberían llegar muestras nuevas para que no se solapen
	DMA_LOGIC_READY,	// ya termino el procesamiento y esta listo para mandar por el DAC
	DMA_DAC_READY,		// el DMA envió los datos del buffer al DAC
} DMA_FST_t; // DMA Finite-State Machine





/******************************************************************************
 *  								EXPORTED VARIABLES
 ******************************************************************************/
extern DMA_FST_t transmit_ready;
extern q31_t * buffer_DMA;





/******************************************************************************
 *  								EXPORTED FUNCTIONS
 ******************************************************************************/
void init_audio (void);


#endif // AUDIO_H_
