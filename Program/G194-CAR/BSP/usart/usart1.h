#ifndef __USART1_H
#define __USART1_H
#include "sys.h"

void usart1_init(void);
typedef struct 
{
	uint16_t speed_raw;
	int16_t speed;
	uint8_t dir;
}CAR_WHEEL;
extern CAR_WHEEL Car_Wheel_LF ,Car_Wheel_RF ,Car_Wheel_LB ,Car_Wheel_RB ;
#endif

