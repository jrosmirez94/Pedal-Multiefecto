#include "fuzz.h"
#define ABS(x) (x>0?x:-x) // funcion para sacar el valor absoluto
#define SIGN(x) (x>0?1:-1)
#define CANT_THRESHOLD 5
#define MPLY_Q15(x1, x2) ((x1*x2)>>15)
#define MPLY_6Q10(x1, x2) ((x1*x2)>>10)
#define MPLY_3Q13(x1, x2) ((x1*x2)>>13)
#define MPLY_4Q12(x1, x2) ((x1*x2)>>12)

q15_t fuzz(q15_t x,q15_t pote)
{
	//pote+=0x008E;
	q15_t threshold [CANT_THRESHOLD]={0, 0x0100, 0x1000, 0x4000, 0x7FFF};
	q15_t pendiente[CANT_THRESHOLD]={0x7FFF, 0x1000, 0x0200, 0x0040, 0x0000};
	q15_t prev=0;
	static q15_t y=0;
	y=x;
	
	int i=0;
	
	for(i=1;i<CANT_THRESHOLD;i++)
	{
	
		if(ABS(x)<threshold[i])
		{
			y=MPLY_4Q12((ABS(x)-threshold[i-1]), pendiente[i-1])+prev;
			
      y=y*SIGN(x);
			break;
		}
		
		prev=prev + MPLY_4Q12((threshold[i]-prev),pendiente[i-1]);
	
	}
	
	return y;
	
}
	
