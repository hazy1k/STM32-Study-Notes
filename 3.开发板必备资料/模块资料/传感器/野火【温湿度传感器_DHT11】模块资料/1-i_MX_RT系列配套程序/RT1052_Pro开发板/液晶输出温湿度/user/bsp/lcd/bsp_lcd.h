#ifndef __BSP_LCD_H
#define	__BSP_LCD_H

#include "fsl_common.h"
#include "./font/fonts.h"

/* �˺�Ϊ��ʱ���Դ����ص�ʹ��RGB888 ��XRGB8888 ��ʽ��
   Ϊ��ʱʹ��RGB565��ʽ���Ƽ�ֵΪ0 
*/
#define LCD_RGB_888 				0

/* �˺�Ϊ��ʱ��ʹ��24��������������Ļ��
	 Ϊ��ʱ��ʹ��16�������ߣ�
	 ������������ӿ�ʹ���˲���LCD�����ź��ߣ�
	 ���Ա�Ӳ����ƽ�֧��LCDʹ��16�������ߣ�
	 �����ظ�ʽ����ΪRGB888��ʽ24λ��������Ϊ16λ��
	 eLCDIF���ʱ���Զ�ת������ʱ�Դ����ص�ʹ��24λ�˷ѿռ䣬
   �����Ƽ�ʹ��RGB565��ʽ
 */
#define LCD_BUS_24_BIT			0


#if LCD_BUS_24_BIT
	/* LCD�����߿�� */
	#define LCD_DATA_BUS_WIDTH    kELCDIF_DataBus24Bit
#else
	/* LCD�����߿�� */
	#define LCD_DATA_BUS_WIDTH    kELCDIF_DataBus16Bit
#endif


/* LCD�������ţ��ߵ�ƽ���� */
#define LCD_BL_GPIO 				      GPIO1
#define LCD_BL_GPIO_PIN 		      (15U)
#define LCD_BL_IOMUXC			        IOMUXC_GPIO_AD_B0_15_GPIO1_IO15

/* LCD �ֱ��� */
#define LCD_PIXEL_WIDTH     800
#define LCD_PIXEL_HEIGHT    480

/* LCDʱ�� */
/* ����Һ�������ֲ����� */
#define LCD_HSW 	1
#define LCD_HFP 	22
#define LCD_HBP 	46
#define LCD_VSW 	1
#define LCD_VFP 	22
#define LCD_VBP 	23
#define LCD_POLARITY_FLAGS	(kELCDIF_DataEnableActiveHigh |	\
															kELCDIF_VsyncActiveLow | 			\
															kELCDIF_HsyncActiveLow | 			\
															kELCDIF_DriveDataOnRisingClkEdge)



/* ��������������, Ϊ���������, LCDIF������Ҫ64B���� */
#define FRAME_BUFFER_ALIGN    64

/* �����Ƿ�ʹ���жϵı�־ */
#define LCD_INTERRUPT_DISABLE   0
#define LCD_INTERRUPT_ENABLE    (!LCD_INTERRUPT_DISABLE)

/* ���ƺ��ߺʹ��ߵı�־ */
#define LINE_DIR_HORIZONTAL       0x0
#define LINE_DIR_VERTICAL         0x1

/*
	LCD ��ɫ���룬CL_��Color�ļ�д
	16Bit�ɸ�λ����λ�� RRRR RGGG GGGB BBBB

	�����RGB �꽫24λ��RGBֵת��Ϊ16λ��ʽ��
	����windows�Ļ��ʳ��򣬵���༭��ɫ��ѡ���Զ�����ɫ�����Ի�õ�RGBֵ��

	�Ƽ�ʹ������ȡɫ���������㿴���Ľ�����ɫ��
*/
#if LCD_RGB_888

	/* eLCDIF ���ظ�ʽ���� */
	#define ELCDIF_PIXEL_FORMAT 	kELCDIF_PixelFormatXRGB8888
	
  /* ���ص���ֽ��� */
	/* ʹ�� 24-bit RGB888 �� XRGB8888 ��ʽ. */
	#define LCD_BPP 				4U 

	/* ���ص�ʹ��32λ���� */
	typedef uint32_t				pixel_t;

	/* RGB888��ɫת�� */
	/* ��8λR,G,Bת��Ϊ 24λRGB888��ʽ */
	#define RGB(R,G,B)	( (R<< 16) | (G << 8) | (B))	

