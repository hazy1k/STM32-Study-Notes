

#ifndef __AD7192_H
#define __AD7192_H

#include "stm32f4xx_hal.h"
extern uint32_t AD7192Registers[8]; //AD7192Registers[8]������Ԫ��Ϊ�޷��ų����ͣ�ÿ��Ԫ��ռ4���ֽ�(��32λ)
extern uint32_t AD7192Data;         //AD7192DataΪ�޷��ų����ͣ�ռ4���ֽ�
//-----------------OLED�˿ڶ���----------------

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

//Communications Register              ͨ�żĴ���(8λ)
#define	WEN				    0x00                //дʹ��
#define WEN_DIS			  0x80                //д��ֹ
#define RW_W			    0x00                //д����
#define	RW_R			    0x40                //������
	
//��AD7192�ڲ��Ĵ�����ַ���к궨��	
#define REG_COM_STA		  0x00                  //ѡ��״̬�Ĵ������в�����   ������Ϊ��00000 000
#define	REG_MODE		    0x01                  //ѡ��ģʽ�Ĵ������в�����   ������Ϊ��00000 001
#define	REG_CONF		    0x02                  //ѡ�����üĴ������в�����   ������Ϊ��00000 010
#define	REG_DATA		    0x03                  //ѡ�����ݼĴ������в�����   ������Ϊ��00000 011
#define	REG_ID			    0x04                  //ѡ��ID�Ĵ������в���  ��   ������Ϊ��00000 100
#define	REG_GPOCON		  0x05                  //ѡ��GPOCON�Ĵ������в����� ������Ϊ��00000 101  
#define	REG_OFFSET		  0x06                  //ѡ��ʧ���Ĵ������в�����   ������Ϊ��00000 110
#define	REG_FS			    0x07                  //ѡ�������̼Ĵ������в����� ������Ϊ��00000 111

#define	CREAD_EN		    0x04                  //������ȡʹ��
#define	CREAD_DIS		    0x00                  //������ȡ��ֹ

//Status Register                       ״̬�Ĵ���(8λ)
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

//Mode Register                         ģʽ�Ĵ���(24λ)
#define	MODE_CONT				    0x000000    //����ת��ģʽ
#define	MODE_SING				    0x200000    //����ת��ģʽ
#define	MODE_IDLE				    0x400000    //����ģʽ
#define	MODE_PD					    0x600000    //�ض�ģʽ
#define	MODE_INZCL				  0x800000    //�ڲ����ƽУ׼
#define	MODE_INFCL				  0xA00000    //�ڲ�������У׼
#define	MODE_SYSZCL				  0xC00000    //ϵͳ���ƽУ׼
#define	MODE_SYSFCL				  0xE00000    //ϵͳ������У׼
#define	DAT_STA_EN				  0x100000    //��ͨ�����ã�������Ĵ�����������
#define	DAT_STA_DIS				  0x000000	  
#define	EXT_XTAL				    0x000000    //�ⲿ����
#define	EXT_CLK					    0x040000    //�ⲿʱ��
#define	INCLK_MCLK2TRI			0x080000    //�ڲ�ʱ��,MCLK2����Ϊ��̬��
#define	INCLK_MCLK2EN			  0x0C0000    //�ڲ�ʱ��,�ɴ�MCLK2���
#define	SINC_4					    0x000000    //ʹ��sinc4�˲���
#define	SINC_3					    0x008000    //ʹ��sinc3�˲���
#define	ENPAR_EN				    0x002000    //ʹ����żУ��λ
#define	ENPAR_DIS				    0x000000    //ʧ����żУ��λ
#define	CLK_DIV_2				    0x001000    //ʹ����ʱ��2��Ƶ��
#define	CLK_DIV_DIS				  0x000000    //��ʱ�Ӳ���Ƶ
#define	SINGLECYCLE_EN			0x000800
#define	SINGLECYCLE_DIS			0x000000
#define	REJ60_EN				    0x000400    //ʹ��60Hz�ݲ�Ƶ��,ʵ��50Hz/60Hzͬʱ����
#define	REJ60_DIS				    0x000000    //ʧ��60Hz�ݲ�Ƶ��,ֻ����50Hz
										
//Configuration Register		              ���üĴ���(24λ)	
#define CHOP_EN					  0x800000     //ն��ʹ��
#define	CHOP_DIS				  0x000000
#define	REF_IN1					  0x000000     //��׼��ѹѡ��λ
#define	REF_IN2					  0x100000     //��׼��ѹѡ��λ
#define	AIN1_AIN2				  0x000100     //ʹ��ͨ��(AIN+)AIN1 (AIN-)AIN2
#define	AIN3_AIN4				  0x000200     //ʹ��ͨ��(AIN+)AIN3 (AIN-)AIN4
#define	TEMP					    0x000400     //ʹ��ͨ��  �¶ȴ�����
#define	AIN2_AIN2				  0x000800     //ʹ��ͨ��(AIN+)AIN2 (AIN-)AIN2
#define	AIN1_COM				  0x001000     //ʹ��ͨ��(AIN+)AIN1 (AIN-)AINCOM 
#define	AIN2_COM				  0x002000     //ʹ��ͨ��(AIN+)AIN2 (AIN-)AINCOM  
#define	AIN3_COM				  0x004000     //ʹ��ͨ��(AIN+)AIN3 (AIN-)AINCOM 
#define	AIN4_COM				  0x008000     //ʹ��ͨ��(AIN+)AIN4 (AIN-)AINCOM
#define	BURN_EN					  0x000080     //ʹ�ܼ�������
#define	BURN_DIS				  0x000000     //���ü�������
#define	REFDET_EN				  0x000040     //ʹ�ܻ�׼��ѹ��⹦��
#define	REFDET_DIS				0x000000     //���û�׼��ѹ��⹦��
#define	BUF_EN					  0x000010     //ʹ��ģ������˵Ļ�����
#define	BUF_DIS					  0x000000  
#define	UB_UNI					  0x000008     //�����Թ���ģʽ
#define UB_BI					    0x000000     //˫���Թ���ģʽ
#define	GAIN_1					  0x000000
#define	GAIN_8					  0x000003
#define	GAIN_16					  0x000004
#define	GAIN_32					  0x000005
#define	GAIN_64					  0x000006
#define	GAIN_128				  0x000007

//GPOCON Register                        ͨ������������ƼĴ���(8λ)
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
#define	NOP						    0x00								//�ղ���	

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
