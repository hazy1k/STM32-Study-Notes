#include "stm32f10x.h"
#include "tim.h"
#include "systick.h"
#include "usart.h"
#include <stdio.h>

int main(void)
{   
    SysTick_Init();
    USARTx_Init(115200);
    printf("TIM2����PWM�ź� -> TIM1����PWM����\r\n");
    printf("PA0 (PWM���) ���ӵ� PA8 (PWM����)\r\n");
    TIM2_PWM_Init();
    TIM1_PWMCapture_Init();
    /* PWM�������� */
    uint32_t periods[] = {1000, 2000, 5000};  // ��ͬ����ֵ
    uint32_t duties[] = {200, 500, 800};      // ��ͬռ�ձ�ֵ
    uint8_t p_index = 0, d_index = 0;
    printf("��ʼPWM�������...\r\n");
    while(1)
    {
        TIM2_SetPWM(periods[p_index], duties[d_index]);
        // ��ӡ���õ�PWM����
        printf("���õ�PWM����: ����=%lu, ռ�ձ�=%lu (%.1f%%)\r\n", 
               periods[p_index], duties[d_index],
               (duties[d_index] * 100.0f) / periods[p_index]);
        Delay_ms(1000);
        // ��ȡ�����PWM����
        PWM_CaptureData cap_data = TIM1_GetPWMCaptureData();
        // ��ӡ������
        printf("�����PWM����:\r\n");
        printf("  ����: %lu ����\r\n", cap_data.period);
        printf("  ռ�ձ�: %lu ����\r\n", cap_data.duty_cycle);
        printf("  Ƶ��: %.2f Hz\r\n", cap_data.frequency);
        printf("  ռ�ձ�: %.2f%%\r\n", cap_data.duty_ratio);
        printf("------------------------------------\r\n");
        // ����PWM��������
        d_index = (d_index + 1) % 3;
        if (d_index == 0) {
            p_index = (p_index + 1) % 3;
        }
        Delay_ms(2000);
    }
}
