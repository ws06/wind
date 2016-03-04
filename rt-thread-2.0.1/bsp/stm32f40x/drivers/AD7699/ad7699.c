#include "ad7699.h"
#include "dma.h"	
#include "stm32f4xx.h"                  // Device header
//////////////////////////////////////////////////////////////////////////////////
//ÑÓÊ±Ô¼1us
void delay(unsigned int t)
{
	unsigned int i,j;
	for(i=0;i<25;i++)
		for(j=0;j<t;j++);
}
////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Èí¼þÄ£Ê½SPI///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
#if SPI_MODE==0
//³õÊ¼»¯SPI FLASHµÄIO¿Ú
void AD7699_Init(void)
{ 
	GPIO_InitTypeDef  AD7699_IO;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	AD7699_IO.GPIO_Pin     = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7;
	AD7699_IO.GPIO_OType   = GPIO_OType_PP;
	AD7699_IO.GPIO_Speed   = GPIO_Speed_100MHz;
	AD7699_IO.GPIO_Mode 	 = GPIO_Mode_OUT;
	AD7699_IO.GPIO_PuPd 	 = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &AD7699_IO);	
	
	AD7699_IO.GPIO_Pin     = GPIO_Pin_6;
	AD7699_IO.GPIO_OType   = GPIO_OType_OD;
  AD7699_IO.GPIO_Mode 	 = GPIO_Mode_IN;
  AD7699_IO.GPIO_PuPd 	 = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &AD7699_IO);	
	
	GPIO_SetBits(GPIOA,GPIO_Pin_4|GPIO_Pin_7);												//AD7699_CS_H,AD7699_DIN_H
	GPIO_ResetBits(GPIOA,GPIO_Pin_5); 																//AD7699_CLK_L
	
//»Ö¸´AD7699Ïà¹Ø¼Ä´æÆ÷Îª³ö³§ÉèÖÃ	
	AD7699_ReadWrite(0xFFFF);
	AD7699_ReadWrite(0xFFFF);
}  

unsigned short SPI_ReadWrite(unsigned short cmd)   
{
	unsigned char i;  
	unsigned short data;

	AD7699_CS_H;
	AD7699_CLK_L;
//	delay(3);
	AD7699_CS_L;
//	delay(3);
	for(i=0;i<16;i++)
	{		
		AD7699_CLK_L;
		
		if(cmd&0x8000)
			AD7699_DIN_H;
		else
			AD7699_DIN_L;
		cmd<<=1;		
//		delay(3);	
		
		AD7699_CLK_H;
//		delay(3);	
		
		data<<=1;
		if(AD7699_DOUT)
			data++;
	}
//	delay(1);
	AD7699_CS_H;
//	delay(1);
	return data;
} 

unsigned short AD7699_ReadWrite(unsigned short cmd)   
{   
  unsigned short dat;
	dat=SPI_ReadWrite(cmd);
	return dat;
}

////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Ó²¼þÄ£Ê½SPI///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
#elif SPI_MODE==1
void AD7699_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef  	SPI_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);						  	//Ê¹ÄÜGPIOAÊ±ÖÓ
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,  ENABLE);							  //Ê¹ÄÜSPI1Ê±ÖÓ
	
	GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode 	 	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd 	 	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
  //GPIOA5,6,7³õÊ¼»¯ÉèÖÃ
  GPIO_InitStructure.GPIO_Pin 		= GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;	//PA5~7¸´ÓÃ¹¦ÄÜÊä³ö	
  GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_AF;											//¸´ÓÃ¹¦ÄÜ
  GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;										//ÍÆÍìÊä³ö
  GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;								//100MHz
  GPIO_InitStructure.GPIO_PuPd 		= GPIO_PuPd_UP;											//ÉÏÀ­
  GPIO_Init(GPIOA, &GPIO_InitStructure);															//³õÊ¼»¯
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_SPI1); 							//PA4¸´ÓÃÎª SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1); 							//PA5¸´ÓÃÎª SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1); 							//PA6¸´ÓÃÎª SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1); 							//PA7¸´ÓÃÎª SPI1
 
	//ÕâÀïÖ»Õë¶ÔSPI¿Ú³õÊ¼»¯
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);									//¸´Î»SPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);								//Í£Ö¹¸´Î»SPI1

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //ÉèÖÃSPIµ¥Ïò»òÕßË«ÏòµÄÊý¾ÝÄ£Ê½:SPIÉèÖÃÎªË«ÏßË«ÏòÈ«Ë«¹¤
	SPI_InitStructure.SPI_Mode 			= SPI_Mode_Master;									//ÉèÖÃSPI¹¤×÷Ä£Ê½:ÉèÖÃÎªÖ÷SPI
	SPI_InitStructure.SPI_DataSize 	= SPI_DataSize_16b;									//ÉèÖÃSPIµÄÊý¾Ý´óÐ¡:SPI·¢ËÍ½ÓÊÕ16Î»Ö¡½á¹¹
	SPI_InitStructure.SPI_CPOL 			= SPI_CPOL_Low;											//´®ÐÐÍ¬²½Ê±ÖÓµÄ¿ÕÏÐ×´Ì¬Îª¸ßµçÆ½
	SPI_InitStructure.SPI_CPHA 			= SPI_CPHA_2Edge;										//´®ÐÐÍ¬²½Ê±ÖÓµÄµÚ¶þ¸öÌø±äÑØ£¨ÉÏÉý»òÏÂ½µ£©Êý¾Ý±»²ÉÑù
	SPI_InitStructure.SPI_NSS 			= SPI_NSS_Soft;											//NSSÐÅºÅÓÉÓ²¼þ£¨NSS¹Ü½Å£©»¹ÊÇÈí¼þ£¨Ê¹ÓÃSSIÎ»£©¹ÜÀí:ÄÚ²¿NSSÐÅºÅÓÐSSIÎ»¿ØÖÆ
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//¶¨Òå²¨ÌØÂÊÔ¤·ÖÆµµÄÖµ:²¨ÌØÂÊÔ¤·ÖÆµÖµÎª256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;									//Ö¸¶¨Êý¾Ý´«Êä´ÓMSBÎ»»¹ÊÇLSBÎ»¿ªÊ¼:Êý¾Ý´«Êä´ÓMSBÎ»¿ªÊ¼
//	SPI_InitStructure.SPI_CRCPolynomial = 7;														//CRCÖµ¼ÆËãµÄ¶àÏîÊ½
	SPI_Init(SPI1, &SPI_InitStructure);  																//¸ù¾ÝSPI_InitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯ÍâÉèSPIx¼Ä´æÆ÷
 
