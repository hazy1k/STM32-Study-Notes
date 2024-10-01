
#ifndef	__CLISTMENU__
#define	__CLISTMENU__

#ifdef	__cplusplus
extern "C" {
#endif
    /*============================================================================*/

#include "emXGUI.h"

/*============================================================================*/
//ListMenu ��Ϣ
enum	eMSG_LISTMENU {
  MSG_SETCFG = WM_WIDGET + 0,
  MSG_MOVE_PREV,
  MSG_MOVE_NEXT,
  MSG_SET_SEL,              // [����ѡ�еĶ���]: <wParam>���ñ�ѡ�е�����ֵ; <lParam>����; <����>����.
  MSG_LIST_DRAG_ENABLE,     // [�б���ʹ��]: <wParam>TRUE:ʹ��;FALSE����ֹ; <lParam>����; <����>����.
  MSG_LISTMENU_END,
  MSG_SET_BGCOLOR,          // [���ñ�����ɫ]: <wParam>Ҫ���õ���ɫֵ;1����ʹ�õ�ǰ��ɫ��ʹ��ͼƬ��Ϊ��
};


//ListMenu ˽�з���־(ֻ���õ�16λ)
#define	LMS_HOR			        (1<<0)    // ˮƽ������
#define	LMS_VER			        (1<<1)    // ��ֱ������
#define	LMS_ICONFRAME	      (1<<2)    // ͼ�����
#define	LMS_PAGEMOVE	      (1<<3)    // ����ҳ�ƶ�.
#define	LMS_ICONTITLE	      (1<<4)    // ͼ�������
#define	LMS_ICONINNERFRAME	(1<<5)    // ͼ����ڿ�
#define LMS_TOUCHSHADOW	    (1<<6)    // ����ʱ�Ƿ����Ӱ

////ListMenu ֪ͨ��
#define	LMN_CLICKED   0 //����
#define	LMN_SELCHANGE 1

    typedef struct {
        NMHDR hdr;
        int idx;
    }LM_NMHDR;


    struct	__obj_list {

        /* APP�·������� */
        const WCHAR *pName;
        /* ͼ���bmp�������ݣ�bmpΪNULLʱʹ��icon�ַ����� */
        const void *bmp;
        /* ͼ����ַ�������bmp�ǿ�ʱ��������Ч */
        void *icon;
        /* ͼ��ʹ�õ���ɫ */
        u32 color;
        /* ͼ���Ӧ��Ӧ�ó��� */
        void(*cbStartup)(HWND hwnd);
    };

    typedef struct
    {

        const struct __obj_list *list_objs;
        int x_num;
        int y_num;
        u32 bg_color; /* ������ɫ */

    }list_menu_cfg_t;



    extern const WNDCLASS wcex_ListMenu;

    /*============================================================================*/
    /*============================================================================*/
    /*============================================================================*/
    /*============================================================================*/
#ifdef	__cplusplus
}
#endif
#endif /*__CLISTMENU__*/
