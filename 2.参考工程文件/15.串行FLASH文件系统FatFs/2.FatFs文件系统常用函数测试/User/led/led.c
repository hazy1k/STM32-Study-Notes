#include "led.h"

// LED��ʼ������
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // ʹ��GPIOBʱ��
    RCC_APB2PeriphClockCmd(LED1_GPIO_CLK | LED2_GPIO_CLK | LED3_GPIO_CLK, ENABLE);
    // ��ʼ��LED1
    GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // ����Ϊ�������ģʽ
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // ����GPIO�ٶ�
    GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);
    // ��ʼ��LED2
    GPIO_InitStructure.GPIO_Pin = LED2_GPIO_PIN;
    GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);
    // ��ʼ��LED3
    GPIO_InitStructure.GPIO_Pin = LED3_GPIO_PIN;
    GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStructure);
    // ��ʼ����ɺ��ȹر�����LED
    LED_RGBOFF();
}
