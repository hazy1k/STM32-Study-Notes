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
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
/*==================================�жϷ�ʽ====================================*/

/**********************�жϷ�����******************************/

extern void xPortSysTickHandler(void);
extern volatile uint32_t g_eventTimeMilliseconds;
/**
  * @brief  SysTick�жϷ�����
  * @param  ��
  * @retval ��
  * @attention
  */
void SysTick_Handler(void)
{
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
      {
    #endif  /* INCLUDE_xTaskGetSchedulerState */  
        xPortSysTickHandler();
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      }
    #endif  /* INCLUDE_xTaskGetSchedulerState */
//#ifdef __CA7_REV
//    SystemClearSystickFlag();
//#endif
//    g_eventTimeMilliseconds++;      
}



/*********************************************END OF FILE**********************/
