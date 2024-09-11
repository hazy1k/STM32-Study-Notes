/**
  ******************************************************************************
  * @file    nmea_decode_test.c
  * @author  fire
  * @version V1.0
  * @date    2016-07-xx
  * @brief   ����NEMA�����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��F103������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./gps/gps_config.h"
#include "ff.h"
#include "nmea/nmea.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./bmp/bsp_bmp.h"
#include "./key/bsp_key.h"  
#include "./led/bsp_led.h"


 
#ifdef __GPS_LOG_FILE             //��SD���ϵ�gpslog.txt�ļ����н��룻����Ҫ��sd���ϴ��gpslog.txt�ļ���

FATFS fs;
FIL log_file;
FRESULT res; 
UINT br, bw;            					/* File R/W count */

/**
  * @brief  nmea_decode_test ����GPS�ļ���Ϣ
  * @param  ��
  * @retval ��
  */
void nmea_decode_test(void)
{
		double deg_lat;//ת����[degree].[degree]��ʽ��γ��
		double deg_lon;//ת����[degree].[degree]��ʽ�ľ���

    nmeaINFO info;          //GPS�����õ�����Ϣ
    nmeaPARSER parser;      //����ʱʹ�õ����ݽṹ  
    
    nmeaTIME beiJingTime;    //����ʱ�� 

    char buff[2048];  
  
    char str_buff[100];
  
  	LCD_SetFont(&Font8x16);
		LCD_SetColors(RED,BLACK);
		
		ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */

    /*����������ɫ������ı�����ɫ*/
    LCD_SetColors(RED,BLACK);
  
  	ILI9341_DispStringLine_EN(LINE(1)," Wildfire STM32-F103");
    ILI9341_DispStringLine_EN(LINE(2),"  GPS module");
    
  
    	/* ע���̷� */
    res = f_mount(&fs,"0:",1);	
  
    if(res != FR_OK)
    {
      printf("\r\n����SD�������ļ�ϵͳʧ�ܡ�(%d)��������������SD��\r\n",res);
      while(1);
    }

    /* �򿪼�¼��GPS��Ϣ���ļ� */
    res = f_open(&log_file,"0:gpslog.txt", FA_OPEN_EXISTING|FA_READ);

    if(!(res == FR_OK))
    {
        printf("\r\n��gpslog.txt�ļ�ʧ�ܣ�����SD���ĸ�Ŀ¼�Ƿ�����gpslog.txt�ļ�!\r\n");
        return ;      
    }
    

    /* �����������������Ϣ�ĺ��� */
    nmea_property()->trace_func = &trace;
    nmea_property()->error_func = &error;
    nmea_property()->info_func = &gps_info;

    /* ��ʼ��GPS���ݽṹ */
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);

    while(!f_eof(&log_file))
    {
      
        f_read(&log_file, &buff[0], 100, &br);

        /* ����nmea��ʽ���� */
        nmea_parse(&parser, &buff[0], br, &info);
      
        /* �Խ�����ʱ�����ת����ת���ɱ���ʱ�� */
        GMTconvert(&info.utc,&beiJingTime,8,1);
        
        /* �������õ�����Ϣ */
				printf("\r\nʱ��%d-%02d-%02d,%d:%d:%d\r\n", beiJingTime.year+1900, beiJingTime.mon,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
				//info.lat lon�еĸ�ʽΪ[degree][min].[sec/60]��ʹ�����º���ת����[degree].[degree]��ʽ
				deg_lat = nmea_ndeg2degree(info.lat);
				deg_lon = nmea_ndeg2degree(info.lon);
			
				printf("\r\nγ�ȣ�%f,����%f\r\n",deg_lat,deg_lon);
        printf("\r\n���θ߶ȣ�%f �� ", info.elv);
        printf("\r\n�ٶȣ�%f km/h ", info.speed);
        printf("\r\n����%f ��", info.direction);
				
				printf("\r\n����ʹ�õ�GPS���ǣ�%d,�ɼ�GPS���ǣ�%d",info.satinfo.inuse,info.satinfo.inview);

				printf("\r\n����ʹ�õı������ǣ�%d,�ɼ��������ǣ�%d",info.BDsatinfo.inuse,info.BDsatinfo.inview);
				printf("\r\nPDOP��%f,HDOP��%f��VDOP��%f",info.PDOP,info.HDOP,info.VDOP);
           
         /* Һ����� */
        
        /* ����ǰ����ɫ��������ɫ��*/
        LCD_SetTextColor(BLUE);
        
        ILI9341_DispStringLine_EN(LINE(5)," GPS Info:");

        /* ����ǰ����ɫ��������ɫ��*/
        LCD_SetTextColor(WHITE);
        
                /* Һ����� */
        
        /* ����ǰ����ɫ��������ɫ��*/
        LCD_SetTextColor(BLUE);
        
        ILI9341_DispStringLine_EN(LINE(5)," GPS Info:");

        /* ����ǰ����ɫ��������ɫ��*/
        LCD_SetTextColor(WHITE);
        
        /* ��ʾʱ������ */
        sprintf(str_buff," Date:%4d/%02d/%02d ", beiJingTime.year+1900, beiJingTime.mon,beiJingTime.day);
        ILI9341_DispStringLine_EN(LINE(6),str_buff);
      
			  sprintf(str_buff," Time:%02d:%02d:%02d", beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
        ILI9341_DispStringLine_EN(LINE(7),str_buff);

        /* γ�� ����*/
        sprintf(str_buff," latitude :%.6f ", deg_lat);
        ILI9341_DispStringLine_EN(LINE(8),str_buff);
        
        sprintf(str_buff," longitude :%.6f",deg_lon);
        ILI9341_DispStringLine_EN(LINE(9),str_buff);
        
        /* ����ʹ�õ����� �ɼ�������*/
        sprintf(str_buff," GPS  Satellite in use :%2d ", info.satinfo.inuse);
        ILI9341_DispStringLine_EN(LINE(10),str_buff);    
        
       sprintf(str_buff," GPS Satellite in view :%2d", info.satinfo.inview);
        ILI9341_DispStringLine_EN(LINE(11),str_buff);    

        /* ����ʹ�õ����� �ɼ�������*/
        sprintf(str_buff," BDS  Satellite in use :%2d ", info.BDsatinfo.inuse);
        ILI9341_DispStringLine_EN(LINE(12),str_buff);    
        
       sprintf(str_buff," BDS Satellite in view :%2d", info.BDsatinfo.inview);
        ILI9341_DispStringLine_EN(LINE(13),str_buff);    
        
        /* ���θ߶� */
        sprintf(str_buff," Altitude:%4.2f m", info.elv);
        ILI9341_DispStringLine_EN(LINE(14),str_buff);
        
        /* �ٶ� */
        sprintf(str_buff," speed:%4.2f km/h", info.speed);
        ILI9341_DispStringLine_EN(LINE(15),str_buff);
        
        /* ���� */
        sprintf(str_buff," Track angle:%3.2f deg", info.direction);
        ILI9341_DispStringLine_EN(LINE(16),str_buff);
        
	
	}

    f_lseek(&log_file, f_size(&log_file));

    /* �ͷ�GPS���ݽṹ */
    nmea_parser_destroy(&parser);
  
    /* �ر��ļ� */
    f_close(&log_file);
    
   
}
#else       //��GPSģ�鴫�ص���Ϣ���н���

FATFS fs;
FIL log_file;
FRESULT res; 
UINT br, bw;            					/* File R/W count */

/**
  * @brief  nmea_decode_test ����GPSģ����Ϣ
  * @param  ��
  * @retval ��
  */
int nmea_decode_test(void)
{
		double deg_lat;//ת����[degree].[degree]��ʽ��γ��
		double deg_lon;//ת����[degree].[degree]��ʽ�ľ���

    nmeaINFO info;          //GPS�����õ�����Ϣ
    nmeaPARSER parser;      //����ʱʹ�õ����ݽṹ  
    uint8_t new_parse=0;    //�Ƿ����µĽ������ݱ�־
  
    nmeaTIME beiJingTime;    //����ʱ�� 
  
    char str_buff[100];
      	/* ע���̷� */
    res = f_mount(&fs,"0:",1);	
  
    if(res != FR_OK)
    {
      printf("\r\n����SD�������ļ�ϵͳʧ�ܡ�(%d)��������������SD��\r\n",res);
      while(1);
    }
    
		LCD_SetFont(&Font8x16);
		LCD_SetColors(RED,BLACK);
		
		ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */

    /*����������ɫ������ı�����ɫ*/
    LCD_SetColors(RED,BLACK);
  
  	ILI9341_DispStringLine_EN(LINE(1)," Wildfire STM32-F103");
    ILI9341_DispStringLine_EN(LINE(2),"  GPS module");

  

    /* �����������������Ϣ�ĺ��� */
    nmea_property()->trace_func = &trace;
    nmea_property()->error_func = &error;
    nmea_property()->info_func = &gps_info;

    /* ��ʼ��GPS���ݽṹ */
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);

    while(1)
    {
			
							/*��ⰴ��*/
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{		
			static uint8_t name_count = 0;
			char name[40];
			
			//�������ý�ͼ���֣���ֹ�ظ���ʵ��Ӧ���п���ʹ��ϵͳʱ����������
			name_count++; 
			sprintf(name,"0:screen_shot_%d.bmp",name_count);

			LED_BLUE;
			printf("\r\n���ڽ�ͼ...");
			
			/*��ͼ�������ú�Һ����ʾ����ͽ�ͼ����*/
			ILI9341_GramScan ( 6 );			
			
			if(Screen_Shot(0,0,LCD_X_LENGTH,LCD_Y_LENGTH,name) == 0)
			{
				printf("\r\n��ͼ�ɹ���");
				LED_GREEN;
			}
			else
			{
				printf("\r\n��ͼʧ�ܣ�");
				LED_RED;
			}
		}
			
      if(GPS_HalfTransferEnd)     /* ���յ�GPS_RBUFF_SIZEһ������� */
      {
        /* ����nmea��ʽ���� */
        nmea_parse(&parser, (const char*)&gps_rbuff[0], HALF_GPS_RBUFF_SIZE, &info);
        
        GPS_HalfTransferEnd = 0;   //��ձ�־λ
        new_parse = 1;             //���ý�����Ϣ��־ 
      }
      else if(GPS_TransferEnd)    /* ���յ���һ������ */
      {

        nmea_parse(&parser, (const char*)&gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &info);
       
        GPS_TransferEnd = 0;
        new_parse =1;
      }
      
      if(new_parse )                //���µĽ�����Ϣ   
      {    
        /* �Խ�����ʱ�����ת����ת���ɱ���ʱ�� */
        GMTconvert(&info.utc,&beiJingTime,8,1);
        
        /* �������õ�����Ϣ */
				printf("\r\nʱ��%d-%02d-%02d,%d:%d:%d\r\n", beiJingTime.year+1900, beiJingTime.mon,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
			
				//info.lat lon�еĸ�ʽΪ[degree][min].[sec/60]��ʹ�����º���ת����[degree].[degree]��ʽ
				deg_lat = nmea_ndeg2degree(info.lat);
				deg_lon = nmea_ndeg2degree(info.lon);
			
				printf("\r\nγ�ȣ�%f,����%f\r\n",deg_lat,deg_lon);
        printf("\r\n���θ߶ȣ�%f �� ", info.elv);
        printf("\r\n�ٶȣ�%f km/h ", info.speed);
        printf("\r\n����%f ��", info.direction);
				
				printf("\r\n����ʹ�õ�GPS���ǣ�%d,�ɼ�GPS���ǣ�%d",info.satinfo.inuse,info.satinfo.inview);

				printf("\r\n����ʹ�õı������ǣ�%d,�ɼ��������ǣ�%d",info.BDsatinfo.inuse,info.BDsatinfo.inview);
				printf("\r\nPDOP��%f,HDOP��%f��VDOP��%f",info.PDOP,info.HDOP,info.VDOP);
        
        
        /* Һ����� */
        
        /* ����ǰ����ɫ��������ɫ��*/
        LCD_SetTextColor(BLUE);
        
        ILI9341_DispStringLine_EN(LINE(5)," GPS Info:");

        /* ����ǰ����ɫ��������ɫ��*/
        LCD_SetTextColor(WHITE);
        
        /* ��ʾʱ������ */
        sprintf(str_buff," Date:%4d/%02d/%02d ", beiJingTime.year+1900, beiJingTime.mon,beiJingTime.day);
        ILI9341_DispStringLine_EN(LINE(6),str_buff);
      
			  sprintf(str_buff," Time:%02d:%02d:%02d", beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
        ILI9341_DispStringLine_EN(LINE(7),str_buff);

        /* γ�� ����*/
        sprintf(str_buff," latitude :%.6f ", deg_lat);
        ILI9341_DispStringLine_EN(LINE(8),str_buff);
        
        sprintf(str_buff," longitude :%.6f",deg_lon);
        ILI9341_DispStringLine_EN(LINE(9),str_buff);
        
        /* ����ʹ�õ����� �ɼ�������*/
        sprintf(str_buff," GPS  Satellite in use :%2d ", info.satinfo.inuse);
        ILI9341_DispStringLine_EN(LINE(10),str_buff);    
        
       sprintf(str_buff," GPS Satellite in view :%2d", info.satinfo.inview);
        ILI9341_DispStringLine_EN(LINE(11),str_buff);    

        /* ����ʹ�õ����� �ɼ�������*/
        sprintf(str_buff," BDS  Satellite in use :%2d ", info.BDsatinfo.inuse);
        ILI9341_DispStringLine_EN(LINE(12),str_buff);    
        
       sprintf(str_buff," BDS Satellite in view :%2d", info.BDsatinfo.inview);
        ILI9341_DispStringLine_EN(LINE(13),str_buff);    
        
        /* ���θ߶� */
        sprintf(str_buff," Altitude:%4.2f m", info.elv);
        ILI9341_DispStringLine_EN(LINE(14),str_buff);
        
        /* �ٶ� */
        sprintf(str_buff," speed:%4.2f km/h", info.speed);
        ILI9341_DispStringLine_EN(LINE(15),str_buff);
        
        /* ���� */
        sprintf(str_buff," Track angle:%3.2f deg", info.direction);
        ILI9341_DispStringLine_EN(LINE(16),str_buff);
        
        
        new_parse = 0;
      }
	
	}

    /* �ͷ�GPS���ݽṹ */
    // nmea_parser_destroy(&parser);

    
    //  return 0;
}

#endif






/**************************************************end of file****************************************/

