#include "max31865.h"
#include "stm32f4xx.h"  
//////////////////////////////////////////////////////////////////////////////////
//延时约1us
void max31865_delay(unsigned int t)
{
	unsigned int i,j;
	for(i=0;i<25;i++)
		for(j=0;j<t;j++);
}
////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////软件模式SPI///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
#if SPI_MODE==0
int MAX31865_SPI_ReadWrite(void);

//初始化SPI FLASH的IO口
void MAX31865_Init(void)
{ 
	GPIO_InitTypeDef  max31865;

	/* GPIO Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	/* output setting */
	max31865.GPIO_Pin     = GPIO_Pin_2;
	max31865.GPIO_OType   = GPIO_OType_PP;
	max31865.GPIO_Speed   = GPIO_Speed_100MHz;    
	max31865.GPIO_Mode 		= GPIO_Mode_OUT;
	max31865.GPIO_PuPd 		= GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &max31865);

	/* output setting */
	max31865.GPIO_Pin     = GPIO_Pin_0|GPIO_Pin_1;
	max31865.GPIO_OType   = GPIO_OType_PP;
	max31865.GPIO_Speed   = GPIO_Speed_100MHz;    
	max31865.GPIO_Mode 		= GPIO_Mode_OUT;
	max31865.GPIO_PuPd 		= GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &max31865);
	
	/* input setting: not pull. */
	max31865.GPIO_Pin     = GPIO_Pin_9;
	max31865.GPIO_OType   = GPIO_OType_PP;
	max31865.GPIO_Speed   = GPIO_Speed_100MHz;
	max31865.GPIO_Mode 		= GPIO_Mode_IN;
	max31865.GPIO_PuPd 		= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &max31865);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_4|GPIO_Pin_7);												//AD7699_CS_H,AD7699_DIN_H
	GPIO_ResetBits(GPIOA,GPIO_Pin_5); 																//AD7699_CLK_L
}  

int MAX31865_SPI_ReadWrite(void)   
{
	unsigned char i;  
	int data=0;

	CS_H;
	CLK_L;
	delay(30);
	CS_L;
	delay(30);
	for(i=0;i<32;i++)
	{		
		CLK_L;
		delay(30);			
		CLK_H;
		delay(20);			
		data<<=1;
		if(SDO)
			data++;
		else
			data=0;
		delay(20);
	}
	for(i=0;i<200;i++)
		delay(1000);
	CS_H;
//	for(i=0;i<200;i++)
		delay(1000);
	return data;
} 

int MAX31865_ReadWrite(void)   
{   
  int dat;
	dat=MAX31865_SPI_ReadWrite();
	return dat;
}

////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////硬件模式SPI///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
#elif SPI_MODE==1
void MAX31865_Init(void)
{

}


unsigned short MAX31865_SPI_ReadWrite(unsigned short TxData) 
{

}

unsigned short MAX31865_ReadWrite(unsigned short cmd)   
{

}
#endif