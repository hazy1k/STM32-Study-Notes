/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   AD7192
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./led/bsp_led.h"  
#include "./ad7192/bsp_ad7192.h"
#include "./ad7192_test/ad7192_test.h"
#include "bsp_SysTick.h"

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
  /* LED �˿ڳ�ʼ�� */
  LED_GPIO_Config();

  /* ���ô���1Ϊ��115200 8-N-1 */
  USART_Config();
  
  /* AD7192��ʼ�� */
  AD7192_Init();
  
  /* �����λ AD7192 */
  AD7192SoftwareReset();
  
  SysTick_Delay_Ms(10);
  ReadFromAD7192ViaSPI(REG_ID, 1, AD7192Registers, REG_ID);
  
  if ((AD7192Registers[REG_ID] & 0x0F) != 0)
  {
    printf("AD7192 ��ʼ��ʧ���������ӣ�\r\n");
    while(1);
  }
  printf("AD7192_ID = 0x%X\r\n", AD7192Registers[REG_ID]);

  /* 4·ADC����ת����ʵ�� */
  single_conversion_voltage();
	
	while (1)
  {
  }
}

