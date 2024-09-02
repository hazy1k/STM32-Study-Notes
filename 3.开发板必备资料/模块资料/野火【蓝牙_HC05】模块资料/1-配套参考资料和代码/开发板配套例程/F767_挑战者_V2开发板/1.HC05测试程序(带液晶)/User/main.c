/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   HC05����ģ����Գ���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32F767 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "main.h"    
#include "stm32f7xx.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./lcd/bsp_lcd.h"
#include "./hc05/bsp_hc05.h"
#include "./hc05/bsp_blt_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./key/bsp_key.h"
#include "./lcd/bsp_lcd.h"
#include <stdlib.h>
#include <string.h>


unsigned int Task_Delay[NumOfTask]; 
BLTDev bltDevList;
char sendData[1024];
char linebuff[1024];

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{   
  char* redata;
	uint16_t len;
	
	static uint8_t hc05_role=0;
	unsigned long count;
	
	char hc05_mode[10]="SLAVE";
	char hc05_name[30]="HC05_SLAVE";
	char hc05_nameCMD[40];
	char disp_buff[200];
	
	/* ϵͳʱ�ӳ�ʼ����216MHz */
	SystemClock_Config();
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
	/* ���ô���1Ϊ��115200 8-N-1 */
	DEBUG_USART_Config();
	/* ������ʼ�� */
  Key_GPIO_Config();
  /* ��ʼ��systick */
	SysTick_Init();	
	/* LCD �˿ڳ�ʼ�� */ 
	LCD_Init();
	/* LCD ��һ���ʼ�� */ 
	LCD_LayerInit(0, LCD_FB_START_ADDRESS,RGB888);
	/* ʹ��LCD������������ */ 
	LCD_DisplayOn(); 
	/* ѡ��LCD��һ�� */
	LCD_SelectLayer(0);
	/* ��һ����������ʾȫ�� */ 
	LCD_Clear(LCD_COLOR_BLACK);  
	LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_BLACK);

	HC05_INFO("**********HC05ģ��ʵ��************");
	LCD_DisplayStringLine (1,(uint8_t*) "HC05 BlueTooth Demo" );

	
	if(HC05_Init() == 0)
	{
		HC05_INFO("HC05ģ����������");
		LCD_DisplayStringLine ( 2,(uint8_t*) "HC05 module detected!" );

	}
	else
	{
		HC05_ERROR("HC05ģ���ⲻ����������ģ���뿪��������ӣ�Ȼ��λ���������²��ԡ�");
		LCD_DisplayStringLine ( 3, (uint8_t*)"No HC05 module detected!"  );
		LCD_DisplayStringLine ( 4,(uint8_t*) "Please check the hardware connection and reset the system." );

		while(1);
	}


	/*��λ���ָ�Ĭ��״̬*/
	HC05_Send_CMD("AT+RESET\r\n",1);	
	delay_ms(800);
	
	HC05_Send_CMD("AT+ORGL\r\n",1);
	delay_ms(200);

	
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
	LCD_DisplayStringLine( 5,(uint8_t*) disp_buff );



	while(1)
	{

		//��������ģ�飬����������
		if(Task_Delay[2]==0 && !IS_HC05_CONNECTED() ) 
		{
			if(hc05_role == 1)	//��ģʽ
			{
				static uint8_t color=0;
				
				HC05_INFO("����ɨ�������豸...");		
				
				if(color==0)	
				{
					LCD_DisplayStringLine ( 6,(uint8_t*)"Scaning bluetooth device..." );
					color=1;
				}
				else
				{

					LCD_DisplayStringLine (6,(uint8_t*)"Scaning bluetooth device..." );
					color=0;
				}				

				/*��������ģ�飬����������*/
				linkHC05();
				Task_Delay[2]=3000; //��ֵÿ1ms���1������0�ſ������½����������ִ�е�������3s

			}
			else	//��ģʽ
			{
					HC05_Send_CMD("AT+INQ\r\n",1);//ģ���ڲ�ѯ״̬���������ױ������豸������
					delay_ms(1000);
					HC05_Send_CMD("AT+INQC\r\n",1);//�жϲ�ѯ����ֹ��ѯ�Ľ�����Ŵ���͸���Ľ���	
					Task_Delay[2]=2000; //��ֵÿ1ms���1������0�ſ������½����������ִ�е�������2s
			}
			
		}				
		
			//���Ӻ�ÿ��һ��ʱ������ջ�����
		if(Task_Delay[0]==0 && IS_HC05_CONNECTED())  
		{
				uint16_t linelen;
			

				LCD_DisplayStringLine(7,(uint8_t*)"Bluetooth connected!"  );


				/*��ȡ����*/
				redata = get_rebuff(&len); 
				linelen = get_line(linebuff,redata,len);
			
				/*��������Ƿ��и���*/
				if(linelen<200 && linelen != 0)
				{
					
					if(strcmp(redata,"AT+LED1=ON")==0)
					{
						LED1_ON;						
						HC05_SendString("+LED1:ON\r\nOK\r\n");	
										
						LCD_DisplayStringLine (8, (uint8_t*)"receive cmd: AT+LED1=ON" );
					}
					else if(strcmp(redata,"AT+LED1=OFF")==0)
					{
						LED1_OFF;
						HC05_SendString("+LED1:OFF\r\nOK\r\n");
						
						LCD_DisplayStringLine (9, (uint8_t*)"receive cmd: AT+LED1=OFF" );
					}
					else
					{
						/*����ֻ��ʾ��ʾ���е����ݣ��������ʾ���������ݣ���ֱ��ʹ��redata����*/
						HC05_INFO("receive:\r\n%s",linebuff);
						

						LCD_DisplayStringLine( 10,(uint8_t*)"receive data:" );

						LCD_DisplayStringLine( 11,(uint8_t*)linebuff );

					}
					
					/*�������ݺ���ս�������ģ�����ݵĻ�����*/
					clean_rebuff();
					
				}
			Task_Delay[0]=200;//��ֵÿ1ms���1������0�ſ������½����������ִ�е�������500ms
		}
		
		//���Ӻ�ÿ��һ��ʱ��ͨ������ģ�鷢���ַ���
		if(Task_Delay[1]==0 && IS_HC05_CONNECTED())
		{
			static uint8_t testdata=0;
		
			sprintf(sendData,"<%s> send data test,testdata=%d\r\n",hc05_name,testdata++);
			HC05_SendString(sendData);			

			Task_Delay[1]=50000;//��ֵÿ1ms���1������0�ſ������½����������ִ�е�������5000ms

		}		
		
		//���KEY1���������л�master-slaveģʽ
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
		{
			hc05_role=!hc05_role;
			if(hc05_role == 0)
			{						
					HC05_Send_CMD("AT+RESET\r\n",1);
					delay_ms(800);

					if(HC05_Send_CMD("AT+ROLE=0\r\n",1) == 0)	
					{				
						delay_ms(100);
						
						sprintf(hc05_mode,"SLAVE");
						HC05_INFO("hc05_mode  = %s",hc05_mode);	

						sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
						sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
						
						if(HC05_Send_CMD(hc05_nameCMD,1) == 0)
						{
							HC05_INFO("�豸���ֱ�����Ϊ��%s",hc05_name);
							
							sprintf(disp_buff,"Device name: %s",hc05_name);

							LCD_DisplayStringLine( 12,(uint8_t*)disp_buff );
						}
						else
						{							
							HC05_ERROR("��������ʧ��");
							
							LCD_DisplayStringLine( 12,(uint8_t*)"Rename fail!" );
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
				delay_ms(800);
				
				if(HC05_Send_CMD("AT+ROLE=1\r\n",1) == 0)	
				{
					delay_ms(100);
					
					sprintf(hc05_mode,"MASTER");
					HC05_INFO("HC05 mode  = %s",hc05_mode);
						
					sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
					sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);	
					
					if(HC05_Send_CMD(hc05_nameCMD,1) == 0)
					{
						HC05_INFO("�豸���ֱ�����Ϊ��%s",hc05_name);
						sprintf(disp_buff,"Device name: %s",hc05_name);
						
						LCD_DisplayStringLine(13,(uint8_t*)disp_buff );

					}
					else
					{
						HC05_ERROR("��������ʧ��");
						
						LCD_DisplayStringLine( 13,(uint8_t*)"Rename fail!" );
					}
						
					HC05_Send_CMD("AT+INIT\r\n",1);
					HC05_Send_CMD("AT+CLASS=0\r\n",1);
					HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);	
					
					//��������豸�б�
					bltDevList.num = 0;

				}					

			}

		}
		
		//���KEY2���������������һ������
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  )
		{
				/*�����������������������*/
				get_tick_count(&count);
				srand(count);
			
				sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
				sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
				
				if(HC05_Send_CMD(hc05_nameCMD,1) == 0)
				{
					HC05_INFO("�豸���ֱ�����Ϊ��%s",hc05_name);
					sprintf(disp_buff,"Device name: %s",hc05_name);
					
					LCD_DisplayStringLine( 14,(uint8_t*)disp_buff );

				}
				else
				{
					HC05_ERROR("��������ʧ��");
					
					LCD_DisplayStringLine(14,(uint8_t*)"Rename fail!"  );
				}

			}
	}
}


