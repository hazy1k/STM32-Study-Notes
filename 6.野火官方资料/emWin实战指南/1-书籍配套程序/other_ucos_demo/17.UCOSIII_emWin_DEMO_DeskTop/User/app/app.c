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
#define ICONVIEW_YSpace    12   /* �ؼ�ICONVIEW��Y������ */ 
#define ICONVIEW_XPos	     3    /* �ؼ�ICONVIEW��X�����������*/
#define BOTWIN_YSpace	     62   /* ��Ļ�·����ִ���Y������*/
#define MIDWIN_yPos		     20   /* �м䴰��Y������ʼλ�ü��������ֿ��ø߶�*/
#define MIDWIN_xPos		     0 	  /* �м䴰��X������ʼλ��*/

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

uint8_t Flag_ICON100  = 0;
uint8_t Flag_ICON101  = 0;
uint8_t Flag_ICON102  = 0;
uint8_t Flag_ICON103  = 0;

uint8_t Flag_ICON104  = 0;
uint8_t Flag_ICON105  = 0;
uint8_t Flag_ICON106  = 0;
uint8_t Flag_ICON107  = 0;

uint8_t Flag_ICON108  = 0;
uint8_t Flag_ICON109  = 0;
uint8_t Flag_ICON110  = 0;
uint8_t Flag_ICON111  = 0;


/* ��������ICONVIEWͼ��Ĵ��� */
typedef struct {
  const GUI_BITMAP * pBitmap;
  const char       * pText;
} BITMAP_ITEM;

typedef struct WIN_PARA{			//����ʹ�õ����û���������������ڻص�������ʹ��
	int xSizeLCD, ySizeLCD;			//LCD��Ļ�ߴ�
	int xPosWin,  yPosWin;			//���ڵ���ʼλ��
	int xSizeWin, ySizeWin;			//���ڳߴ�	
	int xSizeBM,  ySizeBM;
	int ySizeBotWin;
	
	WM_HWIN hWinMid;				//����ʾ���ľ�����в���
	
}WIN_PARA;
WIN_PARA WinPara;			//�û�ʹ�õĴ��ڶ�������

/* ��������ICONVIEW0ͼ��Ĵ��� */
static const BITMAP_ITEM _aBitmapItem0[] = {
	{&bmPhone,     "Phone" 		},
	{&bmmessages,  "Message"	}
};
/* ��������ICONVIEW1ͼ��Ĵ��� */
static const BITMAP_ITEM _aBitmapItem1[] = {
  {&bmkey,  	 	"KEY"		    },
  {&bmrgbled,	 	"Breathing"	}, 
  {&bmadc,  	 	"ADC"				},
	{&bmeeprom,   "EEPROM" 		},
	
  {&bmclock,    "Clock" 		},
  {&bmmusic,    "Music" 		},
	{&bmrecorder,	"Recorder"	},
	{&bmusb,  	  "USB"		    },
	
	{&bmwifi,			"Wifi"		  },	
	{&bmWeather,  "Humiture"  }, 
  {&bmcamera,   "Camera"		}, 
	{&bmcalculator,"Calculator" }
};

void FUN_ICON000Clicked(void)  {printf("FUN_ICON000Clicked\n");}
void FUN_ICON001Clicked(void)  {printf("FUN_ICON001Clicked\n");}

void FUN_ICON100Clicked(void)  {printf("FUN_ICON100Clicked\n");}
void FUN_ICON101Clicked(void)  {printf("FUN_ICON101Clicked\n");}
void FUN_ICON102Clicked(void)  {printf("FUN_ICON102Clicked\n");}
void FUN_ICON103Clicked(void)  {printf("FUN_ICON103Clicked\n");}

