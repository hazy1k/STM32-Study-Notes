#include "includes.h"
#include  "app.h"

#define BK_COLOR       GUI_BLACK
#define MIDWIN_COLOR   0X303030
#define ICON_COLOR     GUI_LIGHTCYAN
#define TEXT_COLOR		 GUI_WHITE
/*
*********************************************************************************************************
*                                         �궨�� 
*********************************************************************************************************
*/
#define ICONVIEW_Width     48   /* �ؼ�ICONVIEW�Ŀ� */  
#define ICONVIEW_Height    48   /* �ؼ�ICONVIEW�ĸߣ�����ͼ���Y������ */  
#define ICONVIEW_XSpace    10   /* �ؼ�ICONVIEW��X������ */ 

/*
*********************************************************************************************************
*                                      ����
*********************************************************************************************************
*/
GUI_XBF_DATA XBF_Data;
GUI_FONT     XBF_Font;

uint8_t UserApp_Flag=0;//��������־

uint8_t Flag_ICON000  = 0;   /* ICONVIEW�ؼ����µı�־��0��ʾδ���£�1��ʾ���� */
uint8_t Flag_ICON001  = 0;



/* ��������ICONVIEWͼ��Ĵ��� */
typedef struct {
  const GUI_BITMAP * pBitmap;
  const char       * pText;
} BITMAP_ITEM;


/* ��������ICONVIEW0ͼ��Ĵ��� */
static const BITMAP_ITEM _aBitmapItem0[] = {
  {&bmmusic,    "Music" 		},
	{&bmrecorder,	"Recorder"	},
};

//void FUN_ICON000Clicked(void)  {printf("FUN_ICON000Clicked\n");}
//void FUN_ICON001Clicked(void)  {printf("FUN_ICON001Clicked\n");}


/*
*********************************************************************************************************
*	�� �� ��: _cbBkWindow
*	����˵��: ���洰�ڵĻص����� 
*	��    �Σ�WM_MESSAGE * pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _cbBkWindow(WM_MESSAGE * pMsg) 
{
	int NCode, Id;
	switch (pMsg->MsgId) 
	{
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
			NCode = pMsg->Data.v;                 /* Notification code */
			switch (Id) 
			{
				case GUI_ID_ICONVIEW0:
					switch (NCode) 
					{
						/* ICON�ؼ������Ϣ */
						case WM_NOTIFICATION_CLICKED:
							UserApp_Flag = 1;
							break;
						
						/* ICON�ؼ��ͷ���Ϣ */
						case WM_NOTIFICATION_RELEASED: 
							
							/* ����Ӧѡ�� */
							switch(ICONVIEW_GetSel(pMsg->hWinSrc))
							{
								/* Phone *******************************************************************/
								case 0:	
									Flag_ICON000 = 1;
									FUN_ICON000Clicked();			
									break;	
								
								/* Message ***********************************************************************/
								case 1:
									Flag_ICON001 = 1;
									FUN_ICON001Clicked();
									break;
								}	
						 break;
					}
				break;
			}			
			break;
		/* �ػ���Ϣ*/
		case WM_PAINT:		
				GUI_SetBkColor(BK_COLOR);
				GUI_SetColor(MIDWIN_COLOR);
				GUI_Clear();	
				GUI_FillRect(2,22,237,259);
				GUI_SetColor(TEXT_COLOR);
				GUI_DispStringHCenterAt("Wildfire emWin",120,2);
		break;
			
	 default:
		WM_DefaultProc(pMsg);
		break;
	}
}

/**
  * @brief  CreateBotWin�������ײ��Ĵ���
  * @param  none
  * @retval none
  */
