#ifndef _GUI_RGBLED_DIALOG_H_
#define _GUI_RGBLED_DIALOG_H_

#include <emXGUI.h>

//#if 0
//  #define GUI_RGB_BACKGROUNG_PIC      "0:/rgbled_desktop.jpg"
//  #define GUI_RGBLED_CHECKED_PIC      "0:/rgbled_checked.png"
//  #define GUI_RGB_SLIDER_PIC          "0:/slider.png"
//  #define GUI_RGB_SLIDER_BTN_PIC      "0:/slider_button.png"
//#else
//  #define GUI_RGB_BACKGROUNG_PIC      "rgbled_desktop.jpg"
//  #define GUI_RGBLED_CHECKED_PIC      "rgbled_checked.png"
//  #define GUI_RGB_SLIDER_PIC          "slider.png"
//  #define GUI_RGB_SLIDER_BTN_PIC      "slider_button.png"
//#endif

#define APP_BackColor                ARGB8888(0,169,169,169)

//��������ɫ����
#define ID_TEXTBOX_Title_BackColor   APP_BackColor          //������ɫ
#define ID_TEXTBOX_Title_TextColor   RGB888(255,255,255)    //������ɫ

#define ID_TEXTBOX_APP_BackColor     APP_BackColor 
#define ID_TEXTBOX_R_LED_BackColor   APP_BackColor
#define ID_TEXTBOX_G_LED_BackColor   APP_BackColor
#define ID_TEXTBOX_B_LED_BackColor   APP_BackColor
#define ID_TEXTBOX_R_NUM_BackColor   APP_BackColor
#define ID_TEXTBOX_R_BackColor       APP_BackColor
#define ID_TEXTBOX_G_BackColor       APP_BackColor
#define ID_TEXTBOX_B_BackColor       APP_BackColor
#define ID_SCROLLBAR_BackColor       APP_BackColor

enum GUI_RGBLEDID
{
   /*������ID�궨��*/
   ID_SCROLLBAR_R = 0x1100,
   ID_SCROLLBAR_G  ,
   ID_SCROLLBAR_B ,

   /* RGB ѡ�� */
   ID_RGB_SELECT_R,
   ID_RGB_SELECT_G,
   ID_RGB_SELECT_B,

   /*�ı���ID�궨��*/
   ID_TEXTBOX_PCT,
   ID_TEXTBOX_SMALL,
   ID_TEXTBOX_BIG,

   ID_TEXTBOX_Title,
  
   ID_TEXTBOX_R_LED,   
   ID_TEXTBOX_R_NUM,
   ID_TEXTBOX_G,	 
  
   ID_TEXTBOX_G_LED,   
   ID_TEXTBOX_G_NUM,  
   ID_TEXTBOX_B,
  
   ID_TEXTBOX_B_LED,   
   ID_TEXTBOX_B_NUM,  
   /*��ťID�궨��*/
   ID_EXIT,  
   ID_CHECKBOX_SW, 
   
   
};
typedef struct
{
   HWND RGBLED_Hwnd;//���ھ��
   BOOL State;      //Ӳ��RGB�ƵĿ���
   HDC hdc_mem;
  
	 SCROLLINFO sif_R;//��ɫ����ֵ
   SCROLLINFO sif_G;//��ɫ����ֵ
   SCROLLINFO sif_B;//��ɫ����ֵ
      
   u8 *jpg_buf;     //����ͼƬ����
   u32 jpg_size;    //����ͼƬ��С
   int col_R;       //��ɫ������ʼֵ
   int col_G;       //��ɫ������ʼֵ
   int col_B;       //��ɫ������ʼֵ
   void (* exit_bt_draw)(DRAWITEM_HDR*);//�����˳���ť�ص�����
}RGBLED_DIALOG_s;
#endif /* _GUI_RGBLED_DIALOG_H_ */


