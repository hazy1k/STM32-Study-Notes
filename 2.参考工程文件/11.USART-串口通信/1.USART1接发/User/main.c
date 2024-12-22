#include "usart.h"
#include "stm32f10x.h"

int main(void)
{
  USART_Config();
  // 测试发送一个字符串
  USART_SendString(USARTx, "这是一个串口测试程序\r\n");
  while(1)
  {

  }
}
