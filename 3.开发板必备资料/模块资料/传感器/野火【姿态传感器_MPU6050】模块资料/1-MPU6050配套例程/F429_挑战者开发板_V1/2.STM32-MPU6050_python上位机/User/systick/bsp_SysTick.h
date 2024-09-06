#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"

/*���������*/
#define TASK_ENABLE 0
#define NumOfTask 3


void SysTick_Init(void);
void Delay_ms(__IO u32 nTime);
int get_tick_count(unsigned long *count);

void TimeStamp_Increment(void);
void TimingDelay_Decrement(void);

#endif /* __SYSTICK_H */
