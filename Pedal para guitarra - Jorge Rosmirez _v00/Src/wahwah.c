#include "main.h"

// ARM includes
#include "arm_const_structs.h"
#include "arm_math.h"
#include "wahwah.h"
#include "math.h"


#define NUM_STAGES 	1
#define RADIUS 			0x73333333 // 0.9
#define COS_FI 			0x40000000 // 0.5
//#define A1 					0x73333333 // (2 * RADIUS * COS_FI) 0.9
//#define A2 					0x67AE147B // (RADIUS^2) 0.81
#define NUM_FILTERS 20
#define MAX_PHASE   0x6000 // 0.75
#define MIN_PHASE   0x2000 // 0.25
#define DELTA_PHASE 0x0333 //	(MAX_PHASE - MIN_PHASE) / NUM_FILTERS;
#define F2Q14(x)  ((q15_t)((float32_t)x * 16384UL))
#define FS 32000
#define GAIN 20
#define Q 10
#define ABS(a) a > 0 ? a : -a
#define MAX(a, b) a > b ? a : b0
#define POW_GAIN 10  // 10^(GAIN/20) GAIN=20
#define POW_GAIN2 3.1622776601683793319988935444327  // 10^(GAIN/40) GAIN=20
#define F_MAX 500
#define F_MIN 200
#define DEBUG 
#define B0_PRECALC 1.1617 // CON Q=10
#define B2_PRECALC 1.0823 // CON Q=10
#define A0_PRECALC 1.0315 // con Q=10
#define A2_PRECALC 0.9685 // con Q=10


static arm_biquad_casd_df1_inst_q15 wahwah_filter [NUM_FILTERS];
static q15_t coeffs [NUM_FILTERS] [6 * NUM_STAGES]; // {b10,0 , b11, b12, a11, a12, b20,0 , b21, b22, a21, a22,...}
static q15_t pState [NUM_FILTERS] [4 * NUM_STAGES];

DEBUG float wo;

    DEBUG float cosW;
    DEBUG float sinW;
    DEBUG float A;
    DEBUG float alpha;

    DEBUG float b0;
    DEBUG float b1;
    DEBUG float b2;
    DEBUG float a0;
    DEBUG float a1;
    DEBUG float a2;
    
    DEBUG float B0;
    DEBUG float B1;
    DEBUG float B2;
    DEBUG float A1;
    DEBUG float A2;
    DEBUG float Mx;

    DEBUG float B0new;
    DEBUG float B1new;
    DEBUG float B2new;

    DEBUG float N0;
    DEBUG float N1;
    DEBUG float N2;
    DEBUG float D1;
    DEBUG float D2;

float f0_g;

void filter_calculate (q15_t * coeff,float f0)
{

		f0_g=f0;
	
    wo = 2*PI*f0/FS;
    cosW = cos(wo);
    sinW = sin(wo);
    A = POW_GAIN2; 					//10^(GAIN/40);
    alpha = sinW/(2*Q*A);

    //Peaking EQ coefficients

    b0 = POW_GAIN*(1 + alpha*A);
    b1 = POW_GAIN*(-2*cos(wo));
    b2 = POW_GAIN*(1 - alpha*A);
    a0 = 1 + (alpha/A);
    a1 =-2*cos(wo);
    a2 =1-(alpha/A);

    //Normalize so that A0 = 1

    B0 = b0/a0;
    B1 = b1/(a0);
    B2 = b2/a0;
    A1 = a1/(-a0);
    A2 = a2/(-a0);
    Mx = MAX (MAX (ABS(B0), ABS(B1)), ABS(B2));


    if (Mx > 1)
    {
    B0new = B0/Mx;
    B1new = B1/Mx;
    B2new = B2/Mx;
    }
		
    else
    {
    B0new = B0;
    B1new = B1;
    B2new = B2;
    }


    N0 = (B0new);
    N1 = (B1new);
    N2 = (B2new);
    D1 = (A1);
    D2 = (A2);

    coeff [0]=F2Q14(N0);
    coeff [1]=0;
    coeff [2]=F2Q14(N1);
    coeff [3]=F2Q14(N2);
    coeff [4]=F2Q14(D1);
    coeff [5]=F2Q14(D2);
}	


