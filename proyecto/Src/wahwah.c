/*
 * wahwah.c
 *
 *  Created on: 5 ago. 2019
 *      Author: pablo
 */





#include "main.h"
#include "init.h"





#define NUM_STAGES 	1
#define RADIUS 		0x73333333 // 0.9
#define COS_FI 		0x40000000 // 0.5
#define A1 			0x73333333 // (2 * RADIUS * COS_FI) 0.9
#define A2 			0x67AE147B // (RADIUS^2) 0.81





arm_biquad_casd_df1_inst_f32 wahwah_filter;
float32_t coeffs [5 * NUM_STAGES] = { // {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
		0x7FFFFFFF,
		0,
		0,
		A1,
		A2
};
float32_t pState [4 * NUM_STAGES];





void wahwah_init ()
{
	arm_biquad_cascade_df1_init_f32 (& wahwah_filter,
									 NUM_STAGES,
									 coeffs,
									 pState);
}



void wahwah (float * in, float * out, int buff_size)
{
	arm_biquad_cascade_df1_f32(& wahwah_filter,
							   in,
							   out,
							   buff_size);
}
