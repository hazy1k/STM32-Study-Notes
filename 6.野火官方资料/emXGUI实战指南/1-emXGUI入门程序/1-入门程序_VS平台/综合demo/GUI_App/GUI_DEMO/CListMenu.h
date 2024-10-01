
#ifndef	__CLISTMENU__
#define	__CLISTMENU__

#ifdef	__cplusplus
extern "C" {
#endif
/*============================================================================*/

#include "emXGUI.h"

/*============================================================================*/
////ListMenu 消息
enum	eMSG_LISTMENU{
	MSG_SETCFG	=WM_WIDGET+0,
	MSG_MOVE_PREV,
	MSG_MOVE_NEXT,
};


////ListMenu 私有风格标志(只能用低16位)
#define	LMS_HOR			(1<<0) //水平滑动的
#define	LMS_VER			(1<<1) //垂直滑动的
#define	LMS_ICONFRAME	(1<<2) //图标带框
#define	LMS_ICONTITLE	(1<<3) //图标带标题

////ListMenu 通知码
#define	LMN_CLICKED 0 //单击

typedef struct {
	NMHDR hdr;
	int idx;
}LM_NMHDR;


struct	__obj_list{

  /* APP下方的文字 */
	const WCHAR *pName;
	/* 图标的bmp数据内容，bmp为NULL时使用icon字符索引 */
  void *bmp;
  /* 图标的字符索引，bmp非空时本配置无效 */
  void *icon;
  /* 图标对应的应用程序 */
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
