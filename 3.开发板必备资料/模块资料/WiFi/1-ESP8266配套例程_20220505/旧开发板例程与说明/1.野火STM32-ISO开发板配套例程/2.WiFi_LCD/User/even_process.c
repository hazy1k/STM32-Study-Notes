#include "even_process.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_touch.h"
#include "bsp_bmp.h"
#include <stdio.h>
#include "bsp_SysTick.h"
#include <stdbool.h>
#include "bsp_lcd_keyboard.h"


/**
  * @brief  funColorSwitch_Keyboard：LCD按键变色函数
  * @param  ucRow_Keyboard：按键在键盘的行位置
	* @param  ucColumn_Keyboard：按键在键盘的列位置
	* @param  bCapsFlag：是否已锁定字母大写
  * @retval 无
  */
static void funColorSwitch_Keyboard ( u8 ucRow_Keyboard, u8 ucColumn_Keyboard, bool bCapsFlag )
{
	static u16 usKeyColor_Lcd [4] = { 0, 0, 0, 0 };
	char cStr [50];
	u8 ucNo;
	
	ucNo = ucRow_Keyboard * 10 + ucColumn_Keyboard;
	
	usKeyColor_Lcd [ ucRow_Keyboard ] ^= ( 1 << ucColumn_Keyboard );
	
	if ( ( ucNo < 10 ) || ( ucNo > 35 ) ) 
	{
		if ( ucNo == 37 )
		{
			usKeyColor_Lcd [ 1 ] &= ( ~ 0x03FF ); 
			usKeyColor_Lcd [ 2 ] &= ( ~ 0x03FF );  
			usKeyColor_Lcd [ 3 ] &= ( ~ 0x003F );  
		}
		
		sprintf ( cStr, "/Keyboard/%d_%s.bmp", ucRow_Keyboard * 10 + ucColumn_Keyboard, usKeyColor_Lcd [ ucRow_Keyboard ] & ( 1 << ucColumn_Keyboard ) ? "Pink":"Blue" );
	}
	
	else if ( 10 <= ucNo <= 35 )
	  sprintf ( cStr, "/Keyboard/%d%s_%s.bmp", ucRow_Keyboard * 10 + ucColumn_Keyboard, bCapsFlag ? "_Caps" : "\0", usKeyColor_Lcd [ ucRow_Keyboard ] & ( 1 << ucColumn_Keyboard ) ? "Pink":"Blue" );
	
	Lcd_show_bmp( ucColumn_Keyboard * 32, ( 3 - ucRow_Keyboard ) * 32, ( unsigned char * ) cStr );
	Lcd_GramScan ( 2 );	
	
}


/**
  * @brief  touch_process触控处理函数，根据触摸位置更新触摸标志
  * @param  无  
  * @retval 无
  */
