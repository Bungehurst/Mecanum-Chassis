#include "BSP.h"
void BSP_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断优先级分组模式 2
	delay_init();	    	 														//延时函数初始化
	usart1_init();																	//FPGA串口
	usart2_init();																	//遥控器串口
	usart3_init();																	//OPENMV串口
	TIM3_PWM_Init(10000-1,72-1);										//PWM初始化  1khz arr=10000,psc=72

}

