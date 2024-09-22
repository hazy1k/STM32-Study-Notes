
#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "bsp_led.h"

/*
 * t : ��ʱʱ�� 
 * Ticks : ���ٸ�ʱ�����ڲ���һ���ж� 
 * f : ʱ��Ƶ�� 72000000
 * t = Ticks * 1/f = (72000000/100000) * (1/72000000) = 10us 
 */ 

/*
************************************************************************************************************************
*                                             ʱ�����ؼĴ�������
************************************************************************************************************************
*/
/*
 ��Cortex-M3������һ�������DWT(Data Watchpoint and Trace)����������һ��32λ�ļĴ�����CYCCNT������һ�����ϵ�
 ����������¼�����ں�ʱ�����еĸ�������ܼ�¼��ʱ��Ϊ��60s=2��32�η�/72000000(�����ں�Ƶ��Ϊ72M���ں���һ�ε�ʱ����Ϊ1/72M=14ns)
 ��CYCCNT���֮�󣬻���0���¿�ʼ���ϼ�����
 ʹ��CYCCNT�����Ĳ������裺
 1����ʹ��DWT���裬����������ں˵��ԼĴ���DEMCR��λ24���ƣ�д1ʹ��
 2��ʹ��CYCCNT�Ĵ���֮ǰ������0
 3��ʹ��CYCCNT�Ĵ����������DWT_CTRL(�����Ϻ궨��ΪDWT_CR)��λ0���ƣ�д1ʹ��
 */


#define  DWT_CR      *(uint32_t *)0xE0001000
#define  DWT_CYCCNT  *(uint32_t *)0xE0001004
#define  DEM_CR      *(uint32_t *)0xE000EDFC


#define  DEM_CR_TRCENA                   (1 << 24)
#define  DWT_CR_CYCCNTENA                (1 <<  0)


/* ��ʼ��ʱ��� */
void CPU_TS_TmrInit(void)
{
    /* ʹ��DWT���� */
	DEM_CR |= (uint32_t)DEM_CR_TRCENA;                
    
	/* DWT CYCCNT�Ĵ���������0 */
	DWT_CYCCNT = (uint32_t)0u;
	
	/* ʹ��Cortex-M3 DWT CYCCNT�Ĵ��� */
    DWT_CR |= (uint32_t)DWT_CR_CYCCNTENA;
}

uint32_t OS_TS_GET(void)
{	
	return ((uint32_t)DWT_CYCCNT);
}

/* CPU��س�ʼ�� */
void  CPU_Init (void)
{
	/* ��ʼ��ʱ��� */
	CPU_TS_TmrInit();
}

uint32_t TimeStart = 0,TimeEnd = 0 ,TimeUse = 0;

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	
	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
	
	CPU_TS_TmrInit();
	
	for(;;)
	{
		LED1( ON );
		
		TimeStart = OS_TS_GET();		
		SysTick_Delay_Ms( 1000 );
		TimeEnd = OS_TS_GET();
		TimeUse = (TimeEnd - TimeStart) / ( 72000000 / 1000 ); /* ��λΪms */
		
		LED1( OFF );
	  
		LED2( ON );
		SysTick_Delay_Ms( 1000 );
		LED2( OFF );
	} 	
}


/*********************************************END OF FILE**********************/
