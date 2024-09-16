
/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ֹͣģʽ����ʵ��
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
#include "./led/bsp_led.h"
#include "./key/bsp_exti.h" 
#include "./usart/bsp_usart.h"



static void Delay(__IO u32 nCount);
static void SYSCLKConfig_STOP(void);


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */ 
int main(void)
{	

	RCC_ClocksTypeDef clock_status_wakeup,clock_status_config;
	uint8_t clock_source_wakeup,clock_source_config; 

	
	LED_GPIO_Config();	
	
  /*��ʼ��USART1*/
  USART_Config();		
	
	/* ��ʼ�������жϣ����°����������жϷ������	*/
	EXTI_Key_Config(); 
	
	printf("\r\n ��ӭʹ��Ұ��  STM32  �����塣\r\n");
  printf("\r\n Ұ��STM32 ֹͣģʽ����\r\n");
	
	printf("\r\n ʵ��˵����\r\n");

	printf("\r\n 1.�������У��̵Ʊ�ʾSTM32�������У���Ʊ�ʾֹͣ״̬�����Ʊ�ʾ�մ�ֹͣ״̬������\r\n");
	printf("\r\n 2.��ֹͣ״̬�£���ʹ��KEY1��KEY2����\r\n");
	printf("\r\n 3.��ʵ��ִ������һ��ѭ����\r\n ------�����̵�(��������)->�����(ֹͣģʽ)->��KEY1��KEY2����->������(�ձ�����)-----��\r\n");
	printf("\r\n 4.��ֹͣ״̬�£�DAP�������޷���STM32���س���\r\n �ɰ�KEY1��KEY2���Ѻ����أ�\r\n �򰴸�λ��ʹоƬ���ڸ�λ״̬��Ȼ���ڵ����ϵ�����ذ�ť�����ͷŸ�λ��������������\r\n");

  while(1)
  {	
		/*********ִ������***************************/
		printf("\r\n STM32�������У����̵�\r\n");
	
		LED_GREEN;	
		Delay(0x3FFFFF);
		
		/*****����ִ����ϣ�����ֹͣ���͹���***********/		
		
		printf("\r\n ����ֹͣģʽ����KEY1��KEY2�����ɻ���\r\n");

		//ʹ�ú��ָʾ������ֹͣ״̬
		LED_RED;
		
		/* ����ֹͣģʽ�����õ�ѹ������Ϊ�͹���ģʽ���ȴ��жϻ��� */
		PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);	
		
	
		//�ȴ��жϻ���  K1��K2�����ж�	
		
		/*********************������***********************/
		
		//��ȡ�ձ�����ʱ��ʱ��״̬	
		//ʱ��Դ
		clock_source_wakeup = RCC_GetSYSCLKSource ();
		//ʱ��Ƶ��
		RCC_GetClocksFreq(&clock_status_wakeup);
		
		//��ֹͣģʽ�±����Ѻ�ʹ�õ���HSIʱ�ӣ��˴�����HSEʱ��,ʹ��PLLCLK
		SYSCLKConfig_STOP();
		
		//��ȡ�������ú��ʱ��״̬	
		//ʱ��Դ
		clock_source_config = RCC_GetSYSCLKSource ();
		//ʱ��Ƶ��
		RCC_GetClocksFreq(&clock_status_config);
	
		//��Ϊ�ջ��ѵ�ʱ��ʹ�õ���HSIʱ�ӣ���Ӱ�촮�ڲ����ʣ�������ԣ���������������ʱ��Դ���ʹ�ô��������
		printf("\r\n�������ú��ʱ��״̬��\r\n");
		printf(" SYSCLKƵ��:%d,\r\n HCLKƵ��:%d,\r\n PCLK1Ƶ��:%d,\r\n PCLK2Ƶ��:%d,\r\n ʱ��Դ:%d (0��ʾHSI��8��ʾPLLCLK)\n", 
			clock_status_config.SYSCLK_Frequency, 
			clock_status_config.HCLK_Frequency, 
			clock_status_config.PCLK1_Frequency, 
			clock_status_config.PCLK2_Frequency, 
			clock_source_config);
			
		printf("\r\n�ջ��ѵ�ʱ��״̬��\r\n");	
		printf(" SYSCLKƵ��:%d,\r\n HCLKƵ��:%d,\r\n PCLK1Ƶ��:%d,\r\n PCLK2Ƶ��:%d,\r\n ʱ��Դ:%d (0��ʾHSI��8��ʾPLLCLK)\n", 
			clock_status_wakeup.SYSCLK_Frequency, 
			clock_status_wakeup.HCLK_Frequency, 
			clock_status_wakeup.PCLK1_Frequency, 
			clock_status_wakeup.PCLK2_Frequency, 
			clock_source_wakeup);
		
		/*ָʾ��*/
		LED_BLUE;	
		Delay(0x1FFFFF);		
			
		printf("\r\n ���˳�ֹͣģʽ\r\n");
		//����ִ��whileѭ��

  }
}


static void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}



/**
  * @brief  ͣ�����Ѻ�����ϵͳʱ��: ʹ�� HSE, PLL
  *         ����ѡ��PLL��Ϊϵͳʱ��.
  * @param  None
  * @retval None
  */
static void SYSCLKConfig_STOP(void)
{
  /* After wake-up from STOP reconfigure the system clock */
  /* ʹ�� HSE */
  RCC_HSEConfig(RCC_HSE_ON);
  
  /* �ȴ� HSE ׼������ */
  while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {
  }
  
  /* ʹ�� PLL */ 
  RCC_PLLCmd(ENABLE);
  
  /* �ȴ� PLL ׼������ */
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {
  }
  
  /* ѡ��PLL��Ϊϵͳʱ��Դ */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
  /* �ȴ�PLL��ѡ��Ϊϵͳʱ��Դ */
  while (RCC_GetSYSCLKSource() != 0x08)
  {
  }
}
/*********************************************END OF FILE**********************/
