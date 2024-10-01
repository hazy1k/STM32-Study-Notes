
#include "gui_drv.h"
/*===================================================================================*/
/* DMA2D ����ʾ��ʹ�õĶ�̬���� */
/* ��DMA2D_EN �� g_dma2d_en��Ϊ��ʱ�Ż�ʹ��dma2d*/
BOOL g_dma2d_en = TRUE;

/*
 *GPU��ͼ������������,�����֧�ֲ�������ֱ�ӷ���FALSE.
 */

#if(DMA2D_EN)
BOOL DMA2D_FillRect(const SURFACE *pSurf,int x,int y,U16 w,U16 h,U32 color);
BOOL DMA2D_DrawBitmap_RGB565(const SURFACE *pSurf,int x,int y,U16 w,U16 h,int width_bytes,const U8 *bits);
BOOL DMA2D_DrawBitmap_ARGB(const SURFACE *pSurf,int x,int y,U16 w,U16 h,int width_bytes,const U8 *bits,U32 color_format);

#endif

/*===================================================================================*/

#if 0
BOOL	GUI_GPU_Init(void)
{

	return TRUE;
}
#endif

/*============================================================================*/
/*
 * ��ָ������ԭʼ���ظ��Ƶ�out������.
 */
BOOL 	GPU_CopyBits(const SURFACE *pSurf,int x,int y,int w,int h,void *out,int width_bytes)
{
	return FALSE;
}

/*============================================================================*/
/*
 * �������.
 */
BOOL	GPU_FillRect(const SURFACE *pSurf,int x,int y,int w,int h,COLORREF c)
{
#if(DMA2D_EN)
  if(g_dma2d_en)
	{
		if(DMA2D_FillRect(pSurf,x,y,w,h,c))
		{
			return TRUE;
		}
		return FALSE;
	}
  else
    return FALSE;
#else
	return FALSE;
#endif
}

/*============================================================================*/
/*
 * �������(ARGB��ʽ).
 */
BOOL	GPU_FillRectARGB(const SURFACE *pSurf,int x,int y,int w,int h,U8 a,U8 r,U8 g,U8 b)
{
	return FALSE;
}

/*============================================================================*/
/*
 * ����λͼ.
 */
BOOL	GPU_DrawBitmap(const SURFACE *pSurf,int x,int y,int w,int h,const U8 *bits,int width_bytes,int format)
{
#if(DMA2D_EN)
  if(g_dma2d_en)	
	{
		switch(format)
		{

		case BM_RGB565:
			return	DMA2D_DrawBitmap_RGB565(pSurf,x,y,w,h,width_bytes,bits);

		case BM_ARGB8888:
			return	DMA2D_DrawBitmap_ARGB(pSurf,x,y,w,h,width_bytes,bits,CM_ARGB8888);

		default:
			return FALSE;

		}
	}
  else
    return FALSE;
#else
	return FALSE;
#endif

}

/*============================================================================*/
/*
 * ����λͼ.
 */
BOOL 	GPU_ScaleBitmap(const SURFACE *pSurf,int x,int y,int w,int h,const BITMAP *bm)
{
#if(DMA2D_EN)
	{
	return FALSE;
	}
#else
	return FALSE;
#endif
}

/*============================================================================*/


