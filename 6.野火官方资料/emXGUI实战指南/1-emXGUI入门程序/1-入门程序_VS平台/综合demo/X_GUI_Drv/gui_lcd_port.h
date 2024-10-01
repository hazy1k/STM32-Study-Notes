#ifndef __GUI_LCD_PORT_H
#define	__GUI_LCD_PORT_H

#ifdef __cplusplus
extern "C" {
#endif  

#include	"emXGUI.h"
#include	"gui_drv_cfg.h"


	/* 屏幕旋转类型定义 */
#define	ROTATE_0	  0
#define	ROTATE_90	1
#define	ROTATE_180	2
#define	ROTATE_270	3

/*============================================================================*/
#if	(LCD_FORMAT==COLOR_FORMAT_RGB565)

#define	LCD_COLOR	    U16
#define	LCD_BUF_SIZE	(LCD_XSIZE*LCD_YSIZE*2)

#endif

////////

#if	(LCD_FORMAT==COLOR_FORMAT_XRGB8888)

#define	LCD_COLOR       U32
#define	LCD_BUF_SIZE	(LCD_XSIZE*LCD_YSIZE*4)

#endif


	SURFACE* GUI_DisplayInit(void);


#ifdef	__cplusplus
}
#endif

#endif /* __GUI_LCD_PORT_H */
