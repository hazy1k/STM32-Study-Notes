/**
  ******************************************************************************
  * @file    bsp_rtc.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   stm32 RTC ����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "./usart/bsp_usart.h"
#include "./rtc/bsp_rtc.h"

#include "./lcd/bsp_ili9341_lcd.h"


/* ���жϱ�־���������ж�ʱ��1����ʱ�䱻ˢ��֮����0 */
__IO uint32_t TimeDisplay = 0;

/*���ڣ���Ф������ASCII��*/
char const *WEEK_STR[] = {"��", "һ", "��", "��", "��", "��", "��"};
char const *zodiac_sign[] = {"��", "��", "ţ", "��", "��", "��", "��", "��", "��", "��", "��", "��"};

/*Ӣ�ģ����ڣ���Ф������ASCII��*/
char const *en_WEEK_STR[] = { "Sunday","Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char const *en_zodiac_sign[] = {"Pig", "Rat", "Ox", "Tiger", "Rabbit", "Dragon", "Snake", "Horse", "Goat", "Monkey", "Rooster", "Dog"};


/*
 * ��������NVIC_Configuration
 * ����  ������RTC���жϵ����ж����ȼ�Ϊ1�������ȼ�Ϊ0
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void RTC_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


/*
 * ��������RTC_CheckAndConfig
 * ����  ����鲢����RTC
 * ����  �����ڶ�ȡRTCʱ��Ľṹ��ָ��
 * ���  ����
 * ����  ���ⲿ����
 */
void RTC_CheckAndConfig(struct rtc_time *tm)
{
   	/*������ʱ��鱸�ݼĴ���BKP_DR1��������ݲ���0xA5A5,
	  ������������ʱ�䲢ѯ���û�����ʱ��*/
	if (BKP_ReadBackupRegister(RTC_BKP_DRX) != RTC_BKP_DATA)
	{
		printf("\r\n\r\n RTC not yet configured....");
		printf("\r\n\r\n RTC configured....");

		/* ʹ��tm��ʱ������RTC�Ĵ��� */
		Time_Adjust(tm);
		
		/*��BKP_DR1�Ĵ���д���־��˵��RTC��������*/
		BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
	}
	else
	{
		
		/* ʹ�� PWR �� Backup ʱ�� */
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
		/* ������� Backup ���� */
	  PWR_BackupAccessCmd(ENABLE);
		
	  /*LSE��������������ʱ��*/
		
#ifdef RTC_CLOCK_SOURCE_LSI		
			/* ʹ�� LSI */
			RCC_LSICmd(ENABLE);

			/* �ȴ� LSI ׼���� */
			while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
			{}
#endif

		/*����Ƿ��������*/
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
		    printf("\r\n\r\n Power On Reset occurred....");
		}
		/*����Ƿ�Reset��λ*/
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
			printf("\r\n\r\n External Reset occurred....");
		}
	
		printf("\r\n No need to configure RTC....");
		
		/*�ȴ��Ĵ���ͬ��*/
		RTC_WaitForSynchro();
		
		/*����RTC���ж�*/
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		
		/*�ȴ��ϴ�RTC�Ĵ���д�������*/
		RTC_WaitForLastTask();
	}
	   /*������ʱ������꣬������У��ʱ�������PC13*/
	#ifdef RTCClockOutput_Enable
	/* ʹ�� PWR �� Backup ʱ�� */
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	/* ������� Backup ���� */
	  PWR_BackupAccessCmd(ENABLE);
	
	  /* ��ֹ Tamper ���� */
	  /* Ҫ��� RTCCLK/64 �� Tamper ����,  tamper ���ܱ����ֹ */	
	  BKP_TamperPinCmd(DISABLE); 
	
	  /* ʹ�� RTC ʱ������� Tamper ���� */
	  BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
	#endif
	
	  /* �����λ��־ flags */
	  RCC_ClearFlag();

}



