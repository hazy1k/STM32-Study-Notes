/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ʹ���ں�ʱ�����������ʱ
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

#include "./led/bsp_led.h"
#include "./delay/core_delay.h" 
#include "./systick/bsp_systick.h"



/*******************************************************************
 * Prototypes
 *******************************************************************/
/**
 * @brief ��ʱһ��ʱ��
 */
void delay(uint32_t count);

/*******************************************************************
 * Code
 *******************************************************************/
/**
 * @note �������ڲ�ͬ���Ż�ģʽ����ʱʱ�䲻ͬ��
 *       ��flexspi_nor_debug��flexspi_nor_release�汾�ĳ����У�
 *       flexspi_nor_release�汾����ʱҪ�̵ö�  
 */ 
void delay(uint32_t count)
{
    volatile uint32_t i = 0;
    for (i = 0; i < count; ++i)
    {
        __asm("NOP"); /* ����nop��ָ�� */
    }
}
#define OS_TS_GET CPU_TS_TmrRd
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
		uint32_t TimeStart = 0,TimeEnd = 0 ,TimeUse = 0;
    /* ��ʼ���ڴ汣����Ԫ */
    BOARD_ConfigMPU();
    /* ��ʼ������������ */
    BOARD_InitPins();
    /* ��ʼ��������ʱ�� */
    BOARD_BootClockRUN();
    /* ��ʼ�����Դ��� */
    BOARD_InitDebugConsole();
  
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
  
		/*	ʱ�����ʼ�� */
		CPU_TS_TmrInit();
		/*�δ�ʱ����ʼ��*/
		SysTick_Init();
    /* ��ʼ��LED���� */
    LED_GPIO_Config();  
    
    while(1)
    {         
				RGB_RED_LED_ON;
				TimeStart = OS_TS_GET();		
				SysTick_Delay_Ms( 1000 );
				TimeEnd = OS_TS_GET();
				TimeUse = (TimeEnd - TimeStart) / ( 528000000 / 1000 ); /* ��λΪms */
				PRINTF("TimeUse %d ms \r\n", TimeUse);  
				RGB_RED_LED_OFF;
			
				RGB_GREEN_LED_ON;
				SysTick_Delay_Ms( 1000 );
				RGB_GREEN_LED_ON;

    }     

}
/****************************END OF FILE**********************/
