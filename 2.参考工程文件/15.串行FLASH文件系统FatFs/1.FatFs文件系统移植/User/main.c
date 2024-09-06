#include "stm32f10x.h"
#include "ff.h" // FatFs�ļ�ϵͳͷ�ļ�
#include "./flash/bsp_spi_flash.h"
#include "./usart/bsp_usart.h"
#include "./led/bsp_led.h"

FATFS fs;				   // FatFs�ļ�ϵͳ����
FIL fnew;				   // �ļ�����
FRESULT res_flash;         // �ļ��������
UINT fnum;            	   // �ļ��ɹ���д���� 
BYTE ReadBuffer[1024]={0}; // ��������
BYTE WriteBuffer[] =  "�Ѿ��ɹ�д���� �����Ǹ������ӣ��½��ļ�ϵͳ�����ļ�\r\n";  // д������

int main(void)
{
	LED_GPIO_Config();	
	LED_BLUE;
	
	// ��ʼ�����Դ��ڣ�һ��Ϊ����1
	USART_Config();	
  	printf("****** ����һ��SPI FLASH �ļ�ϵͳʵ�� ******\r\n");
  
	// ���ⲿSPI Flash�����ļ�ϵͳ���ļ�ϵͳ����ʱ���SPI�豸��ʼ��
	// ��ʼ������������������
	// f_mount()->find_volume()->disk_initialize->SPI_FLASH_Init()

	res_flash = f_mount(&fs, "1:", 1); // �����������ļ�ϵͳ���󡢹��ص㡢��ʽ����־
	
    // ��ʽ���ļ�ϵͳ����
	// ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ
	if(res_flash == FR_NO_FILESYSTEM)
	{
		printf("��FLASH��û���ļ�ϵͳ���������и�ʽ��...\r\n");
    // ������и�ʽ������
		res_flash = f_mkfs("1:", 0, 0); // �������������ص㡢������С����������С							
		
		if(res_flash == FR_OK) // ��ʽ���ɹ�
		{
			printf("��FLASH�ѳɹ���ʽ���ļ�ϵͳ��\r\n");
      	// ��ʽ������ȡ������
			res_flash = f_mount(NULL,"1:",1); // �����������ļ�ϵͳ���󡢹��ص㡢��ʽ����־		
      	// ���¹���			
			res_flash = f_mount(&fs,"1:",1); // �����������ļ�ϵͳ���󡢹��ص㡢��ʽ����־
		}
		else
		{
			LED_RED;
			printf("������ʽ��ʧ�ܡ�����\r\n");
			while(1);
		}
	}
  	else if(res_flash != FR_OK) // ��������
    {
    	printf("�����ⲿFlash�����ļ�ϵͳʧ�ܡ�(%d)\r\n",res_flash);
    	printf("��������ԭ��SPI Flash��ʼ�����ɹ���\r\n");
		printf("������ SPI����д����FLASH ���̲��ԣ�����������ڸ�����f_mount�����if���ǰ��ʱ�����һ�� res_flash = FR_NO_FILESYSTEM; ������ֱ��ִ�и�ʽ������\r\n");
		while(1);
  	}
  	else
  	{
    	printf("���ļ�ϵͳ���سɹ������Խ��ж�д����\r\n");
  	}
  
	// �ļ�ϵͳ���ԣ�д����
	// ���ļ���ÿ�ζ����½�����ʽ�򿪣�����Ϊ��д */
	printf("\r\n****** ���������ļ�д�����... ******\r\n");	
	res_flash = f_open(&fnew, "1:FatFs��д�����ļ�.txt", FA_CREATE_ALWAYS | FA_WRITE ); // f_open()�������ļ����������ļ������ļ���������
	if (res_flash == FR_OK)
	{
		printf("����/����FatFs��д�����ļ�.txt�ļ��ɹ������ļ�д�����ݡ�\r\n");
    // ��ָ���洢������д�뵽�ļ���
		res_flash = f_write(&fnew, WriteBuffer, sizeof(WriteBuffer), &fnum); // f_write()����д���ļ����������ļ�����д�����ݡ�д���ֽ�����ʵ��д���ֽ���
    if(res_flash == FR_OK)
    {
      printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
      printf("�����ļ�д�������Ϊ��\r\n%s\r\n",WriteBuffer);
    }
    else
    {
      printf("�����ļ�д��ʧ�ܣ�(%d)\n",res_flash);
    }    
    f_close(&fnew); // f_close()�����ر��ļ����������ļ�����
	}
	else
	{	
		LED_RED;
		printf("������/�����ļ�ʧ�ܡ�\r\n");
	}
	
	// �ļ�ϵͳ���ԣ�������
	printf("****** ���������ļ���ȡ����... ******\r\n");
	res_flash = f_open(&fnew, "1:FatFs��д�����ļ�.txt",FA_OPEN_EXISTING | FA_READ); // f_open()�������ļ����������ļ������ļ���������
	if(res_flash == FR_OK)
	{
		LED_GREEN;
		printf("�����ļ��ɹ���\r\n");
		res_flash = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); // f_read()������ȡ�ļ����������ļ����󡢶������ݡ������ֽ�����ʵ�ʶ����ֽ���
    if(res_flash == FR_OK)
    {
      printf("���ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\r\n",fnum);
      printf("����ȡ�õ��ļ�����Ϊ��\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      printf("�����ļ���ȡʧ�ܣ�(%d)\n",res_flash);
    }		
	}
	else
	{
		LED_RED;
		printf("�������ļ�ʧ�ܡ�\r\n");
	}
	// ���ٶ�д���ر��ļ�
	f_close(&fnew);	// f_close()�����ر��ļ����������ļ�����
  
	// ����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ
	f_mount(NULL,"1:",1); // �����������ļ�ϵͳ���󡢹��ص㡢��ʽ����־
  
  	// ������ɣ�ͣ��
	while(1)
	{
	}
}