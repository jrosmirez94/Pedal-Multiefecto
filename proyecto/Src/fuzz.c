/* En reset_handler del startup_stm32f411xe.s

	             ;FPU settings
				 LDR     R0, =0xE000ED88           ; Enable CP10,CP11
				 LDR     R1,[R0]
				 ORR     R1,R1,#(0xF << 20)
				 STR     R1,[R0]
*/



#include "main.h"
#include "init.h"


#define A 10
#define B 10
#define C 10
#define D 10


//static float fuzz_eq (float x, int32_t dist);

//static int32_t x_s, fuzz_s;
uint16_t fuzz (int32_t x, int32_t dist)
{
	dist = 3000;
	/*x_s = x;
	float f_x = (x-16000.0)/(100.0+dist/10);
	float fuzz = fuzz_eq (f_x, dist) / fuzz_eq (1.0, dist);
	fuzz = (fuzz*(100.0+dist/10))+16000;

	fuzz_s = (uint16_t) ((fuzz <= 64000) * fuzz + (fuzz > 64000) * 64000);
	return (uint16_t) ((fuzz <= 64000) * fuzz + (fuzz > 64000) * 64000);*/
	return x*8;
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