/*
 * ��������RTC_Configuration
 * ����  ������RTC
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void RTC_Configuration(void)
{
	/* ʹ�� PWR �� Backup ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	/* ������� Backup ���� */
	PWR_BackupAccessCmd(ENABLE);
	
	/* ��λ Backup ���� */
	BKP_DeInit();
	
//ʹ���ⲿʱ�ӻ����ڲ�ʱ�ӣ���bsp_rtc.h�ļ����壩	
//ʹ���ⲿʱ��ʱ������Щ����¾�������
//������Ʒ��ʱ�򣬺����׳����ⲿ����������������̫�ɿ�	
#ifdef 	RTC_CLOCK_SOURCE_LSE
	/* ʹ�� LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	
	/* �ȴ� LSE ׼���� */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{}
	
	/* ѡ�� LSE ��Ϊ RTC ʱ��Դ */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	/* ʹ�� RTC ʱ�� */
	RCC_RTCCLKCmd(ENABLE);
	
	/* �ȴ� RTC �Ĵ��� ͬ��
	 * ��ΪRTCʱ���ǵ��ٵģ��ڻ�ʱ���Ǹ��ٵģ�����Ҫͬ��
	 */
	RTC_WaitForSynchro();
	
	/* ȷ����һ�� RTC �Ĳ������ */
	RTC_WaitForLastTask();
	
	/* ʹ�� RTC ���ж� */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	
	/* ȷ����һ�� RTC �Ĳ������ */
	RTC_WaitForLastTask();
	
	/* ���� RTC ��Ƶ: ʹ RTC ����Ϊ1s  */
	/* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) = 1HZ */
	RTC_SetPrescaler(32767); 
	
	/* ȷ����һ�� RTC �Ĳ������ */
	RTC_WaitForLastTask();
	
#else

	/* ʹ�� LSI */
	RCC_LSICmd(ENABLE);

	/* �ȴ� LSI ׼���� */
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{}
	
	/* ѡ�� LSI ��Ϊ RTC ʱ��Դ */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	
	/* ʹ�� RTC ʱ�� */
	RCC_RTCCLKCmd(ENABLE);
	
	/* �ȴ� RTC �Ĵ��� ͬ��
	 * ��ΪRTCʱ���ǵ��ٵģ��ڻ�ʱ���Ǹ��ٵģ�����Ҫͬ��
	 */
	RTC_WaitForSynchro();
	
	/* ȷ����һ�� RTC �Ĳ������ */
	RTC_WaitForLastTask();
	
	/* ʹ�� RTC ���ж� */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	
	/* ȷ����һ�� RTC �Ĳ������ */
	RTC_WaitForLastTask();
	
	/* ���� RTC ��Ƶ: ʹ RTC ����Ϊ1s ,LSIԼΪ40KHz */
	/* RTC period = RTCCLK/RTC_PR = (40 KHz)/(40000-1+1) = 1HZ */	
	RTC_SetPrescaler(40000-1); 
	
	/* ȷ����һ�� RTC �Ĳ������ */
	RTC_WaitForLastTask();
#endif
	
}



/*
 * ��������Time_Regulate_Get
 * ����  �������û�ʹ�ô������õ�ʱ�䣬
 *         �Ա����ת����ʱ����洢��RTC �����Ĵ����С�
 * ����  �����ڶ�ȡRTCʱ��Ľṹ��ָ��
 * ע��  ���ڴ��ڵ�����������ʱ������������Ҫ�ӻس�
 */
