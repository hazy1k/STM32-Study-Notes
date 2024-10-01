
#include	"BSP.h"
#include	"emXGUI.h"

#include	"GUI_Drv.h"

#include 	"stm32f10x_rcc.h"
#include 	"stm32f10x_gpio.h"
#include 	"stm32f10x_fsmc.h"
#include  "bsp_ov7725.h"//为摄像头数据显示引入
/*=========================================================================================*/
/*
 * 野火STM32F103 3.2寸LCD (ILI9341)
 */

#define	LCD_CMD_ADDR	( ( uint32_t ) 0x60000000 )
#define	LCD_DAT_ADDR	( ( uint32_t ) 0x60020000 )

#define	LCD_CMD	(*(volatile u16*)(LCD_CMD_ADDR))
#define	LCD_DAT	(*(volatile u16*)(LCD_DAT_ADDR))


/*=========================================================================================*/
void ILI9341_OpenWindow ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight );

static void delay_ms(unsigned int t)
{
	volatile int i;
	while(t-- > 0)
	{
		for(i=200;i>0;i--);
	}
}

static	INLINE void write_cmd(u8 a)
{
	LCD_CMD =a;
}

static	INLINE void write_dat(u8 a)
{
 	LCD_DAT =a;
}

/*=========================================================================================*/

#define	__lcd_write_cmd(cmd)    LCD_CMD =cmd
#define	__lcd_write_dat(dat) 	LCD_DAT =dat
#define	__lcd_read_dat()		LCD_DAT

#define	__lcd_map_rgb(r,g,b)	RGB565(r>>3,g>>2,b>>3)


void __lcd_write_start(u16 sx,u16 sy,u16 ex,u16 ey)
{
	sx += 27;
  ex += 27;
	__lcd_write_cmd(0x2A);
	__lcd_write_dat(sx>>8);
	__lcd_write_dat(sx);
	__lcd_write_dat(ex>>8);
	__lcd_write_dat(ex);

	__lcd_write_cmd(0x2B);
	__lcd_write_dat(sy>>8);
	__lcd_write_dat(sy);
	__lcd_write_dat(ey>>8);
	__lcd_write_dat(ey);

	__lcd_write_cmd(0x2C);
}

void __lcd_read_start(u16 sx,u16 sy,u16 ex,u16 ey)
{
	volatile u16 c;

	__lcd_write_cmd(0x2A);
	__lcd_write_dat(sx>>8);
	__lcd_write_dat(sx);
	__lcd_write_dat(ex>>8);
	__lcd_write_dat(ex);

	__lcd_write_cmd(0x2B);
	__lcd_write_dat(sy>>8);
	__lcd_write_dat(sy);
	__lcd_write_dat(ey>>8);
	__lcd_write_dat(ey);

	__lcd_write_cmd(0x2E);
	c =__lcd_read_dat(); //DummyRead
	for(c=0;c<2;c++);
}

void __lcd_write_rgb(U8 r,U8 g,U8 b)
{
	__lcd_write_dat(__lcd_map_rgb(r,g,b));
}

u16 __lcd_read_pixel(void)
{

	u16 c1,c2;

	c1 =__lcd_read_dat(); //RG
	c2 =__lcd_read_dat(); //BX

	return	__lcd_map_rgb(c1>>8,c1,c2>>8);
}

void __lcd_read_rgb(u8 *r,u8 *g,u8 *b)
{
	u16 c1,c2;

	c1 =__lcd_read_dat(); //RG
	c2 =__lcd_read_dat(); //BX

	*r =c1>>8;
	*g =c1;
	*b =c2>>8;
}

/*=========================================================================================*/

