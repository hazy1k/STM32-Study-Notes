/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 ����ģ��
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
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
#include "board.h"
#include "rtthread.h"


/*
*************************************************************************
*                               ����
*************************************************************************
*/
/* �����߳̿��ƿ� */
static rt_thread_t led1_thread = RT_NULL;

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void led1_thread_entry(void* parameter);
void LCD_Test(void);
void Printf_Charater(void);
/*
*************************************************************************
*                             main ����
*************************************************************************
*/
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
    /* 
	 * ������Ӳ����ʼ����RTTϵͳ��ʼ���Ѿ���main����֮ǰ��ɣ�
	 * ����component.c�ļ��е�rtthread_startup()����������ˡ�
	 * ������main�����У�ֻ��Ҫ�����̺߳������̼߳��ɡ�
	 */
	
	led1_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "led1",              /* �߳����� */
                      led1_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      3,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (led1_thread != RT_NULL)
        rt_thread_startup(led1_thread);
    else
        return -1;
}

/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/
/************************************************************
  * @brief   led1_thread_entry
  * @param   NULL
  * @return  NULL
  * @author  fire
  * @github  
  * @date    2018-xx-xx
  * @version v1.0
	* @note    �˴���dma����ʵ�飬Ұ�����д��rt_hw_console_output������
						 rt_kprintf��ʹ���˴���dma��ӡ��
  ***********************************************************/
static void led1_thread_entry(void* parameter)
{	
	rt_kprintf("\r\n ********** Һ����������ʾ�����ֿ����ⲿFLASH��*********** \r\n"); 
	rt_kprintf("\r\n ��������ʾ�����������Ķ������е�readme.txt�ļ�˵��������Ҫ���FLASH��ˢ��ģ����\r\n"); 
	
	Printf_Charater();
	
    while (1)
    {
			
				LCD_Test();
//        LED1_ON;
//        rt_thread_delay(500);   /* ��ʱ500��tick */
//        rt_kprintf("led1_thread running,LED1_ON\r\n");
//        
//        LED1_OFF;     
//        rt_thread_delay(500);   /* ��ʱ500��tick */		 		
//        rt_kprintf("led1_thread running,LED1_OFF\r\n");
    }
}






/*���ڲ��Ը���Һ���ĺ���*/
void LCD_Test(void)
{
	/*��ʾ��ʾ����*/
	static uint8_t testCNT = 0;	
	char dispBuff[100];
	
	testCNT++;	
	
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */
	/********��ʾ�ַ���ʾ��*******/ 
  ILI9341_DispStringLine_EN_CH(LINE(0),"����3.2��LCD������");
  ILI9341_DispStringLine_EN_CH(LINE(1),"�ֱ��ʣ�240x320 px");
  ILI9341_DispStringLine_EN_CH(LINE(2),"ILI9341Һ������");
  ILI9341_DispStringLine_EN_CH(LINE(3),"XPT2046����������");

	/********��ʾ����ʾ��*******/
	LCD_SetTextColor(GREEN);

	/*ʹ��c��׼��ѱ���ת�����ַ���*/
	sprintf(dispBuff,"��ʾ������ %d ",testCNT);
  LCD_ClearLine(LINE(5));	/* ����������� */
	
	/*Ȼ����ʾ���ַ������ɣ���������Ҳ����������*/
	ILI9341_DispStringLine_EN_CH(LINE(5),dispBuff);

	/*******��ʾͼ��ʾ��******/
  /* ��ֱ�� */
  
  LCD_ClearLine(LINE(7));/* ����������� */
	LCD_SetTextColor(BLUE);

  ILI9341_DispStringLine_EN_CH(LINE(7),"��ֱ�ߣ�");
  
	LCD_SetTextColor(RED);
  ILI9341_DrawLine(50,170,210,230);  
  ILI9341_DrawLine(50,200,210,240);
  
	LCD_SetTextColor(GREEN);
  ILI9341_DrawLine(100,170,200,230);  
  ILI9341_DrawLine(200,200,220,240);
	
	LCD_SetTextColor(BLUE);
  ILI9341_DrawLine(110,170,110,230);  
  ILI9341_DrawLine(130,200,220,240);
  
  rt_thread_delay(2000);
  
  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* ��������ʾȫ�� */
  
  
  /*������*/

  LCD_ClearLine(LINE(7));	/* ����������� */
	LCD_SetTextColor(BLUE);

  ILI9341_DispStringLine_EN_CH(LINE(7),"�����Σ�");

	LCD_SetTextColor(RED);
  ILI9341_DrawRectangle(50,200,100,30,1);
	
	LCD_SetTextColor(GREEN);
  ILI9341_DrawRectangle(160,200,20,40,0);
	
	LCD_SetTextColor(BLUE);
  ILI9341_DrawRectangle(170,200,50,20,1);
  
  rt_thread_delay(2000);
	
	ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* ��������ʾȫ�� */

  /* ��Բ */
  LCD_ClearLine(LINE(7));	/* ����������� */
	LCD_SetTextColor(BLUE);
	
  ILI9341_DispStringLine_EN_CH(LINE(7),"��Բ");
	
	LCD_SetTextColor(RED);
  ILI9341_DrawCircle(100,200,20,0);
	
	LCD_SetTextColor(GREEN);
  ILI9341_DrawCircle(100,200,10,1);
	
	LCD_SetTextColor(BLUE);
	ILI9341_DrawCircle(140,200,20,0);

  rt_thread_delay(2000);
  
  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* ��������ʾȫ�� */

}

/*"��"�ַ�����ģ16x16 */ 	  
unsigned char charater_matrix[] =    
{ /*"��",0*/
0x01,0x00,0x21,0x08,0x11,0x08,0x09,0x10,0x09,0x20,0x01,0x00,0x7F,0xF8,0x00,	0x08,
0x00,0x08,0x00,0x08,0x3F,0xF8,0x00,0x08,0x00,0x08,0x00,0x08,0x7F,0xF8,0x00,0x08,

};   
  
/**
  * @brief  ʹ�ô�������λ����ӡ��ģ
	*					��ʾ��ģ��ʾԭ��
  * @retval ��
  */
void Printf_Charater(void)   
{   
	int i,j;   
	unsigned char kk; 
  
	/*i�����м���*/
  for ( i=0;i<16;i++)   
	{   
		/*j����һ�ֽ������ݵ���λ����*/
		/*һ�����صĵ�һ���ֽ�*/
    for(j=0; j<8; j++)   
    {      
			/*һ������λһ������λ�ش���*/
      kk = charater_matrix[2*i] << j ;  //����Jλ          
			if( kk & 0x80)     
			{   
				rt_kprintf("*"); //������λΪ1�����*�ţ���ʾ�ʼ�
			}   
			else  
			{   
				rt_kprintf(" "); //������λΪ0������ո񣬱�ʾ�հ�  
			}   
    }   
		/*һ�����صĵڶ����ֽ�*/
    for(j=0; j<8; j++)   
		{              
      kk = charater_matrix[2*i+1] << j ;  //����Jλ   
       
			if( kk & 0x80)         
			{   
				rt_kprintf("*"); //������λΪ1�����*�ţ���ʾ�ʼ�   
			}   
			else  
			{   
				rt_kprintf(" "); //������λΪ0������ո񣬱�ʾ�հ� 
			}   
		}         
    rt_kprintf("\n");    //�����һ�����أ����� 
	}   
	rt_kprintf("\n\n"); 		//һ����������
}   



/********************************END OF FILE****************************/
