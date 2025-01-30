#include "led.h"
#include "usart.h"
#include "key.h"

static void Delay(__IO uint32_t nCount)	
{
	for(; nCount != 0; nCount--);
}

static uint8_t KEY2_LongPress(void)
{
	uint8_t downCNT = 0; 
	uint8_t upCNT = 0; 
	while(1)
	{
		Delay(0x2FFFF);
		if(GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == SET)
		{
			downCNT++;
			upCNT = 0;
			if(downCNT >= 100)
			{
				return 1;
			}
		}
		else
		{
			upCNT++;
			if(upCNT >= 5)
			{
				return 0;
			}
		}
	}
}

int main(void)
{
	LED_Init();
	KEY_GPIO_Init();
	USART_Config();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); // ʹ��PWR����ʱ��
	if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET)
	{
		LED_BLUE();
		printf("����ģʽ���Ѹ�λ\r\n");
	}
	else
	{
		LED_GREEN();
		printf("��������\r\n");
	}
	while(1)
	{
		if(KEY2_LongPress())
		{
			printf("����KEY2�������ģʽ����KEY1����\r\n");
			LED_RED();
			Delay(0xFFFFFF);
			PWR_ClearFlag(PWR_FLAG_WU); // �������ģʽ���ѱ�־
			PWR_WakeUpPinCmd(ENABLE);  // ʹ�ܴ���ģʽ��������
			PWR_EnterSTANDBYMode();  // �������ģʽ
		}
	}
}