BOOL LCD_SetRotate(int rotate)
{

	switch(rotate)
	{
		case ROTATE_0:
			write_cmd(0x36); // Memory Access Control
			write_dat((0<<7)|(1<<6)|(0<<5)|(0<<3)|(0<<2)|(1<<1));	// 0
			return TRUE;
			////

		case ROTATE_90:
			write_cmd(0x36); // Memory Access Control
			write_dat((0<<7)|(0<<6)|(1<<5)|(0<<3)|(0<<2)|(1<<1));	// 90
			return TRUE;
			////

		case ROTATE_180:
			write_cmd(0x36); // Memory Access Control
			write_dat((1<<7)|(0<<6)|(0<<5)|(0<<3)|(0<<2)|(1<<1));	// 180
			return TRUE;
			////

		case ROTATE_270:
			write_cmd(0x36); // Memory Access Control
			write_dat((1<<7)|(1<<6)|(1<<5)|(0<<3)|(0<<2)|(1<<1));	// 270
			return TRUE;
			////

		default:
			return FALSE;

	}
}

/*=========================================================================================*/


static void DEBUG_DELAY(void)
{
	GUI_msleep(50);
}
#define	ILI9341_Write_Cmd write_cmd
#define	ILI9341_Write_Data	write_dat
#define	ILI9806G_Write_Cmd write_cmd
#define	ILI9806G_Write_Data	write_dat
  
