/**
  ******************************************************************************
  * @file    bsp_rtc.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   stm32 RTC ����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "./usart/bsp_debug_usart.h"
#include "./rtc/bsp_rtc.h"
#include "./usart/bsp_debug_usart.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./beep/bsp_beep.h"  

__IO uint8_t Alarmflag =0;

RTC_HandleTypeDef Rtc_Handle;
/**
  * @brief  ����ʱ�������
  * @param  ��
  * @retval ��
  */
void RTC_CalendarConfig(void)
{
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;

  /*##-1- Configure the Date #################################################*/
  sdatestructure.Year = YEAR;
  sdatestructure.Month = MONTH;
  sdatestructure.Date =  DATE;
  sdatestructure.WeekDay = WEEKDAY;
  
  if(HAL_RTC_SetDate(&Rtc_Handle,&sdatestructure,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    printf("\r\n��ʼ������ʧ��\r\n");
  }

  /*##-2- Configure the Time #################################################*/
  stimestructure.Hours =HOURS;
  stimestructure.Minutes = MINUTES;
  stimestructure.Seconds = SECONDS;

  if (HAL_RTC_SetTime(&Rtc_Handle, &stimestructure, RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
   printf("\r\n��ʼ��ʱ��ʧ��\r\n");;
  }

  /* ��RTC��������Register1��д������ */
  HAL_RTCEx_BKUPWrite(&Rtc_Handle,RTC_BKP_DRX,RTC_BKP_DATA);
}

/**
  * @brief  ��ʾʱ�������
  * @param  ��
  * @retval ��
  */
void RTC_TimeAndDate_Show(void)
{
	uint8_t Rtctmp=0;
	char LCDTemp[100];
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	while(1)
	{
		// ��ȡ����
		HAL_RTC_GetTime(&Rtc_Handle, &RTC_TimeStructure, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&Rtc_Handle, &RTC_DateStructure, RTC_FORMAT_BIN);
			
		// ÿ���ӡһ��
		if(Rtctmp != RTC_TimeStructure.Seconds)
		{
							
			// ��ӡ����
			printf("The Date :  Y:%0.2d - M:%0.2d - D:%0.2d - W:%0.2d\r\n", 
			RTC_DateStructure.Year,
			RTC_DateStructure.Month, 
			RTC_DateStructure.Date,
			RTC_DateStructure.WeekDay);

			//Һ����ʾ����
			//�Ȱ�Ҫ��ʾ��������sprintf����ת��Ϊ�ַ�����Ȼ�������Һ����ʾ������ʾ
			sprintf(LCDTemp,"The Date :  Y:20%0.2d - M:%0.2d - D:%0.2d - W:%0.2d", 
			RTC_DateStructure.Year,
			RTC_DateStructure.Month, 
			RTC_DateStructure.Date,
			RTC_DateStructure.WeekDay);
		#ifdef USE_LCD_DISPLAY
			ILI9341_DispStringLine_EN(LINE(2),LCDTemp); 
#endif
			
			// ��ӡʱ��
			printf("The Time :  %0.2d:%0.2d:%0.2d \r\n\r\n", 
			RTC_TimeStructure.Hours, 
			RTC_TimeStructure.Minutes, 
			RTC_TimeStructure.Seconds);
			
			//Һ����ʾʱ��
			sprintf(LCDTemp,"The Time :  %0.2d:%0.2d:%0.2d", 
			RTC_TimeStructure.Hours, 
			RTC_TimeStructure.Minutes, 
			RTC_TimeStructure.Seconds);
#ifdef USE_LCD_DISPLAY
			ILI9341_DispStringLine_EN(LINE(5),LCDTemp); 
#endif	
       if(Alarmflag)
      { 
         BEEP_ON;
         Alarmflag = 0;       
      }
		}
		Rtctmp = RTC_TimeStructure.Seconds;
	}	
}

/**
  * @brief  RTC���ã�ѡ��RTCʱ��Դ
  * @param  ��
  * @retval ��
  */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{  
	RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  /*##-1- ����PWRʱ�Ӳ����öԱ�����ķ���###################################*/
  /* Ҫ����RTC���ܣ�LSE��LSI����Դʱ�ӣ������룺
     - ʹ��__HAL_RCC_PWR_CLK_ENABLE�������õ�Դʱ��
     - ��ʹ��֮ǰʹ��HAL_PWR_EnableBkUpAccess������������д���� ����RTCʱ��Դ����λ��ִ��һ�Σ���
     - ʹ��__HAL_RCC_BACKUPRESET_FORCE���������ñ�����
     __HAL_RCC_BACKUPRESET_RELEASE������
     - ���������RTcʱ��Դ*/
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();
  
  /* Ϊ���ݼĴ�������BKP CLK */
  __HAL_RCC_BKP_CLK_ENABLE();
  
  /*##-2-��ISE����ΪRTCʱ��Դ ###################################*/ 
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE  ;  
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON; 
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  { 
    printf("\n\r RTC ����ʧ�� \r\n");;
  }
  
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
   printf("\n\r RTC ʱ�ӳ�ʼ��ʧ�� \r\n");
  }
  
  /*##-3- ����RTC����ʱ�� #######################################*/
  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();

}

/**
  * @brief   Ҫʹ�� RTC �����жϣ��谴������˳�������
             1. ���� NVIC �е� RTC_Alarm IRQ ͨ��������ʹ�ܡ�
             2. ���� RTC ������ RTC ���ӣ����� A ������ B����
  * @param  ��
  * @retval ��
  */
void RTC_AlarmSet(void)
{
    RTC_AlarmTypeDef  RTC_AlarmStructure;

    /* RTC �����ж����� */
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 1, 0);
    /* ʹ��RTC�����ж� */
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

    /* ��������ʱ�� */
    RTC_AlarmStructure.Alarm = RTC_Alarm_X;
  
    RTC_AlarmStructure.AlarmTime.Hours   = ALARM_HOURS;
    RTC_AlarmStructure.AlarmTime.Minutes = ALARM_MINUTES;
    RTC_AlarmStructure.AlarmTime.Seconds = ALARM_SECONDS;
  
    HAL_RTC_SetAlarm_IT(&Rtc_Handle,&RTC_AlarmStructure, RTC_FORMAT_BCD); 
}




/**********************************END OF FILE*************************************/
