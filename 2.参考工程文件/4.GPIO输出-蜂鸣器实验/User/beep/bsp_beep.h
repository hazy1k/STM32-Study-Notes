#ifndef __BEEP_H
#define	__BEEP_H


#include "stm32f10x.h"

/* 定义蜂鸣器连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的蜂鸣器引脚 */
#define BEEP_GPIO_PORT    	GPIOA			     // 蜂鸣器使用的GPIO端口
#define BEEP_GPIO_CLK 	    RCC_APB2Periph_GPIOA // 开启GPIOA端口时钟
#define BEEP_GPIO_PIN		GPIO_Pin_8			 // 连接到蜂鸣器的GPIO的引脚

/* 高电平时，蜂鸣器响 */
#define ON  1
#define OFF 0

/* 带参宏，可以像内联函数一样使用 */
#define BEEP(a)	if (a)	\
					GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);\
					else		\
					GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN)

void BEEP_GPIO_Config(void);
					
#endif /* __BEEP_H */
