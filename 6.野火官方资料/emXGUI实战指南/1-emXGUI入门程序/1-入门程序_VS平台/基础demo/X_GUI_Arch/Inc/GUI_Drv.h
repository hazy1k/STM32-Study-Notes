
#ifndef	__GUI_DRV_H__
#define	__GUI_DRV_H__

#include	<stddef.h>
#include	<math.h>

#include "emXGUI.h"
#include "gui_drv_cfg.h"


#include "gui_mem_port.h"
#include "gui_input_port.h"
#include "gui_log_port.h"
#include "gui_lcd_port.h"
#include "gui_font_port.h"
#include "gui_resource_port.h"
#include  "gui_picture_port.h"

#include "emXGUI_Arch.h"

/*============================================================================*/

#define	ROTATE_0	0
#define	ROTATE_90	1
#define	ROTATE_180	2
#define	ROTATE_270	3

/*============================================================================*/
#if 0
struct	LCD_TIMING_CFG
{
   
	U8	VBPD;
	U8	VFPD;
	U8	VSPW;
   
	U8	HBPD;
   	U8	HFPD;
   	U8	HSPW;
   	
   	U8	DPL;
   	U8	EPL;

   	U8	HSPL;
   	U8	VSPL;
   	
   	U8  PCLK;

};

/*============================================================================*/

void	LCD_SPI_Init(void);
void	LCD_SPI_CS(int s);
void	LCD_SPI_MOSI(int s);
int		LCD_SPI_MISO(void);
void	LCD_SPI_SCLK(int s);
U8		LCD_SPI_RW(U8 byte);
#endif
/*============================================================================*/
////Touch State
#define	TS_ACT_NONE	 	0
#define	TS_ACT_DOWN	 	1
#define TS_ACT_UP		2

typedef	struct	__TS_CFG_DATA
{
	U16	rsv,ChkSum;
	U16	LUAdx,LUAdy;
	U16	RUAdx,RUAdy;
	U16	RDAdx,RDAdy;
	U16	LDAdx,LDAdy;
	
}TS_CFG_DATA;

#define	TouchPanel_IsPenDown() (TouchDev_GetAction()==TS_ACT_DOWN)

BOOL	TouchDev_Init(void);
int  	TouchDev_GetAction(void);
BOOL	TouchDev_GetSample(POINT *pt);
BOOL	TouchDev_GetPoint(POINT *pt);

BOOL 	TouchDev_SaveCfg(const TS_CFG_DATA *cfg);
BOOL 	TouchDev_LoadCfg(TS_CFG_DATA *cfg);

/////
void 	GUI_TouchHandler(void);

void 	TouchPanel_TranslateInit(const TS_CFG_DATA *cfg);
BOOL	TouchPanel_TranslatePoint(int width,int height,POINT *pt);
BOOL	TouchPanel_SetCalibrationSample(POINT *pt,int idx);
int		TouchPanel_GetCalibrationPoints(POINT *pt);
BOOL	TouchPanel_CalibrationStart(void);
BOOL	TouchPanel_CalibrationFinish(void);
BOOL	TouchPanel_CalibrationSucceed(void);
BOOL	TouchPanel_CalibrationFailed(void);

/*============================================================================*/
BOOL	MouseDev_Init(void);
BOOL	MouseDev_IsActive(void);
void	MouseDev_Handler(void);

/*============================================================================*/
BOOL	KeyBoardDev_Init(void);
BOOL	KeyBoardDev_IsActive(void);
void	KeyBoardDev_Handler(void);

/*============================================================================*/

void	GUI_memset8(U8 *dst,U8 dat,int count);
void	GUI_memset8_step(U8 *dst,U8 dat,int count,int step);
void	GUI_memset16(U16 *dst,U16 dat,int count);
void	GUI_memset16_step(U16 *dst,U16 dat,int count,int step);
void	GUI_memset32(U32 *dst,U32 dat,int count);
void	GUI_memset32_step(U32 *dst,U32 dat,int count,int step);

U8 GUI_AlphaBlendValue(U8 dst,U8 src,U8 src_a);

/*============================================================================*/

void GL_MEM_8BPP_Init(GL_OP *gd);
void GL_MEM_16BPP_Init(GL_OP *gd);
void GL_MEM_32BPP_Init(GL_OP *gd);
void GL_MPU_16BPP_Init(GL_OP *gd);

extern	GL_OP GL_MEM_8PP;
extern	GL_OP GL_MEM_16PP;
extern	GL_OP GL_MEM_32PP;

void	SurfaceInit_RGB332(SURFACE *pSurf,const GL_OP *gdraw,int w,int h,int line_bytes,void *bits);
void	SurfaceInit_RGB565(SURFACE *pSurf,const GL_OP *gdraw,int w,int h,int line_bytes,void *bits);
void	SurfaceInit_ARGB4444(SURFACE *pSurf,const GL_OP *gdraw,int w,int h,int line_bytes,void *bits);
void	SurfaceInit_XRGB8888(SURFACE *pSurf,const GL_OP *gdraw,int w,int h,int line_bytes,void *bits);
void	SurfaceInit_ARGB8888(SURFACE *pSurf,const GL_OP *gdraw,int w,int h,int line_bytes,void *bits);

#if	GUI_KEYBOARD_EN
BOOL	gdrvKeyboardInit(void);
void	gdrvKeyBoardHandler(void);
#endif

#if	GUI_TOUCHSCREEN_EN
BOOL	gdrvTouchScreenInit(void);
void	gdrvTouchScreenHandler(void);
#endif

#if	GUI_MOUSE_EN
BOOL	gdrvMouseInit(void);
void	gdrvMouseDataInput(const char *data);
void	gdrvMouseHandler(void);
#endif

SURFACE*	GUI_DisplayInit(void);
BOOL		gdrvSetRotate(int rotate);

BOOL    GUI_Init(void);

/*============================================================================*/

void	GUI_Printf(const char *fmt,...);

/*============================================================================*/

#endif
