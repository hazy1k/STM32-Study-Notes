#ifndef __BSP_LCD_KEYBOARD_H
#define __BSP_LCD_KEYBOARD_H


#include "bsp_bmp.h"
#include "ff.h"
#include "bsp_ili9341_lcd.h"


typedef enum {
	enumAll,
	enumSmallLetter,
	enumCapitalLetter
} ENUM_Keyboard_TypeDef;


void                  Display_Bmp_Keyboard                      ( ENUM_Keyboard_TypeDef enumE );


#endif


