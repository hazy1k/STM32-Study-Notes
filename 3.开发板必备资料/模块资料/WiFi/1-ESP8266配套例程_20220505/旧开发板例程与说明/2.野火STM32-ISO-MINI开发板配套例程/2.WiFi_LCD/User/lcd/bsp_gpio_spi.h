#ifndef __BSP_GPIO_SPI_H
#define	__BSP_GPIO_SPI_H

#include "stm32f10x.h"



// PA6 PA3 PA12 PE5 PE5 PD12 - PD6(x)
#define	 SPI_MISO_PIN	  GPIO_Pin_3
#define	 SPI_MISO_PORT	GPIOE

// PA7 PA2 PA11 PE6 PE2 PD6  - PE2(x) 
#define	 SPI_MOSI_PIN	  GPIO_Pin_2
#define	 SPI_MOSI_PORT	GPIOE

// PA5 - PE0
#define	 SPI_CLK_PIN	  GPIO_Pin_0
#define  SPI_CLK_PORT	  GPIOE

// PB7 - PE3
#define  SPI_CS_PIN		  GPIO_Pin_13
#define  SPI_CS_PORT		GPIOD

// PB6 - PE4
#define  TP_INT_PIN	    GPIO_Pin_4
#define  TP_INT_PORT	  GPIOE
#define  INT_IN_2046    GPIO_ReadInputDataBit(TP_INT_PORT,TP_INT_PIN)

// ´¥ÃþÆÁ¼ò³Æ TP
#define TP_DCLK(a)	\
						if (a)	\
						GPIO_SetBits(SPI_CLK_PORT,SPI_CLK_PIN);	\
						else		\
						GPIO_ResetBits(SPI_CLK_PORT,SPI_CLK_PIN)
#define TP_CS(a)	\
						if (a)	\
						GPIO_SetBits(SPI_CS_PORT,SPI_CS_PIN);	\
						else		\
						GPIO_ResetBits(SPI_CS_PORT,SPI_CS_PIN)
#define TP_DIN(a)	\
						if (a)	\
						GPIO_SetBits(SPI_MISO_PORT,SPI_MOSI_PIN);	\
						else		\
						GPIO_ResetBits(SPI_MISO_PORT,SPI_MOSI_PIN)

#define TP_DOUT		 GPIO_ReadInputDataBit(SPI_MISO_PORT,SPI_MISO_PIN)


            
void GPIO_SPI_Config(void);

#endif /* __BSP_GPIO_SPI_H */
