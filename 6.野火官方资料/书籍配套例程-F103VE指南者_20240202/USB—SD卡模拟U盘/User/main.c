/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   STM32 SD�� usb mass storage ��ģ��U�̣�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "./sdio/bsp_sdio_sdcard.h"
#include "./usart/bsp_usart.h"
#include "./led/bsp_led.h"  
#include "hw_config.h" 
#include "usb_lib.h"
#include "usb_pwr.h"




void USB_Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

int main(void)
{
	 /* USART config */
	USART_Config();
	
  LED_GPIO_Config();
  
	/*��ʼ��SD��*/
	Set_System();
  	
	/*����USBʱ��Ϊ48M*/
	Set_USBClock();
 	
	/*����USB�ж�(����SDIO�ж�)*/
	USB_Interrupts_Config();
 
	/*USB��ʼ��*/
 	USB_Init();
 
 	while (bDeviceState != CONFIGURED);	 //�ȴ��������
	   
	printf("\r\n Ұ�� F103-ָ���� STM32 USB MASS STORAGE ʵ��\r\n");
	 
  while (1)
  {
    LED2_TOGGLE;
    USB_Delay(0x0FFFFF);
  }
}

/* -------------------------------------end of file -------------------------------------------- */
