/*
 * adapt_in.h
 *
 *  Created on: 4 ago. 2019
 *      Author: pablo
 */

#ifndef ADAPT_IN_H_
#define ADAPT_IN_H_

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "arm_const_structs.h"
#include "arm_math.h"

static inline float adapt_in (float in_i) // after adc input
{
	return in_i; // + 2^15 - 2000 (2000 es el promedio aproximado)
}

static inline float deadapt_in (float in_i) // before dac output
{
	return in_i; // + 2^15 - 2000 (2000 es el promedio aproximado)
}

static inline void adapt_in_all (float * in_i, int buff_size)
{
	int i;

	for (i = 0; i < buff_size; i += 2)
	{
		in_i [i] += 30000; // + 2^15 - 2000 (2000 es el promedio aproximado)
	}
}

#endif /* ADAPT_IN_H_ */
