#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"



void SysTick_Init(void);
void Delay_ms(__IO uint32_t nTime);
int get_tick_count(unsigned long *count);

void TimeStamp_Increment(void);
void TimingDelay_Decrement(void);

#endif /* __SYSTICK_H */
