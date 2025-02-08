#ifndef __LCD_H
#define	__LCD_H

#include "stm32f10x.h"
#include "fonts.h"

// 根据液晶扫描方向而变化的XY像素宽度
// 调用ILI9341_GramScan函数设置方向时会自动更改
extern uint16_t LCD_X_LENGTH,LCD_Y_LENGTH; 
// 液晶屏扫描模式
// 参数可选值为0-7
extern uint8_t LCD_SCAN_MODE;

/*选择BANK1-BORSRAM1 连接 TFT，地址范围为0X6000 0000 ~ 0X63FF FFFF
FSMC_A16 接LCD的DC(寄存器/数据选择)脚
寄存器基地址 = 0X60000000
RAM基地址 = 0X60020000 = 0X60000000+2^16*2 = 0X60000000 + 0X20000 = 0X60020000*/

// FSMC_Bank1_NORSRAM用于LCD命令操作的地址
#define FSMC_Addr_ST7789V_CMD  ((uint32_t)0x60000000)
// FSMC_Bank1_NORSRAM用于LCD数据操作的地址      
#define FSMC_Addr_ST7789V_DATA ((uint32_t)0x60020000)
// 由片选引脚决定的NOR/SRAM块
#define FSMC_Bank1_NORSRAMx FSMC_Bank1_NORSRAM1
/* 控制引脚 */
// 片选，选择NOR/SRAM块
#define ST7789V_CS_CLK  RCC_APB2Periph_GPIOD // CS->PD7
#define ST7789V_CS_PORT GPIOD
#define ST7789V_CS_PIN  GPIO_Pin_7
// DC引脚，使用FSMC的地址信号控制，本引脚决定了访问LCD时使用的地址
#define ST7789V_DC_CLK  RCC_APB2Periph_GPIOD // DC->PD11
#define ST7789V_DC_PORT GPIOD
#define ST7789V_DC_PIN  GPIO_Pin_11
// 写使能
#define ST7789V_WR_CLK  RCC_APB2Periph_GPIOD // WR->PD5  
#define ST7789V_WR_PORT GPIOD
#define ST7789V_WR_PIN  GPIO_Pin_5
// 读使能
#define ST7789V_RD_CLK  RCC_APB2Periph_GPIOD // RD->PD4     
#define ST7789V_RD_PORT GPIOD
#define ST7789V_RD_PIN  GPIO_Pin_4
// 复位引脚
#define ST7789V_RST_CLK  RCC_APB2Periph_GPIOE // RST->PE1  
#define ST7789V_RST_PORT GPIOE
#define ST7789V_RST_PIN  GPIO_Pin_1
// 背光引脚
#define ST7789V_BK_CLK  RCC_APB2Periph_GPIOD // BK->PD12   
#define ST7789V_BK_PORT GPIOD
#define ST7789V_BK_PIN  GPIO_Pin_12
/* 数据线 */
#define ST7789V_D0_CLK  RCC_APB2Periph_GPIOD // D0->PD14   
#define ST7789V_D0_PORT GPIOD
#define ST7789V_D0_PIN  GPIO_Pin_14
#define ST7789V_D1_CLK  RCC_APB2Periph_GPIOD // D1->PD15   
#define ST7789V_D1_PORT GPIOD
#define ST7789V_D1_PIN  GPIO_Pin_15
#define ST7789V_D2_CLK  RCC_APB2Periph_GPIOD // D2->PD0   
#define ST7789V_D2_PORT GPIOD
#define ST7789V_D2_PIN  GPIO_Pin_0
#define ST7789V_D3_CLK  RCC_APB2Periph_GPIOD // D3->PD1   
#define ST7789V_D3_PORT GPIOD
#define ST7789V_D3_PIN  GPIO_Pin_1
#define ST7789V_D4_CLK  RCC_APB2Periph_GPIOE // D4->PE7   
#define ST7789V_D4_PORT GPIOE
#define ST7789V_D4_PIN  GPIO_Pin_7
#define ST7789V_D5_CLK  RCC_APB2Periph_GPIOE // D5->PE8   
#define ST7789V_D5_PORT GPIOE
#define ST7789V_D5_PIN  GPIO_Pin_8
#define ST7789V_D6_CLK  RCC_APB2Periph_GPIOE // D6->PE9   
#define ST7789V_D6_PORT GPIOE
#define ST7789V_D6_PIN  GPIO_Pin_9
#define ST7789V_D7_CLK  RCC_APB2Periph_GPIOE // D7->PE10  
#define ST7789V_D7_PORT GPIOE
#define ST7789V_D7_PIN  GPIO_Pin_10
#define ST7789V_D8_CLK  RCC_APB2Periph_GPIOE // D8->PE11   
#define ST7789V_D8_PORT GPIOE
#define ST7789V_D8_PIN  GPIO_Pin_11
#define ST7789V_D9_CLK  RCC_APB2Periph_GPIOE // D9->PE12   
#define ST7789V_D9_PORT GPIOE
#define ST7789V_D9_PIN  GPIO_Pin_12
#define ST7789V_D10_CLK  RCC_APB2Periph_GPIOE // D10->PE13  
#define ST7789V_D10_PORT GPIOE
#define ST7789V_D10_PIN  GPIO_Pin_13
#define ST7789V_D11_CLK  RCC_APB2Periph_GPIOE // D11->PE14   
#define ST7789V_D11_PORT GPIOE
#define ST7789V_D11_PIN  GPIO_Pin_14
#define ST7789V_D12_CLK  RCC_APB2Periph_GPIOE // D12->PE15   
#define ST7789V_D12_PORT GPIOE
#define ST7789V_D12_PIN  GPIO_Pin_15
#define ST7789V_D13_CLK  RCC_APB2Periph_GPIOD // D13->PD8   
#define ST7789V_D13_PORT GPIOD
#define ST7789V_D13_PIN  GPIO_Pin_8
#define ST7789V_D14_CLK  RCC_APB2Periph_GPIOD // D14->D9   
#define ST7789V_D14_PORT GPIOD
#define ST7789V_D14_PIN  GPIO_Pin_9
#define ST7789V_D15_CLK  RCC_APB2Periph_GPIOD // D15->PD10   
#define ST7789V_D15_PORT GPIOD
#define ST7789V_D15_PIN  GPIO_Pin_10
/* LCD显示区域 */
#define ST7789V_X_Star   0 // 起始点的X坐标
#define ST7789V_Y_Star   0 // 起始点的Y坐标
#define ST7789V_LESS_PIXEL	240 // 屏较短方向的像素宽度
#define ST7789V_MORE_PIXEL	320 // 屏较长方向的像素宽度
/* 颜色定义 */
#define WHITE   0xFFFF	   
#define BLACK   0x0000	   
#define GREY    0xF7DE	   
#define BLUE    0x001F	   
#define BLUE2   0x051F	    
#define RED     0xF800	    
#define MAGENTA 0xF81F	    
#define GREEN   0x07E0	    
#define CYAN    0x7FFF	   
#define YELLOW  0xFFE0	    
#define BRED    0xF81F
#define GRED    0xFFE0
#define GBLUE   0x07FF
/* ST7789V命令 */
#define CMD_SetCoordinateX 0x2A	// 设置X坐标
#define CMD_SetCoordinateY 0x2B	// 设置Y坐标
#define CMD_SetPixel	   0x2C	// 填充像素
/* 定义 LCD 驱动芯片 ID */
#define LCDID_ST7789V 0x9341
/* 字符显示 */
void ST7789V_DisplayStringEx(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *ptr, uint16_t DrawModel);
void ST7789V_DisplayStringEx_YDir(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *ptr, uint16_t DrawModel);
/* 字模缩放 */
void ST7789V_zoomChar(uint16_t in_width, uint16_t in_heig, uint16_t out_width, uint16_t out_heig, uint8_t *in_ptr, uint8_t *out_ptr, uint8_t en_cn);
void ST7789V_DrawChar_Ex(uint16_t usX, uint16_t usY, uint16_t Font_width, uint16_t Font_Height, uint8_t *c, uint16_t DrawModel);
/* 函数声明 */
void     ST7789V_Init(void);
uint16_t ST7789V_ReadID(void);
void     ST7789V_Rst(void);
void     ST7789V_BackLed_Control(FunctionalState enumState);
void     ST7789V_GramScan(uint8_t ucOtion);
void     ST7789V_OpenWindow(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight);
void     ST7789V_Clear(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight);
void     ST7789V_SetPointPixel(uint16_t usX, uint16_t usY);
uint16_t ST7789V_GetPointPixel(uint16_t usX , uint16_t usY);
void     ST7789V_DrawLine(uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2);
void     ST7789V_DrawRectangle(uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight,uint8_t ucFilled);
void     ST7789V_DrawCircle(uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint8_t ucFilled);
void     ST7789V_DispChar_EN(uint16_t usX, uint16_t usY, const char cChar);
void     ST7789V_DispStringLine_EN(uint16_t line, char* pStr);
void     ST7789V_DispString_EN(uint16_t usX, uint16_t usY, char* pStr);
void 	 ST7789V_DispString_EN_YDir(uint16_t usX,uint16_t usY ,  char* pStr);
void     ST7789V_DispChar_CH(uint16_t usX, uint16_t usY, uint16_t usChar);
void     ST7789V_DispString_CH(uint16_t usX , uint16_t usY, char* pStr);
void     ST7789V_DispStringLine_EN_CH(uint16_t line, char* pStr);
void     ST7789V_DispString_EN_CH(uint16_t usX, uint16_t usY, char* pStr);
void     ST7789V_DispString_EN_CH_YDir(uint16_t usX, uint16_t usY , char * pStr);
void 	 LCD_SetFont(sFONT *fonts);
sFONT   *LCD_GetFont(void);
void 	 LCD_ClearLine(uint16_t Line);
void 	 LCD_SetBackColor(uint16_t Color);
void     LCD_SetTextColor(uint16_t Color)	;
void 	 LCD_SetColors(uint16_t TextColor, uint16_t BackColor);
void 	 LCD_GetColors(uint16_t *TextColor, uint16_t *BackColor);

#endif /* __LCD_H */
