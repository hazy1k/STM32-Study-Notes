#ifndef      __BSP_ILI9341_LCD_H
#define	     __BSP_ILI9341_LCD_H
#include "stm32f10x.h"
/***************************************************************************************
2^26 =0X0400 0000 = 64MB,ÿ�� BANK ��4*64MB = 256MB
64MB:FSMC_Bank1_NORSRAM1:0X6000 0000 ~ 0X63FF FFFF
64MB:FSMC_Bank1_NORSRAM2:0X6400 0000 ~ 0X67FF FFFF
64MB:FSMC_Bank1_NORSRAM3:0X6800 0000 ~ 0X6BFF FFFF
64MB:FSMC_Bank1_NORSRAM4:0X6C00 0000 ~ 0X6FFF FFFF

ѡ��BANK1-BORSRAM4 ���� TFT����ַ��ΧΪ0X6C00 0000 ~ 0X6FFF FFFF
FSMC_A23 ��LCD��DC(�Ĵ���/����ѡ��)��
�Ĵ�������ַ = 0X6C00 0000
RAM����ַ = 0X6D00 0000 = 0X6C00 0000+2^23*2 = 0X6C00 0000 + 0X100 0000 = 0X6D00 0000
��ѡ��ͬ�ĵ�ַ��ʱ����ַҪ���¼���  
****************************************************************************************/

/******************************* ILI9341 ��ʾ���� FSMC �������� ***************************/
#define      macFSMC_Addr_ILI9341_CMD         ( ( uint32_t ) 0x60000000 )	     //FSMC_Bank1_NORSRAM1����LCD��������ĵ�ַ
#define      macFSMC_Addr_ILI9341_DATA        ( ( uint32_t ) 0x60020000 )      //FSMC_Bank1_NORSRAM1����LCD���ݲ����ĵ�ַ  

//����emWinʹ��
#define 		macFSMC_ILI9341_REG								*(__IO uint16_t *)(0x60000000)
#define 		macFSMC_ILI9341_RAM								*(__IO uint16_t *)(0x60020000)

#define      macFSMC_Bank1_NORSRAMx           FSMC_Bank1_NORSRAM1



/******************************* ILI9341 ��ʾ��8080ͨѶ���Ŷ��� ***************************/
#define      macILI9341_CS_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define      macILI9341_CS_CLK                RCC_APB2Periph_GPIOD   
#define      macILI9341_CS_PORT               GPIOD
#define      macILI9341_CS_PIN                GPIO_Pin_7

#define      macILI9341_DC_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define      macILI9341_DC_CLK                RCC_APB2Periph_GPIOD   
#define      macILI9341_DC_PORT               GPIOD
#define      macILI9341_DC_PIN                GPIO_Pin_11

#define      macILI9341_RST_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define      macILI9341_RST_CLK               RCC_APB2Periph_GPIOE 
#define      macILI9341_RST_PORT              GPIOE
#define      macILI9341_RST_PIN               GPIO_Pin_1

#define      macILI9341_BK_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define      macILI9341_BK_CLK                RCC_APB2Periph_GPIOD    
#define      macILI9341_BK_PORT               GPIOD
#define      macILI9341_BK_PIN                GPIO_Pin_12



/*************************************** ����Ԥ�� ******************************************/
#define      macDEBUG_DELAY()                



/***************** ILI934 ��ʾ��ȫ��Ĭ�ϣ�ɨ�跽��Ϊ1ʱ������Ⱥ����߶� *********************/
#define      macILI9341_Default_Max_Width		  240     //��ʼ���X����
#define      macILI9341_Default_Max_Heigth		320     //��ʼ���Y����


/******************************* ���� ILI934 �������� ********************************/
#define      macCMD_SetCoordinateX		 		    0x2A	     //����X����
#define      macCMD_SetCoordinateY		 		    0x2B	     //����Y����
#define      macCMD_SetPixel		 		          0x2C	     //�������


/* ���� LCD ����оƬ ID */
#define     LCDID_UNKNOWN             0
#define     LCDID_ILI9341             0x9341
#define     LCDID_ST7789V             0x8552


/********************************** ���� ILI934 ���� ***************************************/
#define			LCD_BK_EN		  GPIO_ResetBits(macILI9341_BK_PORT, macILI9341_BK_PIN)
#define			LCD_BK_DIS		GPIO_SetBits(macILI9341_BK_PORT, macILI9341_BK_PIN)

void ILI9341_Init ( void );
void ILI9341_OpenWindow ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight );
uint16_t ILI9341_ReadID(void);

extern uint16_t lcdid;

#endif /* __BSP_ILI9341_ILI9341_H */
