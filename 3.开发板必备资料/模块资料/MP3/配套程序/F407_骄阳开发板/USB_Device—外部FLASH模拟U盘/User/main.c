/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ʹ��USB�ӿڼ�����FLASHģ��U��
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
#include "./Bsp/led/bsp_led.h" 
#include "./Bsp/usart/bsp_debug_usart.h"
#include "./bsp/flash/bsp_spi_flash.h"

#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"


__ALIGN_BEGIN USB_OTG_CORE_HANDLE     USB_OTG_dev __ALIGN_END ;

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	__IO uint32_t i = 0;
	
	
	/* ��ʼ��LED */
	LED_GPIO_Config();
	
	/* ��ʼ�����Դ��ڣ�һ��Ϊ����1 */
	Debug_USART_Config();

	printf("SPIFlash�ļ�ϵͳģ��U�̲��Գ���\n");
	
	  /*!< At this stage the microcontroller clock setting is already configured, 
  this is done through SystemInit() function which is called from startup
  file (startup_stm32fxxx_xx.s) before to branch to application main.
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32fxxx.c file
  */ 
	
  USBD_Init(&USB_OTG_dev,
            USB_OTG_FS_CORE_ID,
            &USR_desc,
            &USBD_MSC_cb, 
            &USR_cb);
	
	while(1)
	{
		if (i++ == 0x1000000)
    {
      LED3_TOGGLE;
      i = 0;
    }    
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
