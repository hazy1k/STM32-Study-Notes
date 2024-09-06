/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   �������MPU6050
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
#include "stm32f4xx_it.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"
#include "./i2c/i2c.h"
#include "./mpu6050/mpu6050.h"


//�����Ƿ�ʹ��LCD������ʾ������Ҫ�Ļ��������ע�͵�����
#define USE_LCD_DISPLAY

#ifdef USE_LCD_DISPLAY
 #include "./lcd/bsp_lcd.h"
#endif

/* MPU6050���� */
short Acel[3];
short Gyro[3];
float Temp;

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	/* LED �˿ڳ�ʼ�� */
  LED_GPIO_Config();

  /*��ʼ��USART1*/
  Debug_USART_Config(); 
	
#ifdef USE_LCD_DISPLAY	
		 /*��ʼ��Һ����*/
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
		
			/*����������ɫ������ı�����ɫ*/
		LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);	

		/*Ӣ������*/
		LCD_SetFont(&Font16x24); 	
		
		LCD_DisplayStringLine(LINE(1),(uint8_t* )"           This is a MPU6050 demo       ");			
		LCD_DisplayStringLine(LINE(2),(uint8_t* )"                 (Base Version)       ");		
#endif
	
  /* ����SysTick��ʱ�����ж� */
  SysTick_Init(); //���� SysTick Ϊ 1ms �ж�һ�Σ����ж����ȡ����������
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //������ʱ��
	
	//��ʼ�� I2C
	I2cMaster_Init(); 

	printf("\r\n ��ӭʹ��Ұ��  STM32 F429 �����塣\r\n");		 
	printf("\r\n ����һ��MPU6050�������� \r\n");

  //MPU6050��ʼ��
	MPU6050_Init();
	//���MPU6050
	if( MPU6050ReadID() == 0 )
  {
		printf("\r\nû�м�⵽MPU6050��������\r\n");
		LED_RED;
    
    #ifdef USE_LCD_DISPLAY			
      /*����������ɫ������ı�����ɫ*/
      LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BLACK);	

      LCD_DisplayStringLine(LINE(4),(uint8_t* )"No MPU6050 detected! ");			//Ұ���Դ���16*24��ʾ
      LCD_DisplayStringLine(LINE(5),(uint8_t* )"Please check the hardware connection! ");			//Ұ���Դ���16*24��ʾ
    #endif
    
		while(1);	//��ⲻ��MPU6050 ������Ȼ����
	}

  while(1)
  {
    if( task_readdata_finish ) //task_readdata_finish = 1 ��ʾ��ȡMPU6050�������
    {
      
      printf("���ٶȣ�%8d%8d%8d",Acel[0],Acel[1],Acel[2]);
      
      printf("    ������%8d%8d%8d",Gyro[0],Gyro[1],Gyro[2]);
      
      printf("    �¶�%8.2f\r\n",Temp);
      
      #ifdef USE_LCD_DISPLAY	
      {
        char cStr [ 70 ];
        sprintf ( cStr, "Acceleration:%8d%8d%8d",Acel[0],Acel[1],Acel[2] );	//���ٶ�ԭʼ����
        LCD_DisplayStringLine(LINE(7),(uint8_t* )cStr);			

        sprintf ( cStr, "Gyro        :%8d%8d%8d",Gyro[0],Gyro[1],Gyro[2] );	//��ԭʼ����
        LCD_DisplayStringLine(LINE(8),(uint8_t* )cStr);			

        sprintf ( cStr, "Temperture  :%8.2f",Temp );	//�¶�ֵ
        LCD_DisplayStringLine(LINE(9),(uint8_t* )cStr);			
      }
      #endif
      
      task_readdata_finish = 0; // �����־λ
    }
  }

}




/*********************************************END OF FILE**********************/