void FUN_ICON104Clicked(void)  {printf("FUN_ICON104Clicked\n");}
void FUN_ICON105Clicked(void)  {printf("FUN_ICON105Clicked\n");}
void FUN_ICON106Clicked(void)  {printf("FUN_ICON106Clicked\n");}
void FUN_ICON107Clicked(void)  {printf("FUN_ICON107Clicked\n");}

void FUN_ICON108Clicked(void)  {printf("FUN_ICON108Clicked\n");}
void FUN_ICON109Clicked(void)  {printf("FUN_ICON109Clicked\n");}
void FUN_ICON110Clicked(void)  {printf("FUN_ICON110Clicked\n");}
void FUN_ICON111Clicked(void)  {printf("FUN_ICON111Clicked\n");}


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
				GUI_Clear();		
		break;
			
	 default:
		WM_DefaultProc(pMsg);
		break;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: _cbMidWin
*	����˵��: 
*	��    �Σ�WM_MESSAGE * pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _cbMidWin(WM_MESSAGE * pMsg) 
{
	int NCode, Id;
	switch (pMsg->MsgId) 
	{
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
			NCode = pMsg->Data.v;                 /* Notification code */
			switch (Id) 
			{
				case GUI_ID_ICONVIEW1:
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
								/* KEY  ******************************************************************/
								case 0:	
									Flag_ICON100 = 1;
									FUN_ICON100Clicked();			
									break;	
								
								/* Breathing ***********************************************************************/
								case 1:
									Flag_ICON101 = 1;
									FUN_ICON101Clicked();
									break;
								
								/* ADC *********************************************************************/
								case 2:
									Flag_ICON102 = 1;
									FUN_ICON102Clicked();	
									break;
								
								/* EEPROM ********************************************************************/
								case 3:
									Flag_ICON103 = 1;
									FUN_ICON103Clicked();
									break;
								
								/* Clock **********************************************************************/
								case 4:
									Flag_ICON104 = 1;
									FUN_ICON104Clicked();
									break;
								
								/* Music ********************************************************************/
								case 5:
									Flag_ICON105 = 1;
									FUN_ICON105Clicked();
									break;
								
								/* Recorder ******************************************************************/
								case 6:					
									Flag_ICON106 = 1;
									FUN_ICON106Clicked();
									break;
								
								/* USB *******************************************************************/
								case 7:
									Flag_ICON107 = 1;
									FUN_ICON107Clicked();
									break;
								
								/* Wifi ******************************************************************/
								case 8:
									Flag_ICON108 = 1;
									FUN_ICON108Clicked();
									break;
								 
								 /* Humiture ******************************************************************/
								case 9:
									Flag_ICON109 = 1;
									FUN_ICON109Clicked();
									break;
								 
								 /* Camera ******************************************************************/
								case 10:
									Flag_ICON110 = 1;
									FUN_ICON110Clicked();
									break;
								 
								/* Calculator*****************************************************************/
								case 11:
									Flag_ICON111 = 1;
									FUN_ICON111Clicked();
									break;
								default:break;
								}
							 break;
						}
					break;					
			}			
			break;
			
		/* �ػ���Ϣ*/
		case WM_PAINT:
			{
				GUI_SetBkColor(MIDWIN_COLOR);
				GUI_Clear();
			}			
		break;			
	 default:
		WM_DefaultProc(pMsg);
		break;
	}
}

/**
  * @brief  CreateTopWin�����������Ĵ���
  * @param  none
  * @retval none
  */
