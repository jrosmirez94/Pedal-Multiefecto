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
#define Q 3
#define ABS(a) a > 0 ? a : -a
#define MAX(a, b) a > b ? a : b0
#define POW_GAIN 10  // 10^(GAIN/20) GAIN=20
#define POW_GAIN2 3.1622776601683793319988935444327  // 10^(GAIN/40) GAIN=20
#define F_MAX 1000
#define F_MIN 200
#define B0_PRECALC 1.1617 // CON Q=10
#define B2_PRECALC 1.0823 // CON Q=10
#define A0_PRECALC 1.0315 // con Q=10
#define A2_PRECALC 0.9685 // con Q=10


static arm_biquad_casd_df1_inst_q15 wahwah_filter [NUM_FILTERS];
static q15_t coeffs [NUM_FILTERS] [6 * NUM_STAGES]; // {b10,0 , b11, b12, a11, a12, b20,0 , b21, b22, a21, a22,...}
static q15_t pState [NUM_FILTERS] [4 * NUM_STAGES];

void filter_calculate (q15_t * coeff,float f0)
{

		float wo;

		float cosW;
		float sinW;
		float A;
		float alpha;

		float b0;
		float b1;
		float b2;
		float a0;
		float a1;
		float a2;
				
		float B0;
		float B1;
		float B2;
		float A1;
		float A2;
		float Mx;

		float B0new;
		float B1new;
		float B2new;

		float N0;
		float N1;
		float N2;
		float D1;
		float D2;

		float f0_g;
	
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
					
		filter_calculate(coeffs[i],((F_MAX - F_MIN)/NUM_FILTERS)*i + F_MIN);
		

	  wahwah_filter[i].numStages = NUM_STAGES;  
    wahwah_filter[i].pState = pState[i];  
    wahwah_filter[i].pCoeffs = coeffs[i];  
    wahwah_filter[i].postShift = 1;  
		
		arm_biquad_cascade_df1_init_q15(&wahwah_filter[i], wahwah_filter[i].numStages,  
            wahwah_filter[i].pCoeffs, wahwah_filter[i].pState,  
            wahwah_filter[i].postShift);  
		
	 }
}


void wahwah (q15_t*in, q15_t *out, int buff_size, uint32_t wah_vel)
{
	static int i = 0;
	static int direccion = 0;
	
  static int filt_idx = 0;
	
	i++;
	
	if (i%wah_vel==0)
	{
	
			if (direccion == 0)
		{
			filt_idx++;
		}
		else
		{
			filt_idx--;
		}
			
		if (filt_idx >= NUM_FILTERS)
		{
			direccion = 1;
			filt_idx--;
		}
		else if (filt_idx <= 0)
		{
			direccion = 0;
			filt_idx++;
		}
	}
    //filt_idx = i / wah_vel;
	
	arm_biquad_cascade_df1_q15 (& wahwah_filter [filt_idx], in, out, buff_size);

	return;

}
