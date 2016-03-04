#ifndef __MAX31865_H
#define __MAX31865_H

//使用软件模拟SPI还是硬件SPI
#define SPI_MODE 		0

#define CS_H				GPIOC->ODR|=1<<0
#define CS_L				GPIOC->ODR&=~(1<<0)
#define CLK_H				GPIOC->ODR|=1<<1
#define CLK_L				GPIOC->ODR&=~(1<<1)
#define SDO					GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9)

void MAX31865_Init(void);
int MAX31865_ReadWrite(void);

#endif