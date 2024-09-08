#ifndef __IRDA_H
#define	__IRDA_H

#include "stm32f4xx.h"
void EXTI_PB11_Config(void);
void IrDa_Init(void);
uint8_t Get_Pulse_Time(void);
uint8_t IrDa_Process(void);

#define IRDA_ID 0

//����ң��ʹ�õ�GPIO��ʱ��
#define IRDA_GPIO_PORT    						 GPIOD	              
#define IRDA_GPIO_CLK()	    					 __GPIOD_CLK_ENABLE()	
#define IRDA_GPIO_PIN									 GPIO_PIN_2			        


//�ж����
#define IRDA_EXTI_IRQN								EXTI2_IRQn
#define IRDA_EXTI_IRQHANDLER_FUN			EXTI2_IRQHandler

 //��ȡ���ŵĵ�ƽ
#define  IrDa_DATA_IN()	   HAL_GPIO_ReadPin(IRDA_GPIO_PORT,IRDA_GPIO_PIN)

#endif /* __IRDA_H */
