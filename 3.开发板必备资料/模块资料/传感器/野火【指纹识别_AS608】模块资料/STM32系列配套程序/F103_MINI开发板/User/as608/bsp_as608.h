#ifndef __BSP_AS608_H
#define	__BSP_AS608_H


#include "stm32f10x.h"
#include <stdio.h>
#include "./SysTick/bsp_SysTick.h"
#include "./usart/bsp_usart.h"
#include "./usart/rx_data_queue.h"

#define CHAR_BUFFER1 0x01
#define CHAR_BUFFER2 0x02

#define PS_MAXNUM   240          /*指纹模块最大存储指纹模板数*/
/** 
  * 串口宏定义，不同的串口挂载的总线和IO不一样，移植时需要修改这几个宏
	* 1-修改总线时钟的宏，uart1挂载到apb2总线，其他uart挂载到apb1总线
	* 2-修改GPIO的宏
  */
/*串口2-USART2*/
#define  AS608_USART                    USART2
#define  AS608_USART_CLK                RCC_APB1Periph_USART2
#define  AS608_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
#define  AS608_USART_BAUDRATE           57600

/*USART GPIO 引脚宏定义*/
#define  AS608_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA)
#define  AS608_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  AS608_USART_TX_GPIO_PORT       GPIOA   
#define  AS608_USART_TX_GPIO_PIN        GPIO_Pin_2
#define  AS608_USART_RX_GPIO_PORT       GPIOA
#define  AS608_USART_RX_GPIO_PIN        GPIO_Pin_3

#define  AS608_USART_IRQ                USART2_IRQn
#define  AS608_USART_IRQHandler         USART2_IRQHandler


/*TouchOut引脚定义*/
#define AS608_TouchOut_INT_GPIO_PORT         GPIOA
#define AS608_TouchOut_INT_GPIO_CLK          (RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO)
#define AS608_TouchOut_INT_GPIO_PIN          GPIO_Pin_8
#define AS608_TouchOut_INT_EXTI_PORTSOURCE   GPIO_PortSourceGPIOA
#define AS608_TouchOut_INT_EXTI_PINSOURCE    GPIO_PinSource8
#define AS608_TouchOut_INT_EXTI_LINE         EXTI_Line8
#define AS608_TouchOut_INT_EXTI_IRQ          EXTI9_5_IRQn

#define AS608_TouchOut_IRQHandler            EXTI9_5_IRQHandler


/*信息输出*/
#define AS608_DEBUG_ON          0
#define AS608_DEBUG_ARRAY_ON		0

#define AS608_INFO(fmt,arg...)           printf("<<-AS608-INFO->> "fmt"\n",##arg)
#define AS608_ERROR(fmt,arg...)          printf("<<-AS608-ERROR->> "fmt"\n",##arg)
#define AS608_DEBUG(fmt,arg...)          do{\
                                          if(AS608_DEBUG_ON)\
                                          printf("<<-AS608-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

#define AS608_DEBUG_ARRAY(array, num)    do{\
																									 int32_t i;\
																									 uint8_t* a = array;\
																									 if(AS608_DEBUG_ARRAY_ON)\
																									 {\
																											printf("\n<<AS608-DEBUG-ARRAY->>\n");\
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
