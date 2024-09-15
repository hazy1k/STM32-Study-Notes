/**
  ******************************************************************************
  * @file    nmea_decode_test.c
  * @author  fire
  * @version V1.0
  * @date    2019-07-xx
  * @brief   ����NEMA�����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� H743 Pro������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "stm32f7xx.h"
#include "./usart/bsp_usart.h"
#include "./nmea/nmea.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_usart_dma.h"


/**
  * @brief  nmea_decode_test ����GPSģ����Ϣ
  * @param  ��
  * @retval ��
  */
int nmea_decode_test(void)
{
		double deg_lat;//ת����[degree].[degree]��ʽ��γ��
		double deg_lon;//ת����[degree].[degree]��ʽ�ľ���
	  int count = 0;
    nmeaINFO info;          //GPS�����õ�����Ϣ
    nmeaPARSER parser;      //����ʱʹ�õ����ݽṹ  
    uint8_t new_parse=0;    //�Ƿ����µĽ������ݱ�־
  
    nmeaTIME beiJingTime;    //����ʱ�� 

    /* �����������������Ϣ�ĺ��� */
    nmea_property()->trace_func = &trace;
    nmea_property()->error_func = &error;
    nmea_property()->info_func = &gps_info;
    /* ��ʼ��GPS���ݽṹ */
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);
    while(1)
    {
			
      if(GPS_HalfTransferEnd)     /* ���յ�GPS_RBUFF_SIZEһ������� */
      {
			
        /* ����nmea��ʽ���� */
        nmea_parse(&parser, (const char*)&gps_rbuff[0],HALF_GPS_RBUFF_SIZE, &info);
       
        GPS_HalfTransferEnd = 0;   //��ձ�־λ
         		count ++;
				printf("count2 = %d\n",count);
        new_parse = 1;             //���ý�����Ϣ��־ 
      }
      else if(GPS_TransferEnd)    /* ���յ���һ������ */
      {
				
        nmea_parse(&parser, (const char*)&gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &info);
				
        GPS_TransferEnd = 0;
        		count ++;
				printf("count = %d\n",count);
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
        
        new_parse = 0;
      }
	
	  } 

    /* �ͷ�GPS���ݽṹ */
    // nmea_parser_destroy(&parser);
}



/**************************************************end of file****************************************/

