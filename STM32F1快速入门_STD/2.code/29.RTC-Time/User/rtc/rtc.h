#ifndef __RTC_H__
#define __RTC_H__

#include "stm32f10x.h"

// �Զ�������ʱ��ṹ�壨STD����û��RTC_DateTypeDef/RTC_TimeTypeDef��
typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
} Custom_DateTypeDef;

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
}Custom_TimeTypeDef;

void RTC_Config(void);
void ConvertTimestamp(uint32_t timestamp, Custom_DateTypeDef* date, Custom_TimeTypeDef* time);

#endif
