/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ��������ʵ��
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
#include "./key/bsp_key.h" 
#include "./usart/bsp_usart.h"


static void Delay(__IO u32 nCount);
static uint8_t KEY2_LongPress(void);


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */ 
int main(void)
{	
		/* ʹ�ܵ�Դ����Ԫ��ʱ��,����Ҫʹ��ʱ�Ӳ��ܽ������ģʽ */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR , ENABLE);

	LED_GPIO_Config();	
	
  /*��ʼ��USART1*/
  USART_Config();			

	/*��ʼ������������Ҫ�ж�,����ʼ��KEY2���ɣ�ֻ���ڻ��ѵ�PA0���Ų���Ҫ������ʼ��*/
	Key_GPIO_Config();   
	
	printf("\r\n ��ӭʹ��Ұ��  STM32  �����塣\r\n");
  printf("\r\n Ұ��STM32 ����ģʽ����\r\n");
	
	printf("\r\n ʵ��˵����\r\n");

	printf("\r\n 1.�������У��̵Ʊ�ʾ���θ�λ���ϵ�����Ÿ�λ����Ʊ�ʾ�����������״̬�����Ʊ�ʾ�����Ǵ������ѵĸ�λ\r\n");
	printf("\r\n 2.����KEY2�����󣬻�������ģʽ\r\n");
	printf("\r\n 3.�ڴ���ģʽ�£���KEY1�����ɻ��ѣ����Ѻ�ϵͳ����и�λ�������ͷ��ʼִ��\r\n");
	printf("\r\n 4.��ͨ�����WU��־λȷ����λ��Դ\r\n");
	
	printf("\r\n 5.�ڴ���״̬�£�DAP�������޷���STM32���س�����Ҫ���Ѻ��������");

	
	//��⸴λ��Դ
	if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET)
	{
		LED_BLUE;
		printf("\r\n �������Ѹ�λ \r\n");
	}
	else
	{
		LED_GREEN;
		printf("\r\n �Ǵ������Ѹ�λ \r\n");
	}
	
  while(1)
  {			
		// K2 ���������������ģʽ
		if(KEY2_LongPress())
		{
			
			printf("\r\n �����������ģʽ���������ģʽ��ɰ�KEY1���ѣ����Ѻ����и�λ�������ͷ��ʼִ��\r\n");
			LED_RED;	
			Delay(0xFFFFFF);
			
			/*���WU״̬λ*/
			PWR_ClearFlag (PWR_FLAG_WU);
			
			/* ʹ��WKUP���ŵĻ��ѹ��� ��ʹ��PA0*/
			PWR_WakeUpPinCmd (ENABLE);
			
			/* �������ģʽ */
			PWR_EnterSTANDBYMode();
		}

  }
}


static void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}



 /**
  * @brief  ���ڼ�ⰴ���Ƿ񱻳�ʱ�䰴��
  * @param  ��
  * @retval 1 ����������ʱ�䰴��  0 ������û�б���ʱ�䰴��
  */
static uint8_t KEY2_LongPress(void)
{			
	uint8_t downCnt =0;																				//��¼���µĴ���
	uint8_t upCnt =0;																					//��¼�ɿ��Ĵ���			

	while(1)																										//��ѭ������return����
	{	
		Delay(0x2FFFF);																					//�ӳ�һ��ʱ���ټ��

		if(GPIO_ReadInputDataBit ( KEY2_GPIO_PORT, KEY2_GPIO_PIN ) == SET)	//��⵽���°���
		{
			downCnt++;																					//��¼���´���
			upCnt=0;																						//��������ͷż�¼

			if(downCnt>=100)																		//����ʱ���㹻
			{
				return 1; 																				//��⵽������ʱ�䳤����
			}
		}
		else 
		{
			upCnt++; 																						//��¼�ͷŴ���
			if(upCnt>5)																					//������⵽�ͷų���5��
			{
				return 0;																				//����ʱ��̫�̣����ǰ�����������
			}
		}//	if(GPIO_ReadInputDataBit 
	}//while
}
/*********************************************END OF FILE**********************/
