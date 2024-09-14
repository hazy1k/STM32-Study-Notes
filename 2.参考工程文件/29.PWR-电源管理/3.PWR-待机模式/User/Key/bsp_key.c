#include "./key/bsp_key.h"  

// �������Ŷ���
void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// ���������˿ڵ�ʱ��
	RCC_APB2PeriphClockCmd(KEY2_GPIO_CLK,ENABLE);

// Ϊǿ��PA0��������ʱ�����ʼ�����˴�ע�͵�
// ��PA0��STM32����������ʱ��Ҫ���������⣬��Ӧ���ϳ�ʼ��	
//	//ѡ�񰴼�������
//	GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;  
//	// ���ð���������Ϊ��������
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
//	//ʹ�ýṹ���ʼ������
//	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
	
	// ѡ�񰴼�������
	GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN; 
	// ���ð���������Ϊ��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	// ʹ�ýṹ���ʼ������
	GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);	
}

// ����ɨ�躯��
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{			
	// ����Ƿ��а������� 
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )  
	{	 
		// �ȴ������ͷ� 
		while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON);   
		return 	KEY_ON;	 
	}
	else
		return KEY_OFF;
}