#include <emXGUI.h>

#ifndef __GUI_NETWORK_DIALOG
#define	__GUI_NETWORK_DIALOG

/* �ؼ� ID ���� */
enum
{   
   /* ���� ID */
  ID_TEXTBOX_Title      = 0x00,     // ������
  ID_TEXTBOX_Send        ,     // ������ʾ
  ID_TEXTBOX_Receive     ,     // ������ʾ
  ID_TEXTBOX_RemoteIP   ,     // Զ��IP
//  ID_TEXTBOX_RemoteIP2   ,     // Զ��IP
//  ID_TEXTBOX_RemoteIP3   ,     // Զ��IP
//  ID_TEXTBOX_RemoteIP4   ,     // Զ��IP
  ID_TEXTBOX_RemotePort  ,     // Զ�˶˿�
  ID_MODE_GROUP,
  
  /* ��ť ID */
  eID_BTN_EXIT    ,    // 0x03
  eID_BTN_STATE   ,    // 0x04
  eID_BTN_SEND    ,    // 0x05
  eID_BTN_CLEAR   ,    // 0x06
  
  ID_Hint_Win,
  
};

#endif


