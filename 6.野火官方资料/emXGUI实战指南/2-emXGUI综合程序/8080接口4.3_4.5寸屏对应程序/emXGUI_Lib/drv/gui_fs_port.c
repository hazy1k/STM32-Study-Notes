/**
  *********************************************************************
  * @file    gui_fs_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   �ļ�ϵͳ�ӿڣ���Ҫʵ��FileSystem_Init����
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */ 
#include "gui_fs_port.h"
#include "ff.h"

#if defined(STM32F429_439xx)
#include "sdio/bsp_sdio_sd.h"
#elif defined(STM32H743xx)
#include "./sd_card/bsp_sdio_sd.h"
#elif defined(STM32F767xx)
#include "./sdmmc/bsp_sdmmc_sd.h"
#include "ff_gen_drv.h"
char SDPath[4]; /* SD�߼�������·�� */
extern Diskio_drvTypeDef  SD_Driver;
#elif defined(CPU_MIMXRT1052DVL6B)
#include "./bsp/sd_fatfs_test/bsp_sd_fatfs_test.h"

#define BUFFER_SIZE (100U)

SDK_ALIGN(uint8_t g_bufferWrite[SDK_SIZEALIGN(BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));
SDK_ALIGN(uint8_t g_bufferRead[SDK_SIZEALIGN(BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));
#endif  


	/* FatFs�ļ�ϵͳ���� */
static FATFS fs ;									

void FileSystem_Test(void);



/**
  * @brief  �ļ�ϵͳ��ʼ��
  * @param  ��
  * @retval TRUE  FALSE
  */
BOOL FileSystem_Init(void)
{ 
  /* �ļ�������� */
                

	/* �������������ʹ��rtt �� printf��ӡ
		 ֻ��ʹ��printf	*/
	
	/* ���жϣ�������ӻ�����������board.c�н��г�ʼ��
		 �������жϣ��������г�ʼ����ʱ������Ҫ���ж� */
//	rt_base_t level = 0;
//	rt_hw_interrupt_enable(level);	

  /* ����WiFiģ�� */
#if defined(STM32F429_439xx)
  BL8782_PDN_INIT();
#elif defined(STM32H743xx)
WIFI_PDN_INIT();
#elif defined(STM32F767xx)
  WIFI_PDN_INIT();
#endif  
	

#if defined(STM32F429_439xx) || defined(STM32H743xx) || defined(STM32F767xx) || defined(STM32F10X_HD) || defined(STM32F40_41xxx)
	//���ⲿSPI Flash�����ļ�ϵͳ���ļ�ϵͳ����ʱ���SPI�豸��ʼ��
  FRESULT res_sd; 
#if defined(STM32F767xx)
  FATFS_LinkDriver(&SD_Driver, SDPath);
#endif
	res_sd = f_mount(&fs,"0:",1);
	
	if(res_sd == FR_NO_FILESYSTEM)
	{
		printf("��SD����û���ļ�ϵͳ...\r\n");
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
//			while(1);
		}
    return FALSE;
	}
  else if(res_sd!=FR_OK)
  {
    printf("����SD�������ļ�ϵͳʧ�ܡ�(%d)\r\n",res_sd);
    printf("��������ԭ��û�н���SD����\r\n");
    return FALSE;

  }
  else
  {
    printf("���ļ�ϵͳ���سɹ�\r\n");
    
#if 0
    /* �ļ�ϵͳ���� */
    FileSystem_Test();
#endif 
    
    /* ���Խ���unicode����ת����
      ��ʹ��extern_cc936ʱ���ɲ����Ƿ����cc936��Դ*/
    ff_convert('a',1);
    
    
  }
#elif defined(CPU_MIMXRT1052DVL6B)
  f_mount_test(&fs);
#endif
  
  return TRUE;
}

#if 0
FIL fnew __EXRAM;													/* �ļ����� */
UINT fnum;            					  /* �ļ��ɹ���д���� */
BYTE ReadBuffer[512]={0};        /* �������� */
BYTE WriteBuffer[] =              /* д������*/
"testtesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttsttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttestteesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttsttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttest\r\n";  

/**
  * @brief  �ļ�ϵͳ��д����
  * @param  ��
  * @retval TRUE  FALSE
  */
void FileSystem_Test(void)
{
  /* �ļ�������� */
  FRESULT res_sd;               

	/*----------------------- �ļ�ϵͳ���ԣ�д���� -----------------------------*/
	/* ���ļ�������ļ��������򴴽��� */
	printf("\r\n****** ���������ļ�д�����... ******\r\n");	
	res_sd = f_open(&fnew, "0:FatFs��д�����ļ�.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if ( res_sd == FR_OK )
	{
		printf("����/����FatFs��д�����ļ�.txt�ļ��ɹ������ļ�д�����ݡ�\r\n");
    /* ��ָ���洢������д�뵽�ļ��� */
		res_sd=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
    if(res_sd==FR_OK)
    {
      printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
      printf("�����ļ�д�������Ϊ��\r\n%s\r\n",WriteBuffer);
    }
    else
    {
      printf("�����ļ�д��ʧ�ܣ�(%d)\n",res_sd);
    }    
		/* ���ٶ�д���ر��ļ� */
    f_close(&fnew);
	}
	else
	{	
//		LED_RED;
		printf("������/�����ļ�ʧ�ܡ�\r\n");
	}
	
/*------------------- �ļ�ϵͳ���ԣ������� ------------------------------------*/
	printf("****** ���������ļ���ȡ����... ******\r\n");
	res_sd = f_open(&fnew, "0:FatFs��д�����ļ�.txt", FA_OPEN_EXISTING | FA_READ); 	 
	if(res_sd == FR_OK)
	{
//		LED_GREEN;
		printf("�����ļ��ɹ���\r\n");
		res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(res_sd==FR_OK)
    {
      printf("���ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\r\n",fnum);
      printf("����ȡ�õ��ļ�����Ϊ��\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      printf("�����ļ���ȡʧ�ܣ�(%d)\n",res_sd);
    }	
		/* ���ٶ�д���ر��ļ� */
		f_close(&fnew);	
	}
	else
	{
//		LED_RED;
		printf("�������ļ�ʧ�ܡ�\r\n");
	}
//	/* ���ٶ�д���ر��ļ� */
//	f_close(&fnew);	
  
//	/* ����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ */
//	f_mount(NULL,"0:",1);
	
}
#endif
/********************************END OF FILE****************************/

