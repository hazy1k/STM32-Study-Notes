/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   RFID-RC522ģ��ʵ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������
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
#include "./lcd/bsp_ili9806g_lcd.h"
#include "./font/fonts.h"	
#include "./systick/bsp_SysTick.h"
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
          ILI9806G_DispStringLine_EN(LINE(0) , cStr);
					
					printf ("���Ϊ��%d\r\n",readValue);
					sprintf ( cStr, "TThe residual amount: %d", readValue);				 										 	         
					ILI9806G_DispStringLine_EN(LINE(1) , cStr);
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
	SysTick_Init();       //�δ�ʱ�ӳ�ʼ��
	
	Debug_USART_Config(); //USART1 ����ģʽΪ 115200 8-N-1���жϽ��� 
	
  /* Key ��ʼ�� */
  Key_GPIO_Config();
  
	RC522_Init ();        //RC522ģ����������ĳ�ʼ������
 	
	ILI9806G_Init ();     //LCD ��ʼ��
	ILI9806G_GramScan ( 6 );

	printf ( "WF-RC522 Test\n" );
	
	LCD_SetFont(&Font16x32);
	LCD_SetColors(RED,BLACK);

  ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH); //��������ʾȫ�� 
	ILI9806G_DispStringLine_EN(LINE(16) ,
             "Please put the IC card on WF-RC522 antenna area ..." );
	
	PcdReset ();
	M500PcdConfigISOType ( 'A' );//���ù�����ʽ
		
  while ( 1 )
  {
    IC_test ();//IC�����	
  }	
}


/*********************************************END OF FILE**********************/

