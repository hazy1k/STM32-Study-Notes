/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   ʹ�ô���1���ض���c��printf������usart�˿ڣ��жϽ���ģʽ
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
   
#include "./usart/bsp_debug_usart.h"
#include "./pid/bsp_pid.h"
#include "./led/bsp_led.h"
#include "./tim/bsp_basic_tim.h"
#include "./stepper/bsp_stepper_ctrl.h"

extern _pid pid;
UART_HandleTypeDef UartHandle;

//���ڽ�������
unsigned char UART_RxBuffer[UART_RX_BUFFER_SIZE];
//���ڽ�������ָ��
unsigned char UART_RxPtr;
/* ���������� */
uint8_t receive_cmd = 0;

 /**
  * @brief  DEBUG_USART GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
  */  
void DEBUG_USART_Config(void)
{ 
  
  UartHandle.Instance          = DEBUG_USART;
  
  UartHandle.Init.BaudRate     = DEBUG_USART_BAUDRATE;
  UartHandle.Init.WordLength   = USART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = USART_STOPBITS_1;
  UartHandle.Init.Parity       = USART_PARITY_NONE;
//  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = USART_MODE_TX_RX;
  
  HAL_UART_Init(&UartHandle);
    
  /* ʹ�ܴ��ڿ��ж� */
  __HAL_UART_ENABLE_IT(&UartHandle, USART_IT_IDLE);
  
  HAL_UART_Receive_IT(&UartHandle, UART_RxBuffer, sizeof(UART_RxBuffer));
  
  HAL_NVIC_SetPriority(DEBUG_USART_IRQ, 2, 0);	// ��ռ���ȼ�0�������ȼ�1
  HAL_NVIC_EnableIRQ(DEBUG_USART_IRQ );		      // ʹ��USART1�ж�ͨ�� 
}


/**
  * @brief UART MSP ��ʼ�� 
  * @param huart: UART handle
  * @retval ��
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{  
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  DEBUG_USART_CLK_ENABLE();
	
	DEBUG_USART_RX_GPIO_CLK_ENABLE();
  DEBUG_USART_TX_GPIO_CLK_ENABLE();
  
/**USART1 GPIO Configuration    
  PA9     ------> USART1_TX
  PA10    ------> USART1_RX 
  */
  /* ����Tx����Ϊ���ù���  */
  GPIO_InitStruct.Pin = DEBUG_USART_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);
  
  /* ����Rx����Ϊ���ù��� */
  GPIO_InitStruct.Pin = DEBUG_USART_RX_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct); 
}

//��ս��ջ���
void uart_FlushRxBuffer(void)
{
  UART_RxPtr = 0;
  UART_RxBuffer[UART_RxPtr] = 0;
}

/*****************  �����ַ� **********************/
void Usart_SendByte(uint8_t str)
{
  HAL_UART_Transmit(&UartHandle, &str, 1, 1000);
}

/*****************  �����ַ��� **********************/
void Usart_SendString(uint8_t *str)
{
	unsigned int k=0;
  do 
  {
      HAL_UART_Transmit(&UartHandle,(uint8_t *)(str + k) ,1,1000);
      k++;
  } while(*(str + k)!='\0');
}

///�ض���c�⺯��printf������DEBUG_USART���ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
	/* ����һ���ֽ����ݵ�����DEBUG_USART */
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	
	return (ch);
}

///�ض���c�⺯��scanf������DEBUG_USART����д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		
	int ch;
	HAL_UART_Receive(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}

/**
  * @brief ����У���
  * @param ptr����Ҫ���������
  * @param len����Ҫ����ĳ���
  * @retval У���
  */
uint8_t check_sum(uint8_t init, uint8_t *ptr, uint8_t len )
{
  uint8_t sum = init;
  
  while(len--)
  {
    sum += *ptr;
    ptr++;
  }
  
  return sum;
}

/**
  * @brief ������λ����ֵ
  * @param cmd������
  * @param ch: ����ͨ��
  * @param data������ָ��
  * @param num����������
  * @retval ��
  */
void set_computer_value(uint8_t cmd, uint8_t ch, void *data, uint8_t num)
{
  uint8_t sum = 0;    // У���
  num *= 4;           // һ������ 4 ���ֽ�
  
  static packet_head_t set_packet;
  
  set_packet.head = PACKET_HEAD;     // ��ͷ 0x59485A53
  set_packet.len  = 0x0B + num;      // ����
  set_packet.ch   = ch;              // ����ͨ��
  set_packet.cmd  = cmd;             // ��������
  
  sum = check_sum(0, (uint8_t *)&set_packet, sizeof(set_packet));       // �����ͷУ���
  sum = check_sum(sum, (uint8_t *)data, num);                           // �������У���
  
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&set_packet, sizeof(set_packet), 0xFFFFF);    // ��������ͷ
  HAL_UART_Transmit(&UartHandle, (uint8_t *)data, num, 0xFFFFF);                          // ���Ͳ���
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&sum, sizeof(sum), 0xFFFFF);                  // ����У���
}

/**
  * @brief ͬ����λ����ֵ
  * @param ��
  * @retval ��
  */
void sync_computer_value(void)
{
  
}

extern int pid_status;
extern float set_point;

/**
  * @brief �������ڽ��յ�������
  * @param cmd������
  * @param ch: ����ͨ��
  * @param data������
  * @retval ��
  */
void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *husart)
{
  packet_head_t packet;
    
  packet.cmd = UART_RxBuffer[CMD_INDEX_VAL];
  packet.len  = COMPOUND_32BIT(&UART_RxBuffer[LEN_INDEX_VAL]);     // �ϳɳ���
  packet.head = COMPOUND_32BIT(&UART_RxBuffer[HEAD_INDEX_VAL]);    // �ϳɰ�ͷ
  
  if (packet.head == PACKET_HEAD)    // ����ͷ
  {
    /* ��ͷ��ȷ */
    if (check_sum(0, UART_RxBuffer, packet.len - 1) == UART_RxBuffer[packet.len - 1])    // ���У����Ƿ���ȷ
    {
      switch(packet.cmd)
      {
        case SET_P_I_D_CMD:
        {
          uint32_t temp0 = COMPOUND_32BIT(&UART_RxBuffer[13]);
          uint32_t temp1 = COMPOUND_32BIT(&UART_RxBuffer[17]);
          uint32_t temp2 = COMPOUND_32BIT(&UART_RxBuffer[21]);
          
          float p_temp, i_temp, d_temp;
          
          p_temp = *(float *)&temp0;
          i_temp = *(float *)&temp1;
          d_temp = *(float *)&temp2;
          
          set_p_i_d(p_temp, i_temp, d_temp);    // ���� P I D
        }
        break;

        case SET_TARGET_CMD:
        {
          int actual_temp = COMPOUND_32BIT(&UART_RxBuffer[13]);    // �õ�����

          pid.target_val = actual_temp;    // ����Ŀ��ֵ
        }
        break;
        
        case START_CMD:
        {
          Set_Stepper_Start();             // ����
        }
        break;
        
        case STOP_CMD:
        {
          Set_Stepper_Stop();              // ֹͣ
        }
        break;
        
        case RESET_CMD:
        {
          HAL_NVIC_SystemReset();          // ��λϵͳ
        }
        break;
        
        case SET_PERIOD_CMD:
        {
          uint32_t temp = COMPOUND_32BIT(&UART_RxBuffer[13]);     // ������
          SET_BASIC_TIM_PERIOD(temp);                             // ���ö�ʱ������1~1000ms
        }
        break;
      }
    }
  }
}

/*********************************************END OF FILE**********************/
