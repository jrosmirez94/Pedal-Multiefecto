#include "main.h"

// ARM includes
#include "arm_const_structs.h"
#include "arm_math.h"
#include "wahwah.h"




#define NUM_STAGES 	1
#define RADIUS 			0x73333333 // 0.9
#define COS_FI 			0x40000000 // 0.5
#define A1 					0x73333333 // (2 * RADIUS * COS_FI) 0.9
#define A2 					0x67AE147B // (RADIUS^2) 0.81
#define NUM_FILTERS 20
#define MAX_PHASE   0x6000 // 0.75
#define MIN_PHASE   0x2000 // 0.25
#define DELTA_PHASE 0x0333 //	(MAX_PHASE - MIN_PHASE) / NUM_FILTERS;



static arm_biquad_casd_df1_inst_q15 wahwah_filter [NUM_FILTERS];
static q15_t coeffs [NUM_FILTERS] [6 * NUM_STAGES]; // {b10,0 , b11, b12, a11, a12, b20,0 , b21, b22, a21, a22,...}
static q15_t pState [NUM_FILTERS] [4 * NUM_STAGES];



void wahwah_init ()
{
	q15_t phase = MIN_PHASE;
	q15_t delta_phase = DELTA_PHASE;

	for (int i=0; i<NUM_FILTERS; i++)
	{
		
		coeffs [i] [0] = 0x7FFF;
		coeffs [i] [1] = 0;
		coeffs [i] [2] = 0;
		coeffs [i] [3] = 0;
		coeffs [i] [4] = 0;
		coeffs [i] [5] = 0;
		
		
	/*coeffs [i] [0] = 1-phase;
		coeffs [i] [1] = 0;
		coeffs [i] [2] = 0;
		coeffs [i] [3] = 0x7FFF-phase;
		coeffs [i] [4] = phase;
		coeffs [i] [5] = -phase*phase;
	*/

		
		arm_biquad_cascade_df1_init_q15(& wahwah_filter [i], NUM_STAGES, coeffs [i], pState [i],1);

		phase += delta_phase;
	}
}


void wahwah (q15_t*in, q15_t *out, int buff_size, uint32_t dist)
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
	
	i=i>=NUM_FILTERS?i-NUM_FILTERS:i;
	i=i<0?i+NUM_FILTERS:i;
	
	arm_biquad_cascade_df1_fast_q15 (& wahwah_filter [i], in, out, buff_size);

	return;
}
