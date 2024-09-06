#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"


typedef uint32_t  u32;

extern void SysTick_Init(void);
extern void Delay_ms(__IO u32 nTime);
extern void TimingDelay_Decrement(void);
int get_tick_count(unsigned long *count);

#endif /* __SYSTICK_H */
