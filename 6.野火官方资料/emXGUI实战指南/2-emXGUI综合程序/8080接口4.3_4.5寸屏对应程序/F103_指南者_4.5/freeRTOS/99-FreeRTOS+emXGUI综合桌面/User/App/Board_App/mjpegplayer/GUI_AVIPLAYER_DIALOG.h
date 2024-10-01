#ifndef _GUI_MUSICPLAYER_DIALOG_H
#define _GUI_MUSICPLAYER_DIALOG_H
/*****************�������ؼ�IDֵ*********************/
#define ID_SCROLLBAR_POWER   0x1104   //������
#define ID_SCROLLBAR_TIMER   0x1105   //������

/******************��ť�ؼ�IDֵ***********************/
#define ID_BUTTON_Power      0x1000   //���� 
#define ID_BUTTON_List       0x1001   //����List
#define ID_BUTTON_Back       0x1002   //��һ��
#define ID_BUTTON_Play       0x1003   //����/��ͣ
#define ID_BUTTON_Next       0x1004   //����/��ͣ

/******************TextBox�ؼ�IDֵ*******************/
#define ID_TB1               0x1300   //�ļ���
#define ID_TB2               0x1301   //�ֱ���
#define ID_TB3               0x1302   //֡��
#define ID_TB4               0x1304   //ʱ��
#define ID_TB5               0x1305   //��ǰʱ��

#define GUI_RGB_BACKGROUNG_PIC      "avi_desktop.jpg"

typedef struct{
   char *icon_name;//ͼ����
   RECT rc;        //λ����Ϣ
   BOOL state;     //״̬  
}icon_S;

extern int avi_chl;
extern volatile int win_fps;
extern HWND	VideoPlayer_hwnd;

extern HFONT AVI_Player_hFont48;
extern HFONT AVI_Player_hFont64;
extern HFONT AVI_Player_hFont72;
#endif


