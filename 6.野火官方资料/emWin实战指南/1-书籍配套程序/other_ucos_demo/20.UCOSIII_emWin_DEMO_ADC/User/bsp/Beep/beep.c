#include "..\User\bsp\Beep\beep.h" 

void Beep_Init ( void )
{		
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	
	/*����GPIOB��GPIOF������ʱ��*/
	macBEEP_GPIO_APBxClock_FUN  ( macBEEP_GPIO_CLK, ENABLE ); 

	/*ѡ��Ҫ���Ƶ�GPIOG����*/															   
	GPIO_InitStructure.GPIO_Pin = macBEEP_PIN;	

	/*��������ģʽΪͨ���������*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*������������Ϊ50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*���ÿ⺯������ʼ��GPIOG6*/
	GPIO_Init ( macBEEP_PORT, & GPIO_InitStructure );

	/* �ر�*/
	macBEEP_OFF ();
}
/*********************************************END OF FILE**********************/
