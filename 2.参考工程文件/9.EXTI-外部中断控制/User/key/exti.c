#include "exti.h"

// ��������NVIC
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // ����������
  NVIC_InitStructure.NVIC_IRQChannel = KEY1_EXTI_IRQ; // �����ж�Դ
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // ������ռ���ȼ�
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // ���������ȼ�
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // ʹ���ж�
  NVIC_Init(&NVIC_InitStructure); // ��ʼ��

  // �����ж�Դ2��KEY2
  NVIC_InitStructure.NVIC_IRQChannel = KEY2_EXTI_IRQ; // �����ж�Դ
  NVIC_Init(&NVIC_InitStructure); // �������������
}

// EXTI��ʼ��
void EXTI_KeyInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK|KEY2_GPIO_CLK, ENABLE);
  NVIC_Configuration(); // ����NVIC
  // ����GPIO
  GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��������ģʽ
  GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
  // ����EXTI
  GPIO_EXTILineConfig(KEY1_EXTI_PORTSOURCE, KEY1_EXTI_PINSOURCE);
  EXTI_InitStructure.EXTI_Line = KEY1_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; // �ж�ģʽ
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // �����ش���
  EXTI_InitStructure.EXTI_LineCmd = ENABLE; // ʹ���ж���
  EXTI_Init(&EXTI_InitStructure); // ��ʼ��

  GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN;
  GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);
  GPIO_EXTILineConfig(KEY2_EXTI_PORTSOURCE, KEY2_EXTI_PINSOURCE);
  EXTI_InitStructure.EXTI_Line = KEY2_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // �����ش���
  EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
  EXTI_Init(&EXTI_InitStructure);
}
