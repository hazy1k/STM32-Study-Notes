#ifndef __IRDA_H
#define	__IRDA_H

#include "stm32f10x.h"
void EXTI_PB11_Config(void);
void IrDa_Init(void);
uint8_t Get_Pulse_Time(void);
uint8_t IrDa_Process(void);

#define IRDA_ID 0

//红外遥控使用的GPIO及时钟
#define IRDA_GPIO_PORT    						GPIOC		              
#define IRDA_GPIO_CLK 	    					RCC_APB2Periph_GPIOC		
#define IRDA_GPIO_PIN									GPIO_Pin_0			        
#define IRDA_GPIO_PORT_SOURCE				GPIO_PortSourceGPIOC
#define IRDA_GPIO_PIN_SOURCE					GPIO_PinSource0

//中断相关
#define IRDA_EXTI_LINE								EXTI_Line0
#define IRDA_EXTI_IRQN								EXTI0_IRQn
#define IRDA_EXTI_IRQHANDLER_FUN			EXTI0_IRQHandler

 //读取引脚的电平
#define  IrDa_DATA_IN()	   GPIO_ReadInputDataBit(IRDA_GPIO_PORT,IRDA_GPIO_PIN)

#endif /* __IRDA_H */
