#include "bsp_mcooutput.h"
#include "stm32f10x_rcc.h"

/*
 * ��ʼ��MCO����PA8
 * ��F1ϵ����MCO����ֻ��һ������PA8����F4ϵ���У�MCO���Ż�������
 */
void MCO_GPIO_Config(void)
{
  // 1.����GPIO��ʼ���ṹ��
	GPIO_InitTypeDef GPIO_InitStructure;

	// ����GPIOA��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// ѡ��MCO���������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	
	// ����Ϊ���ù����������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	
	//����IO�ķ�ת����Ϊ50M
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	// ���ù̼����ʼ��GPIOA8
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
