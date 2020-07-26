#include "fuzz.h"

q15_t fuzz(q15_t x)
{
	q15_t y, x2 = x, mix,a;
	//a = 110.0f;
	mix = 1;
	char sign = 1;
	if (x2 < 0)
	{
		sign = -1;
		x2 *= -1;
	}

	y = 1 - (x2-1)*(x2-1);
	y *= sign;

	y = mix * y + (1-mix)*x;
	
	return y;
}