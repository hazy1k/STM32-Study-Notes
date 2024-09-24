 /**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   ���ڴ�SD�����������ļ���FLASH�Ĺ���
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
#include "./led/bsp_led.h"
#include "./flash/bsp_spi_flash.h"
#include "./key/bsp_key.h"  
#include "./FATFS/ff.h"
#include "./res_mgr/res_mgr.h"


/**
  ******************************************************************************
  *                              �������
  ******************************************************************************
  */
extern FATFS sd_fs;													/* Work area (file system object) for logical drives */
extern FATFS flash_fs;

//Ҫ���Ƶ��ļ�·������aux_data.c�޸�
extern char src_dir[];

/*
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  ����
 */
int main(void)
{ 	
  FRESULT res = FR_OK;
  
	/* ��ʼ��LED */
  LED_GPIO_Config();
  
  Key_GPIO_Config();

  /* ��ʼ�����Դ��ڣ�һ��Ϊ����1 */
  USART_Config();
  
  SPI_FLASH_Init();
  
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
  
  while(Key_Scan(GPIOA,GPIO_Pin_0)==0);
  
  printf("\r\n ���ڽ�����Ƭ������ʱ��ܳ��������ĵȺ�...\r\n");
  SPI_FLASH_BulkErase();    
  
  /* ������¼Ŀ¼��Ϣ�ļ� */
  Make_Catalog(src_dir,0);
  
  /* ��¼ Ŀ¼��Ϣ��FLASH*/
  Burn_Catalog();  
  /* ���� Ŀ¼ ��¼������FLASH*/
  Burn_Content();
  /* У����¼������ */
  Check_Resource();
  
//  printf("��ȡ��Դλ�ò���\r\n");
//  printf("xinsongti24.xbf offset = %d\r\n", 
//                  GetResOffset("xinsongti24.xbf"));

  while(1);
}

/*********************************************END OF FILE**********************/
