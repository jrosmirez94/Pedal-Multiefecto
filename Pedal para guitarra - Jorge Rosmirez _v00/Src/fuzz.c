#include "fuzz.h"
#define ABS(x) (x>0?x:-x) // funcion para sacar el valor absoluto
#define SIGN(x) (x>0?1:-1)

q15_t fuzz(q15_t x,q15_t pote)
{
	//pote+=0x008E;
	q15_t threshold_low=pote, threshold_high=0x7FFF; //pendiente de la ganancia baja y alta
	q15_t y=x;
	q15_t sqrt;
	//0.000000001000000  DEC 0.001953
	
	if(ABS(x)<threshold_low)
	{
		y=x;
	}
	else if(ABS(x)<threshold_high)
	{
		//y=(3-(2-3*x)*(2-3*x))/3;
		//y=-(ABS(x)-0x7FFF)*(ABS(x)-0x7FFF)+0x7FFF;
		//y=-(ABS(x>>8)-0x00B5)*(ABS(x>>8)-0x00B5)+0x7FFF;
		//y=-((x-1).*(x-1))+1;
		//y=-((ABS(x)-0x7FFF)*(ABS(x)-0x7FFF))+0x7FFF;
		//y=-((ABS(x)-0x7FFF)*(ABS(x)-0x7FFF))>>16+0x7FFF;
		//y=sqrt(x).*0x00B5;  //181.03
		
		//arm_sqrt_q15(ABS(x)-pote,&sqrt);
		//y=sqrt*0x00B5+pote;
		
		//arm_sqrt_q15(ABS(x+0x0500-pote),&sqrt);
		//y=ABS(sqrt*0x00B5+pote-0x194C);
		
		//y=sqrt(x+0x0500)*181.03-0x194C;
		//y*=SIGN(x); 					// si es negativo lo multiplico por -1, si es postivo por 1.
		
		
	}
	else
	{
		y=0x7FFF; //DEC 32767
		y*=SIGN(x); 					// si es negativo lo multiplico por -1, si es postivo por 1.
	}
		
	return y;
}