/******************************* ¶¨Òå ILI934 ³£ÓÃÃüÁî ********************************/
#define      macCMD_SetCoordinateX		 		    0x2A	     //ÉèÖÃX×ø±ê
#define      macCMD_SetCoordinateY		 		    0x2B	     //ÉèÖÃY×ø±ê
#define      macCMD_SetPixel		 		        0x2C	     //Ìî³äÏñËØ
static void ILI9341_Init ( void )
{	
	//液晶厂商提供了两种版本的屏幕，性能一样，它们的驱动配置稍有区别，
	//本驱动通过#if #else #endif来设置，若屏幕显示花屏，请把#if 0改成#if 1，或1改成0
	//在2017-12-14日后购买的产品，使用#if 0
#if 0
	//旧版
	/* EXTC Command Set enable register */
	DEBUG_DELAY  ();
	ILI9806G_Write_Cmd ( 0xFF  );
	ILI9806G_Write_Data ( 0xFF  );
	ILI9806G_Write_Data ( 0x98  );
	ILI9806G_Write_Data ( 0x06  );

	/* GIP 1(BCh)  */
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd(0xBC);
	ILI9806G_Write_Data(0x01); 
	ILI9806G_Write_Data(0x0E); 
	ILI9806G_Write_Data(0x61); 
	ILI9806G_Write_Data(0xFB); 
	ILI9806G_Write_Data(0x10); 
	ILI9806G_Write_Data(0x10); 
	ILI9806G_Write_Data(0x0B); 
	ILI9806G_Write_Data(0x0F); 
	ILI9806G_Write_Data(0x2E); 
	ILI9806G_Write_Data(0x73); 
	ILI9806G_Write_Data(0xFF); 
	ILI9806G_Write_Data(0xFF); 
	ILI9806G_Write_Data(0x0E); 
	ILI9806G_Write_Data(0x0E); 
	ILI9806G_Write_Data(0x00); 
	ILI9806G_Write_Data(0x03); 
	ILI9806G_Write_Data(0x66); 
	ILI9806G_Write_Data(0x63); 
	ILI9806G_Write_Data(0x01); 
	ILI9806G_Write_Data(0x00); 
	ILI9806G_Write_Data(0x00);

	/* GIP 2 (BDh) */
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd(0xBD);
	ILI9806G_Write_Data(0x01); 
	ILI9806G_Write_Data(0x23); 
	ILI9806G_Write_Data(0x45); 
	ILI9806G_Write_Data(0x67); 
	ILI9806G_Write_Data(0x01); 
	ILI9806G_Write_Data(0x23); 
	ILI9806G_Write_Data(0x45); 
	ILI9806G_Write_Data(0x67); 

	/* GIP 3 (BEh) */
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd(0xBE);
	ILI9806G_Write_Data(0x00); 
	ILI9806G_Write_Data(0x21); 
	ILI9806G_Write_Data(0xAB); 
	ILI9806G_Write_Data(0x60); 
	ILI9806G_Write_Data(0x22); 
	ILI9806G_Write_Data(0x22); 
	ILI9806G_Write_Data(0x22); 
	ILI9806G_Write_Data(0x22); 
	ILI9806G_Write_Data(0x22); 

	/* Vcom  (C7h) */
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd ( 0xC7 );
	ILI9806G_Write_Data ( 0x6F );

	/* EN_volt_reg (EDh)*/
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd ( 0xED );
	ILI9806G_Write_Data ( 0x7F );
	ILI9806G_Write_Data ( 0x0F );
	ILI9806G_Write_Data ( 0x00 );

	/* Power Control 1 (C0h) */
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd ( 0xC0 );
	ILI9806G_Write_Data ( 0x37 );
	ILI9806G_Write_Data ( 0x0B );
	ILI9806G_Write_Data ( 0x0A );

	/* LVGL (FCh) */
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd ( 0xFC );
	ILI9806G_Write_Data ( 0x0A );

	/* Engineering Setting (DFh) */
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd ( 0xDF );
	ILI9806G_Write_Data ( 0x00 );
	ILI9806G_Write_Data ( 0x00 );
	ILI9806G_Write_Data ( 0x00 );
	ILI9806G_Write_Data ( 0x00 );
	ILI9806G_Write_Data ( 0x00 );
	ILI9806G_Write_Data ( 0x20 );

	/* DVDD Voltage Setting(F3h) */
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd ( 0xF3 );
	ILI9806G_Write_Data ( 0x74 );

	/* Display Inversion Control (B4h) */
	ILI9806G_Write_Cmd ( 0xB4 );
	ILI9806G_Write_Data ( 0x00 );
	ILI9806G_Write_Data ( 0x00 );
	ILI9806G_Write_Data ( 0x00 );

	/* 480x854 (F7h)  */
	ILI9806G_Write_Cmd ( 0xF7 );
	ILI9806G_Write_Data ( 0x89 );

	/* Frame Rate (B1h) */
	ILI9806G_Write_Cmd ( 0xB1 );
	ILI9806G_Write_Data ( 0x00 );
	ILI9806G_Write_Data ( 0x12 );
	ILI9806G_Write_Data ( 0x10 );

	/* Panel Timing Control (F2h) */
	ILI9806G_Write_Cmd ( 0xF2 );
	ILI9806G_Write_Data ( 0x80 );
	ILI9806G_Write_Data ( 0x5B );
	ILI9806G_Write_Data ( 0x40 );
	ILI9806G_Write_Data ( 0x28 );

	DEBUG_DELAY ();

	/* Power Control 2 (C1h) */
	ILI9806G_Write_Cmd ( 0xC1 ); 
	ILI9806G_Write_Data ( 0x17 );
	ILI9806G_Write_Data ( 0x7D );
	ILI9806G_Write_Data ( 0x7A );
	ILI9806G_Write_Data ( 0x20 );

	DEBUG_DELAY ();

	ILI9806G_Write_Cmd(0xE0); 
	ILI9806G_Write_Data(0x00); //P1 
	ILI9806G_Write_Data(0x11); //P2 
	ILI9806G_Write_Data(0x1C); //P3 
	ILI9806G_Write_Data(0x0E); //P4 
	ILI9806G_Write_Data(0x0F); //P5 
	ILI9806G_Write_Data(0x0C); //P6 
	ILI9806G_Write_Data(0xC7); //P7 
	ILI9806G_Write_Data(0x06); //P8 
	ILI9806G_Write_Data(0x06); //P9 
	ILI9806G_Write_Data(0x0A); //P10 
	ILI9806G_Write_Data(0x10); //P11 
	ILI9806G_Write_Data(0x12); //P12 
	ILI9806G_Write_Data(0x0A); //P13 
	ILI9806G_Write_Data(0x10); //P14 
	ILI9806G_Write_Data(0x02); //P15 
	ILI9806G_Write_Data(0x00); //P16 

	DEBUG_DELAY ();

	ILI9806G_Write_Cmd(0xE1); 
	ILI9806G_Write_Data(0x00); //P1 
	ILI9806G_Write_Data(0x12); //P2 
	ILI9806G_Write_Data(0x18); //P3 
	ILI9806G_Write_Data(0x0C); //P4 
	ILI9806G_Write_Data(0x0F); //P5 
	ILI9806G_Write_Data(0x0A); //P6 
	ILI9806G_Write_Data(0x77); //P7 
	ILI9806G_Write_Data(0x06); //P8 
	ILI9806G_Write_Data(0x07); //P9 
	ILI9806G_Write_Data(0x0A); //P10 
	ILI9806G_Write_Data(0x0E); //P11 
	ILI9806G_Write_Data(0x0B); //P12 
	ILI9806G_Write_Data(0x10); //P13 
	ILI9806G_Write_Data(0x1D); //P14 
	ILI9806G_Write_Data(0x17); //P15 
	ILI9806G_Write_Data(0x00); //P16  

	/* Tearing Effect ON (35h)  */
	ILI9806G_Write_Cmd ( 0x35 );
	ILI9806G_Write_Data ( 0x00 );

	ILI9806G_Write_Cmd ( 0x3A );
	ILI9806G_Write_Data ( 0x55 );

	ILI9806G_Write_Cmd ( 0x11 );
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd ( 0x29 );
	
#else
	//新版
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xFF);
	ILI9806G_Write_Data(0xFF);
	ILI9806G_Write_Data(0x98);
	ILI9806G_Write_Data(0x06);
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xBA);
	ILI9806G_Write_Data(0x60);  
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xBC);
	ILI9806G_Write_Data(0x03);
	ILI9806G_Write_Data(0x0E);
	ILI9806G_Write_Data(0x61);
	ILI9806G_Write_Data(0xFF);
	ILI9806G_Write_Data(0x05);
	ILI9806G_Write_Data(0x05);
	ILI9806G_Write_Data(0x1B);
	ILI9806G_Write_Data(0x10);
	ILI9806G_Write_Data(0x73);
	ILI9806G_Write_Data(0x63);
	ILI9806G_Write_Data(0xFF);
	ILI9806G_Write_Data(0xFF);
	ILI9806G_Write_Data(0x05);
	ILI9806G_Write_Data(0x05);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0xD5);
	ILI9806G_Write_Data(0xD0);
	ILI9806G_Write_Data(0x01);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x40); 
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xBD);
	ILI9806G_Write_Data(0x01);
	ILI9806G_Write_Data(0x23);
	ILI9806G_Write_Data(0x45);
	ILI9806G_Write_Data(0x67);
	ILI9806G_Write_Data(0x01);
	ILI9806G_Write_Data(0x23);
	ILI9806G_Write_Data(0x45);
	ILI9806G_Write_Data(0x67);  
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xBE);
	ILI9806G_Write_Data(0x01);
	ILI9806G_Write_Data(0x2D);
	ILI9806G_Write_Data(0xCB);
	ILI9806G_Write_Data(0xA2);
	ILI9806G_Write_Data(0x62);
	ILI9806G_Write_Data(0xF2);
	ILI9806G_Write_Data(0xE2);
	ILI9806G_Write_Data(0x22);
	ILI9806G_Write_Data(0x22);
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xC7);
	ILI9806G_Write_Data(0x63); 
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xED);
	ILI9806G_Write_Data(0x7F);
	ILI9806G_Write_Data(0x0F);
	ILI9806G_Write_Data(0x00);
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xC0);
	ILI9806G_Write_Data(0x03);
	ILI9806G_Write_Data(0x0B);
	ILI9806G_Write_Data(0x00);   
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xFC);
	ILI9806G_Write_Data(0x08); 
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xDF);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x20);
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xF3);
	ILI9806G_Write_Data(0x74);
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xF9);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0xFD);
	ILI9806G_Write_Data(0x80);
	ILI9806G_Write_Data(0x80);
	ILI9806G_Write_Data(0xC0);
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xB4);
	ILI9806G_Write_Data(0x02);
	ILI9806G_Write_Data(0x02);
	ILI9806G_Write_Data(0x02); 
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xF7);
	ILI9806G_Write_Data(0x81);
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xB1);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x13);
	ILI9806G_Write_Data(0x13); 
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xF2);
	ILI9806G_Write_Data(0xC0);
	ILI9806G_Write_Data(0x02);
	ILI9806G_Write_Data(0x40);
	ILI9806G_Write_Data(0x28);  
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xC1);
	ILI9806G_Write_Data(0x17);
	ILI9806G_Write_Data(0x75);
	ILI9806G_Write_Data(0x75);
	ILI9806G_Write_Data(0x20); 
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xE0);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x05);
	ILI9806G_Write_Data(0x08);
	ILI9806G_Write_Data(0x0C);
	ILI9806G_Write_Data(0x0F);
	ILI9806G_Write_Data(0x15);
	ILI9806G_Write_Data(0x09);
	ILI9806G_Write_Data(0x07);
	ILI9806G_Write_Data(0x01);
	ILI9806G_Write_Data(0x06);
	ILI9806G_Write_Data(0x09);
	ILI9806G_Write_Data(0x16);
	ILI9806G_Write_Data(0x14);
	ILI9806G_Write_Data(0x3E);
	ILI9806G_Write_Data(0x3E);
	ILI9806G_Write_Data(0x00);
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xE1);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x09);
	ILI9806G_Write_Data(0x12);
	ILI9806G_Write_Data(0x12);
	ILI9806G_Write_Data(0x13);
	ILI9806G_Write_Data(0x1c);
	ILI9806G_Write_Data(0x07);
	ILI9806G_Write_Data(0x08);
	ILI9806G_Write_Data(0x05);
	ILI9806G_Write_Data(0x08);
	ILI9806G_Write_Data(0x03);
	ILI9806G_Write_Data(0x02);
	ILI9806G_Write_Data(0x04);
	ILI9806G_Write_Data(0x1E);
	ILI9806G_Write_Data(0x1B);
	ILI9806G_Write_Data(0x00);
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0x3A);
	ILI9806G_Write_Data(0x55); 
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0x35);
	ILI9806G_Write_Data(0x00); 
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0x11);
	DEBUG_DELAY() ;
	ILI9806G_Write_Cmd(0x29);	   
	DEBUG_DELAY()  ; 
