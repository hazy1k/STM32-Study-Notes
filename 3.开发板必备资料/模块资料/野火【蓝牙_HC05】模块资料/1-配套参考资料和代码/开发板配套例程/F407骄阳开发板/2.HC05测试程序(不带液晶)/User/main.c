/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   HC05���Գ��򣨲���Һ����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ���� ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h"
#include "./usart/bsp_debug_usart.h"
#include "./hc05/bsp_hc05.h"
#include "./hc05/bsp_blt_usart.h"
#include "./systick/bsp_SysTick.h"
#include <stdlib.h>
#include <string.h>

BLTDev bltDevList;
unsigned int Task_Delay[NumOfTask]; 
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
	/* ϵͳʱ�ӳ�ʼ����168MHz */
	SystemClock_Config();
	
   //��ʼ��systick 
	SysTick_Init();
	
	LED_GPIO_Config();
	
	Key_GPIO_Config(); 
	/* ���ô���1Ϊ��115200 8-N-1 */
	DEBUG_USART_Config();
  
	HC05_INFO("**********HC05ģ��ʵ��************");
	
	if(HC05_Init() == 0)
		HC05_INFO("HC05ģ����������");
	else
	{
		HC05_ERROR("HC05ģ���ⲻ����������ģ���뿪��������ӣ�Ȼ��λ���������²��ԡ�");
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
	


	while(1)
	{

		//��������ģ�飬����������
		if(Task_Delay[2]==0 && !IS_HC05_CONNECTED() ) 
		{
			if(hc05_role == 1)	//��ģʽ
			{
				HC05_INFO("����ɨ�������豸...");
				
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

					}
					else if(strcmp(redata,"AT+LED1=OFF")==0)
					{
						LED1_OFF;
						HC05_SendString("+LED1:OFF\r\nOK\r\n");

					}
					else
					{	
						HC05_INFO("receive:\r\n%s",redata);
					}
					
					/*�������ݺ���ս�������ģ�����ݵĻ�����*/
					clean_rebuff();
					
				}
			Task_Delay[0]=600;//��ֵÿ1ms���1������0�ſ������½����������ִ�е�������500ms
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
							HC05_INFO("�豸���ֱ�����Ϊ��%s",hc05_name);
						else
							HC05_ERROR("��������ʧ��");
						
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
						HC05_INFO("�豸���ֱ�����Ϊ��%s",hc05_name);
					else
						HC05_ERROR("��������ʧ��");
						
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
					HC05_INFO("�豸���ֱ�����Ϊ��%s",hc05_name);
				else
					HC05_ERROR("��������ʧ��");
			}
	}

}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
 void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1) {};
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    while(1) {};
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
