#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

void Delay_Init(void);
void Delay_ms(uint32_t ms);
void Delay_us(uint32_t us);
uint32_t GetTick(void);

#endif /* __DELAY_H */
