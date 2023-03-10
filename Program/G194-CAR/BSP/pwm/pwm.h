#ifndef __PWM_H
#define __PWM_H
#include "sys.h"

#define PWM_F_L(x) TIM_SetCompare1(TIM3,x) 	//PA6
#define PWM_F_R(x) TIM_SetCompare2(TIM3,x)	//PA7
#define PWM_B_L(x) TIM_SetCompare3(TIM3,x)	//PB0
#define PWM_B_R(x) TIM_SetCompare4(TIM3,x)	//PB1

#define DIR_CTL_LF(x)	GPIO_WriteBit(GPIOB,GPIO_Pin_7,x)
#define DIR_CTL_RF(x)	GPIO_WriteBit(GPIOB,GPIO_Pin_6,x)
#define DIR_CTL_LB(x)	GPIO_WriteBit(GPIOB,GPIO_Pin_5,x)
#define DIR_CTL_RB(x)	GPIO_WriteBit(GPIOB,GPIO_Pin_2,x)

void TIM3_PWM_Init(u16 arr,u16 psc);

#endif

