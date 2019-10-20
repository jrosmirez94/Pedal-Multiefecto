#ifndef INIT_H
#define INIT_H


#include "arm_const_structs.h"
#include "arm_math.h"

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

void init_CS43L22(void);			//Declaracion de la funcion que inicializa el DAC

#endif // INIT_H

