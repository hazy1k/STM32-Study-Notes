/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   SD���ļ�ϵͳ����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103-�Ե� ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "stm32f4xx.h"
#include "./led/bsp_led.h" 
#include "./usart/bsp_debug_usart.h"
#include "./flash/bsp_spi_flash.h"
#include "./key/bsp_key.h" 
//#include "./delay/core_delay.h"  
/* FatFs includes component */
#include "ff.h"
#include "./res_mgr/RES_MGR.h"

/**
  ******************************************************************************
  *                              �������
  ******************************************************************************
  */
DIR dir; 
FIL fnew;													/* �ļ����� */
UINT fnum;            			  /* �ļ��ɹ���д���� */
BYTE ReadBuffer[1024]={0};        /* �������� */
BYTE WriteBuffer[] =              /* д������*/
"��ӭʹ��Ұ��STM32 F767������ �����Ǹ������ӣ��½��ļ�ϵͳ�����ļ�\r\n";  
char SDPath[4]; /* SD�߼�������·�� */
extern FATFS sd_fs;	
FRESULT res_sd;                /* �ļ�������� */
extern char src_dir[];
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
uint8_t state = 0;
int main(void)
{
    /* ϵͳʱ�ӳ�ʼ����400MHz */
  
    LED_GPIO_Config();
    LED_BLUE;	
    /* ��ʼ��USART1 ����ģʽΪ 115200 8-N-1 */
    Debug_USART_Config();	
  
    Key_GPIO_Config();
    SPI_FLASH_Init();
  
    //���ⲿSD�������ļ�ϵͳ���ļ�ϵͳ����ʱ���SD����ʼ��
    res_sd = f_mount(&sd_fs,"0:",1);  
  
    if(res_sd != FR_OK)
    {
      printf("f_mount ERROR!������������SD��Ȼ�����¸�λ������!");
      LED_RED;
      while(1);
    }
  
#if 1    
    printf("\r\n ��һ��KEY1��ʼ��д�ֿⲢ�����ļ���FLASH�� \r\n"); 
    printf("\r\n ע��ò������FLASH��ԭ���ݻᱻɾ������ \r\n"); 

    while(Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_OFF);
    printf("\r\n ���ڽ�����Ƭ������ʱ��ܳ��������ĵȺ�...\r\n"); 
    
    SPI_FLASH_BulkErase();
    /* ��һ��ִ��Make_Catalog����ʱɾ���ɵ���¼��Ϣ�ļ� */
    res_sd = f_unlink(BURN_INFO_NAME_FULL);
    if ( res_sd == FR_OK )
    {
      printf("����ɾ���ļ��ɹ���(%d)\n",res_sd);
    }
    else
    {
      printf("����ɾ���ļ�ʧ�ܣ�(%d)\n",res_sd);
    }
    
    /* ������¼Ŀ¼��Ϣ�ļ� */
    Make_Catalog(src_dir,0);

    /* ��¼ Ŀ¼��Ϣ��FLASH*/
    Burn_Catalog();
     /* ���� Ŀ¼ ��¼������FLASH*/
    Burn_Content();
    /* У����¼������ */
    Check_Resource(); 
#endif    
    /* ������ɣ�ͣ�� */
    while(1)
    {
    }
}
	  

