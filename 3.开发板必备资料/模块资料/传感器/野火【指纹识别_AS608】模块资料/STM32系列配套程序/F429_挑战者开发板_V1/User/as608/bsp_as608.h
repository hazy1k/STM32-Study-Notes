#ifndef __BSP_AS608_H
#define	__BSP_AS608_H


#include "stm32f4xx.h"
#include <stdio.h>
#include "./SysTick/bsp_SysTick.h"
#include "./usart/bsp_debug_usart.h"
#include "./usart/rx_data_queue.h"

#define CHAR_BUFFER1 0x01
#define CHAR_BUFFER2 0x02

/*接收数据延时*/
#define AS608_DELAY_MS(x) SysTick_Delay_Ms(x)

#define PS_MAXNUM   240          /*指纹模块最大存储指纹模板数*/
/** 
  * 串口宏定义，不同的串口挂载的总线和IO不一样，移植时需要修改这几个宏
	* 1-修改总线时钟的宏，uart1挂载到apb2总线，其他uart挂载到apb1总线
	* 2-修改GPIO的宏
  */
  
  
  
//引脚定义
/*******************************************************/
#define AS608_USART                             USART2
#define AS608_USART_CLK                         RCC_APB1Periph_USART2
#define AS608_USART_BAUDRATE                    57600               //串口波特率

#define AS608_USART_RX_GPIO_PORT                GPIOA
#define AS608_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define AS608_USART_RX_PIN                      GPIO_Pin_3
#define AS608_USART_RX_AF                       GPIO_AF_USART2
#define AS608_USART_RX_SOURCE                   GPIO_PinSource3

#define AS608_USART_TX_GPIO_PORT                GPIOA
#define AS608_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define AS608_USART_TX_PIN                      GPIO_Pin_2
#define AS608_USART_TX_AF                       GPIO_AF_USART2
#define AS608_USART_TX_SOURCE                   GPIO_PinSource2

#define AS608_USART_IRQHandler                  USART2_IRQHandler
#define AS608_USART_IRQ                 				USART2_IRQn  
///*串口2-USART2*/
//#define  AS608_USART                    USART2
//#define  AS608_USART_CLK                RCC_APB1Periph_USART2
//#define  AS608_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
//#define  AS608_USART_BAUDRATE           57600

///*USART GPIO 引脚宏定义*/
//#define  AS608_USART_GPIO_CLK           (RCC_AHB1Periph_GPIOA)
//#define  AS608_USART_GPIO_APBxClkCmd    RCC_AHB1PeriphClockCmd
//    
//#define  AS608_USART_TX_GPIO_PORT       GPIOA   
//#define  AS608_USART_TX_GPIO_PIN        GPIO_Pin_2
//#define  AS608_USART_RX_GPIO_PORT       GPIOA
//#define  AS608_USART_RX_GPIO_PIN        GPIO_Pin_3

//#define  AS608_USART_IRQ                USART2_IRQn
//#define  AS608_USART_IRQHandler         USART2_IRQHandler

/*TouchOut引脚定义*/
#define AS608_TouchOut_INT_GPIO_PORT                GPIOA
#define AS608_TouchOut_INT_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define AS608_TouchOut_INT_GPIO_PIN                 GPIO_Pin_4
#define AS608_TouchOut_INT_EXTI_PORTSOURCE          EXTI_PortSourceGPIOA
#define AS608_TouchOut_INT_EXTI_PINSOURCE           EXTI_PinSource4
#define AS608_TouchOut_INT_EXTI_LINE                EXTI_Line4
#define AS608_TouchOut_INT_EXTI_IRQ                 EXTI4_IRQn

#define AS608_TouchOut_IRQHandler                   EXTI4_IRQHandler


/*信息输出*/
#define AS608_AS608_ON          0
#define AS608_AS608_ARRAY_ON		0

#define AS608_INFO(fmt,arg...)           printf("<<-AS608-INFO->> "fmt"\n",##arg)
#define AS608_ERROR(fmt,arg...)          printf("<<-AS608-ERROR->> "fmt"\n",##arg)
#define AS608_AS608(fmt,arg...)          do{\
                                          if(AS608_AS608_ON)\
                                          printf("<<-AS608-AS608->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

#define AS608_AS608_ARRAY(array, num)    do{\
																									 int32_t i;\
																									 uint8_t* a = array;\
																									 if(AS608_AS608_ARRAY_ON)\
																									 {\
																											printf("\n<<AS608-AS608-ARRAY->>\n");\
																											for (i = 0; i < (num); i++)\
																											{\
																													printf("%02x   ", (a)[i]);\
																													if ((i + 1 ) %10 == 0)\
																													{\
																															printf("\n");\
																													}\
																											}\
																											printf("\n");\
																									}\
																								 }while(0)		
 
extern uint32_t AS608_Addr ;       /*指纹识别模块默认地址*/ 
                                                 
void AS608_Config(void);
void ShowErrMessage( uint16_t  ensure);
uint16_t ReturnFlag( uint16_t *i);
uint16_t PS_GetImage(void);
uint16_t PS_GenChar(uint8_t BufferID);
uint16_t PS_Connect(uint32_t *PS_Addr);
uint16_t PS_Match(void);
uint16_t PS_RegModel(void);
uint32_t GET_NUM(void);
uint16_t PS_StoreChar(uint8_t BufferID,uint16_t PageID);
uint16_t PS_HighSpeedSearch(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,uint16_t *p);
uint16_t PS_DeletChar(uint16_t PageID,uint16_t N);
uint16_t PS_Empty(void);
 



#endif
