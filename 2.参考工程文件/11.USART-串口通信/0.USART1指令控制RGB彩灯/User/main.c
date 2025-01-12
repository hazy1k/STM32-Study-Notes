#include "led.h"
#include "usart.h"

static void Show_Info(void) // ��ʾRGB�ʵƿ�����Ϣ
{
	USART_SendString(USARTx, (uint8_t*)"Please input the RGB value:\r\n");
	USART_SendString(USARTx, (uint8_t*)"1  -----   ��ɫ\n");
	USART_SendString(USARTx, (uint8_t*)"2  -----   ��ɫ\n");
	USART_SendString(USARTx, (uint8_t*)"3  -----   ��ɫ\n");
	USART_SendString(USARTx, (uint8_t*)"4  -----   ��ɫ\n");
	USART_SendString(USARTx, (uint8_t*)"5  -----   ��ɫ\n");
	USART_SendString(USARTx, (uint8_t*)"6  -----   ��ɫ\n");
	USART_SendString(USARTx, (uint8_t*)"7  -----   ��ɫ\n");
	USART_SendString(USARTx, (uint8_t*)"8  -----   �ر�\n");
}

int main(void)
{
	char temp;
    // ��ʼ�����裬��ӡ��ʾ��Ϣ
    LED_Init();
    USART_Config();
    Show_Info();
    while(1)
    {
		temp = USART_ReceiveData(USARTx);
		switch(temp)
		{
			case '1': LED_RED(); break;
			case '2': LED_GREEN(); break;
			case '3': LED_BLUE(); break;
			case '4': LED_YELLOW(); break;
			case '5': LED_PURPLE(); break;
			case '6': LED_CYAN(); break;
			case '7': LED_WHITE(); break;
			case '8': LED_RGBOFF(); break;
		}
	}
}
