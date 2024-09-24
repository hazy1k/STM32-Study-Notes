#ifndef __BEEP_H_
#define	__BEEP_H_

#include "stm32f10x.h"

/******************** BEEP �������ò������� **************************/
#define    BEEP_GPIO_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define    BEEP_GPIO_CLK               RCC_APB2Periph_GPIOC
#define    BEEP_PORT                   GPIOC
#define    BEEP_PIN                    GPIO_Pin_0



/******************** BEEP �����궨�� *******************************/
#define    BEEP_ON()	      GPIO_SetBits(BEEP_PORT,BEEP_PIN)
#define    BEEP_OFF()       GPIO_ResetBits(BEEP_PORT,BEEP_PIN)

/************************** BEEP ��������****************************/
void Beep_Init(void);


#endif /* __BEEP_H_ */
