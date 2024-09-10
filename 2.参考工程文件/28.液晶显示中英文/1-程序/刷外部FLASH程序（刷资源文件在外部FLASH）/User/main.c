/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   SPI FLASH�ļ�ϵͳ���ù�����ʾ
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103-ָ���� ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h"
#include "./flash/bsp_spi_flash.h"
#include "ff.h"
#include "string.h"
#include "aux_data.h"

/**
  ******************************************************************************
  *                              �������
  ******************************************************************************
  */
extern FATFS sd_fs;													/* Work area (file system object) for logical drives */
extern FATFS flash_fs;

//Ҫ���Ƶ��ļ�·������aux_data.c�޸�
extern char src_dir[];
extern char dst_dir[];

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{    	
	
  FRESULT res = FR_OK;
  
	/* ��ʼ��LED */
  LED_GPIO_Config();
  Key_GPIO_Config();

  /* ��ʼ�����Դ��ڣ�һ��Ϊ����1 */
  USART_Config();

  printf("\r\n ʹ��ָ���ߵװ�ʱ ���Ͻ�����λ�� ��Ҫ��PC0������ñ ��ֹӰ��PC0��SPIFLASHƬѡ�� \r\n");
  
  TM_FATFS_FLASH_SPI_disk_initialize();
  
  res = FR_DISK_ERR;
       
  //����SD��
  res = f_mount(&sd_fs,SD_ROOT,1);

  //����ļ�ϵͳ����ʧ�ܾ��˳�
  if(res != FR_OK)
  {
    BURN_ERROR("f_mount ERROR!������������SD��Ȼ�����¸�λ������!");
    LED_RED;
    while(1);
  }    
    
  printf("\r\n ��һ��KEY1��ʼ��д�ֿⲢ�����ļ���FLASH�� \r\n"); 
  printf("\r\n ע��ò������FLASH��ԭ���ݻᱻɾ������ \r\n"); 

  while(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) != KEY_ON);

  printf("\r\n ���ڽ�����Ƭ������ʱ��ܳ��������ĵȺ�...\r\n");
  SPI_FLASH_BulkErase();   
  //��¼���ݵ�flash�ķ��ļ�ϵͳ����    
  res = burn_file_sd2flash(burn_data,AUX_MAX_NUM); 
  
  if(res == FR_OK)
  {
    printf("\r\n\r\n\r\n"); 

    //�����ļ���FLASH���ļ�ϵͳ����
    copy_file_sd2flash(src_dir,dst_dir);
    if(res == FR_OK)
    {
      printf("\r\n ���������ѳɹ����Ƶ�FLASH������ \r\n");  
      LED_GREEN;
    }
    else
    {
      printf("\r\n �����ļ���FLASHʧ��(�ļ�ϵͳ����)���븴λ���ԣ��� \r\n"); 
    }
  }
  else
  {
    printf("\r\n �������ݵ�FLASHʧ��(���ļ�ϵͳ����)���븴λ���ԣ��� \r\n"); 
  }
  
  
  while(1);

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
