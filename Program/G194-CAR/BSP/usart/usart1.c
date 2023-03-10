#include "usart1.h"

#include "led.h" 
#include "delay.h"
#include "stm32f10x.h"
#include "stm32f10x_dma.h"
#include <string.h>
#include <math.h>

#define DMA_Rec_Len 13

static uint8_t buffer_UART1[DMA_Rec_Len];
static uint8_t buf3_xxx[DMA_Rec_Len];
static uint8_t buf2_xxx[DMA_Rec_Len];

static void packet_dec(void);
static int16_t Syn_Wheel_Speed(uint8_t dir,uint16_t speed);
static int16_t Limit_A_Filter(int16_t data);

CAR_WHEEL Car_Wheel_LF = {0};
CAR_WHEEL	Car_Wheel_RF = {0};
CAR_WHEEL Car_Wheel_LB = {0};
CAR_WHEEL Car_Wheel_RB = {0};

void usart1_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; 
  USART_InitTypeDef usart1;
	NVIC_InitTypeDef nvic_USART1_dma;
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	
  USART_DeInit(USART1);  //复位串口1
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                //USART1 RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;            
  GPIO_Init(GPIOA, &GPIO_InitStructure); 

	USART_StructInit(&usart1);
	usart1.USART_BaudRate = 115200;
	usart1.USART_WordLength = USART_WordLength_8b;
	usart1.USART_StopBits = USART_StopBits_1;
	usart1.USART_Parity = USART_Parity_No;
	usart1.USART_Mode = USART_Mode_Rx;
	usart1.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1,&usart1);
	
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);   //使能串口1 DMA接收
	
	//相应的DMA配置
	DMA_DeInit(DMA1_Channel5);   //将DMA的通道5寄存器重设为缺省值  串口1对应的是DMA通道5
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;  //DMA外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)buffer_UART1;  //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设读取发送到内存
	DMA_InitStructure.DMA_BufferSize = DMA_Rec_Len;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; //DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
	
	nvic_USART1_dma.NVIC_IRQChannel = DMA1_Channel5_IRQn;
  nvic_USART1_dma.NVIC_IRQChannelPreemptionPriority = 1;
  nvic_USART1_dma.NVIC_IRQChannelSubPriority = 0;
  nvic_USART1_dma.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_USART1_dma);

  DMA_Cmd(DMA1_Channel5, ENABLE);  //正式驱动DMA传输
	DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,ENABLE);  //DMA传输完成中断 ！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	USART_Cmd(USART1,ENABLE);
}

void DMA1_Channel5_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC5))
  {
		DMA_ClearFlag(DMA1_IT_TC5);
    DMA_ClearITPendingBit(DMA1_IT_TC5);
		if(buffer_UART1[0]==0x5A	&&	buffer_UART1[1]==0xA5)				
		{
			memcpy(buf3_xxx,buffer_UART1,DMA_Rec_Len);
			packet_dec();
		}else
		{
			int bit = 0;
			if(buf2_xxx[DMA_Rec_Len-1]==0x5A	&&	buffer_UART1[0]==0xA5)	
			{
				bit = DMA_Rec_Len-1;
			}else 
			{
				for(int i = 0;i < DMA_Rec_Len;i++)											
				{
					if(buf2_xxx[i]==0x5A && buf2_xxx[i+1]==0xA5) 
						{
							bit = i;		
							break;
						}
					}
			}
			memcpy(buf3_xxx,buf2_xxx+bit,DMA_Rec_Len-bit);
			memcpy(buf3_xxx+DMA_Rec_Len-bit,buffer_UART1,bit);
			memcpy(buf2_xxx,buffer_UART1,DMA_Rec_Len);
			packet_dec();
		}
	}
}

static void packet_dec(void)
{
	uint32_t sum = 0;
	for(int i = 2;i < 10;i++) sum+=buf3_xxx[i] ;
	if(buf3_xxx[10] == (sum&0xFF))
	{
		Car_Wheel_LF.dir =	(uint8_t)(buf3_xxx[2]&0x80)>>7;
		Car_Wheel_LF.speed_raw = (uint16_t)((buf3_xxx[2]&0x7F)<<8 | buf3_xxx[3]);
		Car_Wheel_LF.speed = Limit_A_Filter(Syn_Wheel_Speed(Car_Wheel_LF.dir,Car_Wheel_LF.speed_raw));
		
		Car_Wheel_RF.dir =	(uint8_t)(buf3_xxx[4]&0x80)>>7;
		Car_Wheel_RF.speed_raw = (uint16_t)((buf3_xxx[4]&0x7F)<<8 | buf3_xxx[5]);
		Car_Wheel_RF.speed = Limit_A_Filter(Syn_Wheel_Speed(Car_Wheel_RF.dir,Car_Wheel_RF.speed_raw));
		
		Car_Wheel_LB.dir =	(uint8_t)(buf3_xxx[6]&0x80)>>7;
		Car_Wheel_LB.speed_raw = (uint16_t)((buf3_xxx[6]&0x7F)<<8 | buf3_xxx[7]);		
		Car_Wheel_LB.speed = Limit_A_Filter(Syn_Wheel_Speed(Car_Wheel_LB.dir,Car_Wheel_LB.speed_raw));
		
		Car_Wheel_RB.dir =	(uint8_t)(buf3_xxx[8]&0x80)>>7;
		Car_Wheel_RB.speed_raw = (uint16_t)((buf3_xxx[8]&0x7F)<<8 | buf3_xxx[9]);		
		Car_Wheel_RB.speed = Limit_A_Filter(Syn_Wheel_Speed(Car_Wheel_RB.dir,Car_Wheel_RB.speed_raw));
	}
	
}

static int16_t Syn_Wheel_Speed(uint8_t dir,uint16_t speed_raw)
{
	if(dir == 0) 
		return speed_raw;
	else if(dir == 1)
		return -speed_raw;
	else return 0;
}

//static uint8_t Cal_Dir(uint8_t dir)
//{
//	static uint8_t buf[5] = {0};
//	static uint8_t counter = 0;
//	uint8_t cont= 0;
//	if(counter<5) 
//	{
//		buf[counter++] = dir;
//	}else counter = 0;
//	for(int i = 0;i < 5;i++)
//	{
//		if(buf[i] == 1) cont++;
//	}
//	return (cont>2)?(1):(0);
//}

static int16_t Limit_A_Filter(int16_t data)
{
	static int16_t data_last = 0,data_now = 0;
	data_last = data_now;
	data_now = data;
	if(fabsf(data_now - data_last )< 1500)
		return data_last;
	else
		return data_now;

}
