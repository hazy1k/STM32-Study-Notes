/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   usb mass storage 
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
#include "./usart/bsp_usart.h"   
#include "./led/bsp_led.h"
#include "hw_config.h" 
#include "usb_lib.h"
#include "usb_pwr.h" 
/*-------------   ����ʹ��˵�� ------------------*/

//�����̽������Ѳ���SD���û������û��SD����ʹ����Ϊ��
//��2.STM32-F103-ָ����_USB Mass Storage_flash�����̣���Ȼ��
//û��SD�������ʹ�ñ����̻ᵼ��ģ��U���ٶȷǳ�����

/*---------------------------------------------*/
void USB_Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

int main(void)
{
	 /* USART config */
	USART_Config();
	
  LED_GPIO_Config();
  
	/*��ʼ��*/
	Set_System();
  	
	/*����USBʱ��Ϊ48M*/
	Set_USBClock();
 	
	/*����USB�ж�(����SDIO�ж�)*/
	USB_Interrupts_Config();
 
	/*USB��ʼ��*/
 	USB_Init();
 
 	while (bDeviceState != CONFIGURED);	 //�ȴ��������
	   
	printf("\r\n Ұ�� F103-ָ���� STM32 USB MASS STORAGE ʵ��\r\n");
   printf("\r\n ʹ��ָ���ߵװ�ʱ ���Ͻ�����λ�� ��Ҫ��PC0������ñ ��ֹӰ��PC0��SPIFLASHƬѡ�� \r\n");
	 
  while (1)
  {
    LED2_TOGGLE;
    USB_Delay(0x0FFFFF);
  }
}

/* -------------------------------------end of file -------------------------------------------- */
