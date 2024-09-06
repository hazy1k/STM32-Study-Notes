#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"

#define NumOfTask 3

typedef uint32_t  u32;

extern void SysTick_Init(void);
extern void Delay_us(__IO u32 nTime);
extern void TimingDelay_Decrement(void);
int get_tick_count(unsigned long *count);
#define Delay_ms(x) Delay_us(100*x)	 //µ¥Î»ms

#endif /* __SYSTICK_H */
