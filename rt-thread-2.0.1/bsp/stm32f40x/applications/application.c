/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2014-04-27     Bernard      make code cleanup. 
 */

#include <board.h>
#include <rtthread.h>
#include "ad7699.h"

#ifdef RT_USING_LWIP
#include <lwip/sys.h>
#include <lwip/api.h>
#include <netif/ethernetif.h>
#include "stm32_eth.h"
#endif

#ifdef RT_USING_GDB
#include <gdb_stub.h>
#endif



int max31865_value;


RCC_ClocksTypeDef  rcc_clocks;	
void rt_init_thread_entry(void* parameter)
{
    /* GDB STUB */
#ifdef RT_USING_GDB
    gdb_set_device("uart6");
    gdb_start();
#endif

    /* LwIP Initialization */
#ifdef RT_USING_LWIP
    {
        extern void lwip_sys_init(void);

        /* register ethernetif device */
        eth_system_device_init();

        rt_hw_stm32_eth_init();

        /* init lwip system */
        lwip_sys_init();
        rt_kprintf("TCP/IP initialized!\n");
    }
#endif

		GPIO_InitTypeDef  GPIO_InitStructure;
    /* GPIO Periph clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    /* Configure GPIO_InitStructure */
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;
	
    /* output setting */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
    /* input setting: not pull. */
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_4;		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    GPIO_Init(GPIOF, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_1;
		GPIO_Init(GPIOF, &GPIO_InitStructure);
		GPIO_ResetBits(GPIOF,GPIO_Pin_1);
		GPIO_ResetBits(GPIOF,GPIO_Pin_4);
		bsp_InitAD7606();
		while(1)
		{
			RCC_GetClocksFreq(&rcc_clocks);
			GPIO_SetBits(GPIOB,GPIO_Pin_2);
			rt_thread_delay(50);
			GPIO_ResetBits(GPIOB,GPIO_Pin_2);
//			AD7606_ReadNowAdc();
			rt_thread_delay(50);
		}
}

float v[16],v1[16];
volatile unsigned int t=0,t1=0;

extern unsigned short SPI_Buffer_Tx[16];
extern unsigned short SPI_Buffer_Rx[16];
unsigned short ad7699_data[16],data[16];
unsigned char length;
void ad7699_entry(void* parameter)
{
	float pro=0;															//进度
	unsigned char i,j;
	
	TIM3_Int_Init(20000-1,8400-1);						//定时1s
	
	AD7699_Init();
	SPI1_SetSpeed(SPI_BaudRatePrescaler_2);		//设置硬件SPI的速度,设置SPI1->CR1的bit[2:0]=000,SPI速度为84M/2=42M	
	
//	MYDMA_Config(DMA2_Stream2,DMA_Channel_3,(unsigned int)&SPI1->DR,(unsigned int)SPI_Buffer_Rx,DMA_DIR_PeripheralToMemory,16);			//接收
//	MYDMA_Config(DMA2_Stream3,DMA_Channel_3,(unsigned int)&SPI1->DR,(unsigned int)SPI_Buffer_Tx,DMA_DIR_MemoryToPeripheral,16);			//发送
//	
//	DMA_NVIC_Configuration();														//DMA中断优先级配置
//	DMA_ITConfig(DMA2_Stream2, DMA_IT_TC, ENABLE);			//接收完成产生中断
//		
//	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
//	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
	
	while(1)
	{	
//		length=SPI_DMA_Read(ad7699_data,16);
		for(i=0;i<8;i++)
		{
			ad7699_data[i]=AD7699_ReadWrite(CFG); 
			v[i]=ad7699_data[i]*Vref/65536;
		}
//		ad7699_data[0]=AD7699_ReadWrite(0XFD40); 
//		v[0]=ad7699_data[0]*Vref/65536;
//		ad7699_data[1]=AD7699_ReadWrite(0XFD40); 
//		v[1]=ad7699_data[1]*Vref/65536;
//		ad7699_data[2]=AD7699_ReadWrite(0XFD40); 
//		v[2]=ad7699_data[2]*Vref/65536;
//		
//		ad7699_data[3]=AD7699_ReadWrite(0X0000); 
//		v[3]=ad7699_data[3]*Vref/65536;
//		ad7699_data[4]=AD7699_ReadWrite(0X0000); 
//		v[4]=ad7699_data[4]*Vref/65536;
//		ad7699_data[5]=AD7699_ReadWrite(0X0000); 
//		v[5]=ad7699_data[5]*Vref/65536;
//实测八个通道扫描频率54k(总共计时60s,t=3226255)		
		t++;
		if(t==0xffffffff)
		{
			t=0;
			t1++;
			if(GPIOB->IDR&(1<<2))
				GPIOB->ODR|=1<<2;
			else
				GPIOB->ODR&=~(1<<2);
		}		
		rt_thread_delay(5);		
	}
}

void tem_entry(void* parameter)
{
		GPIO_InitTypeDef  GPIO_InitStructure;
	
    /* GPIO Periph clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		
		/* output setting */
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		while(1)
		{
			GPIOD->ODR|=1<<2;
			rt_thread_delay(20);
			GPIOD->ODR&=~(1<<2);
			rt_thread_delay(20);			
//			max31865_value = MAX31865_SPI_ReadWrite();
		}
}
int rt_application_init()
{
    rt_thread_t tid0,tid1,tid2;

    tid0 = rt_thread_create("init",
        rt_init_thread_entry, RT_NULL,
        512, RT_THREAD_PRIORITY_MAX/3, 20);

    if (tid0 != RT_NULL);
        rt_thread_startup(tid0);
		
		tid1 = rt_thread_create("ad7699",
        ad7699_entry, RT_NULL,
        2048, 5, 20);

    if (tid1 != RT_NULL);
 //       rt_thread_startup(tid1);
		
		tid2 = rt_thread_create("tem",
        tem_entry, RT_NULL,
        512, 5, 20);

    if (tid2 != RT_NULL)
        rt_thread_startup(tid2);
		
    return 0;
}

