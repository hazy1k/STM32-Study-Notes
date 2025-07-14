#include "rtc.h"

// RTC��ʼ��
void RTC_Config(void) {
    // ����PWR��BKPʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    // �������BKP����
    PWR_BackupAccessCmd(ENABLE);
    // ��λ��������
    BKP_DeInit();
    // ����LSEʱ�ӣ�32.768KHz��
    RCC_LSEConfig(RCC_LSE_ON);
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
    // ѡ��LSE��ΪRTCʱ��Դ
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    RCC_RTCCLKCmd(ENABLE);
    
    // �ȴ�RTC�Ĵ���ͬ��
    RTC_WaitForSynchro();
    
    // �ȴ���һ�β������
    RTC_WaitForLastTask();
    
    // ����RTC��Ƶ����32768Hz/(32767+1) = 1Hz
    RTC_SetPrescaler(32767);
    RTC_WaitForLastTask();
    
    // ����Ƿ��״�����
    if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5) {
        RTC_SetCounter(0);  // ��1970-01-01 00:00:00��ʼ����
        RTC_WaitForLastTask();
        
        // ���ñ��ݼĴ�����־
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
    }
}


// ��Unixʱ���ת��Ϊ����ʱ��
void ConvertTimestamp(uint32_t timestamp, Custom_DateTypeDef* date, Custom_TimeTypeDef* time) {
    // �򻯵�ת���߼���ʵ����Ŀ��ʹ�������㷨��
    time->second = timestamp % 60;
    timestamp /= 60;
    time->minute = timestamp % 60;
    timestamp /= 60;
    time->hour = timestamp % 24;
    timestamp /= 24;
    
    // �����ڼ��㣨���������ϸ�ڣ�
    date->year = 1970 + timestamp / 365;
    timestamp %= 365;
    date->month = 1 + timestamp / 30;
    date->day = 1 + timestamp % 30;
}
