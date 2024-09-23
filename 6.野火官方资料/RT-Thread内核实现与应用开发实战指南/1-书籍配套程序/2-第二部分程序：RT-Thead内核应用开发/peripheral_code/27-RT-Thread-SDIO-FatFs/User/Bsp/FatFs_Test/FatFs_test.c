/**
  ******************************************************************************
  *                              ͷ�ļ�
  ******************************************************************************
  */
#include "./FatFs_Test/FatFs_test.h"

/* RT-Thread���ͷ�ļ� */
#include <rthw.h>
#include <rtthread.h>
/*
*************************************************************************
*                               ����
*************************************************************************
*/
FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FIL fnew;													/* �ļ����� */
FRESULT res_sd;                /* �ļ�������� */
UINT fnum;            					  /* �ļ��ɹ���д���� */
BYTE ReadBuffer[1024]={0};        /* �������� */
BYTE WriteBuffer[] =              /* д������*/
"��ӭʹ��Ұ��STM32������ �����Ǹ������ӣ��½��ļ�ϵͳ�����ļ�\r\n";  



/**
  ******************************************************************
  * @brief   �ļ�ϵͳ��ʼ��
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  ******************************************************************
  */ 
void FileSystem_Init(void)
{
	/* �������������ʹ��rtt �� rt_kprintf��ӡ
		 ֻ��ʹ��printf	*/
	
	/* ���жϣ�������ӻ�����������board.c�н��г�ʼ��
		 �������жϣ��������г�ʼ����ʱ������Ҫ���ж� */
//	rt_base_t level = 0;
//	rt_hw_interrupt_enable(level);	

	//���ⲿSPI Flash�����ļ�ϵͳ���ļ�ϵͳ����ʱ���SPI�豸��ʼ��
	res_sd = f_mount(&fs,"0:",1);
	
	if(res_sd == FR_NO_FILESYSTEM)
	{
		printf("��SD����û���ļ�ϵͳ���������и�ʽ��...\r\n");
    /* ��ʽ�� */
		res_sd=f_mkfs("0:",0,0);							
		
		if(res_sd == FR_OK)
		{
			printf("��SD���ѳɹ���ʽ���ļ�ϵͳ��\r\n");
      /* ��ʽ������ȡ������ */
			res_sd = f_mount(NULL,"0:",1);			
      /* ���¹���	*/			
			res_sd = f_mount(&fs,"0:",1);
		}
		else
		{
//			LED_RED;
			printf("������ʽ��ʧ�ܡ�����\r\n");
			while(1);
		}
	}
  else if(res_sd!=FR_OK)
  {
    printf("����SD�������ļ�ϵͳʧ�ܡ�(%d)\r\n",res_sd);
    printf("��������ԭ��SD����ʼ�����ɹ���\r\n");
		while(1);
  }
  else
  {
    printf("���ļ�ϵͳ���سɹ������Խ��ж�д����\r\n");
  }
	
	/* �ر��жϣ�������ӻ�����������board.c�н��г�ʼ��
		 �����ر��жϣ��������г�ʼ����ʱ������Ҫ�ر��ж� */
//		rt_hw_interrupt_disable();
}

/**
  ******************************************************************
  * @brief   �ļ�ϵͳ��д����
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  ******************************************************************
  */ 
void FileSystem_Test(void)
{
	/*----------------------- �ļ�ϵͳ���ԣ�д���� -----------------------------*/
	/* ���ļ�������ļ��������򴴽��� */
	rt_kprintf("\r\n****** ���������ļ�д�����... ******\r\n");	
	res_sd = f_open(&fnew, "0:FatFs��д�����ļ�.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if ( res_sd == FR_OK )
	{
		rt_kprintf("����/����FatFs��д�����ļ�.txt�ļ��ɹ������ļ�д�����ݡ�\r\n");
    /* ��ָ���洢������д�뵽�ļ��� */
		res_sd=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
    if(res_sd==FR_OK)
    {
      rt_kprintf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
      rt_kprintf("�����ļ�д�������Ϊ��\r\n%s\r\n",WriteBuffer);
    }
    else
    {
      rt_kprintf("�����ļ�д��ʧ�ܣ�(%d)\n",res_sd);
    }    
		/* ���ٶ�д���ر��ļ� */
    f_close(&fnew);
	}
	else
	{	
//		LED_RED;
		rt_kprintf("������/�����ļ�ʧ�ܡ�\r\n");
	}
	
/*------------------- �ļ�ϵͳ���ԣ������� ------------------------------------*/
	rt_kprintf("****** ���������ļ���ȡ����... ******\r\n");
	res_sd = f_open(&fnew, "0:FatFs��д�����ļ�.txt", FA_OPEN_EXISTING | FA_READ); 	 
	if(res_sd == FR_OK)
	{
//		LED_GREEN;
		rt_kprintf("�����ļ��ɹ���\r\n");
		res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(res_sd==FR_OK)
    {
      rt_kprintf("���ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\r\n",fnum);
      rt_kprintf("����ȡ�õ��ļ�����Ϊ��\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      rt_kprintf("�����ļ���ȡʧ�ܣ�(%d)\n",res_sd);
    }	
		/* ���ٶ�д���ر��ļ� */
		f_close(&fnew);	
	}
	else
	{
//		LED_RED;
		rt_kprintf("�������ļ�ʧ�ܡ�\r\n");
	}
//	/* ���ٶ�д���ر��ļ� */
//	f_close(&fnew);	
  
//	/* ����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ */
//	f_mount(NULL,"0:",1);
	
}


