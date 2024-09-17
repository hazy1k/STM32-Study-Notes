/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����led
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103 STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx.h"
#include "./led/bsp_led.h"
#include ".\key\bsp_key.h" 
#include "./rcc/bsp_clkconfig.h"

void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
  /* ϵͳʱ�ӳ�ʼ����72MHz */
  SystemClock_Config(RCC_OSCILLATORTYPE_HSE,RCC_PLL_MUL9);
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	 
  
     while (1)
    {
        LED2( ON );			 // �� 
        Delay(0x0FFFFF);
        LED2( OFF );		 // ��
        Delay(0x0FFFFF);
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
