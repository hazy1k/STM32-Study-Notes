/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   ����ң�����ӿ�ʵ��
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
#include "./irda/bsp_irda.h"
#include "./led/bsp_led.h"  

/*******************************************************************
 * Prototypes
 *******************************************************************/
extern uint8_t  frame_flag;
extern uint8_t  isr_cnt;
extern uint8_t  frame_cnt;
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
		uint8_t key_val;
	
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
		/* ���õδ�ʱ�����ȼ� */
		NVIC_SetPriority (SysTick_IRQn, 0);
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
		PRINTF("\r\n ����һ������ң�ط��������ʵ�� \r\n"); 
		LED_GPIO_Config();
		IrDa_Init();
		RGB_RED_LED_TOGGLE
		while(1)
		{ 
			IRDA_LOW;
				if( frame_flag == 1 ) /* һ֡�������ݽ������ */
				{
					key_val = IrDa_Process();
					PRINTF("\r\n key_val=%d \r\n",key_val);
					PRINTF("\r\n ��������frame_cnt=%d \r\n",frame_cnt);
					PRINTF("\r\n �жϴ���isr_cnt=%d \r\n",isr_cnt);
					
					/* ��ͬ��ң��������Ӧ��ͬ�ļ�ֵ����Ҫʵ�ʲ��� */
					switch( key_val )
					{
						case 0:
						RGB_RED_LED_TOGGLE;  
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n Error \r\n");
						break;
						
						case 162:
						RGB_RED_LED_TOGGLE;  
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n POWER \r\n");
						break;
						
						case 226:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n MENU \r\n");
						break;
						
						case 34:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n TEST \r\n");
						break;
						
						case 2:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n + \r\n");
						break;
						
						case 194:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n RETURN \r\n");
						break;
						
						case 224:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n |<< \r\n");
						break;
						
						case 168:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n > \r\n");
						break;
						
						case 144:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n >>| \r\n");
						break;
						
						case 104:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n 0 \r\n");
						break;
						
						case 152:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n - \r\n");
						break;
						
						case 176:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n C \r\n");
						break;
						
						case 48:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n 1 \r\n");
						break;
						
						case 24:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n 2 \r\n");
						break;
						
						case 122:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n 3 \r\n");
						break;
						
						case 16:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n 4 \r\n");
						break;
						
						case 56:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n 5 \r\n");
						break;
						
						case 90:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n 6 \r\n");
						break;
						
							case 66:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n 7 \r\n");
						break;
						
						case 74:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n 8 \r\n");
						break;
						
						case 82:
							RGB_RED_LED_TOGGLE;
							PRINTF("\r\n key_val=%d \r\n",key_val);
							PRINTF("\r\n 9 \r\n");
						break;
						
						default:       
						break;
					}      
				}
		}
}
/****************************END OF FILE**********************/
