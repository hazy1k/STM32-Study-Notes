#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f7xx.h"

typedef uint32_t  uint32_t;

extern void SysTick_Init(void);
extern void Delay_us(__IO uint32_t nTime);
extern void TimingDelay_Decrement(void);
#define Delay_ms(x)       Delay_us(x)	 //单位ms
//#define HAL_Delay(x)    Delay_us(100*x)	 //单位ms

#endif /* __SYSTICK_H */
