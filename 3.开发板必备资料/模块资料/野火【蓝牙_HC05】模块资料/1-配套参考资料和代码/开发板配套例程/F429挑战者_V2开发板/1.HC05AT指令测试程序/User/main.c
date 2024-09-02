/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   HC05����ģ����Գ���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F429 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_blt_usart.h"
#include "./usart/bsp_debug_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./hc05/bsp_hc05.h"
#include "./key/bsp_key.h" 
#include "./lcd/bsp_lcd.h"
#include <string.h>
#include <stdlib.h>
#include "./dwt_delay/core_delay.h"   


extern ReceiveData DEBUG_USART_ReceiveData;
extern ReceiveData BLT_USART_ReceiveData;

//��������
void ATCMD_Test(void);


/*
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  ����
 */
int main(void)
{
  char hc05_name[30]="HC05_SLAVE";
	char hc05_nameCMD[40];
  char disp_buff[200];
  
	/* ��ʱ������ʼ�� */
  CPU_TS_TmrInit();

  
  /* ���Դ��ڳ�ʼ�� USART1 ����ģʽΪ 115200 8-N-1 �����ж� */
	Debug_USART_Config();
	
	LED_GPIO_Config();
	Key_GPIO_Config();
  
  
  /* ��ʼ��Һ���� */
  #ifdef ENABLE_LCD_DISPLAY
	LCD_Init();	
	LCD_LayerInit();
  LTDC_Cmd(ENABLE);
	
	/*�ѱ�����ˢ��ɫ*/
  LCD_SetLayer(LCD_BACKGROUND_LAYER);  
	LCD_Clear(LCD_COLOR_BLACK);
	
  /*��ʼ����Ĭ��ʹ��ǰ����*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*Ĭ�����ò�͸��	���ú�������Ϊ��͸���ȣ���Χ 0-0xff ��0Ϊȫ͸����0xffΪ��͸��*/
  LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BLACK);
	LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);
	/*����LCD_SetLayer(LCD_FOREGROUND_LAYER)������
	����Һ����������ǰ����ˢ�£��������µ��ù�LCD_SetLayer�������ñ�����*/	
  
  /*ѡ������*/
  LCD_SetFont(&Font16x24);
  
  LCD_ClearLine(LCD_LINE_0);
	LCD_DisplayStringLine( LCD_LINE_0,(uint8_t *)"HC05 BlueTooth Demo" );
  #endif
	
	
  /* HC05����ģ���ʼ����GPIO �� USART6 ����ģʽΪ 38400 8-N-1 �����ж� */
	if(HC05_Init() == 0)
	{
		HC05_INFO("HC05ģ����������");
    #ifdef ENABLE_LCD_DISPLAY
		LCD_DisplayStringLine( LCD_LINE_1,(uint8_t *)"HC05 module detected!" );
    #endif
	}
	else
	{
		HC05_ERROR("HC05ģ���ⲻ����������ģ���뿪��������ӣ�Ȼ��λ���������²��ԡ�");
    #ifdef ENABLE_LCD_DISPLAY
		LCD_DisplayStringLine( LCD_LINE_1,(uint8_t *)"No HC05 module detected!"  );
		LCD_DisplayStringLine( LCD_LINE_2, (uint8_t *)"Please check the hardware connection and reset the system." );
    #endif
    
		while(1);
	}


  HC05_INFO("**********HC05ģ��ʵ��************");
  
	/*��λ���ָ�Ĭ��״̬*/
	HC05_Send_CMD("AT+RESET\r\n",1);	//��λָ������֮����Ҫһ��ʱ��HC05�Ż������һ��ָ��
	HC05_Send_CMD("AT+ORGL\r\n",1);

	
	/*�������������ʾ��Ĭ�ϲ���ʾ��
	 *��bsp_hc05.h�ļ���HC05_DEBUG_ON ������Ϊ1��
	 *����ͨ�����ڵ������ֽ��յ�����Ϣ*/	
	
	HC05_Send_CMD("AT+VERSION?\r\n",1);
	
	HC05_Send_CMD("AT+ADDR?\r\n",1);
	
	HC05_Send_CMD("AT+UART?\r\n",1);
	
	HC05_Send_CMD("AT+CMODE?\r\n",1);
	
	HC05_Send_CMD("AT+STATE?\r\n",1);	

	HC05_Send_CMD("AT+ROLE=0\r\n",1);
	
	/*��ʼ��SPP�淶*/
	HC05_Send_CMD("AT+INIT\r\n",1);
	HC05_Send_CMD("AT+CLASS=0\r\n",1);
	HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);
	
	/*����ģ������*/
	sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
	HC05_Send_CMD(hc05_nameCMD,1);

	HC05_INFO("��ģ������Ϊ:%s ,ģ����׼��������",hc05_name);
	
  #ifdef ENABLE_LCD_DISPLAY
	sprintf(disp_buff,"Device name:%s",hc05_name);
	LCD_DisplayStringLine( LCD_LINE_2, (uint8_t *)disp_buff );

	LCD_DisplayStringLine ( LCD_LINE_4, (uint8_t *)"ReceiveData USART1" );	
  LCD_DisplayStringLine ( LCD_LINE_9, (uint8_t *)"ReceiveData HC-05" );
  #endif


	while(1)
	{
    
    ATCMD_Test(); //ATָ�����
    
	}
}


