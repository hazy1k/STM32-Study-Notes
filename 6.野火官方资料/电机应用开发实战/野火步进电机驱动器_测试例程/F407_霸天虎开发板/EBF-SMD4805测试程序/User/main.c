/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ���������������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./key/bsp_exti.h" 
#include "./SMD/MicroStepDriver.h" 
#include "./usart/bsp_debug_usart.h"
#include "./SMD/SMD_test.h" 

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
    //��ʼ��LED
    LED_GPIO_Config();
    //��ʼ������
    EXTI_Key_Config();
    //��ʼ������
    Debug_USART_Config();
	//���������ʼ��
	SMD_Init();
    //��ӡ��������
    ShowHelp();
    
    while(1) 
    {
        //����������
        DealSerialData();
    }                      
}

/*********************************************END OF FILE**********************/

