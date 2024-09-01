#ifndef __LED_H_
#define	__LED_H_



#include "stm32f10x.h"
 
 

/****************************** LED 引脚配置参数定义***************************************/
#define             macLED1_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macLED1_GPIO_CLK                        RCC_APB2Periph_GPIOA
#define             macLED1_GPIO_PORT                       GPIOA
#define             macLED1_GPIO_PIN                        GPIO_Pin_8

#define             macLED2_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macLED2_GPIO_CLK                        RCC_APB2Periph_GPIOA
#define             macLED2_GPIO_PORT                       GPIOA
#define             macLED2_GPIO_PIN                        GPIO_Pin_9

#define             macLED3_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macLED3_GPIO_CLK                        RCC_APB2Periph_GPIOA
#define             macLED3_GPIO_PORT                       GPIOA
#define             macLED3_GPIO_PIN                        GPIO_Pin_10

#define             macLED4_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macLED4_GPIO_CLK                        RCC_APB2Periph_GPIOE
#define             macLED4_GPIO_PORT                       GPIOE
#define             macLED4_GPIO_PIN                        GPIO_Pin_14


/****************************** LED 函数宏定义***************************************/
#define             macLED1_ON()                            GPIO_ResetBits ( macLED1_GPIO_PORT, macLED1_GPIO_PIN )
#define             macLED1_OFF()                           GPIO_SetBits ( macLED1_GPIO_PORT, macLED1_GPIO_PIN )
#define             macLED1_TOGGLE()                        GPIO_ReadOutputDataBit ( macLED1_GPIO_PORT, macLED1_GPIO_PIN ) ? \
                                                            GPIO_ResetBits ( macLED1_GPIO_PORT, macLED1_GPIO_PIN ) : GPIO_SetBits ( macLED1_GPIO_PORT, macLED1_GPIO_PIN )

#define             macLED2_ON()                            GPIO_ResetBits ( macLED2_GPIO_PORT, macLED2_GPIO_PIN )
#define             macLED2_OFF()                           GPIO_SetBits ( macLED2_GPIO_PORT, macLED2_GPIO_PIN )
#define             macLED2_TOGGLE()                        GPIO_ReadOutputDataBit ( macLED2_GPIO_PORT, macLED2_GPIO_PIN ) ? \
                                                            GPIO_ResetBits ( macLED2_GPIO_PORT, macLED2_GPIO_PIN ) : GPIO_SetBits ( macLED2_GPIO_PORT, macLED2_GPIO_PIN )

#define             macLED3_ON()                            GPIO_ResetBits ( macLED3_GPIO_PORT, macLED3_GPIO_PIN )
#define             macLED3_OFF()                           GPIO_SetBits ( macLED3_GPIO_PORT, macLED3_GPIO_PIN )
#define             macLED3_TOGGLE()                        GPIO_ReadOutputDataBit ( macLED3_GPIO_PORT, macLED3_GPIO_PIN ) ? \
                                                            GPIO_ResetBits ( macLED3_GPIO_PORT, macLED3_GPIO_PIN ) : GPIO_SetBits ( macLED3_GPIO_PORT, macLED3_GPIO_PIN )

#define             macLED4_ON()                            GPIO_ResetBits ( macLED4_GPIO_PORT, macLED4_GPIO_PIN )
#define             macLED4_OFF()                           GPIO_SetBits ( macLED4_GPIO_PORT, macLED4_GPIO_PIN )
#define             macLED4_TOGGLE()                        GPIO_ReadOutputDataBit ( macLED4_GPIO_PORT, macLED4_GPIO_PIN ) ? \
                                                            GPIO_ResetBits ( macLED4_GPIO_PORT, macLED4_GPIO_PIN ) : GPIO_SetBits ( macLED4_GPIO_PORT, macLED4_GPIO_PIN )



/************************** PAD 函数声明********************************/
void                LED_Init                         ( void );
 
 

#endif /* __LED_H_ */

