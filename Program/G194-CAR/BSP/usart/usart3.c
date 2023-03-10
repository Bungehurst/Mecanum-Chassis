#include "usart3.h"

#include "led.h" 
#include "delay.h"
#include "stm32f10x.h"
#include "stm32f10x_dma.h"
#include <string.h>
#include <math.h>

#define DMA_Rec_Len_3 9

static uint8_t buffer_UART3[DMA_Rec_Len_3];
static uint8_t buf3_xxx[DMA_Rec_Len_3];
static uint8_t buf2_xxx[DMA_Rec_Len_3];

typedef struct Data_Point_Ave_Fil
{
	int bf_size;
	int counter;
	float data_max;
	float data_min;
	float data_out;
	float buffer[30];
	float sum;
}AVE_FIL;

AVE_FIL Openmv_rho = {30,0,0,0,0,{0},0};
AVE_FIL Openmv_theta = {10,0,0,0,0,{0},0};

static void packet_dec(void);
static float aver_filter(float data,AVE_FIL* ave);
static int16_t Limit_A_Filter(int16_t data);
OPENMV Openmv = {0};

void usart3_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; 
  USART_InitTypeDef usart3;
	NVIC_InitTypeDef nvic_USART3_dma;
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	
  USART_DeInit(USART3);  //复位串口1
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;                //USART3 RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;            
  GPIO_Init(GPIOB, &GPIO_InitStructure); 

	USART_StructInit(&usart3);
	usart3.USART_BaudRate = 115200;
	usart3.USART_WordLength = USART_WordLength_8b;
	usart3.USART_StopBits = USART_StopBits_1;
	usart3.USART_Parity = USART_Parity_No;
	usart3.USART_Mode = USART_Mode_Rx;
	usart3.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART3,&usart3);
	
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);   //使能串口1 DMA接收
	
	//相应的DMA配置
	DMA_DeInit(DMA1_Channel3);   //将DMA的通道5寄存器重设为缺省值  串口1对应的是DMA通道5
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART3->DR;  //DMA外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)buffer_UART3;  //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设读取发送到内存
	DMA_InitStructure.DMA_BufferSize = DMA_Rec_Len_3;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; //DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
	
	nvic_USART3_dma.NVIC_IRQChannel = DMA1_Channel3_IRQn;
  nvic_USART3_dma.NVIC_IRQChannelPreemptionPriority = 3;
  nvic_USART3_dma.NVIC_IRQChannelSubPriority = 0;
  nvic_USART3_dma.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_USART3_dma);

  DMA_Cmd(DMA1_Channel3, ENABLE);  //正式驱动DMA传输
	DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);  //DMA传输完成中断 ！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	USART_Cmd(USART3,ENABLE);
}

void DMA1_Channel3_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC3))
  {
		DMA_ClearFlag(DMA1_IT_TC3);
    DMA_ClearITPendingBit(DMA1_IT_TC3);
		if(buffer_UART3[0] == 0x5A	&&	buffer_UART3[1] == 0xA5)				
		{
			memcpy(buf3_xxx,buffer_UART3,DMA_Rec_Len_3);
			packet_dec();
		}else
		{
			int bit = 0;
			if(buf2_xxx[DMA_Rec_Len_3-1] == 0x5A	&&	buffer_UART3[0] == 0xA5)	
			{
				bit = DMA_Rec_Len_3-1;
			}else 
			{
				for(int i = 0;i < DMA_Rec_Len_3;i++)											
				{
					if(buf2_xxx[i] == 0x5A && buf2_xxx[i+1] == 0xA5) 
						{
							bit = i;		
							break;
						}
					}
			}
			memcpy(buf3_xxx,buf2_xxx+bit,DMA_Rec_Len_3-bit);
			memcpy(buf3_xxx+DMA_Rec_Len_3-bit,buffer_UART3,bit);
			memcpy(buf2_xxx,buffer_UART3,DMA_Rec_Len_3);
			packet_dec();
		}
	}
}

static void packet_dec(void)
{
	uint32_t sum = 0;
	for(int i = 2;i < 6;i++) sum+=buf3_xxx[i] ;
	if(buf3_xxx[6] == (sum&0xFF))
	{
		Openmv.rho = Limit_A_Filter(aver_filter(((int16_t)(buf3_xxx[2] << 8 | buf3_xxx[3])),&Openmv_rho));
		Openmv.theta = aver_filter(((int16_t)(buf3_xxx[4] << 8 | buf3_xxx[5])),&Openmv_theta);
	}
	
}

static int16_t Limit_A_Filter(int16_t data)
{
	static int16_t data_last = 0,data_now = 0;
	data_last = data_now;
	data_now = data;
	if(fabsf(data_now - data_last )< 40.0f)
		return data_last;
	else
		return data_now;

}
static float aver_filter(float data,AVE_FIL* ave)
{
	ave->data_max = -999;
	ave->data_min = 999;
	ave->sum = 0;
	if(ave->counter < ave->bf_size -1)
	{
		ave->buffer[ave->counter++] = data;
	}else 
	{
		ave->buffer[ave->counter] = data;
		ave->counter = 0;
	}
	for(int i = 0 ;i < ave->bf_size;i++)
	{
		if(ave->buffer[i] > ave->data_max) ave->data_max = ave->buffer[i];
		if(ave->buffer[i] < ave->data_min) ave->data_min = ave->buffer[i];
		ave->sum += ave->buffer[i];
	}
	ave->data_out = (ave->sum-ave->data_max-ave->data_min)/(ave->bf_size-2);
	return ave->data_out;
}