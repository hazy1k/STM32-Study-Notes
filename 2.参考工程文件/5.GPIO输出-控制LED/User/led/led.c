#include "led.h"   

void LED_GPIO_Config(void)
{		
		// 1.����ṹ��-����·��
		GPIO_InitTypeDef GPIO_InitStructure;

		// 2.��������GIPO��ʱ��Ҳ����APB2��
		RCC_APB2PeriphClockCmd( LED1_GPIO_CLK | LED2_GPIO_CLK | LED3_GPIO_CLK, ENABLE);
	
		// 3.ѡ������Ҫ���Ƶ�����
		GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;	
	
		// 4.��������ģʽΪͨ���������
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	
		// 5.������������Ϊ50MHz   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		// 6.���ÿ⺯������ʼ��GPIO
		GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);	
		
	    // ��ˣ����Ǿ������GPIO������LED1���ĳ�ʼ�������ˣ���ôLED2��LED3�أ�-�պ�«��ư

		/*ѡ��Ҫ���Ƶ�GPIO����*/
		GPIO_InitStructure.GPIO_Pin = LED2_GPIO_PIN;
	
		/*���ÿ⺯������ʼ��GPIO*/
		GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);
		
		/*ѡ��Ҫ���Ƶ�GPIO����*/
		GPIO_InitStructure.GPIO_Pin = LED3_GPIO_PIN;

		/*���ÿ⺯������ʼ��GPIO*/
		GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStructure);

		// ��ʼ��ʱ�ر�����led��
		GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);
		GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);	 
		GPIO_SetBits(LED3_GPIO_PORT, LED3_GPIO_PIN);
}
