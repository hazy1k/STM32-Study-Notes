
#include <string.h>
#include "GUI_Drv.h"

/*===================================================================================*/

BOOL	GUI_Arch_Init(void)
{

	return TRUE;
}

/*============================================================================*/

BOOL GUI_LowLevelInit(void);
SURFACE* GUI_DisplayInit(void);
HFONT GUI_FontInit(void);
void GUI_DesktopStartup(void);

//void	GUI_Startup(void)
//{
//	SURFACE *pSurf=NULL;
//	HFONT hFont=NULL;

//	if(!GUI_LowLevelInit()) //GUI�ͼ���ʼ��.
//	{
//		return;
//	}

//	if(!GUI_Arch_Init())   //GUI�ܹ�������ʼ��.
//	{
//		return;
//	}

//	if(!X_GUI_Init())	   //GUI�ں˳�ʼ��.
//	{
//		return;
//	}

//	pSurf =GUI_DisplayInit(); //��ʾ�豸��ʼ��.
//	if(pSurf==NULL)
//	{
//		return;
//	}
//	GUI_SetScreenSurface(pSurf); //������ĻSurface����.

//	GUI_CursorInit(pSurf,pSurf->Width>>1,pSurf->Height>>1); //��ʼ�����.

//	hFont =GUI_FontInit(); //��ʼ��Ĭ�ϵ�����.
//	if(hFont==NULL)
//	{
//		return;
//	}
//	GUI_SetDefFont(hFont);  //����Ĭ�ϵ�����.

//	GUI_DesktopStartup();	//�������洰��(�ú������᷵��).

//}


/*============================================================================*/

