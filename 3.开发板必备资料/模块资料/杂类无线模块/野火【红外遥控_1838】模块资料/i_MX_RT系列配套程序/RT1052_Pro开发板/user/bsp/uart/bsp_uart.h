#ifndef __USART_H
#define __USART_H


#include "fsl_common.h"


/*********************************************************
 * UART1 GPIO�˿ڡ����źż�IOMUXC���ú궨��
 
ʹ��i.MX RT1052-Pro�װ壺
 *  TX-----GPIO_AD_B0_12,K14-----(CN5, 9)
 *  RX-----GPIO_AD_B0_13,L14-----(CN5, 11)
 *���Ź���         ���ű��     �����ڿ������϶�Ӧλ��
 *TX���Ŷ�Ӧ������CN5����ĵ�9��(CN5, 9)
 *RX���Ŷ�Ӧ������CN5����ĵ�11��(CN5, 11)
 
ʹ��i.MX RT1052-Mini�װ壺
 *  TX-----GPIO_AD_B0_12,K14-----(CN4, 9)
 *  RX-----GPIO_AD_B0_13,L14-----(CN4, 11)
 *���Ź���         ���ű��     �����ڿ������϶�Ӧλ��
 *TX���Ŷ�Ӧ������CN4����ĵ�9��(CN4, 9)
 *RX���Ŷ�Ӧ������CN4����ĵ�11��(CN4, 11)
 *********************************************************/
#define UART_RX_GPIO                GPIO1
#define UART_RX_GPIO_PIN            (13U)
#define UART_RX_IOMUXC              IOMUXC_GPIO_AD_B0_13_LPUART1_RX

#define UART_TX_GPIO                GPIO1
#define UART_TX_GPIO_PIN            (12U)
#define UART_TX_IOMUXC              IOMUXC_GPIO_AD_B0_12_LPUART1_TX

/*******************************************************************************
 * UART1 ���ںš��жϷ��������жϺ��ض���
 ******************************************************************************/

#define DEBUG_UARTx LPUART1
#define DEBUG_UART_IRQ LPUART1_IRQn
#define DEBUG_UART_IRQHandler LPUART1_IRQHandler

/*******************************************************************************
* UART1 �������ò������壬Ĭ�ϲ�������:
 * .baudRate_Bps = 115200U;
 * .parityMode = kLPUART_ParityDisabled;
 * .stopBitCount = kLPUART_OneStopBit;
 * .txFifoWatermark = 0;
 * .rxFifoWatermark = 0;
 * .enableTx = false;
 * .enableRx = false;
 ******************************************************************************/
#define  DEBUG_UART_BAUDRATE           115200U
#define  DEBUG_UART_STOP_BIT           kLPUART_OneStopBit
#define  DEBUG_UART_ENABLE_SEND        true
#define  DEBUG_UART_ENABLE_RESIVE      true

/*******************************************************************************
 * uart��������
 ******************************************************************************/
#define UART_RX_PAD_CONFIG_DATA            (SRE_0_SLOW_SLEW_RATE| \
                                        DSE_6_R0_6| \
                                        SPEED_1_MEDIUM_100MHz| \
                                        ODE_0_OPEN_DRAIN_DISABLED| \
                                        PKE_1_PULL_KEEPER_ENABLED| \
                                        PUE_1_PULL_SELECTED| \
                                        PUS_3_22K_OHM_PULL_UP| \
                                        HYS_0_HYSTERESIS_DISABLED) 
    /* ����˵�� : */
    /* ת������: ת��������
        ����ǿ��: R0/6 
        �������� : medium(100MHz)
        ��©����: �ر� 
        ��/����������: ʹ��
        ��/������ѡ��: ������
        ����/����ѡ��: 22Kŷķ����(ѡ���˱�������������Ч)
        �ͻ�������: ��ֹ */ 

#define UART_TX_PAD_CONFIG_DATA            (SRE_0_SLOW_SLEW_RATE| \
                                        DSE_6_R0_6| \
                                        SPEED_1_MEDIUM_100MHz| \
                                        ODE_0_OPEN_DRAIN_DISABLED| \
                                        PKE_1_PULL_KEEPER_ENABLED| \
                                        PUE_0_KEEPER_SELECTED| \
                                        PUS_3_22K_OHM_PULL_UP| \
                                        HYS_0_HYSTERESIS_DISABLED)
    /* ����˵�� : */
    /* ת������: ת��������
        ����ǿ��: R0/6 
        �������� : medium(100MHz)
        ��©����: �ر� 
        ��/����������: ʹ��
        ��/������ѡ��: ������
        ����/����ѡ��: 22Kŷķ����(ѡ���˱�������������Ч)
        �ͻ�������: ��ֹ */ 

/*******************************************************************************
 * ��������
 ******************************************************************************/
 void UART_Config(void);
 void UART_ModeConfig(void);
 void UART_IOMUXC_MUX_Config(void);
 void UART_IOMUXC_PAD_Config(void);
 
 void Uart_SendByte(LPUART_Type *base, uint8_t data);
 void Uart_SendString( LPUART_Type *base,  const char *str);
 void Uart_SendHalfWord(LPUART_Type *base, uint16_t ch);


#endif /* __USART_H */