/**
  * @brief  System Clock ����
  *         system Clock �������� : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  ��
  * @retval ��
  */
void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	HAL_StatusTypeDef ret = HAL_OK;

	/* ʹ��HSE������HSEΪPLL��ʱ��Դ������PLL�ĸ��ַ�Ƶ����M N P Q 
	 * PLLCLK = HSE/M*N/P = 25M / 25 *432 / 2 = 216M
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 432;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 9;

	ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
	if(ret != HAL_OK)
	{
		while(1) { ; }
	}

	/* ���� OverDrive ģʽ�Դﵽ216MƵ��  */  
	ret = HAL_PWREx_EnableOverDrive();
	if(ret != HAL_OK)
	{
		while(1) { ; }
	}

	/* ѡ��PLLCLK��ΪSYSCLK�������� HCLK, PCLK1 and PCLK2 ��ʱ�ӷ�Ƶ���� 
	 * SYSCLK = PLLCLK     = 216M
	 * HCLK   = SYSCLK / 1 = 216M
	 * PCLK2  = SYSCLK / 2 = 108M
	 * PCLK1  = SYSCLK / 4 = 54M
	 */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2; 

	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
	if(ret != HAL_OK)
	{
		while(1) { ; }
	}  
}

/*********************************************END OF FILE**********************/

