/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   ��SD���ڵ�GPS���ݽ��н��룬��ȡ��λ��Ϣ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F429 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./Bsp/led/bsp_led.h"
#include "./Bsp/usart/bsp_debug_usart.h"
#include "./gps/gps_config.h"

void Delay(__IO u32 nCount); 

// WIFI��SD������SDIO���ߣ���ʹ��SD��ʱ������WIFI�ĳ�ͻ
// ��Ҫ��WIFI��ʹ������PDN���͡�
void WIFI_PDNPIN_SET(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
		RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOG, ENABLE);															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
		GPIO_Init(GPIOG, &GPIO_InitStructure);	
	
		GPIO_ResetBits(GPIOG,GPIO_Pin_9);
} 

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
	Debug_USART_Config();
  
  GPS_Config();

	// ʧ��WIFI
	WIFI_PDNPIN_SET();

  printf("\r\nҰ�� GPSģ���������\r\n"); 
  
	printf("\r\n�������SD���ڵ�gpslog.txt�ļ���λ��־���ݽ��н���\r\n"); 
	printf("\r\n����Ҫ��GPSģ�鴮�ڴ��ص����ݽ��룬");
	printf("��ע�͵�gps_config.h�ļ��ĺ�__GPS_LOG_FILE \r\n"); 

  /* GPS������� */
  nmea_decode_test();
  
  while(1);


}

void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/

