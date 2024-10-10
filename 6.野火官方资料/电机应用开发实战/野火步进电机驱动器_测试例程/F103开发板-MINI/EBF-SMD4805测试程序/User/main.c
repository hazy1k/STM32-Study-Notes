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
  * ʵ��ƽ̨:Ұ��  STM32 F103 ָ���� ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "stm32f10x.h"
#include "./led/bsp_led.h"
#include "./key/bsp_exti.h"
#include "bsp_usart.h"
#include "./SMD/SMD_test.h" 
#include "./SMD/MicroStepDriver.h" 
 
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
    USART_Config();
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

