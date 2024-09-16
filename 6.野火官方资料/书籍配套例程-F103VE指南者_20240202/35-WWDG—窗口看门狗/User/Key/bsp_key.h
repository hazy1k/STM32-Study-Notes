#ifndef __BSP_KEY_H
#define	__BSP_KEY_H


#include "stm32f10x.h"

// ʹ��KEY1�����Ҫʹ��KEY2�Ļ����Ѻ�KEY1_PA0ע�͵�����
#define   KEYI_PA0

#ifdef    KEYI_PA0
#define               macKEY1_GPIO_CLK                      RCC_APB2Periph_GPIOA
#define               macKEY1_GPIO_PORT    	                GPIOA			   
#define               macKEY1_GPIO_PIN		                  GPIO_Pin_0

#else    //KEY2_PC13
#define               macKEY1_GPIO_CLK                      RCC_APB2Periph_GPIOC
#define               macKEY1_GPIO_PORT    	                GPIOC		   
#define               macKEY1_GPIO_PIN		                  GPIO_Pin_13

#endif

 /*******
 *�������±��ã�����K1��K2Ĭ���ǵ͵�ƽ���������µ�ʱ���Ǹߵ�ƽ
 KEY_ON 0
 KEY_OFF 1
 ********/
#define KEY_ON	1
#define KEY_OFF	0

void SOFT_Delay(__IO u32 nCount);
void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);


#endif /* __BSP_KEY_H */