#endif	
	LCD_SetRotate(ROTATE_90);
}

/******************************* 定义 ILI934 常用命令 ********************************/
#define      CMD_SetCoordinateX		 		    0x2A	     //设置X坐标
#define      CMD_SetCoordinateY		 		    0x2B	     //设置Y坐标
#define      CMD_SetPixel		 		          0X2C	     //填充像素

/**
 * @brief  在LCD显示器上以某一颜色填充像素点
 * @param  ulAmout_Point ：要填充颜色的像素点的总数目
 * @param  usColor ：颜色
 * @retval 无
 */
static __inline void LCD_FillColor ( uint32_t ulAmout_Point, uint16_t usColor )
{
	uint32_t i = 0;
	
	
	/* memory write */
	write_cmd ( CMD_SetPixel );	
		
	for ( i = 0; i < ulAmout_Point; i ++ )
		write_dat ( usColor );
		
	
}

/**
 * @brief  对LCD显示器的某一窗口以某种颜色进行清屏
 * @param  usX ：在特定扫描方向下窗口的起点X坐标
 * @param  usY ：在特定扫描方向下窗口的起点Y坐标
 * @param  usWidth ：窗口的宽度
 * @param  usHeight ：窗口的高度
 * @note 可使用LCD_SetBackColor、LCD_SetTextColor、LCD_SetColors函数设置颜色
 * @retval 无
 */
