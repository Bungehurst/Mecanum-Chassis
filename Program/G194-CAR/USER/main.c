#include "main.h"
int main(void)
{	
	BSP_Init();						//板级初始化
	Calib_rcdata_Zero();
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	timer_Init(); 																	//系统Tick初始化  10ms
	while(1)
	{
		
	} 
}