#else 
	/* eLCDIF ���ظ�ʽ���� */
	#define ELCDIF_PIXEL_FORMAT 	kELCDIF_PixelFormatRGB565

	/* ���ص���ֽ��� */
	/* ʹ�� 16-bit RGB565 ��ʽ. */
	#define LCD_BPP 	2U 

	/* ���ص�ʹ��16λ���� */
	typedef uint16_t	pixel_t;

	/*RGB565 ��ɫת��*/
	/* ��8λR,G,Bת��Ϊ 16λRGB565��ʽ */
	#define RGB(R,G,B)	(((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3))	
	#define RGB565_R(x)  ((x >> 8) & 0xF8)
	#define RGB565_G(x)  ((x >> 3) & 0xFC)
	#define RGB565_B(x)  ((x << 3) & 0xF8)

#endif

/* ������ɫ�� */
enum
{
	CL_WHITE    = RGB(255,255,255),	/* ��ɫ */
	CL_BLACK    = RGB(  0,  0,  0),	/* ��ɫ */
	CL_RED      = RGB(255,	0,  0),	/* ��ɫ */
	CL_GREEN    = RGB(  0,255,  0),	/* ��ɫ */
	CL_BLUE     = RGB(  0,	0,255),	/* ��ɫ */
	CL_YELLOW   = RGB(255,255,  0),	/* ��ɫ */

	CL_GREY    = RGB( 98, 98, 98), 	/* ���ɫ */
	CL_GREY1		= RGB( 150, 150, 150), 	/* ǳ��ɫ */
	CL_GREY2		= RGB( 180, 180, 180), 	/* ǳ��ɫ */
	CL_GREY3		= RGB( 200, 200, 200), 	/* ��ǳ��ɫ */
	CL_GREY4		= RGB( 230, 230, 230), 	/* ��ǳ��ɫ */

	CL_BUTTON_GREY	= RGB( 220, 220, 220), /* WINDOWS ��ť�����ɫ */

	CL_MAGENTA      = RGB(255, 0, 255),	/* ����ɫ�����ɫ */
	CL_CYAN         = RGB( 0, 255, 255),	/* ����ɫ����ɫ */

	CL_BLUE1        = RGB(  0,  0, 240),		/* ����ɫ */
	CL_BLUE2        = RGB(  0,  0, 128),		/* ����ɫ */
	CL_BLUE3        = RGB(  68, 68, 255),		/* ǳ��ɫ1 */
	CL_BLUE4        = RGB(  0, 64, 128),		/* ǳ��ɫ1 */

	/* UI ���� Windows�ؼ�����ɫ */
	CL_BTN_FACE		  = RGB(236, 233, 216),	/* ��ť������ɫ(��) */
	CL_BOX_BORDER1	= RGB(172, 168,153),	/* �����������ɫ */
	CL_BOX_BORDER2	= RGB(255, 255,255),	/* �������Ӱ����ɫ */

	CL_MASK			    = 0x7FFF	/* RGB565��ɫ���룬�������ֱ���͸�� */
};

/*******************************************************************************
 * ȫ�ֱ�������
 ******************************************************************************/
extern volatile bool s_frameDone;
extern pixel_t s_psBufferLcd[2][LCD_PIXEL_HEIGHT][LCD_PIXEL_WIDTH];

/*******************************************************************************
 * ��������
 ******************************************************************************/
void LCD_BackLight_ON(void);
void LCD_Init(bool enableInterrupt);
void LCD_InterruptConfig(void);

void LCD_SetColors(pixel_t TextColor, pixel_t BackColor); 
void LCD_GetColors(pixel_t *TextColor, pixel_t *BackColor);
void LCD_SetTextColor(pixel_t Color);
void LCD_SetBackColor(pixel_t Color);
void LCD_SetFont(sFONT *fonts);
sFONT *LCD_GetFont(void);
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, char Ascii);
void LCD_DispString(uint16_t Xpos, uint16_t Ypos, const uint8_t * pStr );
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr);
void LCD_ClearLine(uint16_t Line);
uint32_t LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);


void LCD_SetFrameBuffer(uint8_t index);
void LCD_SetDisplayBuffer(uint8_t index);
void PutPixel(uint16_t Xpos, uint16_t Ypos);
void LCD_Clear(uint32_t Color);
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, bool Direction);
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void LCD_DrawUniLine( uint16_t Xpos1, uint16_t Ypos1, uint16_t Xpos2, uint16_t Ypos2 );
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);




#endif /* __BSP_LCD_H */
