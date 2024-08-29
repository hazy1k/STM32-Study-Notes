#ifndef __KEY_H
#define	__KEY_H

#include "stm32f1xx.h"
#include "main.h"
//���Ŷ���
/*******************************************************/
#define KEY1_PIN                  GPIO_PIN_11                
#define KEY1_GPIO_PORT            GPIOB                      
#define KEY1_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()

#define KEY2_PIN                  GPIO_PIN_2                 
#define KEY2_GPIO_PORT            GPIOD                 
#define KEY2_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOD_CLK_ENABLE()

#define KEY3_PIN                  GPIO_PIN_3            
#define KEY3_GPIO_PORT            GPIOD                      
#define KEY3_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOD_CLK_ENABLE()

#define KEY4_PIN                  GPIO_PIN_4             
#define KEY4_GPIO_PORT            GPIOD                      
#define KEY4_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOD_CLK_ENABLE()
/*******************************************************/

 /** �������±��ú�
	* ��������Ϊ�ߵ�ƽ������ KEY_ON=1�� KEY_OFF=0
	* ����������Ϊ�͵�ƽ���Ѻ����ó�KEY_ON=0 ��KEY_OFF=1 ����
	*/
#define KEY_ON	1
#define KEY_OFF	0

void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);

#endif /* __LED_H */