char * touch_process( void )
{
	static struct {
		char cStrInput [ macMaxLength_In_Keyboard ];
	  u8 ucStrLength;
		bool bCapsFlag;
	} strKeyboard = { {0}, 0, 0 };

  u8 ucRow_Keyboard, ucColumn_Keyboard;
	char cCh;
	char * pRet = NULL;

  if ( ( touch_flag == 1 ) && ( Get_touch_point(&display, Read_2046_2(), &touch_para ) != DISABLE ) && ( 112 < display.y < 240 ) )
	{
		ucRow_Keyboard    = ( display.y - 112 ) / 32;
		ucColumn_Keyboard = display.x / 32;
		
		if ( ucRow_Keyboard == 0 )
		{
			cCh = '0' + ucColumn_Keyboard;
			macDebugPrintf ( "%c ", cCh );//
			if ( strKeyboard .ucStrLength < macMaxLength_In_Keyboard ) {
				LCD_DispChar( strKeyboard .ucStrLength * STR_WIDTH, macY_LcdDis_StrInput, cCh, BLUE );
				strKeyboard .cStrInput [ strKeyboard .ucStrLength ++ ] = cCh;
		  }
			
		}
		
		else if ( ucRow_Keyboard == 1 )
		{
			cCh = ( strKeyboard .bCapsFlag ? 'A' : 'a' ) + ucColumn_Keyboard;
			macDebugPrintf ( "%c ", cCh );//
			if ( strKeyboard .ucStrLength < macMaxLength_In_Keyboard ) {
				LCD_DispChar( strKeyboard .ucStrLength * STR_WIDTH, macY_LcdDis_StrInput, cCh, BLUE );
				strKeyboard .cStrInput [ strKeyboard .ucStrLength ++ ] = cCh;
			}
			
		}
		
		else if ( ucRow_Keyboard == 2 )
		{
			cCh = ( strKeyboard .bCapsFlag ? 'K' : 'k' ) + ucColumn_Keyboard;
			macDebugPrintf ( "%c ", cCh );//
			if ( strKeyboard .ucStrLength < macMaxLength_In_Keyboard ) {
				LCD_DispChar( strKeyboard .ucStrLength * STR_WIDTH, macY_LcdDis_StrInput, cCh, BLUE );
				strKeyboard .cStrInput [ strKeyboard .ucStrLength ++ ] = cCh;
			}
			
		}
		
		else if ( ucRow_Keyboard == 3 )
		{
			if ( ucColumn_Keyboard < 6 ) {
				cCh = ( strKeyboard .bCapsFlag ? 'U' : 'u' ) + ucColumn_Keyboard;
				macDebugPrintf ( "%c ", cCh );//
				if ( strKeyboard .ucStrLength < macMaxLength_In_Keyboard ) {
					LCD_DispChar( strKeyboard .ucStrLength * STR_WIDTH, macY_LcdDis_StrInput, cCh, BLUE );
					strKeyboard .cStrInput [ strKeyboard .ucStrLength ++ ] = cCh;
				}
			}
			
			else if ( ucColumn_Keyboard == 6 ) {
				cCh = '.';
				macDebugPrintf ( "%c ", cCh );//
				if ( strKeyboard .ucStrLength < macMaxLength_In_Keyboard ) {
					LCD_DispChar( strKeyboard .ucStrLength * STR_WIDTH, macY_LcdDis_StrInput, cCh, BLUE );
					strKeyboard .cStrInput [ strKeyboard .ucStrLength ++ ] = cCh;
			  }
			}
			
			else if ( ucColumn_Keyboard == 7 ) {
				strKeyboard .bCapsFlag = ! strKeyboard .bCapsFlag;
				macDebugPrintf ( "%d ", strKeyboard .bCapsFlag );//
				if ( strKeyboard .bCapsFlag ) Display_Bmp_Keyboard ( enumCapitalLetter );//Display_Bmp_CapitalLetter ();
				else Display_Bmp_Keyboard ( enumSmallLetter );//Display_Bmp_SmallLetter ();
				Lcd_GramScan ( 2 );
			}
			
			else if ( ucColumn_Keyboard == 8 ) {
				macDebugPrintf ( "\b " );//
				if ( strKeyboard .ucStrLength > 0 ){
					strKeyboard .cStrInput [ -- strKeyboard .ucStrLength ] = '\0';
					LCD_DispChar( strKeyboard .ucStrLength * STR_WIDTH, macY_LcdDis_StrInput, ' ', BLUE );
			  }
			}
			
			else if ( ucColumn_Keyboard == 9 ) {
				macDebugPrintf ( "En " );//
				strKeyboard .cStrInput [ strKeyboard .ucStrLength ] = '\0';
				macDebugPrintf ( "%s", strKeyboard .cStrInput );//
				strKeyboard .ucStrLength = 0;
				pRet = strKeyboard .cStrInput;
			}
			
		}
		
		funColorSwitch_Keyboard ( ucRow_Keyboard, ucColumn_Keyboard, strKeyboard .bCapsFlag );
		
		Delay_ms ( 500 );
		
		touch_flag = 0;
		
	}
	
	return pRet;
	
}
/* ----------------------------------------end of file --------------------------------------------*/

