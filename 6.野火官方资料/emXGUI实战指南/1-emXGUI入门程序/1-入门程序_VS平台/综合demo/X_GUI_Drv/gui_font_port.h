#ifndef __GUI_FONT_PORT_H
#define	__GUI_FONT_PORT_H

#ifdef __cplusplus
extern "C" {
#endif 
  
#include	"emXGUI.h"
#include	"gui_drv_cfg.h"

/* 默认字体 */
extern HFONT defaultFont;
/* logo字体 */
extern HFONT logoFont;
/* 图标字体 */
extern HFONT iconFont;
/* 控制图标字体 */
extern HFONT controlFont;
/* 其它 */
extern HFONT GB2312_32_Font;

HFONT GUI_Default_FontInit(void);

#ifdef	__cplusplus
}
#endif

#endif /* __GUI_FONT_PORT_H */
