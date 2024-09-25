#ifndef __BSP_XPT2046_LCD_H
#define	__BSP_XPT2046_LCD_H


#include "stm32f10x.h"



/******************************* XPT2046 �������ж����Ŷ��� ***************************/
#define             macXPT2046_INT_GPIO_CLK                        RCC_APB2Periph_GPIOE   
#define             macXPT2046_INT_GPIO_PORT                       GPIOE
#define             macXPT2046_INT_GPIO_PIN                        GPIO_Pin_4
#define             macXPT2046_INT_Read()                          GPIO_ReadInputDataBit ( macXPT2046_INT_GPIO_PORT, macXPT2046_INT_GPIO_PIN )
#define 						macXPT2046_INT_ActiveLevel											0


/******************************* XPT2046 ������ģ��SPI���Ŷ��� ***************************/
#define             macXPT2046_SPI_GPIO_CLK                         RCC_APB2Periph_GPIOE| RCC_APB2Periph_GPIOD

#define             macXPT2046_SPI_CS_PIN		                        GPIO_Pin_13
#define             macXPT2046_SPI_CS_PORT		                      GPIOD

#define	            macXPT2046_SPI_CLK_PIN	                        GPIO_Pin_0
#define             macXPT2046_SPI_CLK_PORT	                        GPIOE

#define	            macXPT2046_SPI_MOSI_PIN	                        GPIO_Pin_2
#define	            macXPT2046_SPI_MOSI_PORT	                      GPIOE

#define	            macXPT2046_SPI_MISO_PIN	                        GPIO_Pin_3
#define	            macXPT2046_SPI_MISO_PORT	                      GPIOE


#define             macXPT2046_CS_ENABLE()                          GPIO_ResetBits ( macXPT2046_SPI_CS_PORT, macXPT2046_SPI_CS_PIN )    
#define             macXPT2046_CS_DISABLE()                         GPIO_SetBits   ( macXPT2046_SPI_CS_PORT, macXPT2046_SPI_CS_PIN )  

#define             macXPT2046_CLK_HIGH()                           GPIO_SetBits   ( macXPT2046_SPI_CLK_PORT, macXPT2046_SPI_CLK_PIN )    
#define             macXPT2046_CLK_LOW()                            GPIO_ResetBits ( macXPT2046_SPI_CLK_PORT, macXPT2046_SPI_CLK_PIN ) 

#define             macXPT2046_MOSI_1()                             GPIO_SetBits   ( macXPT2046_SPI_MOSI_PORT, macXPT2046_SPI_MOSI_PIN ) 
#define             macXPT2046_MOSI_0()                             GPIO_ResetBits ( macXPT2046_SPI_MOSI_PORT, macXPT2046_SPI_MOSI_PIN )

#define             macXPT2046_MISO()                               GPIO_ReadInputDataBit ( macXPT2046_SPI_MISO_PORT, macXPT2046_SPI_MISO_PIN )



/******************************* XPT2046 �������������� ***************************/
#define	            macXPT2046_CHANNEL_X 	                          0x90 	          //ͨ��Y+��ѡ�������	
#define	            macXPT2046_CHANNEL_Y 	                          0xd0	          //ͨ��X+��ѡ�������




/******************************** XPT2046 �������������� **********************************/
void     XPT2046_Init( void );
uint16_t XPT2046_ReadAdc_Fliter(uint8_t channel); 


#endif /* __BSP_TOUCH_H */

