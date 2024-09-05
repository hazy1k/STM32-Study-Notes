/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ָ��ʶ��ģ��ʵ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./usart/rx_data_queue.h"
#include "./SysTick/bsp_SysTick.h"
#include "./led/bsp_led.h"  
#include "./as608/as608_test.h"
#include "./as608/bsp_as608.h"

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{		
  /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
  USART_Config();
  
  LED_GPIO_Config();	
  
  /*��ʼ�����λ�����*/
  rx_queue_init();
   
  /*��ʼ��ָ��ģ������*/
	AS608_Config();
  
	/*����STM32��ָ��ģ���ͨ��*/
  Connect_Test();
  
  while(1)
  {
    FR_Task();
  } 
}



/*********************************************END OF FILE**********************/
