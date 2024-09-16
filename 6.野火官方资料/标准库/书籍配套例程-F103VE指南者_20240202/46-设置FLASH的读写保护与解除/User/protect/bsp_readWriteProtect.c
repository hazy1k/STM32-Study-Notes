/**
  ******************************************************************************
  * @file    bsp_internalFlash.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   �ڲ�FLASH��д�������Է���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 ָ���� ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./protect/bsp_readWriteProtect.h"   
#include "./usart/bsp_usart.h"



/**
  * @brief  ��תд���������ã�������ʾ
	          ��оƬ����д����״̬��������
						������д����״̬�������ó�д����
  * @param  ��
  * @retval ��
  */
void WriteProtect_Toggle(void)
{
	/* ��ȡд�����Ĵ�����ֵ�����жϣ��Ĵ���λΪ0��ʾ�б�����Ϊ1��ʾ�ޱ��� */
	/*  ��������0xFFFFFFFF����˵���в���ҳ��д������ */
	if(FLASH_GetWriteProtectionOptionByte() != 0xFFFFFFFF )
	{
			FLASH_DEBUG("оƬ����д����״̬������ִ�нⱣ������...");
			
			//�����FLASH_CR�Ĵ����ķ�������
			FLASH_Unlock();

			/* ��������ѡ���ֽڵ����� */
			FLASH_EraseOptionBytes();

			/* ������ҳ��� */
			FLASH_EnableWriteProtection(0x00000000);
			
			FLASH_DEBUG("������ɣ�оƬ���Զ���λ���������ã���λ��оƬ����д����״̬\r\n");

			/* ��λоƬ����ʹѡ���ֽ���Ч */
			NVIC_SystemReset();
	}
	else //��д����
	{
			FLASH_DEBUG("оƬ������д����״̬������ִ��д��������...");
			
			//�����FLASH_CR�Ĵ����ķ�������
			FLASH_Unlock();

			/* �Ȳ�������ѡ���ֽڵ����ݣ���ֹ��Ϊԭ�е�д���������޷�д���µı������� */
			FLASH_EraseOptionBytes();

			/* ������ҳ����д���� */
			FLASH_EnableWriteProtection(FLASH_WRProt_AllPages);

			FLASH_DEBUG("������ɣ�оƬ���Զ���λ���������ã���λ��оƬ�ᴦ��д����״̬\r\n");

			/* ��λоƬ����ʹѡ���ֽ���Ч */
			NVIC_SystemReset();		
	}

}


/**
  * @brief  ��ת�����������ã�������ʾ
	          ��оƬ���ڶ�����״̬��������
						�����Ƕ�����״̬�������óɶ�����
  * @param  ��
  * @retval ��
  */
void ReadProtect_Toggle(void)
{
	if(FLASH_GetReadOutProtectionStatus () == SET )
	{
		FLASH_DEBUG("оƬ���ڶ�����״̬\r\n");
		
		//�����FLASH_CR�Ĵ����ķ�������
		FLASH_Unlock();
		
		FLASH_DEBUG("���������������������������FLASH�������������");
		FLASH_DEBUG("���ڽ���������գ����Ժ��治�����κ���ʾ���");
		FLASH_DEBUG("�ȴ�20��󼴿ɸ�оƬ�����µĳ���...\r\n");
		
		FLASH_ReadOutProtection (DISABLE);		

		//��ʹ�ڴ˴�����printf���ڵ���Ҳ����ִ�еģ���Ϊ�洢�������ƬFLASH���ѱ�������
		FLASH_DEBUG("����FLASH������գ����Ա����벻�ᱻִ�У����ڲ����б���������SRAM����ģʽ�����⣩\r\n");

	}
	else
	{
		FLASH_DEBUG("оƬ�����޶�����״̬������ִ�ж���������...\r\n");
		
		//�����FLASH_CR�Ĵ����ķ�������
		FLASH_Unlock();				

		FLASH_ReadOutProtection (ENABLE);
		
		printf("оƬ�ѱ�����Ϊ���������ϵ縴λ����Ч���������¸��������ϵ磬ֻ����λ����Ч��\r\n");
		printf("���ڱ���״̬���޷����������³��򣬱���Ҫ�Ƚ������״̬������\r\n");

	}
}






