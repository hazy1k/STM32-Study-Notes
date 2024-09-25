#include "stm32f10x.h"
#include "ff.h"
#include "..\User\spi_flash\fatfs_flash_spi.h"
#include "..\User\usart\bsp_usart1.h"
#include "..\User\led\bsp_led.h"  

FIL fnew;													/* file objects */
FATFS fs;													/* Work area (file system object) for logical drives */
FRESULT res_flash; 
UINT br, bw;            					/* File R/W count */
BYTE buffer[1024]={0};       		  /* file copy buffer */
BYTE textFileBuffer[] = "��ӭʹ��Ұ��STM32_ISO-MINI ������  �����Ǹ������ӣ��½��ļ�ϵͳ�����ļ�";


static void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

int main(void)
{
	//LED��ʼ��
	LED_GPIO_Config();
	
	//������Ƭ�ⲿFlash������ʹ�ã�����������Ҫһ��ʱ�䲻Ҫ��;ֹͣ��
//	TM_FATFS_FLASH_SPI_disk_initialize();
//	SPI_FLASH_BulkErase();

	/* ��ʼ������1*/
	USART1_Config();
	printf("\r\n this is a fatfs test demo \r\n");
/*************************   flash �ļ�ϵͳ   *********************************************/
	//���ⲿFlash�����ļ�ϵͳ
	res_flash = f_mount(&fs,"0:",1);
	printf("\r\n f_mount res_flash=%d \r\n",res_flash);
	
	//���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ
	if(res_flash ==FR_NO_FILESYSTEM)
	{
		res_flash=f_mkfs("0:",0,4096);							//��ʽ��
		printf("\r\nmkfs res_flash=%d",res_flash);
		res_flash = f_mount(&fs,"0:",0);						//��ʽ������ȡ������
		res_flash = f_mount(&fs,"0:",1);						//���¹���
	}
/**************************  flash   *****************************************/		
	//�ļ�ϵͳ���ԣ�д����
	//���ļ�������ļ����μ��򴴽���
	res_flash = f_open(&fnew, "0:flashnewfile.txt", FA_CREATE_ALWAYS | FA_WRITE );
	 
	if ( res_flash == FR_OK )
	{
		LED3_ON;
		res_flash = f_write(&fnew, textFileBuffer, sizeof(textFileBuffer), &bw);
		f_close(&fnew);      
	}
	//������
	res_flash = f_open(&fnew, "0:flashnewfile.txt", FA_OPEN_EXISTING | FA_READ); 	 
	res_flash = f_read(&fnew, buffer, sizeof(buffer), &br); 
	printf("\r\n %s ", buffer);
	/* Close open files */
	res_flash = f_close(&fnew);	
	//����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ
	res_flash = f_mount(&fs,"0:",0);
	while(1)
	{
		LED2_TOGGLE;
		Delay(0x1ffff);
	}
}

/*********************************************END OF FILE**********************/
