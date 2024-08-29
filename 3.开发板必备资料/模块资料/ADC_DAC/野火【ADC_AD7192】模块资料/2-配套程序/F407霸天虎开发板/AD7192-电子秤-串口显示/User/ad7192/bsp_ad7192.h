

#ifndef __AD7192_H
#define __AD7192_H

#include "stm32f4xx_hal.h"
extern uint32_t AD7192Registers[8]; //AD7192Registers[8]的数组元素为无符号长整型，每个元素占4个字节(即32位)
extern uint32_t AD7192Data;         //AD7192Data为无符号长整型，占4个字节
//-----------------OLED端口定义----------------

#define AD7192_SPIx                 SPI1
#define AD7192_SPIx_CLK_ENABLE()    __HAL_RCC_SPI1_CLK_ENABLE()
#define AD7192_GPIO_CLK_ENABLE()   {__HAL_RCC_GPIOA_CLK_ENABLE();__HAL_RCC_GPIOB_CLK_ENABLE();} 
#define AD7192_CS_Pin              GPIO_PIN_3
#define AD7192_CS_GPIO_Port        GPIOA
#define AD7192_SCK_Pin             GPIO_PIN_3
#define AD7192_SCK_GPIO_Port       GPIOB
#define AD7192_MISO_Pin            GPIO_PIN_4   //DOUT
#define AD7192_MISO_GPIO_Port      GPIOB
#define AD7192_MOSI_Pin            GPIO_PIN_5   //DIN
#define AD7192_MOSI_GPIO_Port      GPIOB


#define AD7192_SCLK_H              HAL_GPIO_WritePin(AD7192_SCK_GPIO_Port,AD7192_SCK_Pin,GPIO_PIN_SET)
#define AD7192_SCLK_L              HAL_GPIO_WritePin(AD7192_SCK_GPIO_Port,AD7192_SCK_Pin,GPIO_PIN_RESET)

#define AD7192_CS_ENABLE()         HAL_GPIO_WritePin(AD7192_CS_GPIO_Port,AD7192_CS_Pin,GPIO_PIN_RESET)
#define AD7192_CS_DISABLE()        HAL_GPIO_WritePin(AD7192_CS_GPIO_Port,AD7192_CS_Pin,GPIO_PIN_SET)

//Register Map and bit definition

//Communications Register              通信寄存器(8位)
#define	WEN				    0x00                //写使能
#define WEN_DIS			  0x80                //写禁止
#define RW_W			    0x00                //写操作
#define	RW_R			    0x40                //读操作
	
//对AD7192内部寄存器地址进行宏定义	
#define REG_COM_STA		  0x00                  //选定状态寄存器进行操作；   二进制为：00000 000
#define	REG_MODE		    0x01                  //选定模式寄存器进行操作；   二进制为：00000 001
#define	REG_CONF		    0x02                  //选定配置寄存器进行操作；   二进制为：00000 010
#define	REG_DATA		    0x03                  //选定数据寄存器进行操作；   二进制为：00000 011
#define	REG_ID			    0x04                  //选定ID寄存器进行操作  ；   二进制为：00000 100
#define	REG_GPOCON		  0x05                  //选定GPOCON寄存器进行操作； 二进制为：00000 101  
#define	REG_OFFSET		  0x06                  //选定失调寄存器进行操作；   二进制为：00000 110
#define	REG_FS			    0x07                  //选定满量程寄存器进行操作； 二进制为：00000 111

#define	CREAD_EN		    0x04                  //连续读取使能
#define	CREAD_DIS		    0x00                  //连续读取禁止

//Status Register                       状态寄存器(8位)
#define	RDY_H			        0x80
#define	RDY_L			        0x00
#define	ERR_H			        0x40
#define	ERR_L			        0x00
#define	NOREF_H			      0x20
#define	NOREF_L			      0x00
#define	PARITY_H		      0x10
#define	PARITY_L		      0x00
#define	CHDST_AIN1_AIN2	  0x00
#define	CHDST_AIN3_AIN4	  0x01
#define	CHDST_TEMP		    0x02
#define	CHDST_AIN2_AIN2	  0x03
#define	CHDST_AIN1_COM	  0x04
#define	CHDST_AIN2_COM	  0x05
#define	CHDST_AIN3_COM	  0x06
#define	CHDST_AIN4_COM	  0x07

