#ifndef __ad7699_H
#define __ad7699_H

//使用软件模拟SPI还是硬件SPI
#define SPI_MODE 		1

//AD7699相关寄存器配置
#define CFG 					0xFFEC//外部基准源,输入参考GND,8个通道循环扫描
#define CFG1 					0xEF2C//内部基准源,输入参考COM,8个通道循环扫描
#define AD7699_CFG1 	0xF140//外部基准源,输入参考GND,通道1
#define AD7699_CFG2 	0xF340//外部基准源,输入参考GND,通道2
#define AD7699_CFG3 	0xF540//外部基准源,输入参考GND,通道3
#define AD7699_CFG4 	0xF740//外部基准源,输入参考GND,通道4
#define AD7699_CFG5 	0xF940//外部基准源,输入参考GND,通道5
#define AD7699_CFG6 	0xFB40//外部基准源,输入参考GND,通道6
#define AD7699_CFG7 	0xFD40//外部基准源,输入参考GND,通道7
#define AD7699_CFG8 	0xFF40//外部基准源,输入参考GND,通道8


//AD7699基准电压源
#define Vref 		5.000
#define Vref1 	4.096

#define AD7699_CS_H				GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define AD7699_CS_L				GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define AD7699_CLK_H			GPIO_SetBits(GPIOA,GPIO_Pin_5)
#define AD7699_CLK_L			GPIO_ResetBits(GPIOA,GPIO_Pin_5)
#define AD7699_DIN_H			GPIO_SetBits(GPIOA,GPIO_Pin_7)
#define AD7699_DIN_L			GPIO_ResetBits(GPIOA,GPIO_Pin_7)
#define AD7699_DOUT				GPIOA->IDR&(1<<6)
//#define AD7699_DOUT				GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)

void AD7699_Init(void);
unsigned short SPI_ReadWrite(unsigned short cmd); 
unsigned short AD7699_ReadWrite(unsigned short cmd); 
void SPI1_SetSpeed(unsigned char SPI_BaudRatePrescaler);

#endif