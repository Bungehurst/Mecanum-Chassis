#ifndef __USART3_H
#define __USART3_H
#include "sys.h" 
void usart3_init(void);

typedef struct 
{
	int16_t theta;
	int16_t rho;
	int16_t delta_rho;
	int16_t delta_x;
	int16_t delta_theta;
}OPENMV;

extern OPENMV Openmv;
#endif


