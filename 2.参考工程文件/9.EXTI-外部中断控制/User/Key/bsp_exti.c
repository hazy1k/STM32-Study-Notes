#include "bsp_exti.h"

// ����Ƕ�������жϿ�����NVIC
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  // 1.����NVICΪ���ȼ���1
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // ʹ�ÿ⺯��misc.C�еĺ���-NVIC_PriorityGroupConfig()����NVICΪ���ȼ���1
  
  // 2.�����ж�Դ������1
  NVIC_InitStructure.NVIC_IRQChannel = KEY1_INT_EXTI_IRQ; // ʵ������ʹ�õ���EXTI_IRQn

  // 3.�����ж������ȼ���1
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;

  // 4.���������ȼ���1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

  // 5.ʹ���ж�ͨ��
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  // 6.�����ж�Դ������2������ʹ�������������
  NVIC_InitStructure.NVIC_IRQChannel = KEY2_INT_EXTI_IRQ;
  NVIC_Init(&NVIC_InitStructure);
}

// ����IOΪEXTI�жϿڣ��������ж����ȼ�
void EXTI_Key_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;  // ����GPIO��ʼ���ṹ�����
	EXTI_InitTypeDef EXTI_InitStructure; // ����EXTI��ʼ���ṹ�����

	// ʹ��GPIOʱ��
	RCC_APB2PeriphClockCmd(KEY1_INT_GPIO_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(KEY2_INT_GPIO_CLK, ENABLE);
												
	// ����NVIC�ж�
	NVIC_Configuration();
	
/*--------------------------KEY1����-----------------------------*/
	// 1.ѡ�񰴼��õ���GPIO����
  GPIO_InitStructure.GPIO_Pin = KEY1_INT_GPIO_PIN;

  // 2.����Ϊ��������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStructure);

	// 3.ѡ��EXTI���ź�Դ
  GPIO_EXTILineConfig(KEY1_INT_EXTI_PORTSOURCE, KEY1_INT_EXTI_PINSOURCE); 
  EXTI_InitStructure.EXTI_Line = KEY1_INT_EXTI_LINE;
	
	// 4.����EXTIΪ�ж�ģʽ
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;

	// 5.�������ж�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;

  // 6.ʹ���ж�	
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
  /*--------------------------KEY2����-----------------------------*/
	/* ѡ�񰴼��õ���GPIO */	
  GPIO_InitStructure.GPIO_Pin = KEY2_INT_GPIO_PIN;
  /* ����Ϊ�������� */	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(KEY2_INT_GPIO_PORT, &GPIO_InitStructure);

	/* ѡ��EXTI���ź�Դ */
  GPIO_EXTILineConfig(KEY2_INT_EXTI_PORTSOURCE, KEY2_INT_EXTI_PINSOURCE); 
  EXTI_InitStructure.EXTI_Line = KEY2_INT_EXTI_LINE;
	
	/* EXTIΪ�ж�ģʽ */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	/* �½����ж� */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  /* ʹ���ж� */	
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}
