#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

// 12λ��ADC��3.3V��ADCֵΪ0xFFF, �¶�25��ʱ��Ӧ�ĵ�ѹֵΪ1.43V��0x6EE
#define V25 0x6EE
// б��ÿ���϶�4.3mv����Ӧÿ���϶�0x05
#define ADC_K 0x05
#define ADC1_DR_Address ((uint32_t)ADC1_BASE+0x4c)
void ADCx_Init(void);

#endif /* __ADC_H */
