#ifndef __DMA_H
#define	__DMA_H	 
#include "stm32f4xx.h"                  // Device header

void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,unsigned int chx,unsigned int par,unsigned int mar,unsigned int mode,unsigned short ndtr);//配置DMAx_CHx
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,unsigned short ndtr);	//使能一次DMA传输	
void DMA_NVIC_Configuration(void);																			//DMA中断配置
#endif






























