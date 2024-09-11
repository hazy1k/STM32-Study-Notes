#ifndef __GPS_CONFIG_H
#define	__GPS_CONFIG_H

#include "stm32f10x.h"
#include "nmea/nmea.h"


/* ���� */

//#define __GPS_DEBUG     //��������꣬��nmea����ʱ�����������Ϣ

//#define __GPS_LOG_FILE    //��������꣬��SD���ϵ�gpslog.txt�ļ����н��룻������Ļ�ʹ�ô��ڽ���GPS��Ϣ����


/* GPS�ӿ����� ʹ�ò�ͬ�Ĵ���ʱ��Ҫ�޸Ķ�Ӧ�Ľӿ� */

#define GPS_DR_Base            (USART2_BASE+0x04)		  // ���ڵ����ݼĴ�����ַ


#define GPS_DATA_ADDR             GPS_DR_Base        //GPSʹ�õĴ��ڵ����ݼĴ�����ַ
#define GPS_RBUFF_SIZE            512                   //���ڽ��ջ�������С
#define HALF_GPS_RBUFF_SIZE       (GPS_RBUFF_SIZE/2)    //���ڽ��ջ�����һ��  

/* ���� */
/*******************************************************/
//GPS���ڲ�����
#define GPS_USART_BAUDRATE                    9600

#define GPS_USART                             USART2
#define GPS_USART_CLK                         RCC_APB1Periph_USART2
#define GPS_RCC_PeriphClockCmd_Fun          RCC_APB1PeriphClockCmd

#define GPS_USART_RX_GPIO_PORT                GPIOA
#define GPS_USART_RX_GPIO_CLK                 RCC_APB2Periph_GPIOA
#define GPS_USART_RX_PIN                      GPIO_Pin_3


#define GPS_USART_TX_GPIO_PORT                GPIOA
#define GPS_USART_TX_GPIO_CLK                 RCC_APB2Periph_GPIOA
#define GPS_USART_TX_PIN                      GPIO_Pin_2
/************************************************************/
#define GPS_DMA                      DMA1
#define GPS_DMA_CLK                  RCC_AHBPeriph_DMA1
#define GPS_DMA_CHANNEL              DMA1_Channel6
#define GPS_DMA_IRQn                 DMA1_Channel6_IRQn         //GPS�ж�Դ

/* �����־ */
#define GPS_DMA_FLAG_TC              DMA1_FLAG_TC6
#define GPS_DMA_FLAG_TE              DMA1_FLAG_TE6
#define GPS_DMA_FLAG_HT              DMA1_FLAG_HT6
#define GPS_DMA_FLAG_GL              DMA1_FLAG_GL6
#define GPS_DMA_IT_HT                DMA1_IT_HT6
#define GPS_DMA_IT_TC                DMA1_IT_TC6


/* �жϺ��� */
#define GPS_DMA_IRQHANDLER           DMA1_Channel6_IRQHandler   //GPSʹ�õ�DMA�жϷ�����


extern uint8_t gps_rbuff[GPS_RBUFF_SIZE];
extern __IO uint8_t GPS_TransferEnd ;
extern __IO uint8_t GPS_HalfTransferEnd;


void GPS_ProcessDMAIRQ(void);
void GPS_Config(void);

void trace(const char *str, int str_size);
void error(const char *str, int str_size);
void gps_info(const char *str, int str_size);
void GMTconvert(nmeaTIME *SourceTime, nmeaTIME *ConvertTime, uint8_t GMT,uint8_t AREA) ;




#endif 
