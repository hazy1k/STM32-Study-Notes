#ifndef __IRDA_H
#define	__IRDA_H

#include "stm32h7xx.h"
void EXTI_PB11_Config(void);
void IrDa_Init(void);
uint8_t Get_Pulse_Time(void);
uint8_t IrDa_Process(void);

#define IRDA_ID 0

//红外遥控使用的GPIO及时钟
#define IRDA_GPIO_PORT    						 GPIOA	              
#define IRDA_GPIO_CLK()	    					 __GPIOA_CLK_ENABLE()	
#define IRDA_GPIO_PIN									 GPIO_PIN_11			        


//中断相关
#define IRDA_EXTI_IRQN								EXTI15_10_IRQn
#define IRDA_EXTI_IRQHANDLER_FUN			EXTI15_10_IRQHandler

 //读取引脚的电平
#define  IrDa_DATA_IN()	   HAL_GPIO_ReadPin(IRDA_GPIO_PORT,IRDA_GPIO_PIN)

#endif /* __IRDA_H */
