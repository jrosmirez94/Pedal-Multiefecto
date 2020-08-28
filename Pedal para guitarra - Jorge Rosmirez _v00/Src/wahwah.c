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
#define F2Q14(x)  ((q15_t)((float32_t)x * 16384UL))


static arm_biquad_casd_df1_inst_q15 wahwah_filter [NUM_FILTERS];
static q15_t coeffs [NUM_FILTERS] [6 * NUM_STAGES]; // {b10,0 , b11, b12, a11, a12, b20,0 , b21, b22, a21, a22,...}
static q15_t pState [NUM_FILTERS] [4 * NUM_STAGES];



void wahwah_init ()
{
	q15_t phase = MIN_PHASE;
	q15_t delta_phase = DELTA_PHASE;

	for (int i=0; i<NUM_FILTERS; i++)
	{
			
//			coeffs [i] [0] =F2Q14(0.1622); 									/* section  1  B0 */
//  		coeffs [i] [1] =F2Q14(0);
//  		coeffs [i] [2] =F2Q14(0);	 			/* section  1  B1 */
//  		coeffs [i] [3] =F2Q14(-0.1622); 										/* section  1  B2 */
//  		coeffs [i] [4] =F2Q14(-1.6435); 		/* section  1 -A1 */
//  		coeffs [i] [5] =F2Q14(0.6557);/* section  1 -A2 */
			
				
		
			coeffs [i] [0] =F2Q14( 1 ); 									/* section  1  B0 */
			coeffs [i] [1] =F2Q14(0);
			coeffs [i] [2] =F2Q14(0.848876953125);	 			/* section  1  B1 */
			coeffs [i] [3] =F2Q14(1); 										/* section  1  B2 */
			coeffs [i] [4] =F2Q14(1.9149169921875  ); 		/* section  1 -A1 */
			coeffs [i] [5] =F2Q14( -  0.92364501953125  );/* section  1 -A2 */
		
/*  F2Q14(1);										  /* section  2  B0 
		F2Q14( 0.0);
		F2Q14(-1.9989013671875 ); 		/* section  2  B1 
		F2Q14(1); 										/* section  2  B2 
		F2Q14(0.78826904296875  ); 		/* section  2 -A1 
		F2Q14(- 0.4776611328125  );   /* section  2 -A2 
		
*/
		
		
		
		//2q14
		/*
		coeffs [i] [0] = 0x4000; //DEC  32767-->b0	MATLAB N0	
		coeffs [i] [1] = 0x0000; //					    0
		coeffs [i] [2] = 0xC3F9; //DEC -30734-->b1  MATLAB N1	
		coeffs [i] [3] = 0x38E4; //DEC  29129-->b2  MATLAB N2	
		coeffs [i] [4] = 0x3E65; //DEC  31946-->a1  MATLAB D1	
		coeffs [i] [5] = 0xC256; //DEC -31572-->a2  MATLAB D2	
		*/
		
		//q15
		/*
		coeffs [i] [0] = 0x7FFF; //DEC  32767-->b0	MATLAB N0	
		coeffs [i] [1] = 0;			 //					    0
		coeffs [i] [2] = 0x87F2; //DEC -30734-->b1  MATLAB N1	
		coeffs [i] [3] = 0x71C9; //DEC  29129-->b2  MATLAB N2	
		coeffs [i] [4] = 0x7CCA; //DEC  31946-->a1  MATLAB D1	
		coeffs [i] [5] = 0x84AC; //DEC -31572-->a2  MATLAB D2	
		*/
		
		/*
	  coeffs [i] [0] = 1-phase;
		coeffs [i] [1] = 0;
		coeffs [i] [2] = 0;
		coeffs [i] [3] = 0x7FFF-phase;
		coeffs [i] [4] = phase;
		coeffs [i] [5] = -phase*phase;
	  */

	  wahwah_filter[i].numStages = NUM_STAGES;  
    wahwah_filter[i].pState = pState[i];  
    wahwah_filter[i].pCoeffs = coeffs[i];  
    wahwah_filter[i].postShift = 1;  
		
		arm_biquad_cascade_df1_init_q15(&wahwah_filter[i], wahwah_filter[i].numStages,  
            wahwah_filter[i].pCoeffs, wahwah_filter[i].pState,  
            wahwah_filter[i].postShift);  
		
	//arm_biquad_cascade_df1_init_q15(& wahwah_filter [i], NUM_STAGES, coeffs [i], pState [i],1);

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
