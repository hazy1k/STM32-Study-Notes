#ifndef __FONT_H
#define __FONT_H       

#include "stm32f10x.h"
#include "./font/fonts.h"

#define LINE(x) ((x) * (((sFONT *)LCD_GetFont())->Height)) // 计算行偏移
#define LINEY(x) ((x) * (((sFONT *)LCD_GetFont())->Width)) // 计算列偏移

// 定义字体结构体
typedef struct _tFont
{    
  const uint8_t *table; // 字库指针
  uint16_t Width; // 字库宽度
  uint16_t Height; // 字库高度
}sFONT;

extern sFONT Font24x32;
extern sFONT Font16x24;
extern sFONT Font8x16;

#define      GetGBKCode(ucBuffer, usChar) 

#endif 