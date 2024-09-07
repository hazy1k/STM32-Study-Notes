/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   DS18B20 ��ʪ�ȴ�����ʵ��
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************
  */
#include "fsl_debug_console.h"

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "./delay/core_delay.h"   
#include "./systick/bsp_systick.h"
#include "./ds18b20/bsp_ds18b20.h"

/*******************************************************************
 * Prototypes
 *******************************************************************/
__IO float temperature;
/*******************************************************************
 * Code
 *******************************************************************/
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
		uint8_t uc,DS18B20Id[8];
    /* ��ʼ���ڴ汣����Ԫ */
    BOARD_ConfigMPU();
    /* ��ʼ������������ */
    BOARD_InitPins();
    /* ��ʼ��������ʱ�� */
    BOARD_BootClockRUN();
    /* ��ʼ�����Դ��� */
    BOARD_InitDebugConsole();
	  /* ��ʼ����ȷ��ʱ*/
		CPU_TS_TmrInit();
		/* ��ʼ���δ�ʱ�� */
		SysTick_Init();
    /* ��ӡϵͳʱ�� */
    PRINTF("\r\n");
    PRINTF("*****��ӭʹ�� Ұ��i.MX RT1052 ������*****\r\n");
    PRINTF("CPU:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_CpuClk));
    PRINTF("AHB:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_AhbClk));
    PRINTF("SEMC:            %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SemcClk));
    PRINTF("SYSPLL:          %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllClk));
    PRINTF("SYSPLLPFD0:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd0Clk));
    PRINTF("SYSPLLPFD1:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd1Clk));
    PRINTF("SYSPLLPFD2:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk));
    PRINTF("SYSPLLPFD3:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd3Clk));  
		PRINTF("\r\n*** DS18B20 �¶ȴ�����ʵ��***\r\n");
    if(DS18B20_Init()==0)
    {
      PRINTF("DS18B20��ʼ���ɹ�\n");
    }
    else
    {
      PRINTF("DS18B20��ʼ��ʧ��\n");
      PRINTF("�뽫��������ȷ���뵽�����\n");
      /* ͣ�� */
      while(1)
      {}			
    }		
    DS18B20_ReadId ( DS18B20Id  );           // ��ȡ DS18B20 �����к�
    
    PRINTF("\r\nDS18B20�����к��ǣ� 0x");
    
    for ( uc = 0; uc < 8; uc++ )             // ��ӡ DS18B20 �����к�
      PRINTF ( "%.2x", DS18B20Id[uc]);
    PRINTF("\n");
    
    while(1)
    {
      temperature=DS18B20_Get_Temp();
      PRINTF("DS18B20��ȡ�����¶�Ϊ��%0.3f\n",temperature);
      Delay_ms(1000);
    } 
   

}
/****************************END OF FILE**********************/
