/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   dht11 ��ʪ�ȴ�����ʵ��
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
#include "fsl_elcdif.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "./delay/core_delay.h"   
#include "./systick/bsp_systick.h"
#include "./dht11/bsp_dht11.h"
#include "./lcd/bsp_lcd.h" 
#include "stdio.h" 
/*******************************************************************
 * Prototypes
 *******************************************************************/
DHT11_Data_TypeDef DHT11_Data;
/* ��ʾ������ */
uint8_t dis_buf[1024];
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
		PRINTF("\r\n*** dht11 ��ʪ�ȴ�����ʵ��***\r\n");
		/* ��ʼ��LCD */
    LCD_Init(LCD_INTERRUPT_ENABLE);
		/* DHT11��ʼ�� */
		DHT11_GPIO_Config();
		LCD_DisplayStringLine(LINE(1),(uint8_t* )"DHT11 temperature & humidity detect demo");
    while(1)
    {
				/*����DHT11_Read_TempAndHumidity��ȡ��ʪ�ȣ����ɹ����������Ϣ*/
				if( Read_DHT11 ( & DHT11_Data ) == SUCCESS)
				{
						PRINTF("\r\n��ȡDHT11�ɹ�!\r\n\r\nʪ��Ϊ%d.%d ��RH ���¶�Ϊ %d.%d�� \r\n",\
						DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
						LCD_ClearLine(LINE(2));
						LCD_ClearLine(LINE(3));
						sprintf((char*)dis_buf,"humidity: %2d.%2d %%RH ",DHT11_Data.humi_int,DHT11_Data.humi_deci);
						LCD_DisplayStringLine(LINE(4),dis_buf);

						sprintf((char*)dis_buf,"temperature: %2d.%2d degree Celsius ",DHT11_Data.temp_int,DHT11_Data.temp_deci);
						LCD_DisplayStringLine(LINE(5),dis_buf);
				}		
				else
				{
						PRINTF("Read DHT11 ERROR!\r\n");     
						LCD_SetTextColor(CL_RED);
						LCD_DisplayStringLine(LINE(2),(uint8_t* )"DHT11 initialization failed!");
						LCD_DisplayStringLine(LINE(3),(uint8_t* )"Please check the connection!");	
				}
				Delay_ms(2000);
    }     

}
/****************************END OF FILE**********************/
