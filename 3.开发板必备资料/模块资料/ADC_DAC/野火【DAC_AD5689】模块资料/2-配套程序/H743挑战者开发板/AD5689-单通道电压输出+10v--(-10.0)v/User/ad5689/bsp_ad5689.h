#ifndef __AD5689_H
#define __AD5689_H

#include "stm32h7xx_hal.h"

#define AD5689_SPIx                SPI5
#define AD5689_SPIx_CLK_ENABLE()    __HAL_RCC_SPI5_CLK_ENABLE()
#define AD5689_GPIO_CLK_ENABLE()   {__HAL_RCC_GPIOF_CLK_ENABLE();__HAL_RCC_GPIOA_CLK_ENABLE();} 
#define AD5689_SYNC_Pin            GPIO_PIN_6   //SYNC
#define AD5689_SYNC_GPIO_Port      GPIOF
#define AD5689_SCK_Pin             GPIO_PIN_7   //SCLK
#define AD5689_SCK_GPIO_Port       GPIOF
#define AD5689_MISO_Pin            GPIO_PIN_8   //SDO
#define AD5689_MISO_GPIO_Port      GPIOF
#define AD5689_MOSI_Pin            GPIO_PIN_9   //SDIN
#define AD5689_MOSI_GPIO_Port      GPIOF

/**AD5689**/
#define AD5689_LDAC_Pin            GPIO_PIN_4   //LDAC
#define AD5689_LDAC_GPIO_Port      GPIOA
#define AD5689_RESET_Pin           GPIO_PIN_6   //RESET
#define AD5689_RESET_GPIO_Port     GPIOA

#define AD5689_SCLK_H              HAL_GPIO_WritePin(AD5689_SCK_GPIO_Port,AD5689_SCK_Pin,GPIO_PIN_SET)
#define AD5689_SCLK_L              HAL_GPIO_WritePin(AD5689_SCK_GPIO_Port,AD5689_SCK_Pin,GPIO_PIN_RESET)
#define AD5689_RESET_H             HAL_GPIO_WritePin(AD5689_RESET_GPIO_Port,AD5689_RESET_Pin,GPIO_PIN_SET)
#define AD5689_RESET_L             HAL_GPIO_WritePin(AD5689_RESET_GPIO_Port,AD5689_RESET_Pin,GPIO_PIN_RESET)
#define AD5689_SYC_H               HAL_GPIO_WritePin(AD5689_SCK_GPIO_Port,AD5689_SCK_Pin,GPIO_PIN_SET)
#define AD5689_SYC_L               HAL_GPIO_WritePin(AD5689_SCK_GPIO_Port,AD5689_SCK_Pin,GPIO_PIN_RESET)
#define AD5689_SYNC_H              HAL_GPIO_WritePin(AD5689_SYNC_GPIO_Port,AD5689_SYNC_Pin,GPIO_PIN_SET)
#define AD5689_SYNC_L              HAL_GPIO_WritePin(AD5689_SYNC_GPIO_Port,AD5689_SYNC_Pin,GPIO_PIN_RESET)



#define     NormalOperation					0
#define     R1K2GND									1
#define     R100K2GND								2
#define     ThreeState							3

#define     DAC_A    								0x01
#define     DAC_B										0x08
#define     DAC_ALL									0x09

#define     LDAC_Enable     				0
#define     LDAC_Disable						1

#define     Standalone_mode 				0
#define     DCEN_mode								1 
	
#define CMD_WRITEn            	    1
#define CMD_WRITEn_UPDATEn          2
#define CMD_WRITE_UPDATEn           3
#define CMD_POWER_DOWN              4
#define CMD_LDAC_MASK               5
#define CMD_RESET               		6
#define CMD_DCEN_REG            		8
#define CMD_READ_BACK          		  9

void AD5689_Sine(void);
void AD5689_Init(void);
void AD5689_SetRegisterValue(uint8_t command,uint16_t channel,uint16_t data);
void AD5689_Reset(void);
void AD5689_PowerDown(uint16_t powerdown_channel1,uint16_t powerdown_channel2);
void AD5689_Internal_Reference(uint16_t ref);
void AD5689_Write_InputREG(uint16_t channel,uint16_t data);
void AD5689_InputREG_toUpdate_DACREG(uint16_t channel,uint16_t data);
void AD5689_WriteUpdate_DACREG(uint16_t channel,uint16_t data);
void AD5689_DAISYCHAIN_OPERATION(uint16_t decn);

#endif

