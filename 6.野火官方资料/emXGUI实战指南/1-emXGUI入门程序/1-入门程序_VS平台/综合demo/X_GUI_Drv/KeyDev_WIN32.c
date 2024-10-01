/*
 * KeyDev_WIN32.c
 *
 *  Created on: 2018年7月26日
 *      Author: liuwei
 */


#include <emXGUI.h>

/*=====================================================================*/

void X_GUI_KeyDown(int key)
{
	PostKeyMessage(key,TRUE);
	//KeyboardInput(key,KEY_STATE_DOWN);
}

void X_GUI_KeyUp(int key)
{
	//PostKeyMessage(key,KEY_STATE_UP);
	PostKeyMessage(key,FALSE);
}

/*=====================================================================*/
