 /**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ���� 8M����flash���ԣ�����������Ϣͨ������1�ڵ��Եĳ����ն��д�ӡ����
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
#include "./usart/bsp_usart.h"
#include "./key/bsp_key.h"  
#include "./led/bsp_led.h"
#include "./protect/bsp_readWriteProtect.h"   


// ����ԭ������
void Delay(__IO uint32_t nCount);





//�� ������ע�����
//1.��оƬ���ڶ�д����״̬ʱ�����޷������µĳ�����Ҫ�Ƚ������״̬��������
//2.�����̰��������汾������MDK�ġ�Load�����ذ�ť�Աߵ�������ѡ��
//	FLASH�汾��	���ϴ��ڵ������ֺ�ֱ�ӵ��MDK�ġ�Load����ť�ѳ������ص�STM32��FLASH�У�
//				��λ���У����ڻ������ǰоƬ�ı���״̬����ʹ��KEY1��KEY2�л����л�д����
//				״̬ʱ��оƬ���Զ���λ����������ִ�У��л�������״̬ʱ����������Ҫ���¸�
//				�������ϵ縴λ�����òŻ���Ч���ϵ�ʱ����������Ե����ӻ�Ͽ��������ϵ��
//				ע�����´򿪴��ڵ������֣�������ִ�н�����������̣����к�оƬFLASH������
//				�Ĵ��붼����ʧ������Ҫ���¸����������س���
//	RAM�汾  ��	����SRAM���Գ���ľ��飬����ѧϰǰ��ġ�SRAM���ԡ��½ڡ����ϴ��ڵ������ֺ�
//				ֻ��ʹ��MDK�ġ�Debug����ť�ѳ������ص�STM32���ڲ�SRAM�У�Ȼ����ȫ�����У�
//				���ڴ��ڲ鿴�������������SRAM����״̬�£���λ��ʹоƬ�����ҷɣ�����ÿ����
//				��״̬�󣬶�Ҫ���µ����Debug����ť����SRAM������ȫ�����в鿴�����


//3.���Լ��޸ĳ�����ʹоƬ���ڶ�д����״̬���޷����أ�
//  �� FALSH���������ֲ������Խ��״̬�ĳ��򣬿���ʹ�ñ����̵ġ�RAM�汾���������������������ء�


/*
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  ����
 */
int main(void)
{ 	
	  /*��ʼ��USART������ģʽΪ 115200 8-N-1*/
  USART_Config();
	LED_GPIO_Config();
	Key_GPIO_Config();

	LED_BLUE;
	
	//оƬ�Զ���λ�󣬴��ڿ�����С�����쳣����������һ����������
	printf("\r\n��ӭʹ��Ұ��  STM32  �����塣\r\n");	
	printf("���Ƕ�д��������ʵ��\r\n");
	
	/* ��ȡд�����Ĵ�����ֵ�����жϣ��Ĵ���λΪ0��ʾ�б�����Ϊ1��ʾ�ޱ��� */
	/*  ��������0xFFFFFFFF����˵���в���ҳ��д������ */
	if(FLASH_GetWriteProtectionOptionByte() !=0xFFFFFFFF )
	{
		printf("\r\nĿǰоƬ����д����״̬����Key1���������\r\n");
		printf("д�����Ĵ�����ֵ��WRPR=0x%x\r\n",FLASH_GetWriteProtectionOptionByte());
	}
	else //��д����
	{
		printf("\r\nĿǰоƬ�� д �������� Key1 �������ó� д ����\r\n");
		printf("д�����Ĵ�����ֵ��WRPR=0x%x\r\n",FLASH_GetWriteProtectionOptionByte());
	}
	
	
	/*  ������SET��˵�����ڶ�����״̬ */
	if(FLASH_GetReadOutProtectionStatus () == SET )
	{
		printf("\r\nĿǰоƬ���ڶ�����״̬����Key2���������\r\n");
	}
	else
	{
		printf("\r\nĿǰоƬ�� �� �������� Key2 �������ó� �� ����\r\n");
	}
	
	
	while(1)                            
	{	   
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
			LED1_TOGGLE;
			WriteProtect_Toggle();
		} 

		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
			LED2_TOGGLE;
			ReadProtect_Toggle();			
		}		
	}	
 
}


void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
