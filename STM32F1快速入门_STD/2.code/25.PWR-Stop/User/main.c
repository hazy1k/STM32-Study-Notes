#include "stm32f10x.h"
#include "systick.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include <stdio.h>

// ֹͣģʽ���Ѻ�ʹ��HSIʱ�ӣ�������Ҫ�л���HSE
void STOP_SYSConfig(void)
{
    RCC_HSEConfig(RCC_HSE_ON);
    while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(RCC_GetSYSCLKSource() != 0x08); // ϵͳʱ��Դ�л���PLL
}

int main(void)
{
    RCC_ClocksTypeDef clock_status_wakeup;
    RCC_ClocksTypeDef clock_status_config;
    uint8_t clock_source_wakeup;
    uint8_t clock_source_config;
    
    BSP_LED_Init();
    KEY_EXTI_Init();
    SysTick_Init();
    USARTx_Init(115200);
    
    while(1)
    {
        printf("�̵�����ϵͳ����������...\r\n");
        LED_ON(GREEN_LED_Pin);
        Delay_ms(500);
        LED_OFF(GREEN_LED_Pin);
        Delay_ms(500);
        printf("׼������ֹͣģʽ...\r\n");
        // ȷ�����д��������ѷ���
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);    
        PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
        
        // 1. ��ȡ����ʱ��ʱ��״̬ (ʹ����ȷ�Ľṹ��)
        clock_source_wakeup = RCC_GetSYSCLKSource();
        RCC_GetClocksFreq(&clock_status_wakeup);
        
        // 2. ��������ϵͳʱ��
        STOP_SYSConfig();
        
        // 3. ���³�ʼ���ؼ�����
        SysTick_Init();          // ϵͳʱ�Ӹı��������SysTick
        USARTx_Init(115200);     // �������³�ʼ��USART
        
        // 4. ��ȡ�������ú��ʱ��״̬
        clock_source_config = RCC_GetSYSCLKSource();
        RCC_GetClocksFreq(&clock_status_config);
        
        // 5. ��ӡʱ��״̬
        printf("\r\n--- ���Ѻ�ʱ��״̬ ---\r\n");
        printf("ʱ��Դ: %s\r\n", (clock_source_wakeup == 0) ? "HSI" : "PLL");
        printf("SYSCLK: %d Hz\r\n", clock_status_wakeup.SYSCLK_Frequency);
        printf("HCLK  : %d Hz\r\n", clock_status_wakeup.HCLK_Frequency);
        printf("PCLK1 : %d Hz\r\n", clock_status_wakeup.PCLK1_Frequency);
        printf("PCLK2 : %d Hz\r\n", clock_status_wakeup.PCLK2_Frequency);
        
        printf("\r\n--- �����ʱ��״̬ ---\r\n");
        printf("ʱ��Դ: %s\r\n", (clock_source_config == 8) ? "PLL" : "HSI");
        printf("SYSCLK: %d Hz\r\n", clock_status_config.SYSCLK_Frequency);
        printf("HCLK  : %d Hz\r\n", clock_status_config.HCLK_Frequency);
        printf("PCLK1 : %d Hz\r\n", clock_status_config.PCLK1_Frequency);
        printf("PCLK2 : %d Hz\r\n", clock_status_config.PCLK2_Frequency);
        
        printf("\r\n������ɣ��ȴ���һ��ѭ��...\r\n\r\n");
        Delay_ms(2000);
    }
}
