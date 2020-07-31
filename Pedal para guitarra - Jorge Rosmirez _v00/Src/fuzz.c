#include "fuzz.h"
#define ABS(x) (x>0?x:-x) // funcion para sacar el valor absoluto

q15_t fuzz(q15_t x)
{
	q15_t threshold_low=0x6F54, threshold_high=0x7FFF; //pendiente de la ganancia baja y alta
	q15_t y=x;
	//0.000000001000000  DEC 0.001953
	
	if(ABS(x)<threshold_low)
	{
		y=x>>1;
	}
	else if(ABS(x)<threshold_high)
	{
		//y=(3-(2-3*x)*(2-3*x))/3;
		y=-(x-0x7FFF)*(x-0x7FFF)+0x7FFF;
	}
	else
	{
		y=0x7FFF; //DEC 32767
	}
	
	return y;
}
