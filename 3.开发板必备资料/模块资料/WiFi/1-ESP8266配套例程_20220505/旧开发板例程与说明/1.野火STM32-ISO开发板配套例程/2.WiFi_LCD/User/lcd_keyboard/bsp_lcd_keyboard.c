#include "bsp_lcd_keyboard.h"
#include <string.h>


void Display_Bmp_Keyboard( ENUM_Keyboard_TypeDef enumE )
{
	char cStr [100];
	u8 uc, uc1, uc2;
	
	switch ( enumE )
	{
		case enumAll:
			Lcd_show_bmp( 0, 0, ( unsigned char * ) "/Keyboard/Keyboard.bmp" );
		  break;
		
		case enumSmallLetter:
			for ( uc = 10; uc < 36; uc ++ )
			{
				uc1 = uc / 10;
				uc2 = uc - uc1 * 10;
				
				sprintf ( cStr, "/Keyboard/%d_Blue.bmp", uc);

				Lcd_show_bmp( uc2 * 32, ( 3 - uc1 ) * 32, ( unsigned char * ) cStr );
			}
		  break;

		case enumCapitalLetter:
			for ( uc = 10; uc < 36; uc ++ )
			{
				uc1 = uc / 10;
				uc2 = uc - uc1 * 10;
				
				sprintf ( cStr, "/Keyboard/%d_Caps_Blue.bmp", uc);

				Lcd_show_bmp( uc2 * 32, ( 3 - uc1 ) * 32, ( unsigned char * ) cStr );
			}
		  break;		
		
		default:
		  break;		
			
	}

}

