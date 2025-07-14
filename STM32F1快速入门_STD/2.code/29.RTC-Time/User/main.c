#include "stm32f10x.h"
#include "rtc.h"
#include "systick.h"
#include "usart.h"
#include <stdio.h>

int main(void) {
    Custom_DateTypeDef date;
    Custom_TimeTypeDef time;
    // ϵͳʱ�����ã�ʹ���ⲿ8MHz����
    SystemInit();
		SysTick_Init();
    // ��ʼ������
    USARTx_Init(115200);
    RTC_Config();
    printf("STM32F103 RTC Calendar Demo\r\n");
    while(1) {
        // ��ȡ��ǰʱ���
        uint32_t timestamp = RTC_GetCounter();
        // ת��Ϊ����ʱ��
        ConvertTimestamp(timestamp, &date, &time);
        // ��ӡ����ʱ��
        printf("Date: %04d-%02d-%02d  Time: %02d:%02d:%02d\r\n",
               date.year, date.month, date.day,
               time.hour, time.minute, time.second);
       
			Delay_ms(1000);
    }
}
