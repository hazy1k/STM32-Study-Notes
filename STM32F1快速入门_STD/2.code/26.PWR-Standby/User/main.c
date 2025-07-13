#include "stm32f10x.h"
#include "systick.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include <stdio.h>

int main(void)
{    
    BSP_LED_Init();
    KEY_GPIO_Init();
    SysTick_Init();
    USARTx_Init(115200);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET)
    {
        LED_ON(BLUE_LED_Pin); // ����ģʽ����
        printf("����ģʽ���Ѹ�λ\r\n");
    }
    else
    {
        LED_ON(GREEN_LED_Pin);
        printf("��������\r\n");
    }
    while(1)
    {
        if(KEY2_LongPress())
        {
            printf("����KEY2�������ģʽ����KEY1����\r\n");
            LED_ON(RED_LED_Pin);
            Delay_ms(1000);
            PWR_ClearFlag(PWR_FLAG_WU);
            PWR_WakeUpPinCmd(ENABLE);
            PWR_EnterSTANDBYMode();
        }
    }   
}
