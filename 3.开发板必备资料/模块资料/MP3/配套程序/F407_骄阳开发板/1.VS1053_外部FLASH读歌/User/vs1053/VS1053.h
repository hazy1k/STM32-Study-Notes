#ifndef __VS1053_H__
#define __VS1053_H__

#include "stm32f4xx.h"

//////////////////////////////////////////////////////////////
/*定义SPI2作为VS1053的硬件接口*/

extern SPI_HandleTypeDef SpiHandle;

/*等待超时时间*/
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))


#define SPIx_CLK_ENABLE()    __HAL_RCC_SPI2_CLK_ENABLE()
#define VS_SPI                SPI2

#define VS_XCS								GPIO_PIN_0								/*定义VS1053的片选管脚*/
#define VS_SCLK								GPIO_PIN_1								/*定义VS1053的时钟管脚*/
#define VS_MISO								GPIO_PIN_2								/*定义VS1053的MISO管脚*/
#define VS_MOSI								GPIO_PIN_3								/*定义VS1053的MOSI管脚*/
#define VS_SPIGPIO_PORT				GPIOI											/* GPIO端口 */



#define VS_XDCS								GPIO_PIN_2								/*定义VS1053的片选管脚*/
#define VS_GPIO_XDCS_PORT			GPIOD											/* GPIO端口 */

#define VS_GPIO_RST_PORT			GPIOC											/* GPIO端口 */
#define VS_RST								GPIO_PIN_12								/*定义VS1053的RST管脚*/

#define VS_GPIO_DREQ_PORT			GPIOH											/* GPIO端口 */
#define VS_DREQ								GPIO_PIN_7								/*定义VS1053的DREQ管脚*/



#define VS_DREQ_IN						HAL_GPIO_ReadPin(VS_GPIO_DREQ_PORT,VS_DREQ)

#define VS_XDCS_SET						digitalH(VS_GPIO_XDCS_PORT,VS_XDCS)
#define VS_XDCS_CLR						digitalL(VS_GPIO_XDCS_PORT,VS_XDCS)

#define VS_XCS_SET						digitalH(VS_SPIGPIO_PORT,VS_XCS)
#define VS_XCS_CLR						digitalL(VS_SPIGPIO_PORT,VS_XCS)

#define VS_RST_SET						digitalH(VS_GPIO_RST_PORT,VS_RST)
#define VS_RST_CLR						digitalL(VS_GPIO_RST_PORT,VS_RST)


/* 直接操作寄存器的方法控制IO */
#define	digitalH(p,i)			{p->BSRR=i;}			  //设置为高电平		
#define digitalL(p,i)			{p->BSRR=(uint32_t)i << 16;}				//输出低电平

//////////////////////////////////////////////////////////////

__packed typedef struct 
{							  
	uint8_t mvol;		//主音量,范围:0~254
	uint8_t bflimit;		//低音频率限定,范围:2~15(单位:10Hz)
	uint8_t bass;		//低音,范围:0~15.0表示关闭.(单位:1dB)
	uint8_t tflimit;		//高音频率限定,范围:1~15(单位:Khz)
	uint8_t treble;		//高音,范围:0~15(单位:1.5dB)(原本范围是:-8~7,通过函数修改了);
	uint8_t effect;		//空间效果设置.0,关闭;1,最小;2,中等;3,最大.
//	uint8_t saveflag; 	//保存标志,0X0A,保存过了;其他,还从未保存	   
}_vs1053_obj;


extern _vs1053_obj vsset;		//VS1053设置

#define VS_WRITE_COMMAND 	0x02
#define VS_READ_COMMAND 	0x03
//VS1053寄存器定义
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



uint16_t  VS_RD_Reg(uint8_t address);				//读寄存器
uint16_t  VS_WRAM_Read(uint16_t addr);	    	//读RAM
void VS_WRAM_Write(uint16_t addr,uint16_t val);	//写RAM
void VS_WR_Data(uint8_t data);				//写数据
void VS_WR_Cmd(uint8_t address,uint16_t data);	//写命令
uint8_t   VS_HD_Reset(void);			    	//硬复位
void VS_Soft_Reset(void);           	//软复位
uint16_t VS_Ram_Test(void);             		//RAM测试	    
void VS_Sine_Test(void);            	//正弦测试
												
void VS_SPI_SpeedHigh(void);
void VS_SPI_SpeedLow(void);
uint8_t 	 VS_SPI_ReadWriteByte(uint8_t data);
void VS_Init(void);						//初始化VS10XX	 
void VS_Set_Speed(uint8_t t);				//设置播放速度
uint16_t  VS_Get_HeadInfo(void);     		//得到比特率
uint32_t VS_Get_ByteRate(void);				//得到字节速率
uint16_t VS_Get_EndFillByte(void);			//得到填充字节
uint8_t 	 VS_Send_MusicData(uint8_t* buf);		//向VS10XX发送32字节
void VS_Restart_Play(void);				//重新开始下一首歌播放	  
void VS_Reset_DecodeTime(void);			//重设解码时间
uint16_t  VS_Get_DecodeTime(void);   		//得到解码时间

void VS_Load_Patch(uint16_t *patch,uint16_t len);	//加载用户patch
//uint8_t 	 VS_Get_Spec(uint16_t *p);       		//得到分析数据	   
//void VS_Set_Bands(uint16_t *buf,uint8_t bands);	//设置中心频率
void VS_Set_Vol(uint8_t volx);				//设置主音量   
void VS_Set_Bass(uint8_t bfreq,uint8_t bass,uint8_t tfreq,uint8_t treble);//设置高低音
void VS_Set_Effect(uint8_t eft);				//设置音效
void VS_Set_All(void);

/*--------------  以上是VS1053驱动部分 -------------------------*/
/*--------------  下面开始是音乐播放部分 -------------------------*/
#define BUFSIZE				1024*4
void vs1053_player_song(uint8_t *filepath);
#endif