void LCD_Clear ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight )
{
	ILI9341_OpenWindow ( usX, usY, usWidth, usHeight );

	LCD_FillColor ( usWidth * usHeight, 0 );		
	
}

/*=========================================================================================*/

#if 1

#define		EXT_LCD_ARCH
//#define		EXT_LCD_DMA
#include 	"EXT_LCD_Arch.c"
//#ifdef	EXT_LCD_ARCH

#endif

#define	LCD_RST_PIN	GPIOE,GPIO_Pin_1
#define	LCD_BL_PIN	GPIOD,GPIO_Pin_12

/*=========================================================================================*/


void	LCD_Init(void)
{
  GPIO_InitTypeDef gpio_init;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE);

	gpio_init.GPIO_Mode  = GPIO_Mode_Out_PP; //复用输出
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz; //50MHz

	//PG6 - LCD BackLight: 0:ON; 1:OFF
	gpio_init.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOD, &gpio_init); //初始化

	//PG11 - LCD_nRST:
	gpio_init.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOE, &gpio_init); //初始化

	GPIO_SetBits(LCD_RST_PIN);
	GUI_msleep(50);
	GPIO_ResetBits(LCD_RST_PIN);
	GUI_msleep(50);
	GPIO_SetBits(LCD_RST_PIN);
	GUI_msleep(50);
  
	ILI9341_Init();
  LCD_Clear(0, 0, 854, 480);

