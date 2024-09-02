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



extern uint32_t Task_Delay_Group[]; //�˴���Ϊ���ã����ڲ���һ�������
extern int hc05_inquery_connect;
extern int hc05_check_recvbuff;


char sendData[1024];
char linebuff[1024];

char hc05_nameCMD[40];
char disp_buff[200];

uint8_t hc05_role=0;
char hc05_mode[10]="SLAVE";
char hc05_name[30]="HC05_SLAVE";
char * linkhint = "���ȶϿ���������!!!\r\n";
char * linkhint_en = "Please disconnect the Bluetooth connection first";


//���Ժ�������
void CheckConnect_LinkHC05_Test(void);
void CheckRecvBltBuff_Test(void);
void Switch_HC05Mode_Test(void);
void Generate_Modify_HC05Name(void);

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
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
	
	sprintf(disp_buff,"Device name:%s",hc05_name);
  #ifdef ENABLE_LCD_DISPLAY
	NT35510_DispString_EN( 20, 100, disp_buff );
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

    //ÿ 5s ���һ���������ӣ���������ģ�飬����������
    if( 1 == hc05_inquery_connect )
    {
      hc05_inquery_connect = 0; //�����־λ

      CheckConnect_LinkHC05_Test();
    }
    
    //���Ӻ�ÿ��һ��ʱ������ջ�����
		if(1 == hc05_check_recvbuff)
		{
      hc05_check_recvbuff = 0; //�����־λ
      
      CheckRecvBltBuff_Test();
		}
		
    
		//���KEY1���������л�master-slaveģʽ
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
		{
      Switch_HC05Mode_Test();
		}
		
		//���KEY2���������������һ������
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  )
		{
      Generate_Modify_HC05Name();
		}
		
	}
}

/**
  * @brief  �����������
  *         ��Ϊ����ʱ�������������������ֺ��С�HC05��������ģ��
  *         ��Ϊ�ӻ�ʱ�����Ӻ�ͨ������ģ�鷢���ַ���
  * @param  ��
  * @retval ��
  */
void CheckConnect_LinkHC05_Test(void)
{
  if( ! IS_HC05_CONNECTED() )
  {
    Usart_SendString( DEBUG_USART, "������δ����\r\n" );
    
    if(hc05_role == 1)	//��ģʽ
    {
      #ifdef ENABLE_LCD_DISPLAY
      static uint8_t color=0;

      if(color==0)
      {
        color=1;
      }
      else
      {
        LCD_SetColors(BLUE,BLACK); //�л�������ɫΪ��ɫ
        color=0;
      }
      NT35510_DispString_EN ( 5, 130,"Scaning bluetooth device..." );
      #endif
      
      HC05_INFO("����ɨ�������豸...");
      /*��������ģ�飬����������*/
      if( linkHC05() == 0 )
      {
        while( ! IS_HC05_CONNECTED() ); //���ӳɹ����ȴ�HC05ģ���INT����״̬�ı�
      }
      
    }
    else	//��ģʽ
    {
      HC05_INFO("��������������...");
      HC05_Send_CMD("AT+INQ\r\n",1);//ģ���ڲ�ѯ״̬���������ױ������豸������

      #ifdef ENABLE_LCD_DISPLAY
      NT35510_Clear(0,130,480,240);
      #endif
    }
  }
  else
  {
    static uint8_t testdata=0;
    HC05_INFO("����������");
    
    //���Ӻ�ÿ��һ��ʱ��ͨ������ģ�鷢���ַ���
    sprintf(sendData,"<%s> send data test,testdata=%d\r\n",hc05_name,testdata++);
    HC05_SendString(sendData);	
  }
}


/**
  * @brief  ����������ջ������������������
  * @param  ��
  * @retval ��
  */
void CheckRecvBltBuff_Test(void)
{
  char* redata;
	uint16_t len;
  
  if( IS_HC05_CONNECTED() )
  {
    uint16_t linelen;
  
    #ifdef ENABLE_LCD_DISPLAY
    LCD_SetColors(YELLOW,BLACK);
    NT35510_Clear(0,160,480,20);
    NT35510_DispString_EN( 5, 160,"Bluetooth connected!"  );
    #endif

    /*��ȡ����*/
    redata = get_rebuff(&len); 
    linelen = get_line(linebuff,redata,len);
  
    /*��������Ƿ��и���*/
    if(linelen<200 && linelen != 0)
    {
      #ifdef ENABLE_LCD_DISPLAY
      LCD_SetColors(YELLOW,BLACK);
      #endif

      if(strcmp(redata,"AT+LED1=ON")==0)
      {
        LED1_ON;						
        HC05_SendString("+LED1:ON\r\nOK\r\n");	
        
        #ifdef ENABLE_LCD_DISPLAY
        NT35510_Clear(0,200,480,20);							
        NT35510_DispString_EN ( 5, 200, "receive cmd: AT+LED1=ON" );
        #endif
      }
      else if(strcmp(redata,"AT+LED1=OFF")==0)
      {
        LED1_OFF;
        HC05_SendString("+LED1:OFF\r\nOK\r\n");
        
        #ifdef ENABLE_LCD_DISPLAY
        NT35510_Clear(0,200,480,20);	
        NT35510_DispString_EN ( 5, 200, "receive cmd: AT+LED1=OFF" );
        #endif
      }
      else
      {
        /*����ֻ��ʾ��ʾ���е����ݣ��������ʾ���������ݣ���ֱ��ʹ��redata����*/
        HC05_INFO("receive:\r\n%s",linebuff);
        
        #ifdef ENABLE_LCD_DISPLAY
        NT35510_Clear(0,240,480,200);
        LCD_SetColors(RED,BLACK);
        NT35510_DispString_EN( 5, 240,"receive data:" );
        LCD_SetColors(YELLOW,BLACK);
        NT35510_DispString_EN( 5, 280,linebuff );
        #endif
      }
      
      /*�������ݺ���ս�������ģ�����ݵĻ�����*/
      clean_rebuff();
      
    }
  }
  //�������û������
  else
  {
//        Usart_SendString( DEBUG_USARTx, "\r\n����δ���ӡ����յ������������ݣ�\r\n" );
//        redata = get_rebuff(&len); 
//        Usart_SendString( DEBUG_USARTx, (uint8_t *)redata );
//        Usart_SendString( DEBUG_USARTx, "\r\n\r\n" );
  }
}