//Mode Register                         模式寄存器(24位)
#define	MODE_CONT				    0x000000    //连续转换模式
#define	MODE_SING				    0x200000    //单次转换模式
#define	MODE_IDLE				    0x400000    //空闲模式
#define	MODE_PD					    0x600000    //关断模式
#define	MODE_INZCL				  0x800000    //内部零电平校准
#define	MODE_INFCL				  0xA00000    //内部满量程校准
#define	MODE_SYSZCL				  0xC00000    //系统零电平校准
#define	MODE_SYSFCL				  0xE00000    //系统满量程校准
#define	DAT_STA_EN				  0x100000    //多通道启用，详情见寄存器功能描述
#define	DAT_STA_DIS				  0x000000	  
#define	EXT_XTAL				    0x000000    //外部晶振
#define	EXT_CLK					    0x040000    //外部时钟
#define	INCLK_MCLK2TRI			0x080000    //内部时钟,MCLK2引脚为三态。
#define	INCLK_MCLK2EN			  0x0C0000    //内部时钟,可从MCLK2获得
#define	SINC_4					    0x000000    //使用sinc4滤波器
#define	SINC_3					    0x008000    //使用sinc3滤波器
#define	ENPAR_EN				    0x002000    //使能奇偶校验位
#define	ENPAR_DIS				    0x000000    //失能奇偶校验位
#define	CLK_DIV_2				    0x001000    //使能主时钟2分频，
#define	CLK_DIV_DIS				  0x000000    //主时钟不分频
#define	SINGLECYCLE_EN			0x000800
#define	SINGLECYCLE_DIS			0x000000
#define	REJ60_EN				    0x000400    //使能60Hz陷波频率,实现50Hz/60Hz同时抑制
#define	REJ60_DIS				    0x000000    //失能60Hz陷波频率,只抑制50Hz
										
//Configuration Register		              配置寄存器(24位)	
#define CHOP_EN					  0x800000     //斩波使能
#define	CHOP_DIS				  0x000000
#define	REF_IN1					  0x000000     //基准电压选择位
#define	REF_IN2					  0x100000     //基准电压选择位
#define	AIN1_AIN2				  0x000100     //使能通道(AIN+)AIN1 (AIN-)AIN2
#define	AIN3_AIN4				  0x000200     //使能通道(AIN+)AIN3 (AIN-)AIN4
#define	TEMP					    0x000400     //使能通道  温度传感器
#define	AIN2_AIN2				  0x000800     //使能通道(AIN+)AIN2 (AIN-)AIN2
#define	AIN1_COM				  0x001000     //使能通道(AIN+)AIN1 (AIN-)AINCOM 
#define	AIN2_COM				  0x002000     //使能通道(AIN+)AIN2 (AIN-)AINCOM  
#define	AIN3_COM				  0x004000     //使能通道(AIN+)AIN3 (AIN-)AINCOM 
#define	AIN4_COM				  0x008000     //使能通道(AIN+)AIN4 (AIN-)AINCOM
#define	BURN_EN					  0x000080     //使能激励电流
#define	BURN_DIS				  0x000000     //禁用激励电流
#define	REFDET_EN				  0x000040     //使能基准电压检测功能
#define	REFDET_DIS				0x000000     //禁用基准电压检测功能
#define	BUF_EN					  0x000010     //使能模拟输入端的缓冲器
#define	BUF_DIS					  0x000000  
#define	UB_UNI					  0x000008     //单极性工作模式
#define UB_BI					    0x000000     //双极性工作模式
#define	GAIN_1					  0x000000
#define	GAIN_8					  0x000003
#define	GAIN_16					  0x000004
#define	GAIN_32					  0x000005
#define	GAIN_64					  0x000006
#define	GAIN_128				  0x000007

//GPOCON Register                        通用数字输出控制寄存器(8位)
#define BPDSW_CLOSE				0x40
#define	BPDSW_OPEN				0x00
#define	GP32EN					  0x20
#define	GP32DIS					  0x00
#define	GP10EN					  0x10
#define	GP10DIS					  0x00
#define	P3DAT_H					  0x08
#define	P3DAT_L					  0x00
#define	P2DAT_H					  0x04
#define	P2DAT_L					  0x00
#define	P1DAT_H					  0x02
#define	P1DAT_L					  0x00
#define	P0DAT_H					  0x01
#define	P0DAT_L					  0x00

//No Operation
#define	NOP						    0x00								//空操作	

void  AD7192_Init(void);
void ad7192_mode_cfg_reg(uint32_t mode, uint32_t cfg);
void  AD7192SoftwareReset(void);
void AD7192readdata(unsigned char* WriteBuffer, unsigned char *ReadBuffer, unsigned char NumberOfByte);
unsigned char ReadFromAD7192ViaSPI(const unsigned char RegisterStartAddress, const unsigned char NumberOfRegistersToRead, uint32_t *DataBuffer, const unsigned char OffsetInBuffer);
unsigned char WriteToAD7192ViaSPI(const unsigned char RegisterStartAddress, const unsigned char NumberOfRegistersToWrite, uint32_t *DataBuffer, const unsigned char OffsetInBuffer);
void AD7192InternalZeroScaleCalibration(void);
void AD7192InternalFullScaleCalibration(void);
void AD7192ExternalFullScaleCalibration(void);
void AD7192ExternalZeroScaleCalibration(void);

uint32_t AD7192ReadTemperature(void);
float RealTemperature(uint32_t TemperatureCode);

void AD7192StartSingleConvertion(uint32_t Channels);
void AD7192StartContinuousConvertion(uint32_t Channels);
void AD7192ExitContinuousRead(void);
void AD7192ExternalZeroScaleCalibration(void);
void AD7192ExternalFullScaleCalibration(void);
uint32_t AD7192ContinuousRead(void);
void AD7192StartContinuousRead(void);
uint32_t AD7192ReadConvertingData(void);
#endif
