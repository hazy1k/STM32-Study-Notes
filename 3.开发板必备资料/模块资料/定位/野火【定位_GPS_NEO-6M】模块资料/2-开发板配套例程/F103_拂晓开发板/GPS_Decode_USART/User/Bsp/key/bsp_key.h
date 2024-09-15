#ifndef __KEY_H
#define	__KEY_H


#include "stm32f10x.h"

//  ���Ŷ���
#define    KEY1_GPIO_CLK     RCC_APB2Periph_GPIOB
#define    KEY1_GPIO_PORT    GPIOB
#define    KEY1_GPIO_PIN		 GPIO_Pin_11

#define    KEY2_GPIO_CLK     RCC_APB2Periph_GPIOD
#define    KEY2_GPIO_PORT    GPIOD	   
#define    KEY2_GPIO_PIN		  GPIO_Pin_2

#define    KEY3_GPIO_CLK     RCC_APB2Periph_GPIOD
#define    KEY3_GPIO_PORT    GPIOD
#define    KEY3_GPIO_PIN		  GPIO_Pin_3

#define    KEY4_GPIO_CLK     RCC_APB2Periph_GPIOD
#define    KEY4_GPIO_PORT    GPIOD   
#define    KEY4_GPIO_PIN		  GPIO_Pin_4

 /** �������±��ú�
	*  ��������Ϊ�ߵ�ƽ������ KEY_ON=1�� KEY_OFF=0
	*  ����������Ϊ�͵�ƽ���Ѻ����ó�KEY_ON=0 ��KEY_OFF=1 ����
	*/
#define KEY_ON	1
#define KEY_OFF	0

void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);


#endif /* __KEY_H */

