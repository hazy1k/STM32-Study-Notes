#include "usart.h"
#include "stm32f10x.h"

int main(void)
{
  USART_Config();
  // ���Է���һ���ַ���
  USART_SendString(USARTx, "����һ�����ڲ��Գ���\r\n");
  while(1)
  {

  }
}