/**
  * @brief  HC05 ATָ����� 
  * @param  ��
  * @retval ��
  */
void ATCMD_Test(void)
{
  /* ������Դ��ڽ��յ��Ĵ����������� */
  if(DEBUG_USART_ReceiveData.receive_data_flag == 1)
  {
    DEBUG_USART_ReceiveData.uart_buff[DEBUG_USART_ReceiveData.datanum] = 0;
    //�����������AT��ͷ�ģ��Ͱ�KEY�øߣ���������ģ��
    if( strstr((char *)DEBUG_USART_ReceiveData.uart_buff,"AT") == (char *)DEBUG_USART_ReceiveData.uart_buff )
    {
      BLT_KEY_HIGHT;
      Usart_SendStr_length(BLT_USARTx, DEBUG_USART_ReceiveData.uart_buff, DEBUG_USART_ReceiveData.datanum);
      Usart_SendStr_length(BLT_USARTx,"\r\n",2);
      BLT_KEY_LOW;
    }
    
    //����������ʾ���յ�������
    Usart_SendString( DEBUG_USART, "\r\nrecv USART1 data:\r\n" );
    Usart_SendString( DEBUG_USART, DEBUG_USART_ReceiveData.uart_buff );
    Usart_SendString( DEBUG_USART, "\r\n" );
    //LCD��ʾ���յ�������
    #ifdef ENABLE_LCD_DISPLAY
    LCD_ClearLine(LINE(5));
    LCD_ClearLine(LINE(6));
    LCD_ClearLine(LINE(7));
    LCD_ClearLine(LINE(8));
    LCD_DisplayStringLine ( (LINE(5)), DEBUG_USART_ReceiveData.uart_buff );
    #endif
    
    //������Դ������ݻ���
    DEBUG_USART_ReceiveData.receive_data_flag = 0;		//�������ݱ�־����
    DEBUG_USART_ReceiveData.datanum = 0;               
  }
  
  
  /* �����������ڽ��յ����������� */
  if(BLT_USART_ReceiveData.receive_data_flag == 1)
  {
    BLT_USART_ReceiveData.uart_buff[BLT_USART_ReceiveData.datanum] = 0;
    //����������ʾ���յ�������
    Usart_SendString( DEBUG_USART, "\r\nrecv HC-05 data:\r\n" );
    Usart_SendString( DEBUG_USART, BLT_USART_ReceiveData.uart_buff );
    Usart_SendString( DEBUG_USART, "\r\n" );
    
    //LCD��ʾ���յ�������
    #ifdef ENABLE_LCD_DISPLAY
    LCD_ClearLine(LINE(10));
    LCD_ClearLine(LINE(11));
    LCD_ClearLine(LINE(12));
    LCD_ClearLine(LINE(13));
    LCD_DisplayStringLine ( (LINE(10)), BLT_USART_ReceiveData.uart_buff );
    #endif
    
    //���������������ݻ���
    BLT_USART_ReceiveData.receive_data_flag = 0;		//�������ݱ�־����
    BLT_USART_ReceiveData.datanum = 0;  
  }
}



/*********************************************END OF FILE**********************/

