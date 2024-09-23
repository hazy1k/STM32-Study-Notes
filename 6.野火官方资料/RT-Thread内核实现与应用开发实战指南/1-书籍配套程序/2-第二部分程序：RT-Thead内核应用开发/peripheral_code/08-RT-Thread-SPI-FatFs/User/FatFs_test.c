/**
  ******************************************************************************
  *                              ͷ�ļ�
  ******************************************************************************
  */
#include "board.h" 
/*
*************************************************************************
*                               ����
*************************************************************************
*/
FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FIL fnew;													/* �ļ����� */
FRESULT res_flash;                /* �ļ�������� */
UINT fnum;            					  /* �ļ��ɹ���д���� */
BYTE ReadBuffer[1024]={0};        /* �������� */
BYTE WriteBuffer[] =              /* д������*/
"\n��ӭʹ��Ұ��STM32�����壬�����Ǹ�������\n"	
"�ʺ�ѧϰҰ�𿪷�����RT-Thread������\n"	
"����SPI-FatFs�ļ�ϵͳ\n"	;
//"RT-Thread�������һţ�ƵĲ���ϵͳ\n"	
//"�������ţ�ƣ�Ұ��Ҳ�������һ����ӭ���ѧϰ\n"	
//"���ǵĹ����ǣ�http://www.firebbs.cn\n "	
//"RT-Thread�Ĺ����ǣ�https://www.rt-thread.org/\n"	
//"�����½��ļ�ϵͳ�����ļ����\n"; 


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
	/* ���жϣ�������ӻ�����������board.c�н��г�ʼ��
		 �������жϣ��������г�ʼ����ʱ������Ҫ���ж� */
	rt_base_t level = 0;
//	rt_hw_interrupt_enable(level);	
	level = rt_hw_interrupt_disable();

	//���ⲿSPI Flash�����ļ�ϵͳ���ļ�ϵͳ����ʱ���SPI�豸��ʼ��
	//��ʼ������������������
	//f_mount()->find_volume()->disk_initialize->SPI_FLASH_Init()
	res_flash = f_mount(&fs,"1:",1);
	
/*----------------------- ��ʽ������ -----------------*/  
	/* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
	if(res_flash == FR_NO_FILESYSTEM)
	{
		rt_kprintf("��FLASH��û���ļ�ϵͳ���������и�ʽ��...\r\n");
    /* ��ʽ�� */
		res_flash=f_mkfs("1:",0,0);							
		
		if(res_flash == FR_OK)
		{
			rt_kprintf("��FLASH�ѳɹ���ʽ���ļ�ϵͳ��\r\n");
      /* ��ʽ������ȡ������ */
			res_flash = f_mount(NULL,"1:",1);			
      /* ���¹���	*/			
			res_flash = f_mount(&fs,"1:",1);
		}
		else
		{
			LED_RED;
			rt_kprintf("������ʽ��ʧ�ܡ�����\r\n");
			while(1);
		}
	}
  else if(res_flash!=FR_OK)
  {
    rt_kprintf("�����ⲿFlash�����ļ�ϵͳʧ�ܡ�(%d)\r\n",res_flash);
    rt_kprintf("��������ԭ��SPI Flash��ʼ�����ɹ���\r\n");
		while(1);
  }
  else
  {
    rt_kprintf("���ļ�ϵͳ���سɹ������Խ��ж�д����\r\n");
  }
	rt_hw_interrupt_enable(level);
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
/*----------------------- �ļ�ϵͳ���ԣ�д���� -------------------*/
	/* ���ļ���ÿ�ζ����½�����ʽ�򿪣�����Ϊ��д */
	rt_kprintf("\r\n****** ���������ļ�д�����... ******\r\n");	
	res_flash = f_open(&fnew, "1:FatFs��д�����ļ�.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if ( res_flash == FR_OK )
	{
		rt_kprintf("����/����FatFs��д�����ļ�.txt�ļ��ɹ������ļ�д�����ݡ�\r\n");
    /* ��ָ���洢������д�뵽�ļ��� */
		res_flash=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
    if(res_flash==FR_OK)
    {
      rt_kprintf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
//      rt_kprintf("�����ļ�д�������Ϊ��\r\n%s\r\n",WriteBuffer);
    }
    else
    {
      rt_kprintf("�����ļ�д��ʧ�ܣ�(%d)\n",res_flash);
    }    
//		/* ���ٶ�д���ر��ļ� */
    f_close(&fnew);
	}
	else
	{	
		LED_RED;
		rt_kprintf("������/�����ļ�ʧ�ܡ�\r\n");
		rt_kprintf("er = %d \n",res_flash);
	}
	
/*------------------- �ļ�ϵͳ���ԣ������� --------------------------*/
	rt_kprintf("****** ���������ļ���ȡ����... ******\r\n");
	res_flash = f_open(&fnew, "1:FatFs��д�����ļ�.txt",FA_OPEN_EXISTING | FA_READ); 	 
	if(res_flash == FR_OK)
	{
		LED_GREEN;
		rt_kprintf("�����ļ��ɹ���\r\n");
		res_flash = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(res_flash==FR_OK)
    {
      rt_kprintf("���ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\r\n",fnum);
//      rt_kprintf("����ȡ�õ��ļ�����Ϊ��\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      rt_kprintf("�����ļ���ȡʧ�ܣ�(%d)\n",res_flash);
    }		
	}
	else
	{
		LED_RED;
		rt_kprintf("�������ļ�ʧ�ܡ�\r\n");
	}
	/* ���ٶ�д���ر��ļ� */
	f_close(&fnew);	
  
	/* ����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ */
//	f_mount(NULL,"1:",1);
  
  /* ������ɣ�ͣ�� */
	
}


