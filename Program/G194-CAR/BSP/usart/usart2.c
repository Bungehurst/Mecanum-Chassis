#include "usart2.h"
#include <string.h>
#include "delay.h"
#include "timer.h"

#define abs(x) ((x>0)?x:(-x))
#define DMA_Rec_Len_2 25

static uint8_t buffer_UART2[DMA_Rec_Len_2];
uint8_t buf3_xxx[DMA_Rec_Len_2];
static uint8_t buf2_xxx[DMA_Rec_Len_2];

RC_DATA rc_data = {0};

static void packet_dec(void);

void usart2_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; 
  USART_InitTypeDef usart2;
	NVIC_InitTypeDef nvic_USART2_dma;
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	
  USART_DeInit(USART2);  //复位串口1
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;                //USART1 RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;            
  GPIO_Init(GPIOA, &GPIO_InitStructure); 

	USART_StructInit(&usart2);
	usart2.USART_BaudRate = 100e3;
	usart2.USART_WordLength = USART_WordLength_8b;
	usart2.USART_StopBits = USART_StopBits_2;
	usart2.USART_Parity = USART_Parity_Even;
	usart2.USART_Mode = USART_Mode_Rx;
	usart2.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART2,&usart2);
	
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);   //使能串口1 DMA接收
	
	//相应的DMA配置
	DMA_DeInit(DMA1_Channel6);   //将DMA的通道5寄存器重设为缺省值  串口1对应的是DMA通道5
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;  //DMA外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)buffer_UART2;  //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设读取发送到内存
	DMA_InitStructure.DMA_BufferSize = DMA_Rec_Len_2;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; //DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
	
	nvic_USART2_dma.NVIC_IRQChannel = DMA1_Channel6_IRQn;
  nvic_USART2_dma.NVIC_IRQChannelPreemptionPriority = 2;
  nvic_USART2_dma.NVIC_IRQChannelSubPriority = 0;
  nvic_USART2_dma.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_USART2_dma);

  DMA_Cmd(DMA1_Channel6, ENABLE);  //正式驱动DMA传输
	DMA_ITConfig(DMA1_Channel6,DMA_IT_TC,ENABLE);  //DMA传输完成中断 ！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	USART_Cmd(USART2,ENABLE);
}

void DMA1_Channel6_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC6))
  {
		DMA_ClearFlag(DMA1_IT_TC6);
    DMA_ClearITPendingBit(DMA1_IT_TC6);

		if(buffer_UART2[0]==0x0F	&&	buffer_UART2[15]==0x00)				
		{
			memcpy(buf3_xxx,buffer_UART2,DMA_Rec_Len_2);
			packet_dec();
		}else
		{
			int bit = 0;
			if(buf2_xxx[DMA_Rec_Len_2-1]==0x0F	&&	buffer_UART2[DMA_Rec_Len_2 - 2]==0x00)	
			{
				bit = DMA_Rec_Len_2-1;
			}else 
			{
				for(int i = 0;i < DMA_Rec_Len_2;i++)											
				{
					if(buf2_xxx[i]==0x0F) 
						{
							bit = i;		
							break;
						}
					}
			}
			memcpy(buf3_xxx,buf2_xxx+bit,DMA_Rec_Len_2-bit);
			memcpy(buf3_xxx+DMA_Rec_Len_2-bit,buffer_UART2,bit);
			memcpy(buf2_xxx,buffer_UART2,DMA_Rec_Len_2);
			packet_dec();
		}
	}
}
uint32_t time_last,time_now,time_valid = 0,rc_valid = 0,time_err ;
static void packet_dec(void)
{
	time_last = time_now;
	time_now = time_cnt;
	time_err = abs(time_now - time_last);
	if(time_err < 10) time_valid = 1;
	else time_valid = 0;

	rc_data.Channel_Raw[0]  = ((buf3_xxx[1]	   |buf3_xxx[2]<<8)           			& 0x07FF);
	rc_data.Channel_Raw[1]  = ((buf3_xxx[2]>>3 |buf3_xxx[3]<<5)                 & 0x07FF);
	rc_data.Channel_Raw[2]  = ((buf3_xxx[3]>>6 |buf3_xxx[4]<<2 |buf3_xxx[5]<<10)  & 0x07FF);
	rc_data.Channel_Raw[3]  = ((buf3_xxx[5]>>1 |buf3_xxx[6]<<7)                 & 0x07FF);
	rc_data.Channel_Raw[4]  = ((buf3_xxx[6]>>4 |buf3_xxx[7]<<4)                 & 0x07FF);
	rc_data.Channel_Raw[5]  = ((buf3_xxx[7]>>7 |buf3_xxx[8]<<1 |buf3_xxx[9]<<9)   & 0x07FF);
	rc_data.Channel_Raw[6]  = ((buf3_xxx[9]>>2 |buf3_xxx[10]<<6)                & 0x07FF);
	rc_data.Channel_Raw[7]  = ((buf3_xxx[10]>>5|buf3_xxx[11]<<3)                & 0x07FF);
	rc_data.Channel_Raw[8]  = ((buf3_xxx[12]   |buf3_xxx[13]<<8)                & 0x07FF);
	rc_data.Channel_Raw[9]  = ((buf3_xxx[13]>>3|buf3_xxx[14]<<5)                & 0x07FF);
	rc_data.Channel_Raw[10] = ((buf3_xxx[14]>>6|buf3_xxx[15]<<2|buf3_xxx[16]<<10) & 0x07FF);
	rc_data.Channel_Raw[11] = ((buf3_xxx[16]>>1|buf3_xxx[17]<<7)                & 0x07FF);
	rc_data.Channel_Raw[12] = ((buf3_xxx[17]>>4|buf3_xxx[18]<<4)                & 0x07FF);
	rc_data.Channel_Raw[13] = ((buf3_xxx[18]>>7|buf3_xxx[19]<<1|buf3_xxx[20]<<9)  & 0x07FF);
	rc_data.Channel_Raw[14] = ((buf3_xxx[20]>>2|buf3_xxx[21]<<6)                & 0x07FF);
	rc_data.Channel_Raw[15] = ((buf3_xxx[21]>>5|buf3_xxx[22]<<3)                & 0x07FF);
	
	if(rc_data.Channel_Raw[0] == 1001 && rc_data.Channel_Raw[1] == 1001 && rc_data.Channel_Raw[2] == 41 && rc_data.Channel_Raw[3] == 1001)
	{
		rc_valid = 0;
	}else rc_valid = 1;
	if(time_valid == 1 && rc_valid == 1)
	{
		for(int i = 0;i < 16; i++)
		{
			rc_data.Channel_Current[i] = rc_data.Channel_Raw[i] - rc_data.Channel_Zero[i];
		}	
//		rc_data.Channel_Current[0] = rc_data.Channel_Raw[0] - 993;
//		rc_data.Channel_Current[1] = rc_data.Channel_Raw[0] - 881;
//		rc_data.Channel_Current[2] = rc_data.Channel_Raw[0] - 1070;
//		rc_data.Channel_Current[3] = rc_data.Channel_Raw[0] - 921;
//		rc_data.Channel_Current[4] = rc_data.Channel_Raw[0] - 993;
//		rc_data.Channel_Current[5] = rc_data.Channel_Raw[0] - 1801;
//		rc_data.Channel_Current[6] = rc_data.Channel_Raw[0] - 1801;
	}else 
	{
		for(int i = 0;i < 16; i++)
		{
			rc_data.Channel_Current[i] = 0;
		}	
	}

	
}

