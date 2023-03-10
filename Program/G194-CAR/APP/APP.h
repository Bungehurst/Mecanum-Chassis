#ifndef __APP_H
#define __APP_H
#include "sys.h"
#include "pid.h"
#include "pwm.h"
#include "usart1.h"
void APP_Init(void);
void Motor_Control(void);
void Set_Wheel_Speed(int16_t target,CAR_WHEEL *wheel,PID_GENERAL *pid,uint8_t Wheel_Num);
#endif
