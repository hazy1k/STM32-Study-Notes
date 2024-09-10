/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   RFID-R522ģ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 F429 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "rc522_config.h"
#include "rc522_function.h"
#include <stdbool.h>
#include "./lcd/bsp_lcd.h"
#include "./systick/bsp_SysTick.h"
#include "./font/fonts.h"
#include "./sdram/bsp_sdram.h"
#include "./key/bsp_key.h" 


uint8_t KeyValue[]={0xFF ,0xFF, 0xFF, 0xFF, 0xFF, 0xFF};   // ��A��Կ
//uint8_t KeyValue[]={'1' ,'2', '3', '4', '5', '6'};   // ��A��Կ


/**
  * @brief  IC���Ժ���
  * @param  ��
  * @retval ��
  */
void IC_test ( void )
{
	uint32_t writeValue = 100;
	uint32_t readValue;
	char cStr [ 30 ];
  uint8_t ucArray_ID [ 4 ];    /*�Ⱥ���IC�������ͺ�UID(IC�����к�)*/                                                                                         
	uint8_t ucStatusReturn;      /*����״̬*/                                                                                           
  while ( 1 )
  {    
    /*Ѱ��*/
		if ( ( ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID ) ) != MI_OK )
    {
      /*��ʧ���ٴ�Ѱ��*/
      printf ( "Ѱ��ʧ��\n" );
			ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID ); //PICC_REQALL   PICC_REQIDL
    }		                                                

		if ( ucStatusReturn == MI_OK  )
		{
      //printf ( "Ѱ���ɹ�\n" );
      /*����ײ�����ж��ſ������д��������Χʱ������ͻ���ƻ������ѡ��һ�Ž��в�����*/
			if ( PcdAnticoll ( ucArray_ID ) == MI_OK )                                                                   
			{
				PcdSelect(ucArray_ID);			
		
				PcdAuthState( PICC_AUTHENT1A, 0x11, KeyValue, ucArray_ID );//У������ 
        WriteAmount(0x11,writeValue); //д����
        if(ReadAmount(0x11,&readValue) == MI_OK)	//��ȡ���
				{		
					//writeValue +=100;
				  sprintf ( cStr, "The Card ID is: %02X%02X%02X%02X",ucArray_ID [0], ucArray_ID [1], ucArray_ID [2],ucArray_ID [3] );
					printf ( "%s\r\n",cStr );  //��ӡ��ƬID
          LCD_DisplayStringLine(LINE(0), (uint8_t*)cStr);
					
					printf ("���Ϊ��%d\r\n",readValue);
					sprintf ( cStr, "TThe residual amount: %d", readValue);				 										 	         
					LCD_DisplayStringLine(LINE(1) , (uint8_t*)cStr);
          PcdHalt();
				}
        
        /* ����1��� */
        if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
        {
          printf("��� +100\r\n");
          writeValue += 100;
        }
        /* ����2��� */
        if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  )
        {
          printf("��� -100\r\n");
          writeValue -= 100;
        }
			}				
		}		
		    
  }	
}

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main ( void )
{
  /*�δ�ʱ�ӳ�ʼ��*/
  SysTick_Init();   

  /*USART1 ����ģʽΪ 115200 8-N-1���жϽ���*/
  Debug_USART_Config();   

  /* Key ��ʼ�� */
  Key_GPIO_Config();

  /*RC522ģ����������ĳ�ʼ������*/
  RC522_Init ();     

  /*��ʼ��Һ����*/
  LCD_Init();
  LCD_LayerInit();

  printf ( "WF-RC522 Test\n" );

  LTDC_Cmd(ENABLE);
	
	/*�ѱ�����ˢ��ɫ*/
  LCD_SetLayer(LCD_BACKGROUND_LAYER);  
	LCD_Clear(LCD_COLOR_BLACK);
	
  /*��ʼ����Ĭ��ʹ��ǰ����*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
  
	/*Ĭ�����ò�͸��	���ú�������Ϊ��͸���ȣ���Χ 0-0xff ��0Ϊȫ͸����0xffΪ��͸��*/
  LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BLACK);
  
	/*����LCD_SetLayer(LCD_FOREGROUND_LAYER)������
	����Һ����������ǰ����ˢ�£��������µ��ù�LCD_SetLayer�������ñ�����*/		
  LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_BLACK);
  
  LCD_SetTextColor(LCD_COLOR_BLUE);

  LCD_DisplayStringLine(LINE(14),
               (uint8_t*)"Please put the IC card on WF-RC522 antenna area ..." );

  PcdReset ();
  M500PcdConfigISOType ( 'A' );/*���ù�����ʽ*/

  while ( 1 )
  {
    IC_test ();/*IC�����*/	
  }	
}



/*********************************************END OF FILE**********************/

