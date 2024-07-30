#include "stm32f10x.h"
#include "bsp_usart.h"

int main(void)
{	
  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  USART_Config();
	
	/* 发送一个字符串 */
	Usart_SendString(DEBUG_USARTx, "这是一个串口中断接收回显实验\n");
	printf("欢迎使用野火STM32开发板\n\n\n\n");
	
  // 下面是我们写好的函数

  // Usart_SendByte() 函数参数：USARTx 串口号，发送的字节
  // Usart_SendArray() 函数参数：USARTx 串口号，发送的数组，数组长度
  // Usart_SendString() 函数参数：USARTx 串口号，发送的字符串
  // Usart_SendHalfWord() 函数参数：USARTx 串口号，发送的半字

  while(1)
	{	
		
	}	
}