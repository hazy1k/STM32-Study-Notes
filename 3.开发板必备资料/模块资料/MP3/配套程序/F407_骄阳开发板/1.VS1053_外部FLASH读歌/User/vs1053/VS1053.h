#ifndef __VS1053_H__
#define __VS1053_H__

#include "stm32f4xx.h"

//////////////////////////////////////////////////////////////
/*����SPI2��ΪVS1053��Ӳ���ӿ�*/

extern SPI_HandleTypeDef SpiHandle;

/*�ȴ���ʱʱ��*/
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))


#define SPIx_CLK_ENABLE()    __HAL_RCC_SPI2_CLK_ENABLE()
#define VS_SPI                SPI2

#define VS_XCS								GPIO_PIN_0								/*����VS1053��Ƭѡ�ܽ�*/
#define VS_SCLK								GPIO_PIN_1								/*����VS1053��ʱ�ӹܽ�*/
#define VS_MISO								GPIO_PIN_2								/*����VS1053��MISO�ܽ�*/
#define VS_MOSI								GPIO_PIN_3								/*����VS1053��MOSI�ܽ�*/
#define VS_SPIGPIO_PORT				GPIOI											/* GPIO�˿� */



#define VS_XDCS								GPIO_PIN_2								/*����VS1053��Ƭѡ�ܽ�*/
#define VS_GPIO_XDCS_PORT			GPIOD											/* GPIO�˿� */

#define VS_GPIO_RST_PORT			GPIOC											/* GPIO�˿� */
#define VS_RST								GPIO_PIN_12								/*����VS1053��RST�ܽ�*/

#define VS_GPIO_DREQ_PORT			GPIOH											/* GPIO�˿� */
#define VS_DREQ								GPIO_PIN_7								/*����VS1053��DREQ�ܽ�*/



#define VS_DREQ_IN						HAL_GPIO_ReadPin(VS_GPIO_DREQ_PORT,VS_DREQ)

#define VS_XDCS_SET						digitalH(VS_GPIO_XDCS_PORT,VS_XDCS)
#define VS_XDCS_CLR						digitalL(VS_GPIO_XDCS_PORT,VS_XDCS)

#define VS_XCS_SET						digitalH(VS_SPIGPIO_PORT,VS_XCS)
#define VS_XCS_CLR						digitalL(VS_SPIGPIO_PORT,VS_XCS)

#define VS_RST_SET						digitalH(VS_GPIO_RST_PORT,VS_RST)
#define VS_RST_CLR						digitalL(VS_GPIO_RST_PORT,VS_RST)


/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalH(p,i)			{p->BSRR=i;}			  //����Ϊ�ߵ�ƽ		
#define digitalL(p,i)			{p->BSRR=(uint32_t)i << 16;}				//����͵�ƽ

//////////////////////////////////////////////////////////////

__packed typedef struct 
{							  
	uint8_t mvol;		//������,��Χ:0~254
	uint8_t bflimit;		//����Ƶ���޶�,��Χ:2~15(��λ:10Hz)
	uint8_t bass;		//����,��Χ:0~15.0��ʾ�ر�.(��λ:1dB)
	uint8_t tflimit;		//����Ƶ���޶�,��Χ:1~15(��λ:Khz)
	uint8_t treble;		//����,��Χ:0~15(��λ:1.5dB)(ԭ����Χ��:-8~7,ͨ�������޸���);
	uint8_t effect;		//�ռ�Ч������.0,�ر�;1,��С;2,�е�;3,���.
//	uint8_t saveflag; 	//�����־,0X0A,�������;����,����δ����	   
}_vs1053_obj;


extern _vs1053_obj vsset;		//VS1053����

#define VS_WRITE_COMMAND 	0x02
#define VS_READ_COMMAND 	0x03
//VS1053�Ĵ�������
#define SPI_MODE        	0x00   
#define SPI_STATUS      	0x01   
#define SPI_BASS        	0x02   
#define SPI_CLOCKF      	0x03   
#define SPI_DECODE_TIME 	0x04   
#define SPI_AUDATA      	0x05   
#define SPI_WRAM        	0x06   
#define SPI_WRAMADDR    	0x07   
#define SPI_HDAT0       	0x08   
#define SPI_HDAT1       	0x09 
#define SPI_AIADDR      	0x0a   
#define SPI_VOL         	0x0b   
#define SPI_AICTRL0     	0x0c   
#define SPI_AICTRL1     	0x0d   
#define SPI_AICTRL2     	0x0e   
#define SPI_AICTRL3     	0x0f 

#define SM_DIFF         	0x01   
#define SM_JUMP         	0x02   
#define SM_RESET        	0x04   
#define SM_OUTOFWAV     	0x08   
#define SM_PDOWN        	0x10   
#define SM_TESTS        	0x20   
#define SM_STREAM       	0x40   
#define SM_PLUSV        	0x80   
#define SM_DACT         	0x100   
#define SM_SDIORD       	0x200   
#define SM_SDISHARE     	0x400   
#define SM_SDINEW       	0x800   
#define SM_ADPCM        	0x1000   
#define SM_ADPCM_HP     	0x2000 		 

#define I2S_CONFIG			0XC040
#define GPIO_DDR			0XC017
#define GPIO_IDATA			0XC018
#define GPIO_ODATA			0XC019



uint16_t  VS_RD_Reg(uint8_t address);				//���Ĵ���
uint16_t  VS_WRAM_Read(uint16_t addr);	    	//��RAM
void VS_WRAM_Write(uint16_t addr,uint16_t val);	//дRAM
void VS_WR_Data(uint8_t data);				//д����
void VS_WR_Cmd(uint8_t address,uint16_t data);	//д����
uint8_t   VS_HD_Reset(void);			    	//Ӳ��λ
void VS_Soft_Reset(void);           	//��λ
uint16_t VS_Ram_Test(void);             		//RAM����	    
void VS_Sine_Test(void);            	//���Ҳ���
												
void VS_SPI_SpeedHigh(void);
void VS_SPI_SpeedLow(void);
uint8_t 	 VS_SPI_ReadWriteByte(uint8_t data);
void VS_Init(void);						//��ʼ��VS10XX	 
void VS_Set_Speed(uint8_t t);				//���ò����ٶ�
uint16_t  VS_Get_HeadInfo(void);     		//�õ�������
uint32_t VS_Get_ByteRate(void);				//�õ��ֽ�����
uint16_t VS_Get_EndFillByte(void);			//�õ�����ֽ�
uint8_t 	 VS_Send_MusicData(uint8_t* buf);		//��VS10XX����32�ֽ�
void VS_Restart_Play(void);				//���¿�ʼ��һ�׸貥��	  
void VS_Reset_DecodeTime(void);			//�������ʱ��
uint16_t  VS_Get_DecodeTime(void);   		//�õ�����ʱ��

void VS_Load_Patch(uint16_t *patch,uint16_t len);	//�����û�patch
//uint8_t 	 VS_Get_Spec(uint16_t *p);       		//�õ���������	   
//void VS_Set_Bands(uint16_t *buf,uint8_t bands);	//��������Ƶ��
void VS_Set_Vol(uint8_t volx);				//����������   
void VS_Set_Bass(uint8_t bfreq,uint8_t bass,uint8_t tfreq,uint8_t treble);//���øߵ���
void VS_Set_Effect(uint8_t eft);				//������Ч
void VS_Set_All(void);

/*--------------  ������VS1053�������� -------------------------*/
/*--------------  ���濪ʼ�����ֲ��Ų��� -------------------------*/
#define BUFSIZE				1024*4
void vs1053_player_song(uint8_t *filepath);
#endif
