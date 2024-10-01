#ifndef _EXITBUTTON_H_
#define _EXITBUTTON_H_

#include <emXGUI.h>


typedef enum
{
   dir_V,//��ֱ
   dir_H //ˮƽ
}dir_e;

//�ؼ�ͼ�����ṹ��
typedef struct{
   char *icon_name;//ͼ����
   RECT rc;        //λ����Ϣ
   BOOL state;     //״̬  
}icon_S;

typedef struct
{
   dir_e dir;//����������
   int width;//ֻ���ڴ�ֱ������
   int high;//ֻ����ˮƽ������
   COLOR_RGB32 back_c;//����ɫ
   
   COLOR_RGB32 page_c;//�ǻ���������ɫ
   COLOR_RGB32 fore_c;//������ɫ
 
}ScrollBar_S;


extern void home_owner_draw(DRAWITEM_HDR *ds);
extern void scrollbar_owner_draw(DRAWITEM_HDR *ds, ScrollBar_S ScrollBar_parm1, ScrollBar_S ScrollBar_parm2);
#endif /* _EXITBUTTON_H_ */

