#include "usart.h"
#include "stm32f10x.h"

int main(void)
{
  USART_Config();
  // ���Է���һ���ַ���
	USART_SendString(USARTx, "USART1 Test\r\n");
  while(1)
  {

  }
}
