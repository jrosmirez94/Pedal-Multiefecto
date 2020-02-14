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

#define CHANNELS 2
#define TENSION_DC 2070
#define TENSION_DC_FLOAT 0.000000963918864727020263671875f





/******************************************************************************
 *  								ENUMS
 ******************************************************************************/
typedef enum
{
	DMA_INIT,
	DMA_ADC_READY,
	DMA_LOGIC_READY,
	DMA_LOGIC_BUSY,
	DMA_DAC_READY,
} DMA_FST_t; // DMA Finite-State Machine





/******************************************************************************
 *  								EXPORTED VARIABLES
 ******************************************************************************/
extern volatile DMA_FST_t transmit_ready;
extern volatile float * buffer_float;
extern volatile int pote;





/******************************************************************************
 *  								EXPORTED FUNCTIONS
 ******************************************************************************/
void init_audio ();


#endif // AUDIO_H_
