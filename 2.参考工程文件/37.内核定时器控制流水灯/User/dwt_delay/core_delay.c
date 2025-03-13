#include "core_delay.h"   

/*
 ��Cortex-M������һ�������DWT(Data Watchpoint and Trace)��
 ��������һ��32λ�ļĴ�����CYCCNT������һ�����ϵļ�������
 ��¼�����ں�ʱ�����еĸ�������ܼ�¼��ʱ��Ϊ��
 60s=2��32�η�/72000000
 (�����ں�Ƶ��Ϊ72M���ں���һ�ε�ʱ����Ϊ1/72M=13.8ns)
 ��CYCCNT���֮�󣬻���0���¿�ʼ���ϼ�����
 ʹ��CYCCNT�����Ĳ������裺
 1����ʹ��DWT���裬����������ں˵��ԼĴ���DEMCR��λ24���ƣ�д1ʹ��
 2��ʹ��CYCCNT�Ĵ���֮ǰ������0
 3��ʹ��CYCCNT�Ĵ����������DWT_CTRL(�����Ϻ궨��ΪDWT_CR)��λ0���ƣ�д1ʹ��
*/
 
#if USE_DWT_DELAY // �궨�壬���û�ж��壬��ʹ��DWT��ʱ����

#define  DWT_CR      *(__IO uint32_t *)0xE0001000 // DWT���ƼĴ�����ַ
#define  DWT_CYCCNT  *(__IO uint32_t *)0xE0001004 // DWT��������ַ
#define  DEM_CR      *(__IO uint32_t *)0xE000EDFC // ���Լ��������ƼĴ�����ַ
#define  DEM_CR_TRCENA     (1 << 24) // ���Լ��������ƼĴ�����λ24��DWTʹ��λ
#define  DWT_CR_CYCCNTENA  (1 <<  0) // DWT���ƼĴ�����λ0��CYCCNTʹ��λ

// ��ʼ��ʱ���
void CPU_TS_TmrInit(void)
{
    DEM_CR |= (uint32_t)DEM_CR_TRCENA; // ʹ��DWT����
    DWT_CYCCNT = (uint32_t)0u; // ��0������
    DWT_CR |= (uint32_t)DWT_CR_CYCCNTENA; // ʹ��CYCCNT������
}

// ��ȡ��ǰʱ���
uint32_t CPU_TS_TmrRd(void)
{        
  return ((uint32_t)DWT_CYCCNT);
}

// ����CPU���ڲ�����ʵ�־�ȷ��ʱ��32λ������
void CPU_TS_Tmr_Delay_US(__IO uint32_t us)
{
  uint32_t ticks;
  uint32_t told,tnow,tcnt=0;

  /* �ں����ڲ���ʼ��ʱ����Ĵ����� */  
#if (CPU_TS_INIT_IN_DELAY_FUNCTION)  
  /* ��ʼ��ʱ��������� */
  CPU_TS_TmrInit();
#endif
  
  ticks = us * (GET_CPU_ClkFreq() / 1000000);  /* ��Ҫ�Ľ����� */      
  tcnt = 0;
  told = (uint32_t)CPU_TS_TmrRd();         /* �ս���ʱ�ļ�����ֵ */
  while(1)
  {
    tnow = (uint32_t)CPU_TS_TmrRd();  
    if(tnow != told)
    { 
        /* 32λ�������ǵ��������� */    
      if(tnow > told)
      {
        tcnt += tnow - told;  
      }
      /* ����װ�� */
      else 
      {
        tcnt += UINT32_MAX - told + tnow; 
      } 
      told = tnow;
      /*ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳� */
      if(tcnt >= ticks)break;
    }  
  }
}

#endif
