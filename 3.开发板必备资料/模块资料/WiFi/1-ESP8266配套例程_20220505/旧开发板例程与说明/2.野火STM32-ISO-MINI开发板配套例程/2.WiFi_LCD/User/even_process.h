#ifndef __EVEN_PROCESS_H
#define __EVEN_PROCESS_H


#include "stm32f10x.h"
#include <stdio.h>
#include <stdio.h>
#include <stdbool.h>

#include "bsp_ili9341_lcd.h"


#if         0
#define     macDebugPrintf( arg... )      printf ( arg )
#else
#define     macDebugPrintf( arg... )      __NOP ()
#endif

#define     macMaxLength_In_Keyboard      53    //COLUMN(320)/STR_WIDTH(6)=53
#define     macY_LcdDis_StrInput          96    //99

#define     macClear_DisplayFrame()       LCD_Clear ( 0, 0, COLUMN, PAGE, D_YELLOW )
#define     macClr_LcdDis_StrInput()      LCD_Clear ( 0, macY_LcdDis_StrInput, COLUMN, STR_HEIGHT, D_YELLOW )


char *      touch_process                 ( void );
static void funColorSwitch_Keyboard ( u8 ucRow_Keyboard, u8 ucColumn_Keyboard, bool bCapsFlag );


__inline char * inWaiteInput ( void )
{
	char * pCh;
	
	do {
		pCh = touch_process();
	} while ( ! pCh );
	
	return pCh;
	
}


#endif


