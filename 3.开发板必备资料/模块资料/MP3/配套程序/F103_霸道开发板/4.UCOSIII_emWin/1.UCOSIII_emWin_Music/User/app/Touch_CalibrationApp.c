/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2014  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.26 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : TOUCH_CalibrateApp.c
Purpose     : Demonstrates how a touch screen can be calibrated at run time
Requirements: WindowManager - ( )
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
---------------------------END-OF-HEADER------------------------------
*/

#include "includes.h"

extern 	uint8_t IsCal;
/********************************************************************
*
*       Static data
*
*********************************************************************
*/
static const char * _acPos[] = {
  "(upper left position)",
  "(lower right position)"
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _WaitForPressedState
*
* Function description
*   Waits until the touch is in the given pressed state for at least 250 ms
*/
static void _WaitForPressedState(int Pressed) {
  GUI_PID_STATE State;

  do {
    GUI_TOUCH_GetState(&State);
    GUI_Delay(1);
    if (State.Pressed == Pressed) {
      int TimeStart = GUI_GetTime();
      do {
        GUI_TOUCH_GetState(&State);
        GUI_Delay(1);
        if (State.Pressed != Pressed) {
          break;
        } else if ((GUI_GetTime() - 50) > TimeStart) {
          return;
        }
      } while (1);
    }
  } while (1);
}

/*********************************************************************
*
*       _DispStringCentered
*
* Function description
*   Shows the given text horizontally and vertically centered
*/
static void _DispStringCentered(const char * pString) {
  GUI_RECT Rect;

  Rect.x0 = Rect.y0 = 0;
  Rect.x1 = LCD_GetXSize() - 1;
  Rect.y1 = LCD_GetYSize() - 1;
  GUI_DispStringInRect(pString, &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
}

/*********************************************************************
*
*       _GetPhysValues
*
* Function description
*   Asks the user to press the touch screen at the given position
*   and returns the physical A/D values
*/
static void _GetPhysValues(uint16_t LogX, uint16_t LogY, uint16_t * pPhysX, uint16_t * pPhysY, const char * pString) {
  char acText[] = "Press here";
  GUI_RECT Rect;
  int FontSizeY;
  int Align;

  FontSizeY = GUI_GetFontSizeY();
  GUI_Clear();
  GUI_SetColor(GUI_BLACK);
  _DispStringCentered("Runtime calibration,\n"
                      "please touch the screen\n"
                      "at the center of the ring."); /* Ask user to press the touch */
  //
  // Calculate the rectangle for the string
  //
  Rect.y0 = LogY - FontSizeY;
  Rect.y1 = LogY + FontSizeY;
  if (LogX < LCD_GetXSize() / 2) {
    Rect.x0 = LogX + 15;
    Rect.x1 = LCD_GetXSize();
    Align = GUI_TA_LEFT;
  } else {
    Rect.x0 = 0;
    Rect.x1 = LogX - 15;
    Align = GUI_TA_RIGHT;
  }
  //
  // Show the text nearby the ring
  //
  GUI_DispStringInRect(acText, &Rect, Align | GUI_TA_TOP);
  GUI_DispStringInRect(pString, &Rect, Align | GUI_TA_BOTTOM);
  //
  // Draw the ring
  //
  GUI_FillCircle(LogX, LogY, 10);
  GUI_SetColor(GUI_WHITE);
  GUI_FillCircle(LogX, LogY, 5);
  GUI_SetColor(GUI_BLACK);
  //
  // Wait until touch is pressed
  //
  _WaitForPressedState(1);
  *pPhysX =(uint16_t)GUI_TOUCH_GetxPhys();
  *pPhysY =(uint16_t)GUI_TOUCH_GetyPhys();
  //
  // Wait until touch is released
  //
  _WaitForPressedState(0);
}

/********************************************************************
*
*       _Explain
*
* Funtion description
*   Shows a text to give a short explanation of the sample program
*/
static void _Explain(void) {
	uint8_t oneflag=1;
	GUI_PID_STATE State;
	GUI_Clear();
//  _DispStringCentered("This sample shows how\n"
//                      "a touch screen can be\n"
//                      "calibrated at run time.\n"
//                      "Please press the touch\n"
//                      "screen to continue...");
//  GUI_DispStringHCenterAt("TOUCH_Calibrate", LCD_GetXSize() / 2, 5);
	LCD_BK_EN;//��Һ��������
   do {
    GUI_TOUCH_GetState(&State);
    GUI_Delay(1);
    if (State.Pressed == 1) 
	{
      int TimeStart = GUI_GetTime();
      do 
	  {
			GUI_TOUCH_GetState(&State);
			GUI_Delay(1);
			if (State.Pressed != 1) 
			{
			  break;
			} 
			else if ((GUI_GetTime() - 100) > TimeStart) {
			  return;
			}
      } while (1);
    }
	if(oneflag)GUI_Clear();
	oneflag=0;
	GUI_DispStringHCenterAt("TOUCH_Calibrate", LCD_GetXSize() / 2, LCD_GetYSize()/2);
  } while (1);
  _WaitForPressedState(0);
}
/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       MainTask
*/
void Touch_MainTask(void)
{
	uint16_t aCal[8]={0};
	uint8_t i=0;

	GUI_SetBkColor(GUI_WHITE);
	GUI_Clear();
	GUI_Delay(10);
	GUI_Clear();
	
	GUI_SetColor(GUI_BLACK);
	GUI_SetFont(&GUI_Font13B_ASCII);
	
	_Explain();
	/* Set the logical values */
	aCal[0]=15;
	aCal[1]=15;
	aCal[4]=LCD_GetXSize() - 15;
	aCal[5]=LCD_GetYSize() - 15;
	
	/* Get the physical values of the AD converter for 2 positions */	
	for (i = 0; i < 2; i++) 
	{
		_GetPhysValues(aCal[i*4], aCal[i*4+1], &aCal[i*4+2], &aCal[i*4+3], _acPos[i]);
	}
	for( i=0; i<8; ++i)
        printf("aCal[%d]=%d ",i,aCal[i]);
	printf("\r\n");
	SPI_FLASH_SectorErase(CALADD);
	if((aCal[2]<600&&aCal[2]>200)&&(aCal[3]<3750&&aCal[3]>3350)&&(aCal[6]<3800&&aCal[6]>3400)&&(aCal[7]<550&&aCal[7]>150))
	{
		IsCal=0x55;
		SPI_FLASH_BufferWrite(&IsCal,CALADD,1);
		
		SPI_FLASH_BufferWrite((void *)aCal,CALADD+2, sizeof(aCal));	

		/* Use the physical values to calibrate the touch screen */
		GUI_TOUCH_Calibrate(GUI_COORD_X,aCal[0], aCal[4], aCal[6], aCal[2]); /* Calibrate X-axis */
		GUI_TOUCH_Calibrate(GUI_COORD_Y,aCal[1], aCal[5], aCal[7], aCal[3]); /* Calibrate Y-axis */
	}		
	GUI_Delay(10);

	
	
/***********************************************************************************************/
#if 1
	Soft_Reset();
#else
	/* Display the result */
	GUI_CURSOR_Show();
	GUI_Clear();
	_DispStringCentered("Congratulation, your\n"
					  "touch screen has been\n"
					  "calibrated. Please use\n"
					  "the cursor to test\n"
					  "the calibration...");
	/* Let the user play */
	while(1) 
	{
		GUI_PID_STATE State;

		GUI_TOUCH_GetState(&State);
		if (State.Pressed == 1) 
		{
		  GUI_FillCircle(State.x, State.y, 3);
		}
		GUI_Delay(10);
	}
#endif
}

/*************************** End of file ****************************/


