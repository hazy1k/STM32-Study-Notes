#ifndef __LDR_H
#define	__LDR_H


#include "stm32f7xx.h"

//  引脚定义
#define LDR_PIN                  GPIO_PIN_2                 
#define LDR_GPIO_PORT            GPIOE                      
#define LDR_GPIO_CLK_ENABLE()    __GPIOE_CLK_ENABLE()

 /** 按键按下标置宏
	*  按键按下为高电平，设置 KEY_ON=1， KEY_OFF=0
	*  若按键按下为低电平，把宏设置成KEY_ON=0 ，KEY_OFF=1 即可
	*/
#define LDR_ON	1     // 有光
#define LDR_OFF	0     // 无光

void LDR_GPIO_Config(void);
uint8_t LDR_Test(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);


#endif /* __LDR_H */