void Time_Regulate_Get(struct rtc_time *tm)
{
	  uint32_t temp_num = 0;
		uint8_t day_max=0 ;

	  printf("\r\n=========================����ʱ��==================");
		
	  do 
	  {
			printf("\r\n  ���������(Please Set Years),��Χ[1970~2038]�������ַ�����ӻس�:");
			scanf("%d",&temp_num);
			if(temp_num <1970 || temp_num >2038)
			{
				printf("\r\n �������������:%d��������Ҫ��",temp_num);
					  
			}
			else
			{	  
				printf("\n\r  ��ݱ�����Ϊ: %d\n\r", temp_num);

				tm->tm_year = temp_num;
				break;
			}
	  }while(1);


	 do 
	  {
			printf("\r\n  �������·�(Please Set Months):��Χ[1~12]�������ַ�����ӻس�:");
			scanf("%d",&temp_num);
			if(temp_num <1 || temp_num >12)
			{
				printf("\r\n �������������:%d��������Ҫ��",temp_num);
					  
			}
			else
			{	  
				printf("\n\r  �·ݱ�����Ϊ: %d\n\r", temp_num);

				tm->tm_mon = temp_num;
				break;
			}
	  }while(1);
		
		/*�����·ݼ����������*/
		switch(tm->tm_mon)
			{
				case 1:
				case 3:
				case 5:
				case 7:
				case 8:
				case 10:
				case 12:					
						day_max = 31;
					break;
				
				case 4:
				case 6:
				case 9:
				case 11:
						day_max = 30;
					break;
				
				case 2:					
				     /*��������*/
						if((tm->tm_year%4==0) &&
							 ((tm->tm_year%100!=0) || (tm->tm_year%400==0)) &&
							 (tm->tm_mon>2)) 
								{
									day_max = 29;
								} else 
								{
									day_max = 28;
								}
					break;			
			}

		do 
	  {				
			printf("\r\n  ����������(Please Set Months),��Χ[1~%d]�������ַ�����ӻس�:",day_max);
			scanf("%d",&temp_num);
			
			if(temp_num <1 || temp_num >day_max)
			{
				printf("\r\n �������������:%d��������Ҫ��",temp_num);
			}
			else
			{
				printf("\n\r  ���ڱ�����Ϊ: %d\n\r", temp_num);

				tm->tm_mday = temp_num;
				break;
			}
	  }while(1);
		
		do 
	  {				
			printf("\r\n  ������ʱ��(Please Set Hours),��Χ[0~23]�������ַ�����ӻس�:");
			scanf("%d",&temp_num);
			
			if( temp_num >23)
			{
				printf("\r\n �������������:%d��������Ҫ��",temp_num);
			}
			else
			{
				printf("\n\r  ʱ�ӱ�����Ϊ: %d\n\r", temp_num);

				tm->tm_hour = temp_num;
				break;
			}
	  }while(1);

		do 
	  {				
			printf("\r\n  ���������(Please Set Minutes),��Χ[0~59]�������ַ�����ӻس�:");
			scanf("%d",&temp_num);
			
			if( temp_num >59)
			{
				printf("\r\n �������������:%d��������Ҫ��",temp_num);
			}
			else
			{
				printf("\n\r  ���ӱ�����Ϊ: %d\n\r", temp_num);

				tm->tm_min = temp_num;
				break;
			}
	  }while(1);

		do 
	  {				
			printf("\r\n  ����������(Please Set Seconds),��Χ[0~59]�������ַ�����ӻس�:");
			scanf("%d",&temp_num);
			
			if( temp_num >59)
			{
				printf("\r\n �������������:%d��������Ҫ��",temp_num);
			}
			else
			{
				printf("\n\r  ���ӱ�����Ϊ: %d\n\r", temp_num);

				tm->tm_sec = temp_num;
				break;
			}
	  }while(1);

}

/*
 * ��������Time_Show
 * ����  ����ʾ��ǰʱ��ֵ
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */ 
void Time_Show(struct rtc_time *tm)
{	 
	  /* Infinite loop */
	  while (1)
	  {
	    /* ÿ��1s */
	    if (TimeDisplay == 1)
	    {
				/* Display current time */
	      Time_Display( RTC_GetCounter(),tm); 		  
	      TimeDisplay = 0;
	    }
	  }
}


/*
 * ��������Time_Adjust
 * ����  ��ʱ�����
 * ����  �����ڶ�ȡRTCʱ��Ľṹ��ָ�루����ʱ�䣩
 * ���  ����
 * ����  ���ⲿ����
 */
