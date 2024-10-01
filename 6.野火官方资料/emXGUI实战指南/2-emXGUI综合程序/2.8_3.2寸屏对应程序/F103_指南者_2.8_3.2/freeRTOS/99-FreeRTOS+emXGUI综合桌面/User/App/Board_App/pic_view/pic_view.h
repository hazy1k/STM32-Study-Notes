#ifndef _GUI_PICVIEW_H_
#define _GUI_PICVIEW_H_


#include <emXGUI.h>


#define PICFILE_NUM_MAX           20
#define PICFILE_NAME_MAXLEN       100	

#define BMP_FILE_NAME     "red_fish_ARGB8888.bmp"
#define PNG_FILE_NAME     "pic1.png"
#define JPG_FILE_NAME     "gui_demo_pic.jpg"
#define GIF_FILE_NAME     "king.gif"

#define BMP_FILE_NAME_INT "blue_fish_ARGB8888.bmp"
#define PNG_FILE_NAME_INT "logo.png"
#define GIF_FILE_NAME_INT "king.gif"
typedef enum 
{
  eID_Pic_Name,
  eID_Pic_EXIT,
  eID_Pic_PREV,
  eID_Pic_NEXT,
  eID_Pic_MsgBOX,
  eID_Pic_Time,
  eID_Pic_Res,
  eID_Pic_Res_Value,
  //eID_Pic_Time,
  eID_Pic_Time_Value,
  eID_Pic_INTFLASH,
  eID_Pic_EXTFLASH,
  eID_Pic_SDCARD,
  eID_Pic_Return,
  eID_Pic_JPG,
  eID_Pic_PNG,
  eID_Pic_GIF,
  eID_Pic_BMP,
  eID_Pic_Title,
  eID_Pic_Def,
  eID_FILEPATH,
  eID_ZOOMIN,
  eID_ZOOMOUT,
  eID_Pic_FPS,
  eID_Pic_FPS_Value,
  eID_LIST_1,
}GUI_PicViewer_ID;

typedef struct 
{
  HWND mPicViewer;
  HWND mPicMenu;
  HDC  mhdc_bk;        //������
  HDC  mhdc_pic;       //ͼƬ��
  
  char** pic_list;   //�ļ�·���������ļ�����
  char** pic_lcdlist;//�ļ���
  
  uint16_t pic_width;
  uint16_t pic_height;
  
  int scale_time;
  float scale_y;  
  int8_t show_index;
  uint8_t pic_nums;
  
  int8_t Menu_State;
  int8_t SecMenu_State;
  int cur_type;
  int cur_path;
  int frame_num;
  int delay;
  int gif_state;//0---�򿪣�1---����
  int scale_state;//0:�����ţ�1���Ŵ�2����С
  int is_scale;//����
  
}PicViewer_Master_Struct;
#endif /* _CDLG_PICVIEW_H_ */


