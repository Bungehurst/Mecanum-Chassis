#include "timer.h"
u32 time_cnt = 0;
void timer_Init(void)   //period  10ms
{
	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
	
	TIM_TimeBaseStructure.TIM_Period = 72-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 
	TIM_TimeBaseStructure.TIM_Prescaler = 1000-1; //设置用来作为TIMx时钟频率除数的预分频值  不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  						//TIM2 中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  	//先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  				//从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 						//IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);					//允许更新中断 

  TIM_Cmd(TIM4,ENABLE);
	
}

void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4,TIM_IT_Update)!= RESET) 
	{
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
		time_cnt++;
		//Motor_Control();//10 ms
		APP_Init();
  }
}

