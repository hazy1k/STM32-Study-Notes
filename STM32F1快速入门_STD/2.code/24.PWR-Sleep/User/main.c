#include "stm32f10x.h"
#include "systick.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include <stdio.h>

volatile uint8_t key_wakeup_flag = 0;  // �������ѱ�־

int main(void)
{
    BSP_LED_Init();
    BSP_KEY_Init();
    SysTick_Init();
    USARTx_Init(115200);
    
    printf("System Started.\r\n");
    printf("Press KEY to enter sleep mode...\r\n");
    
    // ��ʼ״̬��������˯��
    uint8_t sleep_enabled = 0;
    
    while(1)
    {
        if(key_wakeup_flag)
        {
            key_wakeup_flag = 0;
            
            if(!sleep_enabled)
            {
                // ��һ�ΰ�����׼������˯��
                LED_ON(GREEN_LED_Pin);
                printf("Preparing to sleep. Press KEY again to wake up.\r\n");
                Delay_ms(1000);
                LED_OFF(GREEN_LED_Pin);
                
                sleep_enabled = 1;  // �������˯��
            }
            else
            {
                // ���Ѻ����
                LED_ON(GREEN_LED_Pin);
                printf("Waked up by KEY!\r\n");
                printf("Press KEY to enter sleep mode...\r\n");
                Delay_ms(1000);
                LED_OFF(GREEN_LED_Pin);
                
                sleep_enabled = 0;  // ��ֹ�ٴ���������˯��
            }
        }
        
        if(sleep_enabled)
        {
            // ����˯��ģʽ
            printf("Entering sleep mode...\r\n");
            
            // ȷ�����ڷ������
            while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
            
            // ������ѱ�־
            PWR->CR |= PWR_CR_CWUF;
            
            // ����˯��ģʽ
            __WFI();
            
            // ���Ѻ����ִ��
            printf("Woke up from sleep!\r\n");
            sleep_enabled = 0;
        }
    }
}

void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line0);
        key_wakeup_flag = 1;
    }
}
