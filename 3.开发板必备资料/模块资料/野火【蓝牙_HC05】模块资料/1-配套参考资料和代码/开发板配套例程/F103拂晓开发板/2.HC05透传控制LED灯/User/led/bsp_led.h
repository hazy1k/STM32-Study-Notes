#ifndef __LED_H
#define	__LED_H

#include "stm32f10x.h"

//引脚定义
/*******************************************************/
//LED1
#define LED1_GPIO_PIN             GPIO_Pin_8                
#define LED1_GPIO_PORT            GPIOA                     
#define LED1_GPIO_CLK             RCC_APB2Periph_GPIOA

//LED2
#define LED2_GPIO_PIN             GPIO_Pin_9
#define LED2_GPIO_PORT            GPIOA                     
#define LED2_GPIO_CLK             RCC_APB2Periph_GPIOA

//LED3
#define LED3_GPIO_PIN             GPIO_Pin_10              
#define LED3_GPIO_PORT            GPIOA                      
#define LED3_GPIO_CLK             RCC_APB2Periph_GPIOA

//LED4
#define LED4_GPIO_PIN             GPIO_Pin_14              
#define LED4_GPIO_PORT            GPIOE                       
#define LED4_GPIO_CLK             RCC_APB2Periph_GPIOE

/************************************************************/


/** 控制LED灯亮灭的宏，
	* LED低电平亮，设置ON=0，OFF=1
	* 若LED高电平亮，把宏设置成ON=1 ，OFF=0 即可
	*/
#define ON  GPIO_PIN_RESET
#define OFF GPIO_PIN_SET

/* 带参宏，可以像内联函数一样使用 */
#define LED1(a)	  if (a)  GPIO_SetBits(LED1_GPIO_PORT,LED1_GPIO_PIN);\
                  else    GPIO_ResetBits(LED1_GPIO_PORT,LED1_GPIO_PIN)


#define LED2(a)	  if (a)  GPIO_SetBits(LED2_GPIO_PORT,LED2_GPIO_PIN);\
                  else    GPIO_ResetBits(LED2_GPIO_PORT,LED2_GPIO_PIN)


#define LED3(a)	  if (a)  GPIO_SetBits(LED3_GPIO_PORT,LED3_GPIO_PIN);\
                  else    GPIO_ResetBits(LED3_GPIO_PORT,LED3_GPIO_PIN)

#define LED4(a)	  if (a)  GPIO_SetBits(LED4_GPIO_PORT,LED4_GPIO_PIN);\
                  else    GPIO_ResetBits(LED4_GPIO_PORT,LED4_GPIO_PIN)


/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)			{p->BSRR=i;}			  //设置为高电平		
#define digitalLo(p,i)			{p->BSRR=(uint32_t)i << 16;}				//输出低电平
#define digitalToggle(p,i)		{p->ODR ^=i;}			//输出反转状态


/* 定义控制IO的宏 */
#define LED1_TOGGLE		digitalToggle(LED1_GPIO_PORT,LED1_GPIO_PIN)
#define LED1_OFF		digitalHi(LED1_GPIO_PORT,LED1_GPIO_PIN)
#define LED1_ON			digitalLo(LED1_GPIO_PORT,LED1_GPIO_PIN)

#define LED2_TOGGLE		digitalToggle(LED2_GPIO_PORT,LED2_GPIO_PIN)
#define LED2_OFF		digitalHi(LED2_GPIO_PORT,LED2_GPIO_PIN)
#define LED2_ON			digitalLo(LED2_GPIO_PORT,LED2_GPIO_PIN)

#define LED3_TOGGLE		digitalToggle(LED3_GPIO_PORT,LED3_GPIO_PIN)
#define LED3_OFF		digitalHi(LED3_GPIO_PORT,LED3_GPIO_PIN)
#define LED3_ON			digitalLo(LED3_GPIO_PORT,LED3_GPIO_PIN)

#define LED4_TOGGLE		digitalToggle(LED4_GPIO_PORT,LED4_GPIO_PIN)
#define LED4_OFF		digitalHi(LED4_GPIO_PORT,LED4_GPIO_PIN)
#define LED4_ON			digitalLo(LED4_GPIO_PORT,LED4_GPIO_PIN)

//黑(全部关闭)
#define LED_ALLOFF	\
					LED1_OFF;\
					LED2_OFF;\
          LED3_OFF;\
					LED4_OFF
					
//(全部打开)
#define LED_ALLON	\
					LED1_ON;\
					LED2_ON;\
          LED3_ON;\
					LED4_ON



void LED_GPIO_Config(void);

#endif /* __LED_H */
