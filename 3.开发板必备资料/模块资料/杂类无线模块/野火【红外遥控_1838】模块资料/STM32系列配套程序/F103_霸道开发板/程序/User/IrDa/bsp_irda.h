#ifndef __IRDA_H
#define	__IRDA_H

#include "stm32f10x.h"
void EXTI_PB11_Config(void);
void IrDa_Init(void);
uint8_t Get_Pulse_Time(void);
uint8_t IrDa_Process(void);

#define IRDA_ID 0

//红外遥控使用的GPIO及时钟
#define IRDA_GPIO_PORT    						GPIOD			              
#define IRDA_GPIO_CLK 	    					RCC_APB2Periph_GPIOD		
#define IRDA_GPIO_PIN									GPIO_Pin_7			        
#define IRDA_GPIO_PORT_SOURCE				GPIO_PortSourceGPIOD
#define IRDA_GPIO_PIN_SOURCE					GPIO_PinSource7

//中断相关
#define IRDA_EXTI_LINE								EXTI_Line7
#define IRDA_EXTI_IRQN								EXTI9_5_IRQn
#define IRDA_EXTI_IRQHANDLER_FUN			EXTI9_5_IRQHandler

 //读取引脚的电平
#define  IrDa_DATA_IN()	   GPIO_ReadInputDataBit(IRDA_GPIO_PORT,IRDA_GPIO_PIN)

#endif /* __IRDA_H */