static void CreateIconWin(void)
{
	uint8_t i;
	WM_HWIN BOTWIN;
	/*��ָ��λ�ô���ָ���ߴ��ICONVIEW С����*/
	BOTWIN=ICONVIEW_CreateEx(
							 62,                					      /* С���ߵ��������أ��ڸ������У�*/
							 27, 								                /* С���ߵ��������أ��ڸ������У�*/
							 116,                               /* С���ߵ�ˮƽ�ߴ磨��λ�����أ�*/
							 62,				        /* С���ߵĴ�ֱ�ߴ磨��λ�����أ�*/
							 WM_HBKWIN, 				          /* �����ڵľ�������Ϊ0 ������С���߽���Ϊ���棨�������ڣ����Ӵ��� */
							 WM_CF_SHOW | WM_CF_HASTRANS,       /* ���ڴ�����ǡ�ΪʹС���������ɼ���ͨ��ʹ�� WM_CF_SHOW */ 
							 0,//ICONVIEW_CF_AUTOSCROLLBAR_V, 	/* Ĭ����0�����������ʵ������������ֱ������ */
							 GUI_ID_ICONVIEW0, 			            /* С���ߵĴ���ID */
							 ICONVIEW_Width, 				            /* ͼ���ˮƽ�ߴ� */
							 ICONVIEW_Height+10);					        /* ͼ��Ĵ�ֱ�ߴ� */
	WM_BringToTop(BOTWIN);
	/* ��ICONVIEW С���������ͼ�� */
	for (i = 0; i < GUI_COUNTOF(_aBitmapItem0); i++) 
	{
		ICONVIEW_AddBitmapItem(BOTWIN, _aBitmapItem0[i].pBitmap,_aBitmapItem0[i].pText);
		ICONVIEW_SetTextColor(BOTWIN,i,TEXT_COLOR);
	}	
	/* ����С���ߵı���ɫ 32 λ��ɫֵ��ǰ8 λ������alpha��ϴ���Ч��*/
	ICONVIEW_SetBkColor(BOTWIN, ICONVIEW_CI_SEL, ICON_COLOR | 0x80000000);
	/* �������� */
	ICONVIEW_SetFont(BOTWIN, &GUI_Font10_ASCII);
	/* ���ó�ʼѡ���ͼ��Ϊ -1 (��ʾ��δѡ��)*/
	ICONVIEW_SetSel(BOTWIN,-1);
	/* ����ͼ����x ��y �����ϵļ�ࡣ*/
	ICONVIEW_SetSpace(BOTWIN, GUI_COORD_X, ICONVIEW_XSpace);
//	ICONVIEW_SetSpace(hWin, GUI_COORD_Y, ICONVIEW_YSpace);
	/* ���ö��뷽ʽ ��5.22�汾�����¼���� */
	ICONVIEW_SetIconAlign(BOTWIN, ICONVIEW_IA_HCENTER|ICONVIEW_IA_TOP);
}
/*
*********************************************************************************************************
*
*       _cbGetData
*
* Function description
*   Callback function for getting font data
*
* Parameters:
*   Off      - Position of XBF file to be read
*   NumBytes - Number of requested bytes
*   pVoid    - Application defined pointer
*   pBuffer  - Pointer to buffer to be filled by the function
*
* Return value:
*   0 on success, 1 on error
*********************************************************************************************************
*/
static int _cbGetData(U32 Offset, U16 NumBytes, void * pVoid, void * pBuffer)
{
	SPI_FLASH_BufferRead(pBuffer,60*4096+Offset,NumBytes);
	return 0;
}

static void _CreateXBF(void) 
{
	//
	// Create XBF font
	//
	GUI_XBF_CreateFont(&XBF_Font,       // Pointer to GUI_FONT structure in RAM
					   &XBF_Data,         // Pointer to GUI_XBF_DATA structure in RAM
					   GUI_XBF_TYPE_PROP,//GUI_XBF_TYPE_PROP_EXT, 		// Font type to be created
					   _cbGetData,        // Pointer to callback function
					   0);        // Pointer to be passed to GetData function
}

/*
*********************************************************************************************************
*	�� �� ��: UserAPP
*	����˵��: GUI������ 
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void UserAPP(void)
{
	UserApp_Flag=0;	
	/***************************����Ƥ��ɫ*****************************/
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);
	/***************************����Ĭ������**********************************/
	_CreateXBF();
	GUI_UC_SetEncodeUTF8();
  GUI_SetDefaultFont(&XBF_Font);
	/*************************************************************************/
	WM_SetCallback(WM_HBKWIN, _cbBkWindow);
	
	/* ������������ ״̬������������������*/	
	CreateIconWin();
	GUI_Delay(50);
	WM_InvalidateWindow(WM_GetDialogItem(WM_HBKWIN, GUI_ID_ICONVIEW0));
	LCD_BK_EN;
	while(1)
	{
		GUI_Delay(5);
	}
}
