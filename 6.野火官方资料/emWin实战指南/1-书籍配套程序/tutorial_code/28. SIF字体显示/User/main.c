/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS V9.0.0  + STM32 �̼�������
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 ȫϵ�п����� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
/* ������Ӳ��bspͷ�ļ� */
#include "./led/bsp_led.h"
#include "./usart/bsp_usart.h"
#include "./key/bsp_key.h"
#include "./lcd/bsp_ili9341_lcd_emwin.h"
#include "./lcd/bsp_xpt2046_lcd.h"
#include "./flash/bsp_spi_flash.h"
#include "./sram/bsp_fsmc_sram.h"
#include "./TPad/bsp_tpad.h"
#include "./beep/bsp_beep.h" 
#include <string.h>
/* STemWINͷ�ļ� */
#include "GUI.h"
#include "DIALOG.h"

/* FATFS */
#include "ff.h"
#include "diskio.h"
#include "integer.h"

/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
/* ���������� */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* LED������ */
static TaskHandle_t LED_Task_Handle = NULL;
/* Touch������ */
static TaskHandle_t Touch_Task_Handle = NULL;
/* GUI������ */
static TaskHandle_t GUI_Task_Handle = NULL;


/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
FATFS   fs;			/* FatFs�ļ�ϵͳ���� */
FIL     file;		/* file objects */
UINT    bw;     /* File R/W count */
FRESULT result; 
FILINFO fno;
DIR dir;

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void);/* ���ڴ������� */

static void LED_Task(void* parameter);/* LED_Task����ʵ�� */
static void GUI_Task(void* parameter);/* GUI_Task����ʵ�� */
static void Touch_Task(void* parameter);
static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */
static void ESP_PDN_INIT(void);
	
/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  /* ������Ӳ����ʼ�� */
  BSP_Init();
  
  printf("\r\n ********** emwin DEMO *********** \r\n");
  
   /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )256,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
  /* ����������� */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1;  
  
  while(1);   /* ��������ִ�е����� */    
}


/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  taskENTER_CRITICAL();           //�����ٽ���
  
	xReturn = xTaskCreate((TaskFunction_t)LED_Task,/* ������ں��� */
											 (const char*    )"LED_Task",/* �������� */
											 (uint16_t       )65,       /* ����ջ��С */
											 (void*          )NULL,      /* ������ں������� */
											 (UBaseType_t    )4,         /* ��������ȼ� */
											 (TaskHandle_t   )&LED_Task_Handle);/* ������ƿ�ָ�� */
	if(pdPASS == xReturn)
		printf("����LED1_Task����ɹ���\r\n");
  
//  xReturn = xTaskCreate((TaskFunction_t)Touch_Task,/* ������ں��� */
//											 (const char*      )"Touch_Task",/* �������� */
//											 (uint16_t         )256,     /* ����ջ��С */
//											 (void*            )NULL,    /* ������ں������� */
//											 (UBaseType_t      )3,       /* ��������ȼ� */
//											 (TaskHandle_t     )&Touch_Task_Handle);/* ������ƿ�ָ�� */
//	if(pdPASS == xReturn)
//		printf("����Touch_Task����ɹ���\r\n");
	
	xReturn = xTaskCreate((TaskFunction_t)GUI_Task,/* ������ں��� */
											 (const char*      )"GUI_Task",/* �������� */
											 (uint16_t         )1024,      /* ����ջ��С */
											 (void*            )NULL,      /* ������ں������� */
											 (UBaseType_t      )2,         /* ��������ȼ� */
											 (TaskHandle_t     )&GUI_Task_Handle);/* ������ƿ�ָ�� */
	if(pdPASS == xReturn)
		printf("����GUI_Task����ɹ���\r\n");
	
	vTaskDelete(AppTaskCreate_Handle);//ɾ��AppTaskCreate����
	
	taskEXIT_CRITICAL();//�˳��ٽ���
}

/**
  * @brief LED��������
  * @note ��
  * @param ��
  * @retval ��
  */
static void LED_Task(void* parameter)
{
	while(1)
	{
		LED3_TOGGLE;
		vTaskDelay(1000);
	}
}

/**
  * @brief ���������������
  * @note ��
  * @param ��
  * @retval ��
  */
static void Touch_Task(void* parameter)
{
	while(1)
	{
		GUI_TOUCH_Exec();//��������ʱɨ��
		vTaskDelay(10);
	}
}

/**
  * @brief GUI��������
  * @note ��
  * @param ��
  * @retval ��
  */
static void GUI_Task(void* parameter)
{
	/* ��ʼ��STemWin */
  GUI_Init();

	while(1)
	{
		MainTask();
	}
}

/**
  * @brief �弶�����ʼ��
  * @note ���а����ϵĳ�ʼ�����ɷ��������������
  * @param ��
  * @retval ��
  */
static void BSP_Init(void)
{
  /* CRC��emWinû�й�ϵ��ֻ������Ϊ�˿�ı���������
   * ����STemWin�Ŀ�ֻ������ST��оƬ���棬���оƬ���޷�ʹ�õġ�
   */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
  
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED ��ʼ�� */
	LED_GPIO_Config();

  
	/* ���ڳ�ʼ��	*/
	USART_Config();
  
  /* ������ʼ��	*/
  Key_GPIO_Config();
  
  /* ��������ʼ�� */
	XPT2046_Init();
  
  /* ��ǰ��ʼ��LCD��������GUI_Init����LCD_X_Config֮ǰȷ��LCD����оƬID: lcdid */
  ILI9341_Init();
	
		
  /* �����ļ�ϵͳ������ʱ���SD����ʼ�� */
	/* ����WIFI */
	ESP_PDN_INIT();
	
  result = f_mount(&fs,"0:",1);
	if(result != FR_OK)
	{
		printf("SD����ʼ��ʧ�ܣ���ȷ��SD������ȷ���뿪���壬��һ��SD�����ԣ�\n");
		while(1);
	}
}

static void ESP_PDN_INIT(void)
{
  /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOB, ENABLE); 							   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
  
  GPIO_ResetBits(GPIOB,GPIO_Pin_8);  //����WiFiģ��
}


/********************************END OF FILE****************************/