void Time_Adjust(struct rtc_time *tm)
{
	
			/* RTC ���� */
		RTC_Configuration();

	  /* �ȴ�ȷ����һ�β������ */
	  RTC_WaitForLastTask();
		  
	  /* �������� */
	  GregorianDay(tm);

	  /* �����ڼ���ʱ�����д�뵽RTC�����Ĵ��� */
	  RTC_SetCounter(mktimev(tm)-TIME_ZOOM);

	  /* �ȴ�ȷ����һ�β������ */
	  RTC_WaitForLastTask();
}

/*
 * ��������Time_Display
 * ����  ����ʾ��ǰʱ��ֵ
 * ����  ��-TimeVar RTC����ֵ����λΪ s
 * ���  ����
 * ����  ���ڲ�����
 */	
void Time_Display(uint32_t TimeVar,struct rtc_time *tm)
{
	   static uint32_t FirstDisplay = 1;
	   uint32_t BJ_TimeVar;
	   uint8_t str[200]; // �ַ����ݴ�  	

	   /*  �ѱ�׼ʱ��ת��Ϊ����ʱ��*/
	   BJ_TimeVar =TimeVar + TIME_ZOOM;

	   to_tm(BJ_TimeVar, tm);/*�Ѷ�ʱ����ֵת��Ϊ����ʱ��*/	
	
	  if((!tm->tm_hour && !tm->tm_min && !tm->tm_sec)  || (FirstDisplay))
	  {
	      
	      GetChinaCalendar((u16)tm->tm_year, (u8)tm->tm_mon, (u8)tm->tm_mday, str);	
					printf("\r\n ����������%0.2d%0.2d,%0.2d,%0.2d", str[0], str[1], str[2],  str[3]);
	
	      GetChinaCalendarStr((u16)tm->tm_year,(u8)tm->tm_mon,(u8)tm->tm_mday,str);
					printf("\r\n ����ũ����%s\r\n", str);
	
	     if(GetJieQiStr((u16)tm->tm_year, (u8)tm->tm_mon, (u8)tm->tm_mday, str))
					printf("\r\n ����ũ����%s\r\n", str);
	
	      FirstDisplay = 0;
	  }	 	  	

	  /* ���ʱ���������ʱ�� */
	  printf(" UNIXʱ��� = %d ��ǰʱ��Ϊ: %d��(%s��) %d�� %d�� (����%s)  %0.2d:%0.2d:%0.2d\r",TimeVar,
	                    tm->tm_year, zodiac_sign[(tm->tm_year-3)%12], tm->tm_mon, tm->tm_mday, 
	                    WEEK_STR[tm->tm_wday], tm->tm_hour, 
	                    tm->tm_min, tm->tm_sec);
		
#ifdef  USE_LCD_DISPLAY

		//ʱ���
		sprintf((char *)str," UNIX TimeStamp = %d ",TimeVar);

		ILI9341_DispStringLine_EN(LINE(3),(char*)str);
		
		//����
		sprintf((char *)str," Date: %d-%d-%d       ",
																						tm->tm_year, 
																						tm->tm_mon,
																						tm->tm_mday);
		ILI9341_DispStringLine_EN(LINE(5),(char*)str);
		
		//��Ф
		sprintf((char *)str," Chinese %s year      ",en_zodiac_sign[(tm->tm_year-3)%12]);
		
		ILI9341_DispStringLine_EN(LINE(6),(char*)str);
		
		//����
		sprintf((char *)str," %s                  ",en_WEEK_STR[tm->tm_wday]);
		
		ILI9341_DispStringLine_EN(LINE(7),(char*)str);
		
		//ʱ��
		sprintf((char *)str," Time:  %0.2d:%0.2d:%0.2d",
																						tm->tm_hour, 									
																						tm->tm_min, 
																						tm->tm_sec);
		
		ILI9341_DispStringLine_EN(LINE(8),(char*)str);
#endif
		
}




/************************END OF FILE***************************************/