void wahwah_init ()
{
	
//intermediate parameters
	
	for (int i=0; i<NUM_FILTERS; i++)
	{
			
//FUNCION NORMALIZAR FDATOOL
//			coeffs [i] [0] =F2Q14(0.689880371093750); 									/* section  1  B0 */
//  		coeffs [i] [1] =F2Q14(0);
//  		coeffs [i] [2] =F2Q14(0);	 			/* section  1  B1 */
//  		coeffs [i] [3] =F2Q14(-0.689941406250000); 										/* section  1  B2 */
//  		coeffs [i] [4] =F2Q14(1.134521484375000); 		/* section  1 -A1 */
//  		coeffs [i] [5] =F2Q14(-0.620239257812500);/* section  1 -A2 */
			
				
//FILTERTEST Q=1 GAIN 0.8
			//coeffs [i] [0] =F2Q14(1); 									/* section  1  B0 */
			//coeffs [i] [1] =F2Q14(0);
			//coeffs [i] [2] =F2Q14(-0.522399902343750);	 			/* section  1  B1 */
			//coeffs [i] [3] =F2Q14(0.477569580078125); 										/* section  1  B2 */
			//coeffs [i] [4] =F2Q14(0.534667968750000); 		/* section  1 -A1 */
			//coeffs [i] [5] =F2Q14(-0.512359619140625);/* section  1 -A2 */
		
//FILTERTEST Q=10 GAIN 0.8
		  //	coeffs [i] [0] =F2Q14(0.999969482421875); 									/* section  1  B0 */
		  //	coeffs [i] [1] =F2Q14(0);
		  //	coeffs [i] [2] =F2Q14(-0.682952880859375);	 			/* section  1  B1 */
	   	//	coeffs [i] [3] =F2Q14(0.931671142578125); 										/* section  1  B2 */
		  //	coeffs [i] [4] =F2Q14(0.684997558593750); 		/* section  1 -A1 */
	   	//	coeffs [i] [5] =F2Q14(-0.937500000000000);/* section  1 -A2 */
		
//FILTERTEST Q=10 GAIN 1
		//	coeffs [i] [0] =F2Q14(0.999969482421875); 									/* section  1  B0 */
		//	coeffs [i] [1] =F2Q14(0);
		//	coeffs [i] [2] =F2Q14(-0.682952880859375);	 			/* section  1  B1 */
		//	coeffs [i] [3] =F2Q14(0.931671142578125); 										/* section  1  B2 */
		//	coeffs [i] [4] =F2Q14(0.685455322265625); 		/* section  1 -A1 */
		//	coeffs [i] [5] =F2Q14(-0.938903808593750);/* section  1 -A2 */
		
		filter_calculate(coeffs[i],((F_MAX - F_MIN)/NUM_FILTERS)*i + F_MIN);
		
/*		
    F2Q14(1);										  /* section  2  B0 
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
		
	 }
}


void wahwah (q15_t*in, q15_t *out, int buff_size, uint32_t dist)
{
	static int i = 0;
	static int direccion = 0;
	
    int filt_idx = 0;
	
    if (direccion == 0)
	{
		i++;
	}
	else
	{
		i--;
	}
    
	if (i >= dist*NUM_FILTERS)
	{
		direccion = 1;
		i--;
	}
	else if (i <= 0)
	{
		direccion = 0;
		i++;
	}

    filt_idx = i / dist;
	
	arm_biquad_cascade_df1_fast_q15 (& wahwah_filter [filt_idx], in, out, buff_size);

	return;

}
