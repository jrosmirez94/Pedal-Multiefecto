#ifndef AUDIO_H_
#define AUDIO_H_

/******************************************************************************
 *  								INCLUDES
 ******************************************************************************/





/******************************************************************************
 *  								DEFINES
 ******************************************************************************/
// tamanio de la mitad del buffer DMA para hacer ping pong
#define DMA_HALF_SIZE 1024





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
extern volatile DMA_FST_t transmit_ready;
extern volatile float * buffer_float;





/******************************************************************************
 *  								EXPORTED FUNCTIONS
 ******************************************************************************/
void init_audio ();


#endif // AUDIO_H_
