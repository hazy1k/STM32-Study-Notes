#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"

#ifndef _GUI_RECORDER_DIALOG_H
#define _GUI_RECORDER_DIALOG_H

#define MUSIC_MAX_NUM           10
#define FILE_NAME_LEN           50			
#define MUSIC_NAME_LEN          50		
#define LYRIC_MAX_SIZE          100				
#define MAX_LINE_LEN            100				
#define _DF1S                   0x81

#define LRC_GBK   1  //1��ʾLRC���ΪGBK���룬0��ʾUTF8����
#define ID_TB1               0x1301
#define ID_TB2               0x1302
#define ID_TB5               0x1305

typedef enum 
{ 
  /******************��ť�ؼ�IDֵ*******************/
  ID_RECORD_bPOWER = 0x1000,   // ���� 
  ID_RECORD_BACK,              // ��һ��
  ID_RECORD_NEXT,              // ��һ��
  ID_RECORD_PLAY,              // ������ͣ��ť
  ID_RECORD_BUGLE,             // ���Ȱ�ť
  ID_RECORD_PADNC,             // ¼��������ͣ
  ID_RECORD_START,             // ¼����ʼ
  ID_RECORD_STOP,              // ֹͣ¼��
  ID_RECORD_EXIT,              // �˳���ť���������������ť�ػ溯����һ����
  /*****************�������ؼ�IDֵ*******************/
  ID_RECORD_sPOWER,            // ������
  ID_PLAY_PROGRESS,            // ������
  /*****************�ı��ؼ�IDֵ*********************/
  ID_RECORD_STATE = 0x1301 - 2,                // ¼��״̬
  ID_RECORD_TIME,                 // ¼��ʱ��
  ID_PLAY_TOTAL_TIME = 0x1301,    // ������ʱ���������ֲ���������ͬһ��ID��
  ID_PLAY_TIME = 0x1302,          // ����ʱ���������ֲ���������ͬһ��ID��
  /*****************�б�ؼ�IDֵ*********************/
  ID_RECORD_LIST,              // �ļ��б�
  /*****************�б�ؼ�IDֵ*********************/
  ID_Record_Timer,
  /******************�û��Զ�����Ϣ******************/
  eMSG_SCAN_FILE = WM_USER + 1,

}record_id_t;

typedef struct{
   WCHAR *icon_name; // ͼ����
   RECT rc;         // λ����Ϣ
   record_id_t id;      // ��ťID
}recorder_icon_t;

#endif

