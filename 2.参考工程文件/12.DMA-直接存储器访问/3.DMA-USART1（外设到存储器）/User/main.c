#include "stm32f10x.h"
#include "bsp_usart_dma.h"
#include "bsp_led.h"

static void Delay(__IO u32 nCount);

int main(void)
{
  USART_Config(); 
  USARTx_DMA_Config();
  LED_GPIO_Config();
  
  printf("\r\nDMA外设到存储器模式，用电脑向开发板串口发送数据，数据会返回到电脑。\r\n");
  
  // USART1 向 DMA发出RX请求
  USART_DMACmd(DEBUG_USARTx, USART_DMAReq_Rx, ENABLE); // 函数参数：选用的串口，DMA请求，使能/禁止
  // 用电脑向开发板串口发送数据，数据会返回到电脑。
  while(1)
  {
    LED1_TOGGLE
    Delay(0xFFFFF);
  }
}

static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}