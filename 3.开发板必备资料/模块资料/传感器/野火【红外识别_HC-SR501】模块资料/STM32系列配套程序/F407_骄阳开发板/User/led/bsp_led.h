#ifndef __LED_H
#define	__LED_H

#include "stm32f4xx.h"

//引脚定义
/*******************************************************/
#define LED1_PIN                  GPIO_Pin_15                 
#define LED1_GPIO_PORT            GPIOA                    
#define LED1_GPIO_CLK             RCC_AHB1Periph_GPIOA

#define LED2_PIN                  GPIO_Pin_15                 
#define LED2_GPIO_PORT            GPIOG                      
#define LED2_GPIO_CLK             RCC_AHB1Periph_GPIOG

#define LED3_PIN                  GPIO_Pin_8                 
#define LED3_GPIO_PORT            GPIOB                       
#define LED3_GPIO_CLK             RCC_AHB1Periph_GPIOB

#define LED4_PIN                  GPIO_Pin_2                 
#define LED4_GPIO_PORT            GPIOE                     
#define LED4_GPIO_CLK             RCC_AHB1Periph_GPIOE
/************************************************************/


/** 控制LED灯亮灭的宏，
	* LED低电平亮，设置ON=0，OFF=1
	* 若LED高电平亮，把宏设置成ON=1 ，OFF=0 即可
	*/
#define ON  0
#define OFF 1

/* 带参宏，可以像内联函数一样使用 */
#define LED1(a)	if (a)	\
					GPIO_SetBits(LED1_GPIO_PORT,LED1_PIN);\
					else		\
					GPIO_ResetBits(LED1_GPIO_PORT,LED1_PIN)

#define LED2(a)	if (a)	\
					GPIO_SetBits(LED2_GPIO_PORT,LED2_PIN);\
					else		\
					GPIO_ResetBits(LED2_GPIO_PORT,LED2_PIN)

#define LED3(a)	if (a)	\
					GPIO_SetBits(LED3_GPIO_PORT,LED3_PIN);\
					else		\
					GPIO_ResetBits(LED3_GPIO_PORT,LED3_PIN)

#define LED4(a)	if (a)	\
					GPIO_SetBits(LED4_GPIO_PORT,LED4_PIN);\
					else		\
					GPIO_ResetBits(LED4_GPIO_PORT,LED4_PIN)
					
					
/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)			 {p->BSRRL=i;}		//设置为高电平
#define digitalLo(p,i)			 {p->BSRRH=i;}		//输出低电平
#define digitalToggle(p,i)	 {p->ODR ^=i;}		//输出反转状态

/* 定义控制IO的宏 */
#define LED1_TOGGLE		digitalToggle(LED1_GPIO_PORT,LED1_PIN)
#define LED1_OFF			digitalHi(LED1_GPIO_PORT,LED1_PIN)
#define LED1_ON				digitalLo(LED1_GPIO_PORT,LED1_PIN)

#define LED2_TOGGLE		digitalToggle(LED2_GPIO_PORT,LED2_PIN)
#define LED2_OFF			digitalHi(LED2_GPIO_PORT,LED2_PIN)
#define LED2_ON				digitalLo(LED2_GPIO_PORT,LED2_PIN)

#define LED3_TOGGLE		digitalToggle(LED3_GPIO_PORT,LED3_PIN)
#define LED3_OFF			digitalHi(LED3_GPIO_PORT,LED3_PIN)
#define LED3_ON				digitalLo(LED3_GPIO_PORT,LED3_PIN)

#define LED4_TOGGLE		digitalToggle(LED4_GPIO_PORT,LED4_PIN)
#define LED4_OFF			digitalHi(LED4_GPIO_PORT,LED4_PIN)
#define LED4_ON				digitalLo(LED4_GPIO_PORT,LED4_PIN)

/* 基本混色，后面高级用法使用PWM可混出全彩颜色,且效果更好 */

#define LED_1  \
					LED1_ON;\
					LED2_OFF;\
					LED3_OFF;\
					LED4_OFF	

#define LED_2		\
					LED1_OFF;\
					LED2_ON;\
					LED3_OFF;\
					LED4_OFF	

#define LED_3	\
					LED1_OFF;\
					LED2_OFF;\
					LED3_ON;\
					LED4_OFF	

#define LED_4	\
					LED1_OFF;\
					LED2_OFF;\
					LED3_OFF;\
					LED4_ON	
					
#define LED_OFF	\
					LED1_OFF;\
					LED2_OFF;\
					LED3_OFF;\
					LED4_OFF			

#define LED_ON	\
					LED1_ON;\
					LED2_ON;\
					LED3_ON;\
					LED4_ON		


void LED_GPIO_Config(void);

#endif /* __LED_H */