void Calib_rcdata_Zero(void)
{
	static uint32_t Last_Data[16] = {0};
	delay_ms(1000);
	for(int i = 0 ; i < 10;i++)
	{
		Last_Data[0] += rc_data.Channel_Raw[0];
		Last_Data[1] += rc_data.Channel_Raw[1];
		Last_Data[2] += rc_data.Channel_Raw[2];
		Last_Data[3] += rc_data.Channel_Raw[3];
		Last_Data[4] += rc_data.Channel_Raw[4];
		Last_Data[5] += rc_data.Channel_Raw[5];
		Last_Data[6] += rc_data.Channel_Raw[6];
		Last_Data[7] += rc_data.Channel_Raw[7];
		Last_Data[8] += rc_data.Channel_Raw[8];
		Last_Data[9] += rc_data.Channel_Raw[9];
		Last_Data[10] += rc_data.Channel_Raw[10];
		Last_Data[11] += rc_data.Channel_Raw[11];
		Last_Data[12] += rc_data.Channel_Raw[12];
		Last_Data[13] += rc_data.Channel_Raw[13];
		Last_Data[14] += rc_data.Channel_Raw[14];
		Last_Data[15] += rc_data.Channel_Raw[15];
		
		delay_ms(10);
	}
	
	rc_data.Channel_Zero[0] = Last_Data[0]/10;
	rc_data.Channel_Zero[1] = Last_Data[1]/10;
	rc_data.Channel_Zero[2] = Last_Data[2]/10;
	rc_data.Channel_Zero[3] = Last_Data[3]/10;
	rc_data.Channel_Zero[4] = Last_Data[4]/10;
	rc_data.Channel_Zero[5] = Last_Data[5]/10;
	rc_data.Channel_Zero[6] = Last_Data[6]/10;
	rc_data.Channel_Zero[7] = Last_Data[7]/10;
	rc_data.Channel_Zero[8] = Last_Data[8]/10;
	rc_data.Channel_Zero[9] = Last_Data[9]/10;
	rc_data.Channel_Zero[10] = Last_Data[10]/10;
	rc_data.Channel_Zero[11] = Last_Data[11]/10;
	rc_data.Channel_Zero[12] = Last_Data[12]/10;
	rc_data.Channel_Zero[13] = Last_Data[13]/10;
	rc_data.Channel_Zero[14] = Last_Data[14]/10;
	rc_data.Channel_Zero[15] = Last_Data[15]/10;

}



	
