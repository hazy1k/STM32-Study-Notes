#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32f1xx.h"
#include <stdio.h>

//���ڽ��ջ��������С
#define UART_RX_BUFFER_SIZE 256 

extern unsigned char UART_RxBuffer[UART_RX_BUFFER_SIZE];
extern uint8_t receive_cmd;

//���ڲ�����
#define DEBUG_USART_BAUDRATE                    115200

//���Ŷ���
/*******************************************************/
#define DEBUG_USART                             USART1
#define DEBUG_USART_CLK_ENABLE()                __HAL_RCC_USART1_CLK_ENABLE();

#define DEBUG_USART_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_RX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USART_RX_PIN                      GPIO_PIN_10

#define DEBUG_USART_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_TX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USART_TX_PIN                      GPIO_PIN_9

#define DEBUG_USART_IRQHandler                  USART1_IRQHandler
#define DEBUG_USART_IRQ                 		    USART1_IRQn
/************************************************************/

/* ��λ���궨�� */

/* ����ͷ�ṹ�� */
typedef __packed struct
{
  uint32_t head;    // ��ͷ
  uint8_t ch;       // ͨ��
  uint32_t len;     // ������
  uint8_t cmd;      // ����
//  uint8_t sum;      // У���
  
}packet_head_t;

#define PACKET_HEAD     0x59485A53    // ��ͷ

/* ͨ���궨�� */
#define CURVES_CH1      0x01
#define CURVES_CH2      0x02
#define CURVES_CH3      0x03
#define CURVES_CH4      0x04
#define CURVES_CH5      0x05

/* ָ��(��λ�� -> ��λ��) */
#define SEED_TARGET_CMD      0x01     // ������λ��ͨ����Ŀ��ֵ
#define SEED_FACT_CMD        0x02     // ����ͨ��ʵ��ֵ
#define SEED_P_I_D_CMD       0x03     // ���� PID ֵ��ͬ����λ����ʾ��ֵ��
#define SEED_START_CMD       0x04     // ��������ָ�ͬ����λ����ť״̬��
#define SEED_STOP_CMD        0x05     // ����ָֹͣ�ͬ����λ����ť״̬��
#define SEED_PERIOD_CMD      0x06     // �������ڣ�ͬ����λ����ʾ��ֵ��

/* ָ��(��λ�� -> ��λ��) */
#define SET_P_I_D_CMD        0x10     // ���� PID ֵ
#define SET_TARGET_CMD       0x11     // ����Ŀ��ֵ
#define START_CMD            0x12     // ����ָ��
#define STOP_CMD             0x13     // ָֹͣ��
#define RESET_CMD            0x14     // ��λָ��
#define SET_PERIOD_CMD       0x15     // ��������

/* ����ֵ�궨�� */
#define HEAD_INDEX_VAL       0x3u     // ��ͷ����ֵ��4�ֽڣ�
#define CHX_INDEX_VAL        0x4u     // ͨ������ֵ��1�ֽڣ�
#define LEN_INDEX_VAL        0x8u     // ��������ֵ��4�ֽڣ�
#define CMD_INDEX_VAL        0x9u     // ��������ֵ��1�ֽڣ�

#define EXCHANGE_H_L_BIT(data)      ((((data) << 24) & 0xFF000000) |\
                                     (((data) <<  8) & 0x00FF0000) |\
                                     (((data) >>  8) & 0x0000FF00) |\
                                     (((data) >> 24) & 0x000000FF))     // �����ߵ��ֽ�

#define COMPOUND_32BIT(data)        (((*(data-0) << 24) & 0xFF000000) |\
                                     ((*(data-1) << 16) & 0x00FF0000) |\
                                     ((*(data-2) <<  8) & 0x0000FF00) |\
                                     ((*(data-3) <<  0) & 0x000000FF))      // �ϳ�Ϊһ����

void uart_FlushRxBuffer(void);
void Usart_SendByte(uint8_t str);
void Usart_SendString(uint8_t *str);
void DEBUG_USART_Config(void);
//int fputc(int ch, FILE *f);
extern UART_HandleTypeDef UartHandle;
void set_computer_value(uint8_t cmd, uint8_t ch, void *data, uint8_t num);
uint8_t check_sum(uint8_t init, uint8_t *ptr,uint8_t len);
void parse_data(void);
#endif /* __USART1_H */