//  SPI_SSOutputCmd(SPI1,ENABLE);
	SPI_Cmd(SPI1, ENABLE); //Ê¹ÄÜSPIÍâÉè
	
//»Ö¸´AD7699Ïà¹Ø¼Ä´æÆ÷Îª³ö³§ÉèÖÃ	
	AD7699_ReadWrite(0xFFFF);
	AD7699_ReadWrite(0xFFFF);
}

void SPI1_SetSpeed(unsigned char SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler)); 	 	//ÅÐ¶ÏÓÐÐ§ÐÔ
	SPI1->CR1&=0XFFC7;																									//Î»3-5ÇåÁã£¬ÓÃÀ´ÉèÖÃ²¨ÌØÂÊ
	SPI1->CR1|=SPI_BaudRatePrescaler;																		//ÉèÖÃSPI1ËÙ¶È 
	SPI_Cmd(SPI1,ENABLE); 																							//Ê¹ÄÜSPI1
} 

unsigned short SPI_ReadWrite(unsigned short TxData) 
{
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);			//µÈ´ý·¢ËÍÇø¿Õ  
	
	SPI_I2S_SendData(SPI1, TxData); 	//Í¨¹ýÍâÉèSPIx·¢ËÍÁ½¸öbyteÊý¾Ý
		
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 		//µÈ´ý½ÓÊÕÍêÁ½¸öbyte  
 
	return SPI_I2S_ReceiveData(SPI1); //·µ»ØÍ¨¹ýSPIx×î½ü½ÓÊÕµÄÊý¾Ý		
}

unsigned short AD7699_ReadWrite(unsigned short cmd)   
{
	unsigned short data;
  AD7699_CS_H;
	AD7699_CS_L;
	data=SPI_ReadWrite(cmd);
	AD7699_CS_H;
	return data;
}

////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////DMAÄ£Ê½///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
unsigned short SPI_Buffer_Tx[16];
unsigned short SPI_Buffer_Rx[16];

void SPI1_ReceiveSendByte( unsigned short num )
{	
		MYDMA_Enable(DMA2_Stream2,16);     										//¿ªÊ¼Ò»´ÎDMA´«Êä£
		MYDMA_Enable(DMA2_Stream3,16);     										//¿ªÊ¼Ò»´ÎDMA´«Êä£
	
		DMA_ClearFlag(DMA2_Stream2,DMA_FLAG_TCIF2);						//Çå³ýDMA2_Steam2´«ÊäÍê³É±êÖ¾
		DMA_ClearFlag(DMA2_Stream3,DMA_FLAG_TCIF3);						//Çå³ýDMA2_Steam3´«ÊäÍê³É±êÖ¾

    SPI1->DR ;																						//¶ÁÒ»´ÎÊý£¬Çå¿ÕSPI->DR

		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);//µÈ´ý·¢ËÍÇø¿Õ  
	
    DMA_Cmd(DMA2_Stream2, ENABLE);                      	//¿ªÆôDMA´«Êä 
    DMA_Cmd(DMA2_Stream3, ENABLE);                      	//¿ªÆôDMA´«Êä  
	
//		while(!DMA_GetFlagStatus(DMA2_Stream2,DMA_FLAG_TCIF2));//µÈ´ýDMA2_Steam0´«ÊäÍê³É

//		DMA_Cmd(DMA2_Stream2, DISABLE);                      	//¹Ø±ÕDMA´«Êä 
//    DMA_Cmd(DMA2_Stream3, DISABLE);                      	//¹Ø±ÕDMA´«Êä
}

unsigned char SPI_DMA_Read(unsigned short* data,unsigned char bytesNumber)
{
    unsigned char byte = 0;
    for(byte = 0; byte < bytesNumber; byte++)
    {
       SPI_Buffer_Tx[byte]=0xFE4C;
    }
    AD7699_CS_H;
		AD7699_CS_L;
    SPI1_ReceiveSendByte(bytesNumber);
    AD7699_CS_H;
    for(byte = 0; byte < bytesNumber; byte++)
    {
       data[byte]=SPI_Buffer_Rx[byte];
    }		
    return bytesNumber;
}
#endif


