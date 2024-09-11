/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   ��GPSģ�鴫������ݽ��н��룬��ȡ��λ��Ϣ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F4 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./gps/gps_config.h"

void Delay(__IO u32 nCount); 

extern void nmea_decode_test(void);

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	/*���ڳ�ʼ��*/
	Debug_USART_Config();
  
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
/*********************************************END OF FILE**********************/

