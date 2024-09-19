
#ifndef __RTC_H
#define	__RTC_H

#include "stm32f1xx.h"

//�Ƿ�ʹ��LCD��ʾ
#define USE_LCD_DISPLAY

extern RTC_HandleTypeDef Rtc_Handle;
extern __IO uint8_t Alarmflag ;
// ʱ��Դ�궨��
#define RTC_CLOCK_SOURCE_LSE      
//#define RCC_OSCILLATORTYPE_LSI  
 
#define HOURS                     0x12          // 0~23
#define MINUTES                   0x12          // 0~59
#define SECONDS                   0x50          // 0~59


// ���ں궨��  ����WEEKDAYΪ��Ч�궨�壬ֻҪ���������գ�HAL����Զ������ WEEKDAY
#define WEEKDAY                   6           // 1~7
#define DATE                      0x24        // 1~31
#define MONTH                     0x12        // 1~12
#define YEAR                      0x18        // 0~99


// ������غ궨��
#define ALARM_HOURS             0x12         // 0~23
#define ALARM_MINUTES           0x13          // 0~59
#define ALARM_SECONDS           0x0       // 0~59

#define RTC_Alarm_X               RTC_ALARM_A



// ������Ĵ����궨��
#define RTC_BKP_DRX          RTC_BKP_DR1
// д�뵽���ݼĴ��������ݺ궨��
#define RTC_BKP_DATA         0X32F2 
                                  
void RTC_CalendarConfig(void);
void RTC_TimeAndDate_Show(void);
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc);
void RTC_AlarmSet(void);

#endif /* __XXX_H */
