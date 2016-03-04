#include "dma.h"	

//DMAx的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMA通道选择,@ref DMA_channel DMA_Channel_0~DMA_Channel_7
//par:外设地址
//mar:存储器地址
//ndtr:数据传输量  
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,unsigned int chx,unsigned int par,unsigned int mar,unsigned int mode,unsigned short ndtr)
{  
	DMA_InitTypeDef  DMA_InitStructure;
	
	if((unsigned int)DMA_Streamx>(unsigned int)DMA2)														//得到当前stream是属于DMA2还是DMA1
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);												//DMA2时钟使能 
		
	}else 
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);												//DMA1时钟使能 
	}
  DMA_DeInit(DMA_Streamx);
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}													//等待DMA可配置 
	
  /* 配置 DMA Stream */
  DMA_InitStructure.DMA_Channel = chx;  																			//通道选择
  DMA_InitStructure.DMA_PeripheralBaseAddr = par;															//DMA外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr = mar;																//DMA 存储器0地址
  DMA_InitStructure.DMA_DIR = mode;																						//外设到存储器模式选择
  DMA_InitStructure.DMA_BufferSize = ndtr;																		//数据传输量 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;						//外设非增量模式
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;											//存储器增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//外设数据长度:16位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;					//存储器数据长度:16位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;																// 使用普通模式 
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;												//中等优先级
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;									//存储器突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;					//外设突发单次传输
  DMA_Init(DMA_Streamx, &DMA_InitStructure);																	//初始化DMA Stream
} 
//开启一次DMA传输
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:数据传输量  
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,unsigned short ndtr)
{
 
	DMA_Cmd(DMA_Streamx, DISABLE);                      //关闭DMA传输 
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}	//确保DMA可以被设置  
		
	DMA_SetCurrDataCounter(DMA_Streamx,ndtr);          	//数据传输量  
 
	DMA_Cmd(DMA_Streamx, ENABLE);                      	//开启DMA传输 
}	 

//配置DMA中断
void DMA_NVIC_Configuration(void)
{
	NVIC_InitTypeDef   DMA_NVIC;
/* Enable and set DMA Interrupt */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	DMA_NVIC.NVIC_IRQChannel = DMA2_Stream2_IRQn;
	DMA_NVIC.NVIC_IRQChannelPreemptionPriority = 0x00;
	DMA_NVIC.NVIC_IRQChannelSubPriority = 0x00;
	DMA_NVIC.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&DMA_NVIC);
}

void DMA2_Stream2_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream2, DMA_IT_TCIF2)==SET)		   //核对是否发生DMA中断
	{
		if(DMA_GetFlagStatus(DMA2_Stream2,DMA_FLAG_TCIF2)==SET); //DMA2_Steam0传输是否完成
		{
			DMA_Cmd(DMA2_Stream2, DISABLE);                      	 //关闭DMA传输 
			DMA_Cmd(DMA2_Stream3, DISABLE);                      	 //关闭DMA传输
		}
	}
	DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);				 //清楚中断标志位
}

