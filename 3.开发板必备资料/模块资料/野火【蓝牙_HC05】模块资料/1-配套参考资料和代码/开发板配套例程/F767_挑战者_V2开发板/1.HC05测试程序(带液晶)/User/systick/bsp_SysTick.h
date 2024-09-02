#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f7xx.h"



#define NumOfTask 3
typedef uint32_t  u32;

void SysTick_Init(void);
void Delay_us(__IO u32 nTime);
int get_tick_count(unsigned long *count);
#define Delay_ms(x) Delay_us(100*x)	 //µ¥Î»ms

#endif /* __SYSTICK_H */
