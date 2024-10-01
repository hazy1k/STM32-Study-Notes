
#ifndef	__CLISTMENU__
#define	__CLISTMENU__

#ifdef	__cplusplus
extern "C" {
#endif
/*============================================================================*/

#include "emXGUI.h"

/*============================================================================*/
////ListMenu ��Ϣ
enum	eMSG_LISTMENU{
	MSG_SETCFG	=WM_WIDGET+0,
	MSG_MOVE_PREV,
	MSG_MOVE_NEXT,
};


////ListMenu ˽�з���־(ֻ���õ�16λ)
#define	LMS_HOR			(1<<0) //ˮƽ������
#define	LMS_VER			(1<<1) //��ֱ������
#define	LMS_ICONFRAME	(1<<2) //ͼ�����
#define	LMS_ICONTITLE	(1<<3) //ͼ�������

////ListMenu ֪ͨ��
#define	LMN_CLICKED 0 //����

typedef struct {
	NMHDR hdr;
	int idx;
}LM_NMHDR;


struct	__obj_list{

  /* APP�·������� */
	const WCHAR *pName;
	/* ͼ���bmp�������ݣ�bmpΪNULLʱʹ��icon�ַ����� */
  void *bmp;
  /* ͼ����ַ�������bmp�ǿ�ʱ��������Ч */
  void *icon;
  /* ͼ���Ӧ��Ӧ�ó��� */
	void (*cbStartup)(HWND hwnd);
};

typedef struct
{

	const struct __obj_list *list_objs;
	int x_num;
	int y_num;

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
