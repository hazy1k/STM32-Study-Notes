/**
  ******************************************************************************
  * @file    main.c
  * @author  Ұ��
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   emWin��������������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"
#include "GUI.h"
#include "..\User\usart\bsp_usart1.h"
#include "..\User\SysTick\bsp_SysTick.h"
#include "..\User\led\bsp_led.h"
#include "..\User\lcd\bsp_xpt2046_lcd.h"
#include "..\User\lcd\bsp_ili9341_lcd.h"

extern void Touch_MainTask(void);
/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	
  
  /* ��ʼ����ʱ�� */
	SysTick_Init();
	
	XPT2046_Init();
	
	/*CRC��emWinû�й�ϵ��ֻ������Ϊ�˿�ı��������ģ�����STemWin�Ŀ�ֻ������ST��оƬ���棬���оƬ���޷�ʹ�õġ� */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	
	/* ��ʼ��GUI */
  ILI9341_Init(); /* ��ǰ��ʼ��LCD��������GUI_Init����LCD_X_Config֮ǰȷ��LCD����оƬID: lcdid */
	GUI_Init();
	
	/* ��ʼ������ */
	USART1_Config();
	
	/* ���ڵ�����Ϣ */
	printf("emWin touch demo\r\n");

	Touch_MainTask();

}


/*********************************************END OF FILE**********************/
