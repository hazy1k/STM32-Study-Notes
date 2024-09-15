/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ��GPSģ�鴫������ݽ��н��룬��ȡ��λ��Ϣ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103-ָ���� ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./led/bsp_led.h"
#include "./gps/gps_config.h"

extern void nmea_decode_test(void);

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	 
  
  LED_BLUE;
  
	/*���ڳ�ʼ��*/
	USART_Config();
  
  GPS_Config();

  printf("\r\nҰ�� GPSģ���������\r\n"); 
  
	printf("\r\n�������GPSģ�鴮�ڴ��ص����ݽ��룬");
	printf("ʵ��ʱ������������GPSģ�� \r\n"); 

  /* GPS������� */
  nmea_decode_test();
  
  while(1);


}

void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