static void CreateTopWin(void)
{
	WM_HWIN hText;
	/* ������ "wildfire OS "�ı� */
	hText = TEXT_CreateEx(0, 2, 60 , 16, WM_HBKWIN, WM_CF_SHOW, GUI_TA_LEFT|TEXT_CF_VCENTER, GUI_ID_TEXT0, "Wildfire OS");
	TEXT_SetFont(hText, GUI_FONT_10_ASCII);
	TEXT_SetTextColor(hText,TEXT_COLOR);
	
	/* ״̬����ʱ����ʾ�ı� */
	hText = TEXT_CreateEx(WinPara.xSizeLCD/2-25,2,50,16,WM_HBKWIN,WM_CF_SHOW,GUI_TA_HCENTER|TEXT_CF_VCENTER,GUI_ID_TEXT1,"11:56:00");
	//TEXT_SetBkColor(hText,GUI_INVALID_COLOR);
	TEXT_SetTextColor(hText,TEXT_COLOR);
	TEXT_SetFont(hText,GUI_FONT_8_ASCII);
}
/**
  * @brief  CreateMidWin�������м�Ĵ���
  * @param  none
  * @retval none
  */
static void CreateMidWin(void)
{
	uint8_t i=0;
	WM_HWIN MIDWIN;
	WinPara.hWinMid= WM_CreateWindowAsChild(
											WinPara.xPosWin,											
											WinPara.yPosWin,
											WinPara.xSizeWin,
											WinPara.ySizeWin,	
											WM_HBKWIN, 
											WM_CF_SHOW, 
											_cbMidWin, 
											sizeof(WIN_PARA *)
											);
/*-------------------------------------------------------------------------------------*/
					/*��ָ��λ�ô���ָ���ߴ��ICONVIEW1 С����*/
					MIDWIN=ICONVIEW_CreateEx(
											 ICONVIEW_XPos, 					/* С���ߵ��������أ��ڸ������У�*/
											 0, 								/* С���ߵ��������أ��ڸ������У�*/
											 WinPara.xSizeLCD-ICONVIEW_XPos,	/* С���ߵ�ˮƽ�ߴ磨��λ�����أ�*/
											 3*(WinPara.ySizeBM+20), 			/* С���ߵĴ�ֱ�ߴ磨��λ�����أ�*/
											 WinPara.hWinMid, 				    /* �����ڵľ�������Ϊ0 ������С���߽���Ϊ���棨�������ڣ����Ӵ��� */
											 WM_CF_SHOW | WM_CF_HASTRANS,       /* ���ڴ�����ǡ�ΪʹС���������ɼ���ͨ��ʹ�� WM_CF_SHOW */ 
											 0,//ICONVIEW_CF_AUTOSCROLLBAR_V, 	/* Ĭ����0�����������ʵ������������ֱ������ */
											 GUI_ID_ICONVIEW1, 			        /* С���ߵĴ���ID */
											 ICONVIEW_Width, 				    /* ͼ���ˮƽ�ߴ� */
											 ICONVIEW_Height+10);				/* ͼ��Ĵ�ֱ�ߴ� */
											 
		/* ��ICONVIEW С���������ͼ�� */
		for (i = 0; i < GUI_COUNTOF(_aBitmapItem1); i++) 
		{
			ICONVIEW_AddBitmapItem(MIDWIN, _aBitmapItem1[i].pBitmap,_aBitmapItem1[i].pText);
			ICONVIEW_SetTextColor(MIDWIN,i,TEXT_COLOR);
		}	
		/* ����С���ߵı���ɫ 32 λ��ɫֵ��ǰ8 λ������alpha��ϴ���Ч��*/
		ICONVIEW_SetBkColor(MIDWIN, ICONVIEW_CI_SEL, ICON_COLOR | 0x80000000);
		/* �������� */
		ICONVIEW_SetFont(MIDWIN, &GUI_Font10_ASCII);
		/* ���ó�ʼѡ���ͼ��Ϊ -1 (��ʾ��δѡ��)*/
		ICONVIEW_SetSel(MIDWIN,-1);
		/* ����ͼ����x ��y �����ϵļ�ࡣ*/
		ICONVIEW_SetSpace(MIDWIN, GUI_COORD_X, ICONVIEW_XSpace);
		ICONVIEW_SetSpace(MIDWIN, GUI_COORD_Y, ICONVIEW_YSpace);
	//	ICONVIEW_SetSpace(hWin, GUI_COORD_Y, ICONVIEW_YSpace);
		/* ���ö��뷽ʽ ��5.22�汾�����¼���� */
		ICONVIEW_SetIconAlign(MIDWIN, ICONVIEW_IA_HCENTER|ICONVIEW_IA_TOP);
}

