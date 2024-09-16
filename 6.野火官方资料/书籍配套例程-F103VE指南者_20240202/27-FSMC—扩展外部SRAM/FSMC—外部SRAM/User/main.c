/**
  ******************************************************************************
  * @file    FSMC���ⲿSRAM
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   sramӦ������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32  ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./led/bsp_led.h"  
#include "./sram/sram.h"



/*���Զ�λ��ʽ����SRAM,���ַ�ʽ���붨���ȫ�ֱ���*/
uint8_t testValue __attribute__((at(Bank1_SRAM3_ADDR)));



 /**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
 int main(void)
 {	     
	 
	LED_GPIO_Config();
	//���ڳ�ʼ��  
	Debug_USART_Config();		 	                       
  //��ʼ���ⲿSRAM  
  FSMC_SRAM_Init();			     
	 
	printf ( "\r\nҰ���ⲿ SRAM ����\r\n" );
	  	
  
  /*����������ʾ���ڶ�дSRAM����*/
  LED_BLUE;
  
  /*��SRAM���ж�д���ԣ����SRAM�Ƿ�����*/
  if(SRAM_Test()==1)
  {
		//�������� �̵���
    LED_GREEN;			  
  }
	else
	{
		//����ʧ�� �����
		LED_RED;
	}
	
	/*ָ�뷽ʽ����SRAM*/
	{	
	 uint32_t temp;
	
	 printf("\r\nָ�뷽ʽ����SRAM\r\n");
	/*��SRAMд��8λ����*/
	 *( uint8_t*) (Bank1_SRAM3_ADDR ) = (uint8_t)0xAA;
	 printf("\r\nָ�����SRAM��д������0xAA \r\n");

	 /*��SRAM��ȡ����*/
	 temp =  *( uint8_t*) (Bank1_SRAM3_ADDR );
	 printf("��ȡ���ݣ�0x%X \r\n",temp);

	 /*д/�� 16λ����*/
	 *( uint16_t*) (Bank1_SRAM3_ADDR+10 ) = (uint16_t)0xBBBB;
	 printf("ָ�����SRAM��д������0xBBBB \r\n");
	 
	 temp =  *( uint16_t*) (Bank1_SRAM3_ADDR+10 );
	 printf("��ȡ���ݣ�0x%X \r\n",temp);


	 /*д/�� 32λ����*/
	 *( uint32_t*) (Bank1_SRAM3_ADDR+20 ) = (uint32_t)0xCCCCCCCC;
	 printf("ָ�����SRAM��д������0xCCCCCCCC \r\n");	 
	 temp =  *( uint32_t*) (Bank1_SRAM3_ADDR+20 );
	 printf("��ȡ���ݣ�0x%X \r\n",temp);

	}
	
	/*���Զ�λ��ʽ����SRAM,���ַ�ʽ���붨���ȫ�ֱ���*/
	{
		testValue = 0xDD;
		printf("\r\n���Զ�λ����SRAM��д������0xDD,��������0x%X,������ַΪ%X\r\n",testValue,(uint32_t )&testValue);	 
	}
	
	 
	 
}


/*********************************************END OF FILE**********************/
