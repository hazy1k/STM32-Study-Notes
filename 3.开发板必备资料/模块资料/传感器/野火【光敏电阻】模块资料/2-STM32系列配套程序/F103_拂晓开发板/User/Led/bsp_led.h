#ifndef __LED_H
#define	__LED_H


#include "stm32f10x.h"


/* 定义LED连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的LED引脚 */
//LED1
#define LED1_GPIO_PORT    	GPIOA			              /* GPIO端口 */
#define LED1_GPIO_CLK 	    RCC_APB2Periph_GPIOA		/* GPIO端口时钟 */
#define LED1_GPIO_PIN		   GPIO_Pin_8			        /* 连接到SCL时钟线的GPIO */

//LED2
#define LED2_GPIO_PORT    	GPIOA			              /* GPIO端口 */
#define LED2_GPIO_CLK 	    RCC_APB2Periph_GPIOA		/* GPIO端口时钟 */
#define LED2_GPIO_PIN		    GPIO_Pin_9			        /* 连接到SCL时钟线的GPIO */

//LED3
#define LED3_GPIO_PORT    	GPIOA			              /* GPIO端口 */
#define LED3_GPIO_CLK 	    RCC_APB2Periph_GPIOA		/* GPIO端口时钟 */
#define LED3_GPIO_PIN		   GPIO_Pin_10			        /* 连接到SCL时钟线的GPIO */

//LED4
#define LED4_GPIO_PORT    	GPIOE			              /* GPIO端口 */
#define LED4_GPIO_CLK 	    RCC_APB2Periph_GPIOE		/* GPIO端口时钟 */
#define LED4_GPIO_PIN		   GPIO_Pin_14			        /* 连接到SCL时钟线的GPIO */



/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */
#define ON  0
#define OFF 1

/* 使用标准的固件库控制IO*/
#define LED1(a)	if (a)	\
					GPIO_SetBits(LED1_GPIO_PORT,LED1_GPIO_PIN);\
					else		\
					GPIO_ResetBits(LED1_GPIO_PORT,LED1_GPIO_PIN)

#define LED2(a)	if (a)	\
					GPIO_SetBits(LED2_GPIO_PORT,LED2_GPIO_PIN);\
					else		\
					GPIO_ResetBits(LED2_GPIO_PORT,LED2_GPIO_PIN)

#define LED3(a)	if (a)	\
					GPIO_SetBits(LED3_GPIO_PORT,LED3_GPIO_PIN);\
					else		\
					GPIO_ResetBits(LED3_GPIO_PORT,LED3_GPIO_PIN)

#define LED4(a)	if (a)	\
					GPIO_SetBits(LED4_GPIO_PORT,LED4_GPIO_PIN);\
					else		\
					GPIO_ResetBits(LED4_GPIO_PORT,LED4_GPIO_PIN)
					
					


/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)		 {p->BSRR=i;}	 //输出为高电平		
#define digitalLo(p,i)		 {p->BRR=i;}	 //输出低电平
#define digitalToggle(p,i) {p->ODR ^=i;} //输出反转状态


/* 定义控制IO的宏 */
#define LED1_TOGGLE		 digitalToggle(LED1_GPIO_PORT,LED1_GPIO_PIN)
#define LED1_OFF		   digitalHi(LED1_GPIO_PORT,LED1_GPIO_PIN)
#define LED1_ON			   digitalLo(LED1_GPIO_PORT,LED1_GPIO_PIN)

#define LED2_TOGGLE		 digitalToggle(LED2_GPIO_PORT,LED2_GPIO_PIN)
#define LED2_OFF		   digitalHi(LED2_GPIO_PORT,LED2_GPIO_PIN)
#define LED2_ON			   digitalLo(LED2_GPIO_PORT,LED2_GPIO_PIN)

#define LED3_TOGGLE		 digitalToggle(LED3_GPIO_PORT,LED3_GPIO_PIN)
#define LED3_OFF		   digitalHi(LED3_GPIO_PORT,LED3_GPIO_PIN)
#define LED3_ON			   digitalLo(LED3_GPIO_PORT,LED3_GPIO_PIN)


#define LED4_TOGGLE		digitalToggle(LED4_GPIO_PORT,LED4_PIN)
#define LED4_OFF		digitalHi(LED4_GPIO_PORT,LED4_PIN)
#define LED4_ON			digitalLo(LED4_GPIO_PORT,LED4_PIN)
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
