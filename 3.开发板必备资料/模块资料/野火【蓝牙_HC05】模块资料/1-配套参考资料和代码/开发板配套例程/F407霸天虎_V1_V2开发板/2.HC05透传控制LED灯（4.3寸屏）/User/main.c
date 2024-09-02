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
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������ 
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
#include "./lcd/bsp_nt35510_lcd.h"
#include <string.h>
#include <stdlib.h>
#include "./dwt_delay/core_delay.h"   
#include "./i2c/i2c.h"
#include "./mpu6050/mpu6050.h"



extern ReceiveData DEBUG_USART_ReceiveData;
extern ReceiveData BLT_USART_ReceiveData;
extern int hc05_inquery_connect;

//��������
void TransData_CtrlLED_Test(void);


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
  char hc05_name[30]="HC05_SLAVE";
	char hc05_nameCMD[40];
  
  unsigned char mpu_reg = 0; //�����ȡMPU6050�Ĵ�����ֵ
  
  
	/* ��ʱ������ʼ�� */
  CPU_TS_TmrInit();

  /* LCD��ʼ�� */
  #ifdef ENABLE_LCD_DISPLAY
	NT35510_Init();	
	NT35510_GramScan( 6 );
	LCD_SetFont(&Font16x32);
	LCD_SetColors(RED,BLACK);
  NT35510_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */
  #endif
	
  /* ���Դ��ڳ�ʼ�� USART1 ����ģʽΪ 115200 8-N-1 �����ж� */
	Debug_USART_Config();

	LED_GPIO_Config();
	Key_GPIO_Config();
	
  /* HC05����ģ���ʼ����GPIO �� USART3 ����ģʽΪ 38400 8-N-1 �����ж� */
	if(HC05_Init() == 0)
	{
		HC05_INFO("HC05ģ����������");
		#ifdef ENABLE_LCD_DISPLAY
		NT35510_DispString_EN ( 40, 60, "HC05 module detected!" );
    #endif
	}
	else
	{
		HC05_ERROR("HC05ģ���ⲻ����������ģ���뿪��������ӣ�Ȼ��λ���������²��ԡ�");
		#ifdef ENABLE_LCD_DISPLAY
		NT35510_DispString_EN ( 20, 60, "No HC05 module detected!"  );
		NT35510_DispString_EN ( 5, 100, "Please check the hardware connection and reset the system." );
    #endif
    
		while(1);
	}


	HC05_INFO("**********HC05ģ��ʵ��************");
	#ifdef ENABLE_LCD_DISPLAY
	NT35510_DispString_EN ( 40, 20, "HC05 BlueTooth Demo" );
  #endif
  
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
	NT35510_DispStringLine_EN ( (LINE(4)), "ReceiveData USART1" );	
  NT35510_DispStringLine_EN ( (LINE(12)), "ReceiveData HC-05" );
  #endif
  
  
  
  /* �ⲿ���Ƕ���Ĵ���
     Ŀ����Ϊ�˷�ֹ���� MPU6050 ��INT���Ÿ���STM32 PF10���ŵĵ�ƽ�仯
     ʹ�ð��컢�������ⲿ�ֲ���ȥ��������Ӱ��HC05������״̬�ļ��
  */
	I2cMaster_Init(); //��ʼ�� I2C
	if( ! MPU6050ReadID() ) {printf("�޷���ȡMPU6050\r\n"); while(1);}
  MPU6050_WriteReg(MPU6050_RA_INT_PIN_CFG, (mpu_reg |((1<<MPU6050_INTCFG_INT_LEVEL_BIT) | 
                                                      (1<<MPU6050_INTCFG_INT_OPEN_BIT) | 
                                                      (1<<MPU6050_INTCFG_LATCH_INT_EN_BIT)) ) );
  
  
  /* SysTick 10ms�жϳ�ʼ�� */
  SysTick_Init();
  

	while(1)
	{
    //ÿ 5s ���һ����������
    if( 1 == hc05_inquery_connect )
    {
      hc05_inquery_connect = 0; //�����־λ

      if( ! IS_HC05_CONNECTED() )
      {
        HC05_Send_CMD("AT+INQ\r\n",1);//ģ���ڲ�ѯ״̬���������ױ������豸������
        Usart_SendString( DEBUG_USART, "������δ���ӡ������ֻ���������������������������\r\n" );
      }
      else
      {
        Usart_SendString( DEBUG_USART, "���������ӡ����͡�RED_LED���ɿ��Ʒ�תLED��\r\n" );
      }
    }
    
    TransData_CtrlLED_Test(); //��������
    
    
	}
  
  
}


