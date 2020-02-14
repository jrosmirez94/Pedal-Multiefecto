/* En reset_handler del startup_stm32f411xe.s

	             ;FPU settings
				 LDR     R0, =0xE000ED88           ; Enable CP10,CP11
				 LDR     R1,[R0]
				 ORR     R1,R1,#(0xF << 20)
				 STR     R1,[R0]
*/



#include "main.h"

// ARM includes
#include "arm_const_structs.h"
#include "arm_math.h"


#define A 10
#define B 10
#define C 10
#define D 10
#define EULER 2.71828182845904523536028747135266249775724709369995f


//static float fuzz_eq (float x, int32_t dist);

//static int32_t x_s, fuzz_s;
float fuzz (float x, float dist)
{
	if (x > dist)
	{
		return dist;
	}
	if (x < -dist)
	{
		return -dist;
	}
/*
	float y, x2 = x, mix;
	a = 110.0f;
	mix = 1.0f;
	int sign = 1;
	if (x2 < 0)
	{
		sign = -1;
		x2 *= -1;
	}

	y = 1 - (x2-1)*(x2-1);
	y *= sign;

	y = mix * y + (1-mix)*x;
*/
	return x;
}
/*
float fuzz_eq (float f_x, int32_t dist)
{
	float fuzz;
	float mod_x;

	mod_x = (f_x >= 0) * f_x + (f_x < 0) * (-f_x);
	float gain = 100;
	float q = f_x / mod_x;
	float taylor_x = gain*q*f_x;
	float taylor = 1 + taylor_x + 0.5*taylor_x*taylor_x + (1/6)*taylor_x*taylor_x*taylor_x;
	fuzz = q * (1 - taylor);

	return fuzz;
}*/