#ifdef	EXT_LCD_DMA
	LCD_DMA_Init(LCD_DAT_ADDR);
#endif

}

void LCD_BackLed_Control(int on)
{
	if(on)
	{
		GPIO_SetBits( LCD_BL_PIN );
	}
	else
	{
		GPIO_ResetBits(LCD_BL_PIN);
	}
}

/**
 * @brief  在ILI9341显示器上开辟一个窗口
 * @param  usX ：在特定扫描方向下窗口的起点X坐标
 * @param  usY ：在特定扫描方向下窗口的起点Y坐标
 * @param  usWidth ：窗口的宽度
 * @param  usHeight ：窗口的高度
 * @retval 无
 */
void ILI9341_OpenWindow ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight )
{	
	ILI9341_Write_Cmd ( 0x2A ); 				 /* 设置X坐标 */
	ILI9341_Write_Data ( usX >> 8  );	 /* 先高8位，然后低8位 */
	ILI9341_Write_Data ( usX & 0xff  );	 /* 设置起始点和结束点*/
	ILI9341_Write_Data ( ( usX + usWidth - 1 ) >> 8  );
	ILI9341_Write_Data ( ( usX + usWidth - 1 ) & 0xff  );

	ILI9341_Write_Cmd ( 0x2B ); 			     /* 设置Y坐标*/
	ILI9341_Write_Data ( usY >> 8  );
	ILI9341_Write_Data ( usY & 0xff  );
	ILI9341_Write_Data ( ( usY + usHeight - 1 ) >> 8 );
	ILI9341_Write_Data ( ( usY + usHeight - 1) & 0xff );
	
}

/**
  * @brief  向ILI9341写入数据,16位数据版本
  * @param  usData :要写入的数据
  * @retval 无
  */	
 void ILI9341_Write_Data_V2 ( uint16_t usData )
{
	* ( __IO uint16_t * ) ( LCD_DAT_ADDR ) = usData;
}

/**
  * @brief  
	* @param  sx:x起始显示位置
	* @param  sy:y起始显示位置
	* @param  width:显示窗口宽度,要求跟OV7725_Window_Set函数中的width一致
	* @param  height:显示窗口高度，要求跟OV7725_Window_Set函数中的height一致
  * @retval 无
  */
void Write_Pixel_From_Buf(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
	uint16_t i, j; 
	uint16_t Camera_Data;
	
	ILI9341_OpenWindow(sx,sy,width,height);
	ILI9341_Write_Cmd ( 0x2C );	//指令:写入像素

	for(i = 0; i < width; i++)
	{
		for(j = 0; j < height; j++)
		{
			READ_FIFO_PIXEL(Camera_Data);
//			GUI_DEBUG("%d\t",Camera_Data);
			/* 从FIFO读出一个rgb565像素到Camera_Data变量 */
			ILI9341_Write_Data_V2(Camera_Data);
		}
	}
}

/*=========================================================================================*/
