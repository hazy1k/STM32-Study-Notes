/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2014-08-xx
  * @brief   GSMģ��绰���Ų��Գ���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-MINI STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
	*/
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./gsm_gprs/bsp_gsm_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./gsm_gprs/bsp_gsm_gprs.h"
#include "./key/bsp_key.h" 
#include <string.h>

char    testbuff[100];
uint8_t len;
char *  rebuff;

/*
 * ���Ե绰���Ź���
 * 
 */
int main(void)
{
	static uint8_t timecount=0;
	char num[20]={0};
    /* ����USART */
    USART_Config();
    Key_GPIO_Config();
    /* ��ʼ��ϵͳ��ʱ�� */
    SysTick_Init();
    
    printf("\r\nҰ��BH-GSMģ�鲦������\r\n");          
    
    
    //���ģ����Ӧ�Ƿ�����
    while(gsm_init()!= GSM_TRUE)
    {
      printf("\r\nģ����Ӧ���Բ���������\r\n");
      printf("\r\n��ģ����Ӧ����һֱ������������ģ������ӻ��Ƿ��ѿ�����Դ����\r\n");
			GSM_DELAY(500);

    }
    
    printf("\r\nͨ����ģ����Ӧ���ԣ�5���ʼ���Ų���...\r\n");
    
    /* ��ʱ3���ٷ������ģ�� */
    GSM_DELAY(5000);
    
    //����绰
  
    gsm_call("112");                      //����112�绰����
    rebuff = gsm_waitask(0);
    if(strstr(rebuff,"ATD") != NULL)           //��ӦOK����ʾģ���������յ�����
    {
      printf("\r\n���ں���\r\n");
      GSM_CLEAN_RX();                     //������ջ�����
      rebuff = gsm_waitask(0);            //���µȴ���Ϣ
     
      if(strstr(rebuff,"NO CARRIER") != NULL) //��ӦNO CARRIER,ͨ������
       {
         printf("\r\nͨ������\r\n");
       }
      else if(strstr(rebuff,"NO ANSWER") != NULL)   //��ӦNO ANSWER�����˽���
      {
        printf("\r\n�㲦��ĵ绰��ʱ���˽��������Ժ��ٲ�\r\n");
      }  
    }
	
	GSM_CLEAN_RX();                     //������ջ�����
	
    while(1)
	{
		if(timecount>=100)
		{
			if(IsRing(num)== GSM_TRUE)
			{
				printf("Ringing:\nFrom:%s\n�밴��KEY2�������߰���KEY1�Ҷ�\n\r",num);	
			}
			timecount=0;
		}
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
			GSM_HANGON();
		}
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
			GSM_HANGOFF();
		}	
		timecount++;
		GSM_DELAY(10);
	}
	
}