/**
  * @brief  ���������ݿ���LED��
  * @param  ��
  * @retval ��
  */
void TransData_CtrlLED_Test(void)
{
  /* ������Դ��ڽ��յ��Ĵ����������� */
  if(DEBUG_USART_ReceiveData.receive_data_flag == 1)
  {
    DEBUG_USART_ReceiveData.uart_buff[DEBUG_USART_ReceiveData.datanum] = 0;
    //����������Լ�������Ҫ���յ��ַ���Ȼ����
    //������յ����ڵ������ַ����� ��RED_LED���ͻ�Ѱ�������ĺ��ȡ��һ��
    if( strstr((char *)DEBUG_USART_ReceiveData.uart_buff,"RED_LED") )
    {
      LED1_TOGGLE;
    }

    //����ֻ�����������ģ�飬���͵��ֻ�����ʾ����
    if( IS_HC05_CONNECTED() )
    {
      BLT_KEY_LOW;
      Usart_SendStr_length(BLT_USARTx, DEBUG_USART_ReceiveData.uart_buff, DEBUG_USART_ReceiveData.datanum);	
    }
    //�������û�б����ӣ������������AT��ͷ�ģ��Ͱ�KEY�øߣ���������ģ��
    else if( strstr((char *)DEBUG_USART_ReceiveData.uart_buff,"AT") == (char *)DEBUG_USART_ReceiveData.uart_buff )
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
    NT35510_DispStringLine_EN ( (LINE(5)), (char *)DEBUG_USART_ReceiveData.uart_buff );
    #endif
    
    //������Դ������ݻ���
    DEBUG_USART_ReceiveData.receive_data_flag = 0;		//�������ݱ�־����
    DEBUG_USART_ReceiveData.datanum = 0;               
  }
  
  
  /* �����������ڽ��յ����������� */
  if(BLT_USART_ReceiveData.receive_data_flag == 1)
  {
    BLT_USART_ReceiveData.uart_buff[BLT_USART_ReceiveData.datanum] = 0;
    //����������Լ�������Ҫ���յ��ַ���Ȼ����
    //������յ��ֻ����������� ��RED_LED���ͻ�Ѱ�������ĺ��ȡ��һ��
    if(strstr((char *)BLT_USART_ReceiveData.uart_buff,"RED_LED"))
    {
      LED1_TOGGLE;
    }
    
    //����������ʾ���յ�������
    Usart_SendString( DEBUG_USART, "\r\nrecv HC-05 data:\r\n" );
    Usart_SendString( DEBUG_USART, BLT_USART_ReceiveData.uart_buff );
    Usart_SendString( DEBUG_USART, "\r\n" );
    
    //LCD��ʾ���յ�������
    #ifdef ENABLE_LCD_DISPLAY
    LCD_ClearLine(LINE(13));
    LCD_ClearLine(LINE(14));
    LCD_ClearLine(LINE(15));
    LCD_ClearLine(LINE(16));
    NT35510_DispStringLine_EN ( (LINE(13)), (char *)BLT_USART_ReceiveData.uart_buff );
    #endif
    
    //���������������ݻ���
    BLT_USART_ReceiveData.receive_data_flag = 0;		//�������ݱ�־����
    BLT_USART_ReceiveData.datanum = 0;  
  }
}




/*********************************************END OF FILE**********************/