void Switch_HC05Mode_Test(void)
{
  if( ! IS_HC05_CONNECTED() )
  {
    hc05_role = !hc05_role;
    if(hc05_role == 0)
    {						
        HC05_Send_CMD("AT+RESET\r\n",1);
        Delay_ms(800);

        if(HC05_Send_CMD("AT+ROLE=0\r\n",1) == 0)	
        {				
          Delay_ms(100);
          
          sprintf(hc05_mode,"SLAVE");
          HC05_INFO("hc05_mode  = %s",hc05_mode);	

          sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
          sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
          
          if(HC05_Send_CMD(hc05_nameCMD,1) == 0)
          {
            HC05_INFO("�豸���ֱ�����Ϊ��%s",hc05_name);
            
            sprintf(disp_buff,"Device name: %s",hc05_name);
            
            #ifdef ENABLE_LCD_DISPLAY
            LCD_SetColors(RED,BLACK);
            NT35510_Clear(0,100,480,700);
            NT35510_DispString_EN( 5, 100,disp_buff );
            #endif
          }
          else
          {							
            HC05_ERROR("��������ʧ��");
            
            #ifdef ENABLE_LCD_DISPLAY
            LCD_SetColors(BLUE,BLACK);
            NT35510_Clear(0,100,480,700);
            NT35510_DispString_EN( 5, 100,"Rename fail!" );
            #endif
          }
          
          HC05_Send_CMD("AT+INIT\r\n",1);
          HC05_Send_CMD("AT+CLASS=0\r\n",1);
          HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);
          
          //��������豸�б�
          bltDevList.num = 0;
        }
    }
    else
    {
      HC05_Send_CMD("AT+RESET\r\n",1);
      Delay_ms(800);
      
      if(HC05_Send_CMD("AT+ROLE=1\r\n",1) == 0)	
      {
        Delay_ms(100);
        
        sprintf(hc05_mode,"MASTER");
        HC05_INFO("HC05 mode  = %s",hc05_mode);
          
        sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
        sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);	
        
        if(HC05_Send_CMD(hc05_nameCMD,1) == 0)
        {
          HC05_INFO("�豸���ֱ�����Ϊ��%s",hc05_name);
          sprintf(disp_buff,"Device name: %s",hc05_name);
          
          #ifdef ENABLE_LCD_DISPLAY
          LCD_SetColors(RED,BLACK);
          NT35510_Clear(0,100,480,20);
          NT35510_DispString_EN( 5, 100,disp_buff );
          #endif
        }
        else
        {
          HC05_ERROR("��������ʧ��");
          
          #ifdef ENABLE_LCD_DISPLAY
          LCD_SetColors(BLUE,BLACK);
          NT35510_Clear(0,100,480,20);
          NT35510_DispString_EN( 5, 100,"Rename fail!" );
          #endif
        }
          
        HC05_Send_CMD("AT+INIT\r\n",1);
        HC05_Send_CMD("AT+CLASS=0\r\n",1);
        HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);	
        
        //��������豸�б�
        bltDevList.num = 0;

      }					

    }
  }
  else
  {
    HC05_INFO("%s", linkhint);
    HC05_SendString(linkhint_en);
  }
}


void Generate_Modify_HC05Name(void)
{
  unsigned long count;
	
  if( ! IS_HC05_CONNECTED() )
  {
    /*�����������������������*/
    count = Task_Delay_Group[0];
    srand(count);

    sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
    sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);

    if(HC05_Send_CMD(hc05_nameCMD,1) == 0)
    {
      HC05_INFO("�豸���ֱ�����Ϊ��%s",hc05_name);
      sprintf(disp_buff,"Device name: %s",hc05_name);
      
      #ifdef ENABLE_LCD_DISPLAY
      LCD_SetColors(RED,BLACK);
      NT35510_Clear(0,100,480,20);
      NT35510_DispString_EN( 5, 100,disp_buff );
      #endif
    }
    else
    {
      HC05_ERROR("��������ʧ��");
      
      #ifdef ENABLE_LCD_DISPLAY
      LCD_SetColors(BLUE,BLACK);
      NT35510_Clear(0,100,480,20);
      NT35510_DispString_EN( 5, 100,"Rename fail!"  );
      #endif
    }
  }
  else
  {
    HC05_INFO("%s", linkhint); //��ʾ����ʧ�ܣ���Ҫ�ȶϿ��������Ӳ��ܸ���
    HC05_SendString(linkhint_en);
  }
}



/*********************************************END OF FILE**********************/

