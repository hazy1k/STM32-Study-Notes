#ifndef _GUI_VIDEOPLAYER_DIALOG_H_
#define _GUI_VIDEOPLAYER_DIALOG_H_
#include "emXGUI.h"
#include "def.h"

#define GUI_RGB_BACKGROUNG_PIC      "avi_desktop.jpg"

typedef enum
{
  eID_Vedio_Power = 0x1000,
  eID_Vedio_List,
  eID_BUTTON_LRC,
  eID_Vedio_BACK,
  eID_Vedio_NEXT,
  eID_Vedio_TRUM, //喇叭
  eID_Vedio_START,
  
  eID_SBN_TIMER,
  eID_SBN_POWER,
  
                  
  eID_FileList,
  
  eID_VIDEO_EXIT,
  eID_MUSICLIST,
  
  eMUSIC_VIEWER_ID_PREV,
  eMUSIC_VIEWER_ID_NEXT,
  eID_VIDEO_RETURN,
  
  eID_TEXTBOX_ITEM,
  eID_TEXTBOX_RES,
  eID_TEXTBOX_FPS,
  eID_TEXTBOX_CURTIME,
  eID_TEXTBOX_ALLTIME,
  eID_TEXTBOX_LRC5,
  
  
}VideoDlg_Master_ID;

typedef struct
{
  HWND Video_Hwnd;
  HWND List_Hwnd;
  HWND SBN_TIMER_Hwnd;
  HDC hdc_bk;
  
  int power;  
  int curtime;
  int alltime;
  
  uint8_t avi_file_num;
  int8_t  playindex;
  BOOL LOAD_STATE;
  BOOL SWITCH_STATE;
  BOOL LIST_STATE;
  BOOL avi_chl;
}VIDEO_DIALOG_Typedef;
extern VIDEO_DIALOG_Typedef VideoDialog;
extern char lcdlist[20][100];//显示lis
extern SURFACE *pSurf1;
extern uint8_t chgsch_TouchUp;
extern GUI_SEM *Delete_VideoTask_Sem;//做任务同步,结束播放器前先关闭播放任务

#endif


