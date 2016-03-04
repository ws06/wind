#include "stm32_timer.h"
#include "stm32f4xx.h"                  // Device header

extern float v[8];
extern unsigned int t;
extern unsigned short ad7699_data[16];
unsigned int tim;
//定时器3中断服务程序	 
void TIM3_IRQHandler(void)
{
	unsigned char i;
	if(TIM3->SR&0X0001)//溢出中断
	{	
		tim	=t;
		t=0;
	}	
//	if(tim==0xffffffff)
//		tim=0;
	TIM3->SR&=~(1<<0);//清除中断标志位 	    
}
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为42M
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM3_Int_Init(unsigned short arr,unsigned short psc)
{	
	NVIC_InitTypeDef TIM3_NVIC;
	
	RCC->APB1ENR|=1<<1;	//TIM3时钟使能    
 	TIM3->ARR=arr;  		//设定计数器自动重装值 
	TIM3->PSC=psc;  		//预分频器	  
	TIM3->DIER|=1<<0;   //允许更新中断	  
	TIM3->CR1|=0x01;    //使能定时器3
	
	TIM3_NVIC.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	TIM3_NVIC.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	TIM3_NVIC.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	TIM3_NVIC.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&TIM3_NVIC);  	
}














