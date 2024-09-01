/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
	*/
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_spi_flash.h"
#include "bsp_led.h" 
#include "ff.h"
#include  "VS1053.h"	
#include "bsp_SysTick.h"
//#include "bsp_exti.h"
#include "bsp_key.h"
#include "recorder.h"

FATFS fs;													/* Work area (file system object) for logical drives */

/**
  * @brief  
  * @param  ��
  * @retval ��
 */
int main(void)
{
	SysTick_Init();
	
	/* ��ʼ������ */
	USART1_Config();
	f_mount(&fs,"0:",1);

	/* ��ʼ��LED */
	LED_GPIO_Config();
	
  /* ��ʼ������ */
	//EXTI_Key_Config();
  Key_GPIO_Config();

	/* ��ʼ��VS1053��IO�� */
	VS_Init();
	
	/* ��ӡ���Խ�� */
	printf("vs1053:%4X\n",VS_Ram_Test());
	
	/* ��ʱ100���� */
	Delay_ms(100);
	
	/* ���Ҳ���  */
	VS_Sine_Test();	
	
	/* Ӳ��λMP3 */
	VS_HD_Reset();
	
	/* ��λVS10XX */
	VS_Soft_Reset();
	
	while(1)
	{
		Recorder_Run();
	}
}


/******************* end of file**************************/

