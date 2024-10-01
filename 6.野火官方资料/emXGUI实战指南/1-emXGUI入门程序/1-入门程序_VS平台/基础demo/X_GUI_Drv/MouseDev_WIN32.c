
#include	"GUI_Drv.h"

/*=====================================================================*/
static int x_pos,y_pos,key_state;

static int x_old;
static int y_old;
static int key_old;

void XGUI_MouseLButtonDown(int x,int y)
{
	x_pos =x;
	y_pos =y;
	key_state |=MK_LBUTTON;
	//MouseInput(x,y,MK_LBUTTON);
}

void XGUI_MouseLButtonUp(int x,int y)
{
	x_pos =x;
	y_pos =y;
	key_state &=~MK_LBUTTON;

	//MouseInput(x,y,0);
}

void XGUI_MouseMove(int x,int y)
{
	x_pos =x;
	y_pos =y;

	//MouseInput(x,y,a);
}
 
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/

#define	MOUSE_ACT_NONE	 	0
#define MOUSE_ACT_KEY		1
#define	MOUSE_ACT_MOVE		2

static int mouse_active=MOUSE_ACT_NONE;


BOOL MouseDev_Init(void)
{

	mouse_active=MOUSE_ACT_NONE;

	x_pos=0;
	y_pos=0;
	key_state=0;

	x_old=0;
	y_old=0;
	key_old=0;

	return TRUE;
}

BOOL MouseDev_IsActive(void)
{

	if(x_pos!=x_old) return TRUE;
	if(y_pos!=y_old) return TRUE;
	if(key_state!=key_old) return TRUE;

	return FALSE;

}

void MouseDev_Handler(void)
{

	MouseInput(x_pos,y_pos,key_state);

	x_old =x_pos;
	y_old =y_pos;
	key_old =key_state;


}

/*=====================================================================*/
/*=====================================================================*/

