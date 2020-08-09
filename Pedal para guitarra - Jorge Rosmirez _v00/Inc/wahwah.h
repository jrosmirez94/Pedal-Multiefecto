#ifndef WAHWAH_H_
#define WAHWAH_H_



#include "main.h"
#include "arm_math.h"


void wahwah_init (void);
void wahwah (q15_t *x, q15_t *out, int buff_size, uint32_t dist);



#endif /* WAHWAH_H_ */

