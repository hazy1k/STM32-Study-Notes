#ifndef __IRDA_H
#define	__IRDA_H

#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"

void EXTI_PB11_Config(void);
void IrDa_Init(void);
uint8_t Get_Pulse_Time(void);
uint8_t IrDa_Process(void);

#define IRDA_ID 0
#define RESET 0
#define SET !RESET

//红外遥控使用的GPIO及时钟

#define IRDA_GPIO             	GPIO1
#define IRDA_GPIO_PIN         	(18U)
#define IRDA_IOMUXC           	IOMUXC_GPIO_AD_B1_02_GPIO1_IO18

//中断相关，IRQ中断号及IRQHandler中断服务函数
#define IRDA_IRQ           GPIO1_Combined_16_31_IRQn
#define IRDA_IRQHandler    GPIO1_Combined_16_31_IRQHandler

 //读取引脚的电平
#define  IrDa_DATA_IN()	  GPIO_PinRead(IRDA_GPIO,IRDA_GPIO_PIN)				

#define IRDA_LOW GPIO_PinWrite(IRDA_GPIO, IRDA_GPIO_PIN, 0)
#define IRDA_HIGH GPIO_PinWrite(IRDA_GPIO, IRDA_GPIO_PIN, 1)

#endif /* __IRDA_H */



