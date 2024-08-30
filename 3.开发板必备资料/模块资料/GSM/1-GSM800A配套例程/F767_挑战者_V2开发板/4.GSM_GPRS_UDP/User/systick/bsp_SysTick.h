#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f7xx.h"

typedef uint32_t  u32;

extern void SysTick_Init(void);
extern void Delay_us(__IO u32 nTime);
extern void TimingDelay_Decrement(void);
#define Delay_ms(x) Delay_us(100*x)	 //��λms
#define HAL_Delay(x)    Delay_us(100*x)	 //��λms

#endif /* __SYSTICK_H */