/**
  * @brief  CreateBotWin�������ײ��Ĵ���
  * @param  none
  * @retval none
  */
static void CreateBotWin(void)
{
	uint8_t i;
	WM_HWIN BOTWIN;
	/*��ָ��λ�ô���ָ���ߴ��ICONVIEW С����*/
	BOTWIN=ICONVIEW_CreateEx(
							 62,                					      /* С���ߵ��������أ��ڸ������У�*/
							 320-62, 								                /* С���ߵ��������أ��ڸ������У�*/
							 116,                               /* С���ߵ�ˮƽ�ߴ磨��λ�����أ�*/
							 WinPara.ySizeBotWin,				        /* С���ߵĴ�ֱ�ߴ磨��λ�����أ�*/
								WM_HBKWIN, 				          /* �����ڵľ�������Ϊ0 ������С���߽���Ϊ���棨�������ڣ����Ӵ��� */
							 WM_CF_SHOW | WM_CF_HASTRANS,       /* ���ڴ�����ǡ�ΪʹС���������ɼ���ͨ��ʹ�� WM_CF_SHOW */ 
							 0,//ICONVIEW_CF_AUTOSCROLLBAR_V, 	/* Ĭ����0�����������ʵ������������ֱ������ */
							 GUI_ID_ICONVIEW0, 			            /* С���ߵĴ���ID */
							 ICONVIEW_Width, 				            /* ͼ���ˮƽ�ߴ� */
							 ICONVIEW_Height);					        /* ͼ��Ĵ�ֱ�ߴ� */
	/* ��ICONVIEW С���������ͼ�� */
	for (i = 0; i < GUI_COUNTOF(_aBitmapItem0); i++) 
	{
		ICONVIEW_AddBitmapItem(BOTWIN, _aBitmapItem0[i].pBitmap,"");
	}	
	/* ����С���ߵı���ɫ 32 λ��ɫֵ��ǰ8 λ������alpha��ϴ���Ч��*/
	ICONVIEW_SetBkColor(BOTWIN, ICONVIEW_CI_SEL, ICON_COLOR | 0x80000000);
	/* �������� */
//	ICONVIEW_SetFont(hWinicon0, &GUI_Font10_ASCII);
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
	//׼������3�����ڣ�������ʹ�õ����û���������������ڻص�������ʹ��
	WinPara.xSizeLCD = LCD_GetXSize();				//LCD��Ļ�ߴ�
	WinPara.ySizeLCD = LCD_GetYSize();				//LCD��Ļ�ߴ�
	WinPara.xSizeBM  = ICONVIEW_Width;				//ͼ����
	WinPara.ySizeBM  = ICONVIEW_Height;				//ͼ��߶�
	WinPara.ySizeBotWin=BOTWIN_YSpace;				//�����·����ڸ߶�
	WinPara.xPosWin	 = MIDWIN_xPos;							//���ڵ���ʼλ��
	WinPara.yPosWin  = MIDWIN_yPos;							//���ڵ���ʼλ��
	WinPara.xSizeWin = WinPara.xSizeLCD;						//���ڳߴ�
	WinPara.ySizeWin = WinPara.ySizeLCD-WinPara.ySizeBotWin-WinPara.yPosWin;	//���ڳߴ�	
	
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
	CreateBotWin();
	GUI_Delay(10);
	CreateMidWin();
	CreateTopWin();
	GUI_Delay(50);
	WM_BringToTop(WM_GetDialogItem(WM_HBKWIN, GUI_ID_ICONVIEW0));
	LCD_BK_EN;
	while(1)
	{
		GUI_Delay(5);
	}
}
