/*
 *  ģ�� SPI Ӧ�� BSP
 */

#include "bsp_gpio_spi.h"


/*-------------------------------------------------------------------------------------------------------
 * XPT 2046 IRQ ��������
 * ��IRQ  ��������������ʱ����Ҫ�޸��������������
 * ͬʱstm32f10x_it.c�е��жϷ�����ҲӦ�޸�
 */
#if 0
static void TP_INT_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 

    /*ʹ��EXTI4 �ж� */		      
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn  ;    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    
}
static void TP_INT_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/* config the extiline clock and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
												
	/* config the NVIC */
	TP_INT_NVIC_Config();

	/* EXTI line gpio config */	
  GPIO_InitStructure.GPIO_Pin = TP_INT_PIN;       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 // ��������
  GPIO_Init(TP_INT_PORT, &GPIO_InitStructure);

	/* EXTI line mode config */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource6);  // PE4 
  EXTI_InitStructure.EXTI_Line = EXTI_Line6;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½����ж�
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure); 
}
#elif 1
static void TP_INT_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 

    /*ʹ��EXTI4 �ж� */		      
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn  ;    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    
}
static void TP_INT_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/* config the extiline clock and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO,ENABLE);
												
	/* config the NVIC */
	TP_INT_NVIC_Config();

	/* EXTI line gpio config */	
  GPIO_InitStructure.GPIO_Pin = TP_INT_PIN;       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 // ��������
  GPIO_Init(TP_INT_PORT, &GPIO_InitStructure);

	/* EXTI line mode config */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);  // PE4 
  EXTI_InitStructure.EXTI_Line = EXTI_Line4;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½����ж� 
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure); 
}
#endif 

/*------------------------------------------------------------------------------------------------------
 * ģ��SPI��GPIO���� ��SPI��4���ź��߻���������IOʱ��ֻ��Ҫ�޸ĸú���ͷ�ļ��ĺ궨�弴��
 * ���иó�ʼ�����������IO��ʱ��Ҳ��Ҫ�ı�
 */
void GPIO_SPI_Config(void) 
{ 
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* ����GPIOʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE| RCC_APB2Periph_GPIOD, ENABLE);
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  /* ģ��SPI GPIO��ʼ�� */          
  GPIO_InitStructure.GPIO_Pin=SPI_CLK_PIN;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz ;	  
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_Init(SPI_CLK_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PIN;
  GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = SPI_MISO_PIN; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      
  GPIO_Init(SPI_MISO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = SPI_CS_PIN; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      
  GPIO_Init(SPI_CS_PORT, &GPIO_InitStructure); 
   
  /* ����Ƭѡ��ѡ��XPT2046 */
  GPIO_ResetBits(SPI_CS_PORT,SPI_CS_PIN);
  //GPIO_SetBits(SPI_CS_PORT,SPI_CS_PIN);
  
  /* XPT2046 �ж�IO���� */
  TP_INT_GPIO_Config();
}

