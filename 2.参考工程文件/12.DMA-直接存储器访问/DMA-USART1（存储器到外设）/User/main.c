
// DMA �洢�������裨���ڣ����ݴ���ʵ��

#include "stm32f10x.h"
#include "bsp_usart_dma.h"
#include "bsp_led.h"

extern uint8_t SendBuff[SENDBUFF_SIZE];
static void Delay(__IO u32 nCount); 

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
  uint16_t i;
  /* ��ʼ��USART */
  USART_Config(); 

  /* ����ʹ��DMAģʽ */
  USARTx_DMA_Config();
  
  /* ����RGB��ɫ�� */
  LED_GPIO_Config();

  //printf("\r\n USART1 DMA TX ���� \r\n");
  
  /*��佫Ҫ���͵�����*/
  for(i=0;i<SENDBUFF_SIZE;i++)
  {
    SendBuff[i]	 = 'P';
    
  }

  /*Ϊ��ʾDMA�������ж�CPU���ܴ����������飬����ʹ��DMA�������ݣ����ǳ���
  *��ʱ�����п��ܻᵼ�µ��Զ˴��ڵ������ֻῨ��������ҷɵ������
  *���DMA�����е�ѭ��ģʽ��Ϊ����ģʽ*/		
  
  /* USART1 �� DMA����TX���� */
  USART_DMACmd(DEBUG_USARTx, USART_DMAReq_Tx, ENABLE);

  /* ��ʱCPU�ǿ��еģ����Ը����������� */  
  //����ͬʱ����LED
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
/*********************************************END OF FILE**********************/
