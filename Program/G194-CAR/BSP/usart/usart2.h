#ifndef __USART2_H
#define __USART2_H
#include "sys.h"
#include "stm32f10x_dma.h"

typedef struct 
{
	int16_t Channel_Current[16];
	uint16_t Channel_Zero[16];
	uint16_t Channel_Raw[16];
}RC_DATA;
extern RC_DATA rc_data;
void usart2_init(void);

void Calib_rcdata_Zero(void);

#endif
