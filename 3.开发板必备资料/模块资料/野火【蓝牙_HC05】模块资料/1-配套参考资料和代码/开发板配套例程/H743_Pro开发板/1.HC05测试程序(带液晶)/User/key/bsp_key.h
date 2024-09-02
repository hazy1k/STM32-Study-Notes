#ifndef __EXTI_H
#define	__EXTI_H

#include "stm32h7xx.h"

//���Ŷ���
/*******************************************************/
#define KEY1_GPIO_PORT                GPIOA
#define KEY1_INT_GPIO_CLK_ENABLE()        __GPIOA_CLK_ENABLE();
#define KEY1_PIN                         GPIO_PIN_0


#define KEY2_GPIO_PORT                GPIOC
#define KEY2_INT_GPIO_CLK_ENABLE()        __GPIOC_CLK_ENABLE();
#define KEY2_PIN                          GPIO_PIN_13



/*******************************************************/


 /** �������±��ú�
	* ��������Ϊ�ߵ�ƽ������ KEY_ON=1�� KEY_OFF=0
	* ����������Ϊ�͵�ƽ���Ѻ����ó�KEY_ON=0 ��KEY_OFF=1 ����
	*/
#define KEY_ON	1
#define KEY_OFF	0

void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);

#endif /* __EXTI_H */
