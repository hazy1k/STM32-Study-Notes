#include "stm32f10x.h"
#include "bsp_usart_dma.h"
#include "bsp_led.h"

static void Delay(__IO u32 nCount);

int main(void)
{
  USART_Config(); 
  USARTx_DMA_Config();
  LED_GPIO_Config();
  
  printf("\r\nDMA���赽�洢��ģʽ���õ����򿪷��崮�ڷ������ݣ����ݻ᷵�ص����ԡ�\r\n");
  
  // USART1 �� DMA����RX����
  USART_DMACmd(DEBUG_USARTx, USART_DMAReq_Rx, ENABLE); // ����������ѡ�õĴ��ڣ�DMA����ʹ��/��ֹ
  // �õ����򿪷��崮�ڷ������ݣ����ݻ᷵�ص����ԡ�
  while(1)
  {
    LED1_TOGGLE
    Delay(0xFFFFF);
  }
}

static void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}