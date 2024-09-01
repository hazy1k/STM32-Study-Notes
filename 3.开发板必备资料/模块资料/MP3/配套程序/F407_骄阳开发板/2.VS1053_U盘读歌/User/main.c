/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   ��SD���ڵ�GPS���ݽ��н��룬��ȡ��λ��Ϣ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F4 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "key/bsp_key.h" 
#include "ff.h"
#include "./USBAPP/usbh_bsp.h"
#include  "VS1053.h"	
#include "./systick/bsp_SysTick.h"
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
"��ӭʹ��Ұ��STM32 F407���������� �����Ǹ������ӣ��½��ļ�ϵͳ�����ļ�\r\n";  
char SDPath[4]; /* SD�߼�������·�� */
FATFS sd_fs[2];
FRESULT res_sd;                /* �ļ�������� */
extern char src_dir[];

uint8_t state = 0;

void Delay(__IO u32 nCount); 
extern void nmea_decode_test(void);
 
#define Other_Part   0
PARTITION VolToPart[]=
{
	{0,1},/* "0:" */
	{0,2},/* "1:" */
	{0,3}	/* "2:" */
};


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	SysTick_Init();
	/*���ڳ�ʼ��*/
	Debug_USART_Config();
	
 		/* ��ʼ��USB */
	USBH_Init(&USB_OTG_Core,
		        USB_OTG_FS_CORE_ID,
		        &USB_Host,
		        &USBH_MSC_cb,
		        &USR_cb);
	
	printf("����U�̺���ʾ��Device Attached����������ʾU���Ѽ�⵽������KEY1��������\n");  
	while(1)
	{
		/* USB״̬��⴦�������ڼ���豸����ʱʹ�� */
		USBH_Process(&USB_OTG_Core, &USB_Host);
		 
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
		{
			//���ⲿU�̹����ļ�ϵͳ���ļ�ϵͳ����ʱ���U�̳�ʼ��
			res_sd = f_mount(&sd_fs[0],"0:",1);  
			if(res_sd != FR_OK)
			{
			  printf("f_mount 1ERROR!������������U��Ȼ�����¸�λ������!");
			}

#if Other_Part
			res_sd = f_mount(&sd_fs[1],"1:",1);
		  
			if(res_sd != FR_OK)
			{
			  printf("f_mount 2ERROR!������������U��Ȼ�����¸�λ������!");
			}
#endif
			VS_Init();
			printf("vs1053:%4X\n",VS_Ram_Test());
			Delay_ms(100);
			VS_Sine_Test();	
			VS_HD_Reset();
			VS_Soft_Reset();
			while(1)
			{
				// ���Ը�������SD����Ŀ¼��
				vs1053_player_song("0:TestFile.mp3");
				printf("MusicPlay End\n");
			}
		}
	}	


}

void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/

