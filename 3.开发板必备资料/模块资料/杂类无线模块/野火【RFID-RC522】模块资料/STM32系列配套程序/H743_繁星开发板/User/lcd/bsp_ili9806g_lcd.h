#ifndef      __BSP_ILI9806G_LCD_H
#define	     __BSP_ILI9806G_LCD_H


#include "stm32h7xx.h"
#include "./font/fonts.h"


/***************************************************************************************
2^26 =0X0400 0000 = 64MB,ÿ�� BANK ��4*64MB = 256MB
64MB:FSMC_Bank1_NORSRAM1:0X6000 0000 ~ 0X63FF FFFF
64MB:FSMC_Bank1_NORSRAM2:0X6400 0000 ~ 0X67FF FFFF
64MB:FSMC_Bank1_NORSRAM3:0X6800 0000 ~ 0X6BFF FFFF
64MB:FSMC_Bank1_NORSRAM4:0X6C00 0000 ~ 0X6FFF FFFF

ѡ��BANK1-BORSRAM3 ���� TFT����ַ��ΧΪ0X6800 0000 ~ 0X6BFF FFFF
FSMC_A0 ��LCD��DC(�Ĵ���/����ѡ��)��
�Ĵ�������ַ = 0X6C00 0000
RAM����ַ = 0X6D00 0000 = 0X6C00 0000+2^0*2 = 0X6800 0000 + 0X2 = 0X6800 0002
��ѡ��ͬ�ĵ�ַ��ʱ����ַҪ���¼���  
****************************************************************************************/

/******************************* ILI9806G ��ʾ���� FSMC �������� ***************************/
//FSMC_Bank1_NORSRAM����LCD��������ĵ�ַ
#define      FSMC_Addr_ILI9806G_CMD         ( ( uint32_t ) 0x68000000 )

//FSMC_Bank1_NORSRAM����LCD���ݲ����ĵ�ַ      
#define      FSMC_Addr_ILI9806G_DATA        ( ( uint32_t ) 0x68000002 )

////��Ƭѡ���ž�����NOR/SRAM��
//#define      FSMC_Bank1_NORSRAMx           FSMC_Bank1_NORSRAM3

#define      DEBUG_DELAY()               Delay(0x25000)

// MPU��������
#define       FMC_REGION_NUMBER		         MPU_REGION_NUMBER0		      // ʹ��region0
#define       FMC_ADDRESS_START		         (FSMC_Addr_ILI9806G_CMD)			// ʹ�õ��׵�ַ
#define       FMC_REGION_SIZE			         MPU_REGION_SIZE_256MB       // ʹ�õĴ�С

/***************************** ILI934 ��ʾ�������ʼ������������� ***************************/
#define      ILI9806G_DispWindow_X_Star		    0     //��ʼ���X����
#define      ILI9806G_DispWindow_Y_Star		    0     //��ʼ���Y����

#define 			ILI9806G_LESS_PIXEL	  		480			//Һ�����϶̷�������ؿ��
#define 			ILI9806G_MORE_PIXEL	 		854			//Һ�����ϳ���������ؿ��

//����Һ��ɨ�跽����仯��XY���ؿ��
//����ILI9806G_GramScan�������÷���ʱ���Զ�����
extern uint16_t LCD_X_LENGTH,LCD_Y_LENGTH; 

//Һ����ɨ��ģʽ
//������ѡֵΪ0-7
extern uint8_t LCD_SCAN_MODE;

/******************************* ���� ILI934 ��ʾ��������ɫ ********************************/
#define      BACKGROUND		                BLACK   //Ĭ�ϱ�����ɫ

#define      WHITE		 		                  0xFFFF	   //��ɫ
#define      BLACK                         0x0000	   //��ɫ 
#define      GREY                          0xF7DE	   //��ɫ 
#define      BLUE                          0x001F	   //��ɫ 
#define      BLUE2                         0x051F	   //ǳ��ɫ 
#define      RED                           0xF800	   //��ɫ 
#define      MAGENTA                       0xF81F	   //����ɫ�����ɫ 
#define      GREEN                         0x07E0	   //��ɫ 
#define      CYAN                          0x7FFF	   //����ɫ����ɫ 
#define      YELLOW                        0xFFE0	   //��ɫ 
#define      BRED                          0xF81F
#define      GRED                          0xFFE0
#define      GBLUE                         0x07FF



/******************************* ���� ILI934 �������� ********************************/
#define      CMD_SetCoordinateX		 		    0x2A	     //����X����
#define      CMD_SetCoordinateY		 		    0x2B	     //����Y����
#define      CMD_SetPixel		 		          0x2C	     //�������




/********************************** ���� ILI934 ���� ***************************************/
void                     ILI9806G_Init                    ( void );
void                     ILI9806G_Rst                     ( void );
void                     ILI9806G_BackLed_Control         ( FunctionalState enumState );
void                     ILI9806G_GramScan                ( uint8_t ucOtion );
void                     ILI9806G_OpenWindow              ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight );
void                     ILI9806G_Clear                   ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight );
void                     ILI9806G_SetPointPixel           ( uint16_t usX, uint16_t usY );
uint16_t                 ILI9806G_GetPointPixel           ( uint16_t usX , uint16_t usY );
void                     ILI9806G_DrawLine                ( uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2 );
void                     ILI9806G_DrawRectangle           ( uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight,uint8_t ucFilled );
void                     ILI9806G_DrawCircle              ( uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint8_t ucFilled );
void                     ILI9806G_DispChar_EN             ( uint16_t usX, uint16_t usY, const char cChar );
void                     ILI9806G_DispStringLine_EN      ( uint16_t line, char * pStr );
void                     ILI9806G_DispString_EN      			( uint16_t usX, uint16_t usY, char * pStr );
void 											ILI9806G_DispString_EN_YDir 		(   uint16_t usX,uint16_t usY ,  char * pStr );

void 											LCD_SetFont											(sFONT *fonts);
sFONT 										*LCD_GetFont											(void);
void 											ILI9806G_ClearLine										(uint16_t Line);
void 											LCD_SetBackColor								(uint16_t Color);
void 											LCD_SetTextColor								(uint16_t Color)	;
void 											LCD_SetColors										(uint16_t TextColor, uint16_t BackColor);
void 											LCD_GetColors										(uint16_t *TextColor, uint16_t *BackColor);

#define 									LCD_ClearLine 						ILI9806G_ClearLine

/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalH(p,i)			{p->BSRR=i;}			  //����Ϊ�ߵ�ƽ		
#define digitalL(p,i)			{p->BSRR=(uint32_t)i << 16;}				//����͵�ƽ
//#define digitalToggle(p,i)		{p->ODR ^=i;}			//�����ת״̬
#endif /* __BSP_ILI9806G_ILI9806G_H */


