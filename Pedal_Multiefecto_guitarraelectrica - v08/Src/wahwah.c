/*
 * wahwah.c
 *
 *  Created on: 5 ago. 2019
 *      Author: pablo
 */





#include "main.h"

// ARM includes
#include "arm_const_structs.h"
#include "arm_math.h"





#define NUM_STAGES 	1
#define RADIUS 		0x73333333 // 0.9
#define COS_FI 		0x40000000 // 0.5
#define A1 			0x73333333 // (2 * RADIUS * COS_FI) 0.9
#define A2 			0x67AE147B // (RADIUS^2) 0.81
#define NUM_FILTERS 20
#define MAX_PHASE   0.75f
#define MIN_PHASE   0.25f




static arm_biquad_casd_df1_inst_f32 wahwah_filter [NUM_FILTERS];
static float32_t coeffs [NUM_FILTERS] [5 * NUM_STAGES]; // {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
static float32_t pState [NUM_FILTERS] [4 * NUM_STAGES];





void wahwah_init ()
{
	float phase = MIN_PHASE;
	float delta_phase = (MAX_PHASE - MIN_PHASE) / NUM_FILTERS;

	for (int i=0; i<NUM_FILTERS; i++)
	{
		coeffs [i] [0] = 1-phase;
		coeffs [i] [1] = 0.0f;
		coeffs [i] [2] = -phase+1;
		coeffs [i] [3] = 2*phase*0.5f;
		coeffs [i] [4] = -phase*phase;

		arm_biquad_cascade_df1_init_f32 (& wahwah_filter [i],
										 NUM_STAGES,
										 coeffs [i],
										 pState [i]);

		phase += delta_phase;
	}
}


void wahwah (float *in, float *out, int buff_size, uint32_t dist)
{
	static int i = 0;
	static int direccion = 0;
	if (direccion == 0)
	{
		i++;
	}
	else
	{
		i--;
	}

	if (i >= dist * NUM_FILTERS)
	{
		direccion = 1;
		i--;
	}
	else if (i <= 0)
	{
		direccion = 0;
		i++;
	}

	arm_biquad_cascade_df1_f32 (& wahwah_filter [(i/dist) % NUM_FILTERS],
							   in,
							   out,
							   buff_size);

	return;
}
