#ifndef __CORE_DELAY_H
#define __CORE_DELAY_H

#include "stm32f4xx.h"

/* ��ȡ�ں�ʱ��Ƶ�� */
#define GET_CPU_ClkFreq()       HAL_RCC_GetSysClockFreq()
#define CPU_MHZ									168
#define SysClockFreq            (CPU_MHZ*1000000)
/* Ϊ����ʹ�ã�����ʱ�����ڲ�����CPU_TS_TmrInit������ʼ��ʱ����Ĵ�����
   ����ÿ�ε��ú��������ʼ��һ�顣
   �ѱ���ֵ����Ϊ0��Ȼ����main����������ʱ����CPU_TS_TmrInit�ɱ���ÿ�ζ���ʼ�� */  

#define CPU_TS_INIT_IN_DELAY_FUNCTION   0  


/*******************************************************************************
 * ��������
 ******************************************************************************/
uint32_t CPU_TS_TmrRd(void);
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority);
uint32_t HAL_GetTick(void);
//ʹ�����º���ǰ�����ȵ���CPU_TS_TmrInit����ʹ�ܼ���������ʹ�ܺ�CPU_TS_INIT_IN_DELAY_FUNCTION
//�����ʱֵΪ8��
void CPU_TS_Tmr_Delay_US(uint32_t us);
#define HAL_Delay(ms)     					CPU_TS_Tmr_Delay_US(ms*1000)
#define CPU_TS_Tmr_Delay_S(s)       CPU_TS_Tmr_Delay_MS(s*1000)
		
#define delay_ms(ms)	CPU_TS_Tmr_Delay_US(1000*ms)
#define delay_us(us)	CPU_TS_Tmr_Delay_US(us)


#endif /* __CORE_DELAY_H */
