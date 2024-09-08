/**
  ******************************************************************************
  * @file    bsp_systick.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   systick�����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "./systick/bsp_systick.h"

/*==================================�жϷ�ʽ====================================*/
static __IO uint32_t TimingDelay;
 
/**
  * @brief  ����ϵͳ�δ�ʱ�� SysTick
  * @param  ��
  * @retval ��
  */
void SysTick_Init(void)
{
  /* SystemFrequency / 1000    1ms�ж�һ��
   * SystemFrequency / 100000  10us�ж�һ��
   * SystemFrequency / 1000000 1us�ж�һ��
   */
  if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* ������ */ 
    while (1);
  }
}

/**
  * @brief   ms��ʱ����
  * @param  
  *   @arg nTime: ��Ҫ��ʱ����ms
  * @retval  ��
  */
void Delay_ms(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  /* TimingDelay��һ��ȫ�ֱ�������SysTick�жϷ������ݼ� */
  while(TimingDelay != 0);
}

/**********************�жϷ�����******************************/

/**
  * @brief  SysTick�жϷ�����
  * @param  ��
  * @retval ��
  * @attention
  */
void SysTick_Handler(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}


#if 0
// SysTick_Config�� core_cm7.h��ʵ��
static __INLINE uint32_t SysTick_Config(uint32_t ticks)
{ 
  // reload �Ĵ���Ϊ24bit�����ֵΪ2^24
  if (ticks > SysTick_LOAD_RELOAD_Msk)  return (1);
  
  // ���� reload �Ĵ����ĳ�ʼֵ  
  SysTick->LOAD  = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;
  
  // �����ж����ȼ�Ϊ 1<<4-1 = 15�����ȼ�Ϊ���
  NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 
  
  // ���� counter ��������ֵ
  SysTick->VAL   = 0;
  
  // ����systick ��ʱ��Ϊ 528M
  // ʹ���ж�
  // ʹ��systick
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | 
                   SysTick_CTRL_TICKINT_Msk   | 
                   SysTick_CTRL_ENABLE_Msk;                    
  return (0); 
}
#endif

/*==================================��ѯ��ʽ==================================*/
 /*
  * couter ��1��ʱ�� ���� 1/systick_clk
  * ��counter �� reload ��ֵ��С��0��ʱ��Ϊһ��ѭ��������������ж���ִ���жϷ������
  * ͬʱ CTRL �� countflag λ����1
  * ��һ��ѭ����ʱ��Ϊ reload * (1/systick_clk)
  */

/**
  * @brief  SysTick us��ʱ����
  * @param  ��
  *   @arg us: ��Ҫ��ʱ����us
  * @retval ��
  * @attention
  */
void SysTick_Delay_Us( __IO uint32_t us)
{
  uint32_t i;
  SysTick_Config(SystemCoreClock/1000000);
  
  for(i=0;i<us;i++)
  {
    /* ����������ֵ��С��0��ʱ��CRTL�Ĵ�����λ16����1 */
    while( !((SysTick->CTRL)&(1<<16)) );
  }
  /* �ر�SysTick��ʱ�� */
  SysTick->CTRL &=~SysTick_CTRL_ENABLE_Msk;
}

/**
  * @brief  SysTick ms��ʱ����
  * @param  ��
  *   @arg ms: ��Ҫ��ʱ����ms
  * @retval ��
  * @attention
  */
void SysTick_Delay_Ms( __IO uint32_t ms)
{
  uint32_t i; 
  SysTick_Config(SystemCoreClock/1000);
  
  for(i=0;i<ms;i++)
  {
    /* ����������ֵ��С��0��ʱ��CRTL�Ĵ�����λ16����1
       ����1ʱ����ȡ��λ����0 */
    while( !((SysTick->CTRL)&(1<<16)) );
  }
  /* �ر�SysTick��ʱ�� */
  SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;
}


/*********************************************END OF FILE**********************/
