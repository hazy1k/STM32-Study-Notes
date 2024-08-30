#ifndef __KEY_H
#define	__KEY_H

#include "stm32f4xx.h"

//���Ŷ���
/*******************************************************/
#define KEY1_PIN                  GPIO_Pin_0                 
#define KEY1_GPIO_PORT            GPIOA                      
#define KEY1_GPIO_CLK             RCC_AHB1Periph_GPIOA

#define KEY2_PIN                  GPIO_Pin_13                 
#define KEY2_GPIO_PORT            GPIOC                      
#define KEY2_GPIO_CLK             RCC_AHB1Periph_GPIOC

#define KEY3_PIN                  GPIO_Pin_2                 
#define KEY3_GPIO_PORT            GPIOG                     
#define KEY3_GPIO_CLK             RCC_AHB1Periph_GPIOG

#define KEY4_PIN                  GPIO_Pin_3                 
#define KEY4_GPIO_PORT            GPIOG                      
#define KEY4_GPIO_CLK             RCC_AHB1Periph_GPIOG

#define KEY5_PIN                  GPIO_Pin_4                 
#define KEY5_GPIO_PORT            GPIOG                    
#define KEY5_GPIO_CLK             RCC_AHB1Periph_GPIOG
/*******************************************************/

 /** �������±��ú�
	* ��������Ϊ�ߵ�ƽ������ KEY_ON=1�� KEY_OFF=0
	* ����������Ϊ�͵�ƽ���Ѻ����ó�KEY_ON=0 ��KEY_OFF=1 ����
	*/
#define KEY_ON	1
#define KEY_OFF	0

void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin);

#endif /* __LED_H */

