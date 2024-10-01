
/*----------------------------------------------------------------------------/
/ emXGUI - ͨ��ͼ�ν������
/-----------------------------------------------------------------------------/
/
/ Copyright (C) 2006-2018, Liuwei, all right reserved.
/...
/----------------------------------------------------------------------------*/

#ifndef	__EMXGUI_H_20190904_1705__
#define	__EMXGUI_H_20190904_1705__

#ifdef	__cplusplus
extern	"C"{
#endif

#include	"def.h"

#ifndef	_T
#define	_T(s) L##s
#endif


/*============================================================================*/
//��������

typedef	U32 			COLOR_RGB32;
typedef	U32	            COLORREF;
typedef const void*     LPCVOID;
typedef U32 	        WPARAM;
typedef LONG 	        LPARAM;
typedef LONG 	        LRESULT;

typedef	struct	tagDC*			HDC;
typedef	struct	tagWINDOW*		HWND;
typedef	struct	tagRECT_LL*		HRGN;
typedef	struct  tagGUI_TIMER*	HTMR;
typedef	struct  tagFONT_OBJ*	HFONT;
typedef	struct  tagBMP_CTX*		HBMP;
typedef	struct	tagSURFACE		SURFACE;

	
typedef	HANDLE		HGDIOBJ;
typedef	HANDLE		HBITMAP;
typedef	HANDLE		HPEN;
typedef	HANDLE		HBRUSH;
typedef	HANDLE		HMENU;
typedef	HANDLE		HICON;
typedef	HANDLE		HCURSOR;
typedef HANDLE      HINSTANCE;

/*============================================================================*/
typedef	int	(FN_ReadData)(void *buf,int offset,int size,LPARAM lParam);

typedef	struct	
{
	LPARAM lParam;           //�ò���ֵ������pfReadData�ص�������
	FN_ReadData *pfReadData; //��ȡ���ݵĻص�����ָ�롣
	
}GUI_GET_DATA;

/*============================================================================*/

#define	MAKE_WPARAM(h,l)	((h&0xFFFF)<<16)|(l&0xFFFF)
#define	MAKE_LPARAM(h,l)	((h&0xFFFF)<<16)|(l&0xFFFF)

//#define MAKEWORD(a, b)      ((U16)(((U8)(a)) | ((U16)((U8)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((U16)(a)) | ((U32)((U16)(b))) << 16))
#define LOWORD(l)           ((U16)(l))
#define HIWORD(l)           ((U16)(((U32)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((U8)(w))
#define HIBYTE(w)           ((U8)(((U16)(w) >> 8) & 0xFF))

#define	MAKE_LPARAM_XY(x,y)	((y&0xFFFF)<<16)|(x&0xFFFF)
#define	GET_LPARAM_X(l) 	(S16)(l)
#define GET_LPARAM_Y(l) 	(S16)((l>>16)&0xFFFF)

/*============================================================================*/

typedef	struct	tagSIZE
{
	int	cx;
	int	cy;
}SIZE;

typedef	struct	tagSIZE16
{
	U16	cx;
	U16	cy;
}SIZE16;

typedef	struct	tagPOINT
{
	int x,y;
}POINT,*LPPOINT;

typedef	struct tagRECT
{
	int  x,y,w,h;
}RECT;

/*============================================================================*/

typedef	void	(*TIMERPROC)(HWND hwnd,UINT uMsg,UINT idEvent,U32 dwTime);
typedef LRESULT (*WNDPROC)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef LRESULT	(*DLGPROC)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef BOOL 	(*WNDENUMPROC)(HWND hwnd, LPARAM lParam);
typedef BOOL	(*WNDERASEBKGND)(HDC hdc,const RECT *lprc,HWND hwnd);


/*============================================================================*/

#if 0
/* Graphics Modes */
#define GM_COMPATIBLE       1
#define GM_ADVANCED         2
#define GM_LAST             2

/* Draw Flags*/
//#define	ANTI_ALIAS_FLAG		(1<<0)		//����ݱ��
//#define	FILTER_BITMAP_FLAG	(1<<1)		//λͼ���ع���
#endif

/* Text Alignment Options */
#define TA_NOUPDATECP        0
#define TA_UPDATECP          1

#define TA_TOP              (0<<0)  //��ֱ��������.
#define TA_BOTTOM           (1<<0)  //��ֱ�ײ�����.
#define	TA_CENTER			(2<<0)  //��ֱ�м����.
#define TA_V_MASK			(3<<0)

#define TA_LEFT             (0<<2)  //ˮƽ�������.
#define TA_RIGHT            (1<<2)  //ˮƽ���Ҷ���.
#define TA_VCENTER          (2<<2)  //ˮƽ���ж���.
#define	TA_H_MASK			(3<<2)

#define TA_BASELINE         (1<<4)
#define TA_RTLREADING       (1<<5)
/*
#define TA_MASK       (TA_BASELINE+TA_CENTER+TA_UPDATECP+TA_RTLREADING)

#define VTA_BASELINE TA_BASELINE
#define VTA_LEFT     TA_BOTTOM
#define VTA_RIGHT    TA_TOP
#define VTA_CENTER   TA_CENTER
#define VTA_BOTTOM   TA_RIGHT
#define VTA_TOP      TA_LEFT
*/




/*
 * DrawText() Format Flags
 */
#define DT_TOP          (0<<0)  //��ֱ��������.
#define DT_BOTTOM       (1<<0)  //��ֱ�ײ�����.
#define DT_VCENTER      (2<<0)  //��ֱ�м����.
#define DT_ALIGN_MASK_V (3<<0)  //��ֱ��������.

#define DT_LEFT         (0<<2)  //ˮƽ�������.
#define DT_RIGHT        (1<<2)  //ˮƽ���Ҷ���.
#define DT_CENTER       (2<<2)  //ˮƽ���ж���.
#define DT_ALIGN_MASK_H (3<<2)  //ˮƽ��������.

#define DT_SINGLELINE   (1<<4)  //����ģʽ.
#define DT_NOCLIP       (1<<5)  //�����ַ����м���.
#define	DT_BORDER		(1<<6)	//�Ƿ���Ʊ߿�.
#define	DT_BKGND		(1<<7)	//�Ƿ���Ʊ���.
#define DT_WORDBREAK	(1<<8)  //���ַ��������α߽�ʱ,�Զ�����(ע:�ڶ���ģʽ+DT_LEFT+DT_TOPģʽ����Ч).

typedef struct
{
    u16    cbSize;     // ����Ϊ DRAWTEXTPARAMS�ṹ�� ���ֽ�����
    u16    TabLength;  // �Ʊ����ռ�ַ����ȡ�
    int	   iXOffset;   // �ַ����ھ����ڵ� X ����ƫ�ơ�
    int	   iYOffset;   // �ַ����ھ����ڵ� Y ����ƫ�ơ�
} DRAWTEXTPARAMS;

#if 0 ////�°治����Щ������...

#define SRCCOPY             (UINT)0x00CC0020 /* dest = source                   */
#define SRCPAINT            (UINT)0x00EE0086 /* dest = source OR dest           */
#define SRCAND              (UINT)0x008800C6 /* dest = source AND dest          */
#define SRCINVERT           (UINT)0x00660046 /* dest = source XOR dest          */
#define SRCERASE            (UINT)0x00440328 /* dest = source AND (NOT dest )   */
#define NOTSRCCOPY          (UINT)0x00330008 /* dest = (NOT source)             */
#define NOTSRCERASE         (UINT)0x001100A6 /* dest = (NOT src) AND (NOT dest) */
#define MERGECOPY           (UINT)0x00C000CA /* dest = (source AND pattern)     */
#define MERGEPAINT          (UINT)0x00BB0226 /* dest = (NOT source) OR dest     */
#define PATCOPY             (UINT)0x00F00021 /* dest = pattern                  */
#define PATPAINT            (UINT)0x00FB0A09 /* dest = DPSnoo                   */
#define PATINVERT           (UINT)0x005A0049 /* dest = pattern XOR dest         */
#define DSTINVERT           (UINT)0x00550009 /* dest = (NOT dest)               */
#define BLACKNESS           (UINT)0x00000042 /* dest = BLACK                    */
#define WHITENESS           (UINT)0x00FF0062 /* dest = WHITE                    */


#define MAKEROP4(fore,back) (UINT)((((back) << 8) & 0xFF000000) | (fore))

#else ////

#define SRCCOPY 			(UINT)0

#endif
/*============================================================================*/

#define	GUI_XSIZE		GetSystemMetrics(SM_CXSCREEN)
#define	GUI_YSIZE		GetSystemMetrics(SM_CYSCREEN)

#define	SCREEN_XSIZE	GetSystemMetrics(SM_CXSCREEN)
#define	SCREEN_YSIZE	GetSystemMetrics(SM_CYSCREEN)

#define	HWND_DESKTOP	GetDesktopWindow()

/*============================================================================*/

/*
 * GetSystemMetrics() codes
 */

#define SM_CXSCREEN             0
#define SM_CYSCREEN             1
#define SM_CXVSCROLL            2
#define SM_CYHSCROLL            3
#define SM_CYCAPTION            4
#define SM_CXBORDER             5
#define SM_CYBORDER             6
#define SM_CXDLGFRAME           7
#define SM_CYDLGFRAME           8
#define SM_CYVTHUMB             9
#define SM_CXHTHUMB             10
#define SM_CXICON               11
#define SM_CYICON               12
#define SM_CXCURSOR             13
#define SM_CYCURSOR             14
#define SM_CYMENU               15
#define SM_CXFULLSCREEN         16
#define SM_CYFULLSCREEN         17
#define SM_CYKANJIWINDOW        18
#define SM_MOUSEPRESENT         19
#define SM_CYVSCROLL            20
#define SM_CXHSCROLL            21
#define SM_DEBUG                22
#define SM_SWAPBUTTON           23
#define SM_RESERVED1            24
#define SM_RESERVED2            25
#define SM_RESERVED3            26
#define SM_RESERVED4            27
#define SM_CXMIN                28
#define SM_CYMIN                29
#define SM_CXSIZE               30
#define SM_CYSIZE               31
#define SM_CXFRAME              32
#define SM_CYFRAME              33
#define SM_CXMINTRACK           34
#define SM_CYMINTRACK           35
#define SM_CXDOUBLECLK          36
#define SM_CYDOUBLECLK          37
#define SM_CXICONSPACING        38
#define SM_CYICONSPACING        39
#define SM_MENUDROPALIGNMENT    40
#define SM_PENWINDOWS           41
#define SM_DBCSENABLED          42
#define SM_CMOUSEBUTTONS        43

/*============================================================================*/

////BlendOp
#define AC_SRC_OVER                 0x00

////BlendFlags
#define AC_SRC_NO_PREMULT_ALPHA     0x01
#define AC_SRC_NO_ALPHA             0x02
#define AC_DST_NO_PREMULT_ALPHA     0x10
#define AC_DST_NO_ALPHA             0x20

typedef struct _BLENDFUNCTION
{
    U8   BlendOp;
    U8   BlendFlags;
    U8   SourceConstantAlpha;
    U8   AlphaFormat;
}BLENDFUNCTION,*PBLENDFUNCTION;

/*============================================================================*/
#if 0
typedef struct tagRGBTRIPLE {
        U8    rgbtBlue;
        U8    rgbtGreen;
        U8    rgbtRed;
}RGBTRIPLE;

typedef struct tagARGBQUAD {
        U8    B;
        U8    G;
        U8    R;
        U8    A;
}ARGBQUAD;

typedef union
{
		struct
		{
			U8    B;
			U8    G;
			U8    R;
			U8    A;
		}C;

		U32 U;
}COLORREFx;
#endif

/*============================================================================*/

#define	COLOR_FORMAT_LUT1		1  //1λ����ɫ.
#define	COLOR_FORMAT_LUT2		2  //2λ����ɫ.
#define	COLOR_FORMAT_LUT4		3  //4λ����ɫ.
#define	COLOR_FORMAT_LUT8		4  //8λ����ɫ.
#define	COLOR_FORMAT_XRGB2222	5  //XXRRGGBB.
#define	COLOR_FORMAT_ARGB2222	6  //AARRGGBB.
#define	COLOR_FORMAT_RGB332		7  //RRRGGGBB.
#define	COLOR_FORMAT_RGB565		8  //RRRRRGGGGGGBBBBB
#define	COLOR_FORMAT_XRGB1555	9  //XRRRRRGGGGGBBBBB
#define	COLOR_FORMAT_ARGB1555	10 //ARRRRRGGGGGBBBBB
#define	COLOR_FORMAT_XRGB4444	11 //XXXXRRRRGGGGBBBB
#define	COLOR_FORMAT_ARGB4444	12 //AAAARRRRGGGGBBBB
#define	COLOR_FORMAT_RGB888		13 //RRRRRRRRGGGGGGGGBBBBBBBB
#define	COLOR_FORMAT_XRGB8888	14 //XXXXXXXXRRRRRRRRGGGGGGGGBBBBBBBB
#define	COLOR_FORMAT_ARGB8888	15 //AAAAAAAARRRRRRRRGGGGGGGGBBBBBBBB
#define	COLOR_FORMAT_AL1		16 //1λAlpha/�Ҷ�.
#define	COLOR_FORMAT_AL2		17 //2λAlpha/�Ҷ�.
#define	COLOR_FORMAT_AL4		18 //4λAlpha/�Ҷ�.
#define	COLOR_FORMAT_AL8		19 //8λAlpha/�Ҷ�.

#define	RGB332(r,g,b)		((r&0x7)<<5)|((g&0x7)<<2)|(b&0x3)
#define	RGB565(r,g,b)		((r&0x1F)<<11)|((g&0x3F)<<5)|(b&0x1F)
#define	XRGB1555(r,g,b)		((r&0x1F)<<10)|((g&0x1F)<<5)|(b&0x1F)
#define	ARGB1555(a,r,g,b)	((a<<15)|(r&0x1F)<<10)|((g&0x1F)<<5)|(b&0x1F)
#define	ARGB4444(a,r,g,b)	((a<<12)|((r&0xF)<<8)|((g&0xF)<<4)|(b&0xF))
#define	RGB888(r,g,b)		((r&0xFF)<<16)|((g&0xFF)<<8)|(b&0xFF)
#define	XRGB8888(r,g,b)		((r&0xFF)<<16)|((g&0xFF)<<8)|(b&0xFF)
#define	ARGB8888(a,r,g,b)	((a&0xFF)<<24)|((r&0xFF)<<16)|((g&0xFF)<<8)|(b&0xFF)

#define	RGB565toRGB332(c)	RGB332((c>>(11+2)),(c>>(5+3)),(c>>3))
#define	XRGB1555toRGB332(c)	RGB332((c>>(10+2)),(c>>(5+2)),(c>>3))
#define	XRGB4444toRGB332(c) RGB332(((c>>8)&0xF)>>1,((c>>4)&0xF)>>1,((c&0xF)>>2))
#define	XRGB8888toRGB332(c) RGB332(c>>(16+5),c>>(8+5),c>>6)
#define	ARGB8888toRGB332(c)	RGB332(c>>(16+5),c>>(8+5),c>>6)

#define	RGB332toRGB565(c)	RGB565((c>>5)<<2,(c>>2)<<3,(c<<3))
#define	XRGB1555toRGB565(c)	RGB565((c>>10),(c>>5)<<1,c)
#define	XRGB4444toRGB565(c) RGB565(((c>>8)&0xF)<<1,((c>>4)&0xF)<<2,((c&0xF)<<1))
#define	XRGB8888toRGB565(c) RGB565((c>>(16+3)),(c>>(8+2)),(c>>3))
#define	ARGB8888toRGB565(c)	RGB565((c>>(16+3)),(c>>(8+2)),(c>>3))

#define	RGB332toARGB4444(c)		ARGB4444(0xF,(c>>5)<<1,(c>>2)<<1,c<<2)
#define	RGB565toARGB4444(c) 	ARGB4444(0xF,(c>>(11+1)),(c>>(5+2)),(c>>1))
#define	XRGB1555toARGB4444(c)	ARGB4444(0xF,(c>>(10+1)),(c>>(5+1)),(c>>1))
#define	ARGB1555toARGB4444(c)	ARGB4444((c>>15)<<3,(c>>(10+1)),(c>>(5+1)),(c>>1))
#define	XRGB8888toARGB4444(c)	ARGB4444((c>>(24+4)),(c>>(16+4)),(c>>(8+4)),(c>>4))
#define	ARGB8888toARGB4444(c)	ARGB4444((c>>(24+4)),(c>>(16+4)),(c>>(8+4)),(c>>4))

#define	RGB332toXRGB8888(c)		XRGB8888((c>>5)<<5,(c>>2)<<5,(c<<6))
#define	RGB565toXRGB8888(c)		XRGB8888((c>>11)<<3,(c>>5)<<2,(c<<3))
#define	XRGB1555toXRGB8888(c)	XRGB8888((c>>10)<<3,(c>>5)<<3,(c<<3))
#define	XRGB4444toXRGB8888(c) 	XRGB8888(((c>>8)&0xF)<<4,((c>>4)&0xF)<<4,((c&0xF)<<4))

#define	RGB332toARGB8888(c)		ARGB8888(0xFF,(c>>5)<<5,(c>>2)<<5,(c<<6))
#define	RGB565toARGB8888(c)		ARGB8888(0xFF,(c>>11)<<3,(c>>5)<<2,(c<<3))
#define	XRGB1555toARGB8888(c)	ARGB8888(0xFF,(c>>10)<<3,(c>>5)<<3,(c<<3))
#define	ARGB1555toARGB8888(c)	ARGB8888((c>>15)<<8,(c>>10)<<3,(c>>5)<<3,(c<<3))
#define	XRGB4444toARGB8888(c) 	ARGB8888(0xFF,((c>>8)&0xF)<<4,((c>>4)&0xF)<<4,((c&0xF)<<4))
#define	ARGB4444toARGB8888(c) 	ARGB8888(((c>>12)&0xF)<<4,((c>>8)&0xF)<<4,((c>>4)&0xF)<<4,((c&0xF)<<4))


/*============================================================================*/

//#define	BM_DEVICE	0

typedef enum
{
	BM_LUT1	= COLOR_FORMAT_LUT1,
	BM_LUT2	= COLOR_FORMAT_LUT2,
	BM_LUT4	= COLOR_FORMAT_LUT4,
	BM_LUT8	= COLOR_FORMAT_LUT8,
	BM_RGB332 = COLOR_FORMAT_RGB332,
	BM_RGB565 = COLOR_FORMAT_RGB565,
	BM_XRGB1555 = COLOR_FORMAT_XRGB1555,
	BM_ARGB1555 = COLOR_FORMAT_ARGB1555,
	BM_ARGB4444 = COLOR_FORMAT_ARGB4444,
	BM_RGB888   = COLOR_FORMAT_RGB888,
	BM_XRGB8888 = COLOR_FORMAT_XRGB8888,
	BM_ARGB8888 = COLOR_FORMAT_ARGB8888,
	BM_AL1 = COLOR_FORMAT_AL1,
	BM_AL2 = COLOR_FORMAT_AL2,
	BM_AL4 = COLOR_FORMAT_AL4,
	BM_AL8 = COLOR_FORMAT_AL8,

}BITMAP_FORMAT;


typedef enum
{   
   SURF_SCREEN      = 0,
   SURF_RGB332      = COLOR_FORMAT_RGB332,
   SURF_RGB565      = COLOR_FORMAT_RGB565,
   SURF_ARGB4444    = COLOR_FORMAT_ARGB4444,
   SURF_XRGB8888    = COLOR_FORMAT_XRGB8888,
   SURF_ARGB8888    = COLOR_FORMAT_ARGB8888,
   
}SURF_FORMAT;


typedef struct tagBITMAPINFO
{
    U32      Format;
    U32      Width;
    U32      Height;
    U32      WidthBytes;
} BITMAPINFO;	
									   
typedef struct tagBITMAP
{
    U32      Format;     //λͼ��ʽ.
    U32      Width;      //λͼ���(��).
    U32      Height;     //λͼ�߶�(��).
    U32      WidthBytes; //λͼͼ��ÿһ�е��ֽ���.
    LPVOID   Bits;       //ָ��λͼ����.
    COLORREF *LUT;       //��ɫ��,ֻ������λͼ,BM_ALPHA4,BM_ALPHA8��ʽʱ���õ�.
} BITMAP;	

/*============================================================================*/

/*
 * GetDCEx() flags
 */
#define DCX_WINDOW           0x00000001L
//#define DCX_CACHE            0x00000002L
//#define DCX_NORESETATTRS     0x00000004L
#define DCX_CLIPCHILDREN     0x00000008L
#define DCX_CLIPSIBLINGS     0x00000010L
#define DCX_PARENTCLIP       0x00000020L

//#define DCX_EXCLUDERGN       0x00000040L
//#define DCX_INTERSECTRGN     0x00000080L

//#define DCX_EXCLUDEUPDATE    0x00000100L
//#define DCX_INTERSECTUPDATE  0x00000200L

//#define DCX_LOCKWINDOWUPDATE 0x00000400L

//#define DCX_VALIDATE         0x00200000L

/*============================================================================*/

/* Pen Styles */
#define PS_SOLID            0
#define PS_DASH             1       /* -------  */
#define PS_DOT              2       /* .......  */
#define PS_DASHDOT          3       /* _._._._  */
#define PS_DASHDOTDOT       4       /* _.._.._  */
#define PS_NULL             5
#define PS_INSIDEFRAME      6
#define PS_USERSTYLE        7
#define PS_ALTERNATE        8
#define PS_STYLE_MASK       0x0000000F

#define PS_ENDCAP_ROUND     0x00000000
#define PS_ENDCAP_SQUARE    0x00000100
#define PS_ENDCAP_FLAT      0x00000200
#define PS_ENDCAP_MASK      0x00000F00

#define PS_JOIN_ROUND       0x00000000
#define PS_JOIN_BEVEL       0x00001000
#define PS_JOIN_MITER       0x00002000
#define PS_JOIN_MASK        0x0000F000

#define PS_COSMETIC         0x00000000
#define PS_GEOMETRIC        0x00010000
#define PS_TYPE_MASK        0x000F0000

/* Brush Styles */
#define BS_SOLID            0
#define BS_NULL             1
#define BS_HOLLOW           BS_NULL
#define BS_HATCHED          2
#define BS_PATTERN          3
#define BS_INDEXED          4
#define BS_DIBPATTERN       5
#define BS_DIBPATTERNPT     6
#define BS_PATTERN8X8       7
#define BS_DIBPATTERN8X8    8
#define BS_MONOPATTERN      9

#if 0
/* Hatch Styles */
#define HS_HORIZONTAL       0       /* ----- */
#define HS_VERTICAL         1       /* ||||| */
#define HS_FDIAGONAL        2       /* \\\\\ */
#define HS_BDIAGONAL        3       /* ///// */
#define HS_CROSS            4       /* +++++ */
#define HS_DIAGCROSS        5       /* xxxxx */
#endif

/*============================================================================*/

typedef	struct	tagIMAGE_INFO
{
	U8	Tag[7];
	U8	Bpp;
	U16	Width;
	U16	Height;
	
}IMAGE_INFO;

/*============================================================================*/
//FONT_ATTR Mode
#if 0
#define	FT_DEFAULT		0x0000
#define	FT_ITALIC		(1<<1)	//б��
#define	FT_BOLD			(1<<2)	//����
#define	FT_UNDERLINE	(1<<3)	//�»���
#define	FT_OUTLINE		(1<<4)	//���
#define	FT_HATCH		(1<<5)	//��Ӱ
#endif

////FONT_INFO.Flag
#define	FT_FIX		(1<<0) //�ȿ�����.
#define	FT_VAR		(1<<1) //�ǵȿ�����.
#define	FT_SMOOTH	(1<<2) //ƽ������.

typedef struct //������Ϣ�ṹ��.
{
	U16 Flag;   //������.
	S16	Height; //����߶�.
	S16 Width;  //������.
	U16 Rsv;    //����.

}FONT_INFO;

typedef struct //�ַ���Ϣ�ṹ��.
{
	S16 Width,Height; //�ַ��Ŀ�Ⱥ͸߶�.
	S16 X0,Y0; //�ַ���ʼX,Y����.
	S16 X1,Y1; //�ַ�����X,Y����.
}CHAR_INFO;


typedef	BOOL (FN_CreateFont)(const void **handler,const void *pdata);
typedef	BOOL (FN_DeleteFont)(const void *handler);
typedef	BOOL (FN_GetFontInfo)(const void *handler,FONT_INFO *fnt_info);
typedef	BOOL (FN_GetCharInfo)(const void *handler,CHAR_INFO *chr_info,int chr);
typedef	BOOL (FN_DrawChar)(const void *handler,HDC hdc,int x,int y,int chr,COLORREF color,CHAR_INFO *chr_info);


typedef	struct //��������������ṹ��(�ص�����).
{	
    FN_CreateFont   *pfCreateFont;    //��������.
    FN_DeleteFont   *pfDeleteFont;    //ɾ������.
	FN_GetFontInfo	*pfGetFontInfo;   //���������Ϣ.
	FN_GetCharInfo	*pfGetCharInfo;   //���ָ���ַ���Ϣ.
	FN_DrawChar		*pfDrawChar;      //���Ƶ���ָ���ַ�.
}FONT_OPS;


/*============================================================================*/
// ��������������.

typedef enum{
	PB_ORG_LEFT  =0, //������������.
	PB_ORG_RIGHT,    //������������.
	PB_ORG_TOP,      //������������.
	PB_ORG_BOTTOM,   //������������.
}ePB_ORG;

/*============================================================================*/

#define	WNDCLASS_TAG	((U32)0xFFFE0000)

//��������
#define CS_VREDRAW          0x00000001
#define CS_HREDRAW          0x00000002
//#define CS_DBLCLKS          0x00000008
//#define CS_OWNDC            0x00000020
//#define CS_CLASSDC          0x00000040
//#define CS_PARENTDC         0x00000080
//#define CS_NOCLOSE          0x00000200
//#define CS_SAVEBITS         0x00000800
//#define CS_BYTEALIGNCLIENT  0x00001000
//#define CS_BYTEALIGNWINDOW  0x00002000
//#define CS_GLOBALCLASS      0x00004000
//#define CS_IME              0x00010000

typedef struct tagWNDCLASS{

    U32		  Tag;          /*  ��������Ϊ WNDCLASS_TAGֵ. */
    U32       Style;        /*  ��������.  */
    WNDPROC   lpfnWndProc;  /*  ���ڹ��̺��� . */
    U32       cbClsExtra;   /*  ��������չ���ݴ�С.  */
    U32       cbWndExtra;   /*  ������չ���ݴ�С.  */
    HINSTANCE hInstance;
    HICON     hIcon;
    HCURSOR   hCursor;
    
} WNDCLASS;

/*============================================================================*/

typedef struct	tagNCPAINT_STRUCT{

	S16 Hit;             // ����ڴ����е�λ��.
	U8  BorderSize;      // С�߿��С(���).
	U8  DlgFrameSize;    // ��߿��С(���).
	UINT Width,Height;   // ���ڵĿ�Ⱥ͸߶�.
	const WCHAR *pText;  // ��������.
	RECT rcCaption;      // �����ľ��β���.
	RECT rcCloseBox;     // "�ر�"��ť�ľ��β���.

}NCPAINT_STRUCT;

typedef struct tagPAINTSTRUCT {
    HDC         hdc;
    RECT        rcPaint;
    BOOL        fErase;

} PAINTSTRUCT;


typedef struct tagDRAWITEM_HDR {
    HWND  hwnd;   /* ���ƵĴ��ڶ���.*/
    UINT  ID;     /* ���ƵĴ���ID.*/
    HDC   hDC;    /* ��ͼ������.*/
    UINT  Style;  /* ���ƴ��ڵķ���־ֵ.*/
    UINT  State;  /* ���ƴ��ڵ�״ֵ̬.*/
    RECT  rc;     /* ���ƴ��ڵľ���λ��.*/
} DRAWITEM_HDR;

/*============================================================================*/

typedef struct tagDLGTEMPLATE
{
    LPCWSTR		lpCaption; 
	int			x, y, w, h; 
    UINT		Style;
    UINT		ExStyle;
    HANDLE      hIcon;
	HANDLE		hCursor;
	COLORREF	BkColor;
    
} DLGTEMPLATE;

/*============================================================================*/

////PeekMessage Mode
#define	PM_NOREMOVE	(0<<1)  /* ��ȡ��Ϣ��,����Ϣ�������Ϣ����������. */
#define	PM_REMOVE	(1<<1)  /* ��ȡ��Ϣ��,����Ϣ�������Ϣ����������. */
#define	PM_NOYIELD	(1<<2)  /* ��Ϣ������û����Ϣʱ,��ǰ�̲߳��ᱻ����.*/

/*============================================================================*/
/*
 * ϵͳ��׼�����ֵ.
 */
#define VK_LBUTTON        0x01
#define VK_RBUTTON        0x02
#define VK_CANCEL         0x03
#define VK_MBUTTON        0x04

#define VK_BACK           0x08
#define VK_TAB            0x09

#define VK_CLEAR          0x0C
#define VK_RETURN         0x0D

#define VK_SHIFT          0x10
#define VK_CONTROL        0x11
#define VK_MENU           0x12
#define VK_PAUSE          0x13
#define VK_CAPITAL        0x14

#define VK_KANA           0x15
#define VK_HANGEUL        0x15
#define VK_HANGUL         0x15
#define VK_JUNJA          0x17
#define VK_FINAL          0x18
#define VK_HANJA          0x19
#define VK_KANJI          0x19

#define VK_ESCAPE         0x1B

#define VK_CONVERT        0x1C
#define VK_NONCONVERT     0x1D
#define VK_ACCEPT         0x1E
#define VK_MODECHANGE     0x1F

#define VK_SPACE          0x20
#define VK_PRIOR          0x21
#define VK_NEXT           0x22
#define VK_END            0x23
#define VK_HOME           0x24
#define VK_LEFT           0x25
#define VK_UP             0x26
#define VK_RIGHT          0x27
#define VK_DOWN           0x28
#define VK_SELECT         0x29
#define VK_PRINT          0x2A
#define VK_EXECUTE        0x2B
#define VK_SNAPSHOT       0x2C
#define VK_INSERT         0x2D
#define VK_DELETE         0x2E
#define VK_HELP           0x2F

/* VK_0 thru VK_9 are the same as ASCII '0' thru '9' (0x30 - 0x39) */
/* VK_A thru VK_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A) */

#define VK_LWIN           0x5B
#define VK_RWIN           0x5C
#define VK_APPS           0x5D

#define VK_NUMPAD0        0x60
#define VK_NUMPAD1        0x61
#define VK_NUMPAD2        0x62
#define VK_NUMPAD3        0x63
#define VK_NUMPAD4        0x64
#define VK_NUMPAD5        0x65
#define VK_NUMPAD6        0x66
#define VK_NUMPAD7        0x67
#define VK_NUMPAD8        0x68
#define VK_NUMPAD9        0x69
#define VK_MULTIPLY       0x6A
#define VK_ADD            0x6B
#define VK_SEPARATOR      0x6C
#define VK_SUBTRACT       0x6D
#define VK_DECIMAL        0x6E
#define VK_DIVIDE         0x6F
#define VK_F1             0x70
#define VK_F2             0x71
#define VK_F3             0x72
#define VK_F4             0x73
#define VK_F5             0x74
#define VK_F6             0x75
#define VK_F7             0x76
#define VK_F8             0x77
#define VK_F9             0x78
#define VK_F10            0x79
#define VK_F11            0x7A
#define VK_F12            0x7B
#define VK_F13            0x7C
#define VK_F14            0x7D
#define VK_F15            0x7E
#define VK_F16            0x7F
#define VK_F17            0x80
#define VK_F18            0x81
#define VK_F19            0x82
#define VK_F20            0x83
#define VK_F21            0x84
#define VK_F22            0x85
#define VK_F23            0x86
#define VK_F24            0x87

#define VK_NUMLOCK        0x90
#define VK_SCROLL         0x91

/*
 * VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
 * No other API or message will distinguish left and right keys in this way.
 */
#define VK_LSHIFT         0xA0
#define VK_RSHIFT         0xA1
#define VK_LCONTROL       0xA2
#define VK_RCONTROL       0xA3
#define VK_LMENU          0xA4
#define VK_RMENU          0xA5

#define VK_PROCESSKEY     0xE5

#define VK_ATTN           0xF6
#define VK_CRSEL          0xF7
#define VK_EXSEL          0xF8
#define VK_EREOF          0xF9
#define VK_PLAY           0xFA
#define VK_ZOOM           0xFB
#define VK_NONAME         0xFC
#define VK_PA1            0xFD
#define VK_OEM_CLEAR      0xFE

/***********************
 * WM_KEYUP/DOWN/CHAR HIWORD(lParam) flags
 */
#define KF_EXTENDED         0x0100
#define KF_DLGMODE          0x0800
#define KF_MENUMODE         0x1000
#define KF_ALTDOWN          0x2000
#define KF_REPEAT           0x4000
#define KF_UP               0x8000

/***********************
 * Key State
 */
#define	KEY_STATE_UP		0x00
#define	KEY_STATE_DOWN		0x80

/***********************
 * Mouse Event Flags
 */
#define MOUSEEVENTF_MOVE        0x0001 /* mouse move */
#define MOUSEEVENTF_LEFTDOWN    0x0002 /* left button down */
#define MOUSEEVENTF_LEFTUP      0x0004 /* left button up */
#define MOUSEEVENTF_RIGHTDOWN   0x0008 /* right button down */
#define MOUSEEVENTF_RIGHTUP     0x0010 /* right button up */
#define MOUSEEVENTF_MIDDLEDOWN  0x0020 /* middle button down */
#define MOUSEEVENTF_MIDDLEUP    0x0040 /* middle button up */
#define MOUSEEVENTF_WHEEL       0x0800 /* wheel button rolled */
#define MOUSEEVENTF_ABSOLUTE    0x8000 /* absolute move */


/*
 * Key State Masks for Mouse Messages
 */
#define MK_LBUTTON          0x0001
#define MK_RBUTTON          0x0002
#define MK_MBUTTON          0x0004
//#define MK_SHIFT            0x0008
//#define MK_CONTROL          0x000C


/*============================================================================*/

typedef	struct tagMSG{

	HWND    hwnd;    //Ŀ�괰��.
	UINT  	message; //��Ϣ��.
	WPARAM  wParam;  //����1.
	LPARAM  lParam;  //����2.
	LONG	ExtData; //��չ����.

}MSG;


/*============================================================================*/
//֪ͨ��Ϣͷ�ṹ��.

typedef struct tagNMHDR
{
    HWND  hwndFrom; //��������Ϣ�Ĵ��ڶ�����.
    UINT  idFrom;   //��������Ϣ�Ĵ���ID.
    UINT  code;     //֪ͨ��.
}NMHDR;

/*============================================================================*/
/*============================================================================*/

/*
 * ���ڹ����ķ���־(ʹ�ø�16λ,��16λ���������ؼ���˽�з���־) / Common Window Styles
 */

#define WS_TRANSPARENT      0x40000000UL // ������͸������,���ᱻ�����ڼ���.
//#define WS_MINIMIZE         0x20000000UL
//#define WS_MAXIMIZE         0x10000000UL
#define WS_DISABLED         0x08000000UL // ���ڴ�����,������Ӧ�����豸���¼�(���������).
#define WS_CLIPSIBLINGS     0x04000000UL // ���ڻ��ƻ�����ֵܴ�������(�Կؼ���Ч,��������һ�������ֵܴ��ڼ��е�,���ܸñ�־����).
#define WS_CLIPCHILDREN     0x02000000UL // ���ڻ��ƻ�����Ӵ�������.
#define WS_VISIBLE          0x01000000UL // ���ڴ�����,Ĭ���ǿɼ���.
#define WS_BORDER           0x00800000UL // ���ڻ����С�߿�.
#define WS_DLGFRAME         0x00400000UL // ���ڻ���д�߿�.
#define	WS_CLOSEBOX			0x00200000UL // ���ڻ���С��رա���ť(���������WS_CAPTION��־).
//#define WS_MINIMIZEBOX      0x00200000UL
//#define WS_MAXIMIZEBOX      0x00100000UL
#define WS_CAPTION          0x00080000UL // ���ڻ���б�����.
//#define WS_WINSURFACE       0x00040000UL
#define WS_OWNERDRAW        0x00020000UL // �����Ի�,���Ӵ�����Ч,������WM_DRAWITEM��Ϣ.
//#define WS_OWNERDC          0x00010000UL

#define WS_OVERLAPPED       WS_CLIPSIBLINGS // �ɵ���Ĵ���,�߱������ԵĴ��ڿ����໥����,������������Ĭ���ǿ��Ե����.

//Ĭ�ϵĴ��ڷ��
#define WS_OVERLAPPEDWINDOW (\
		 	 	 	 	 	 WS_OVERLAPPED     | \
							 WS_CLIPCHILDREN   | \
                             WS_CAPTION        | \
							 WS_CLOSEBOX       | \
                             WS_BORDER         | \
                             WS_DLGFRAME )



/*
 * ������չ����־ / Extended Window Styles
 */
#define	WS_EX_OVERLAPPED		0x00000000L
//#define WS_EX_DLGMODALFRAME     0x00000001L
//#define WS_EX_NOPARENTNOTIFY    0x00000004L
//#define WS_EX_TOPMOST           0x00000008L
//#define WS_EX_ACCEPTFILES       0x00000010L
//#define WS_EX_TRANSPARENT       0x00000020L
//#define WS_EX_MDICHILD          0x00000040L
//#define WS_EX_TOOLWINDOW        0x00000080L
//#define WS_EX_WINDOWEDGE        0x00000100L
//#define WS_EX_CLIENTEDGE        0x00000200L
//#define WS_EX_CONTEXTHELP       0x00000400L

//#define WS_EX_RIGHT             0x00001000L
//#define WS_EX_LEFT              0x00000000L
//#define WS_EX_RTLREADING        0x00002000L
//#define WS_EX_LTRREADING        0x00000000L
//#define WS_EX_LEFTSCROLLBAR     0x00004000L
//#define WS_EX_RIGHTSCROLLBAR    0x00000000L

//#define WS_EX_CONTROLPARENT     0x00010000L
//#define WS_EX_STATICEDGE        0x00020000L
//#define WS_EX_APPWINDOW         0x00040000L
#define	WS_EX_LOCKPOS			0x00100000L /* ���ڲ����϶�. */
#define	WS_EX_LOCKZORDER		0x00200000L /* ���ڲ���ͨ��������ı�Z�� .*/
#define	WS_EX_NOFOCUS			0x00400000L /* ���ڴ���ʱ,�����ý���.*/
#define	WS_EX_FRAMEBUFFER		0x00800000L /* ����ʹ��֡����(����������Ч,�Ӵ��ڻ�̳и�����,���������û���ṩ֡����֧��,�����ʹ��ֱ��д����ʽ.). */

////
#define WS_EX_OVERLAPPEDWINDOW  (WS_EX_OVERLAPPED)
#define	WS_EX_NODRAG			(WS_EX_LOCKPOS)

/*============================================================================*/

#if 0
/* PrintWindow flags */
#define PRF_CHECKVISIBLE    0x00000001L
#define PRF_NONCLIENT       0x00000002L
#define PRF_CLIENT          0x00000004L
#define PRF_ERASEBKGND      0x00000008L
#define PRF_CHILDREN        0x00000010L
#define PRF_OWNED           0x00000020L
#endif

/*============================================================================*/
/*
 * RedrawWindow() flags
 */
#define RDW_INVALIDATE          0x0001  /*�����ô�����Ч��.��*/
//#define RDW_INTERNALPAINT       0x0002
#define RDW_ERASE               0x0004	/*����������.��*/
//#define RDW_VALIDATE            0x0008
//#define RDW_NOINTERNALPAINT     0x0010
//#define RDW_NOERASE             0x0020
//#define RDW_NOCHILDREN          0x0040
#define RDW_ALLCHILDREN         0x0080  /* �ػ������Ӵ���. */
#define RDW_UPDATENOW           0x0100  /* ����ִ���ػ�. */
#define RDW_ERASENOW            0x0200
#define RDW_FRAME               0x0400  /* �ػ洰�ڱ߿�(�ػ�ǿͻ���). */
//#define RDW_NOFRAME             0x0800

/*============================================================================*/
/*
 * ShowWindow() commands
 */

#define SW_SHOW            0    /* �õ�ǰ�Ĵ�С��λ����ʾһ�����ڣ�ͬʱ�������״̬*/    
#define	SW_HIDE            1    /* ���ش��ڣ��״̬����һ������*/   
#define	SW_MINIMIZE        2    /* ��С�����ڣ��״̬����һ������*/  
#define SW_SHOWMAXIMIZED   3    /* ��󻯴��ڣ������伤��*/
#define SW_RESTORE         4    /* ��ԭ���Ĵ�С��λ����ʾһ�����ڣ�ͬʱ�������״̬*/
#define SW_SHOWMINIMIZED   5    /* ��С�����ڣ������伤��*/
#define SW_SHOWMINNOACTIVE 6    /* ��С��һ�����ڣ�ͬʱ���ı�����*/
#define SW_SHOWNA          7    /* �õ�ǰ�Ĵ�С��λ����ʾһ�����ڣ����ı�����*/
#define	SW_SHOWNOACTIVATE  8    /* ������Ĵ�С��λ����ʾһ�����ڣ�ͬʱ���ı�����*/
#define SW_SHOWNORMAL      9    /* ��SW_RESTORE��ͬ*/

/*============================================================================*/

#if 0
#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L
//#define	MB_BUTTONMASK				0x0000000FL
#endif

//// MessageBox Flag-Icon
#define MB_ICONERROR                0x00000010L /* ��ʾ "����" ͼ�� */
#define MB_ICONQUESTION             0x00000020L /* ��ʾ "ѯ��" ͼ�� */
#define MB_ICONWARNING		        0x00000030L /* ��ʾ "����" ͼ�� */
#define MB_ICONINFORMATION          0x00000040L /* ��ʾ "��Ϣ" ͼ�� */
#define	MB_ICONMASK					0x000000F0L

//// MessageBox Flag-Button
#define	MB_BTN_WIDTH(a)				(0x0000FF00L&(a<<8))  /* ���ð�ť���,��ť�����м��������  */
#define	MB_BTN_AUTOSIZE				MB_BTN_WIDTH(0)       /* ��ϵͳ�Զ����ð�ť���(���ݰ�ť�ַ�����),��ť�����Ҷ������� */

//// MessageBox Button ID
#if 0
#define IDOK		1
#define IDCANCEL	2
#define IDABORT		3
#define IDRETRY		4
#define IDIGNORE	5
#define IDYES		6
#define IDNO		7
#define IDCLOSE		8
#define IDHELP		9
#endif

typedef	struct __MSGBOXOPTIONS
{
	const WCHAR **pButtonText; //��ť�ַ���.
	int ButtonCount;           //��ť����.
	u32 Flag;                  //���.
}MSGBOX_OPTIONS;

/*============================================================================*/
//GetWindowLong/SetWindowLong commands

//#define GWL_WNDPROC         (1)
//#define GWL_HINSTANCE       (2)  /* ���̱�ʶ(����). */
//#define GWL_HWNDPARENT      (3)  /* ������.  */
#define GWL_STYLE           (4)  /* ���ڷ���־.  */
#define GWL_EXSTYLE         (5)  /* ������չ����־ . */
#define GWL_USERDATA        (6)  /* �����û�����ֵ.  */
#define GWL_ID              (7)  /* ����ID. */
//#define	GWL_BKGNDERASEPROC	(8)
#define	GWL_EXITCODE		(9)
#define	GWL_STATE			(11) /* ����״ֵ̬. */
#define	GWL_EXDATA_ADDR		(12) /* ������չ��������ʼ��ַ. */

/*============================================================================*/
#if 0
/*
 *GetClassLong commands
 */
#define GCL_MENUNAME        (-8)
#define GCL_HBRBACKGROUND   (-10)
#define GCL_HCURSOR         (-12)
#define GCL_HICON           (-14)
#define GCL_HMODULE         (-16)
#define GCL_CBWNDEXTRA      (-18)
#define GCL_CBCLSEXTRA      (-20)
#define GCL_WNDPROC         (-24)
#define GCL_STYLE           (-26)
#define GCW_ATOM            (-32)
#define GCL_HICONSM         (-34)
#endif

/*============================================================================*/
/*
 *GetWindow commands
 */

#define	GW_MAINWINDOW		(1)		/* ������ */
#define	GW_CHILD			(2)		/* Z����˿ͻ����Ӵ��� */
#define	GW_NCCHILD			(3)		/* Z����˷ǿͻ����Ӵ��� */
//#define	GW_ENABLEDPOPUP		(4)		/* ʹ��״̬����ʽ���� */
#define	GW_HWNDFIRST		(5)		/* Z����˴��� */
#define	GW_HWNDLAST			(6)		/* Z����׶˴��� */
#define	GW_HWNDNEXT			(7)		/* Z����һ�㴰�� */
#define	GW_HWNDPREV			(8)		/* Z����һ�㴰�� */
#define	GW_OWNER			GW_MAINWINDOW

/*============================================================================*/

/*
 * SetWindowPos Flags
 */
#define SWP_NOSIZE          0x0001  /* ���ı䴰�ڴ�С. */
#define SWP_NOMOVE          0x0002  /* ���ı䴰��λ��. */
#define SWP_NOZORDER        0x0004  /* ���ı䴰��Z��. */
#define SWP_NOREDRAW        0x0008  /* ���ػ洰��. */
//#define SWP_NOACTIVATE      0x0010
//#define SWP_FRAMECHANGED    0x0020  /* The frame changed: send WM_NCCALCSIZE */
#define SWP_SHOWWINDOW      0x0040  /* ������������ʾ״̬. */
#define SWP_HIDEWINDOW      0x0080  /* ��������������״̬. */
//#define SWP_NOCOPYBITS      0x0100
//#define SWP_NOOWNERZORDER   0x0200  /* Don't do owner Z ordering */
//#define SWP_NOSENDCHANGING  0x0400  /* Don't send WM_WINDOWPOSCHANGING */
//#define SWP_DRAWFRAME       SWP_FRAMECHANGED
//#define SWP_NOREPOSITION    SWP_NOOWNERZORDER
//#define SWP_DEFERERASE      0x2000
//#define SWP_ASYNCWINDOWPOS  0x4000

#define HWND_TOP        ((HWND)1)
#define HWND_BOTTOM     ((HWND)2)
#define HWND_TOPMOST    ((HWND)3)
#define HWND_NOTOPMOST  ((HWND)4)


/*============================================================================*/
/*
 *ϵͳ�Դ��Ļ����ؼ���.
 */

#define	BUTTON		((WNDCLASS*)L"BUTTON")       //��ť/��ѡ��ť/��ѡ��ť.
#define	TEXTBOX		((WNDCLASS*)L"TEXTBOX")      //���ֿ�.
#define	GROUPBOX	((WNDCLASS*)L"GROUPBOX")     //�����.
#define	PROGRESSBAR	((WNDCLASS*)L"PROGRESSBAR")  //������.
#define	SCROLLBAR	((WNDCLASS*)L"SCROLLBAR")    //����.
#define	LISTBOX		((WNDCLASS*)L"LISTBOX")      //�б��.

/*============================================================================*/
//�ؼ���ɫ�ṹ�塣
typedef	struct
{
	COLOR_RGB32 TextColor;   //������ɫ.
	COLOR_RGB32 BorderColor; //�߿���ɫ.
	COLOR_RGB32 BackColor;   //������ɫ.
	COLOR_RGB32 ForeColor;   //ǰ����ɫ.

}CTLCOLOR;

/*============================================================================*/
/*
 * Window Messages / ������Ϣ
 */
#define	WM_SYSTEM 	0x1000 /* ������ϵͳ�ڲ�����Ϣ. */
#define	WM_WIDGET	0x7000 /* �������ؼ�����Ϣ. */
#define	WM_USER		0x8000 /* �������û��Զ�����Ϣ. */


#define WM_NULL                         0x0000
#define WM_CREATE                       0x0001 /* [���ڴ���]: <wParam>����; <lParam>��CreateWindow��lParam���������ֵ; <����>TRUE:���ڴ����ɹ�;FALSE:���ڴ���ʧ��. */
#define WM_DESTROY                      0x0002 /* [��������]: <wParam>����; <lParam>����; <����>����. */


#if 0
#define WM_ACTIVATE                     0x0006
/*
 * WM_ACTIVATE state values
 */
#define     WA_INACTIVE     0
#define     WA_ACTIVE       1
#define     WA_CLICKACTIVE  2
#endif

#define WM_SETFOCUS                     0x0007  /* [���ڻ���˽���]: <wParam>�ɽ��㴰��(HWND); <lParam>����; <����>����. */
#define WM_KILLFOCUS                    0x0008  /* [����ʧȥ�˽���]: <wParam>�½��㴰��(HWND); <lParam>����; <����>����. */
#define WM_ENABLE                       0x000A  /* [���ڱ�ʹ�ܻ��ֹ]: <wParam>TRUE:���ڱ�ʹ��;FALSE:���ڱ���ֹ; <lParam>����; <����>����. */
#define WM_SETREDRAW                    0x000B
#define WM_SETTEXT                      0x000C
#define WM_GETTEXT                      0x000D
#define WM_GETTEXTLENGTH                0x000E
#define WM_PAINT                        0x000F /* [���ڻ���]: <wParam>����; <lParam>����; <����>����. */
#define WM_CLOSE                        0x0010 /* [���ڹر�]: <wParam>����; <lParam>����; <����>TRUE:���ڽ����ر�;FALSE:���ڼ�������. */
//#define WM_QUERYENDSESSION              0x0011
#define WM_QUIT                         0x0012 /* [�����˳�]: <wParam>����; <lParam>����; <����>����. */
//#define WM_QUERYOPEN                    0x0013
#define WM_ERASEBKGND                   0x0014 /* [�������ڱ���]: <wParam>HDC; <lParam>��Ҫ����������(RECTָ��); <����>TRUE:������һ����Ҫ����������; FALSE:��������. */
//#define WM_SYSCOLORCHANGE               0x0015
//#define WM_ENDSESSION                   0x0016
#define WM_SHOWWINDOW                   0x0018 /* [���ڱ���ʾ������]: <wParam>TRUE:���ڱ���ʾ;FALSE:���ڱ�����; <lParam>����; <����>����. */
//#define WM_WININICHANGE                 0x001A
//#define WM_SETTINGCHANGE                WM_WININICHANGE

//#define WM_WINDOWPOSCHANGING            0x001A
//#define WM_WINDOWPOSCHANGED             0x001B

#define WM_MOVING                       0x001C /* [������Ҫ�ı�λ��]: <wParam>����; <lParam>����; <����>TRUE:�����ڸı�λ��;FALSE:�������ڸı�λ��. */
#define WM_MOVE                         0x001D /* [�����Ѿ��ı�λ��]: <wParam>����; <lParam>����; <����>����. */
#define WM_SIZING                       0x001E /* [������Ҫ�ı��С]: <wParam>����; <lParam>����; <����>TRUE:�����ڸı��С;FALSE:�������ڸı��С. */
#define WM_SIZE                         0x001F /* [�����Ѿ��ı��С]: <wParam>����; <lParam>����; <����>����. */

//#define WM_PAINTICON                    0x0026
//#define WM_ICONERASEBKGND               0x0027
//#define WM_NEXTDLGCTL                   0x0028
//#define WM_SPOOLERSTATUS                0x002A
#define WM_DRAWITEM                     0x002B /* [�Ӵ����Ի���]: <wParam>��Ҫ�Ի��ƵĴ���ID; <lParam>DRAWITEM_HDRָ��; <����>TRUE:�ɹ�;FALSE:ʧ��. */
//#define WM_MEASUREITEM                  0x002C
//#define WM_DELETEITEM                   0x002D
//#define WM_VKEYTOITEM                   0x002E
//#define WM_CHARTOITEM                   0x002F
#define WM_SETFONT                      0x0030 /* [���ô�������]: <wParam>����; <lParam>������(HFONT); <����>����. */
#define WM_GETFONT                      0x0031 /* [��ô�������]: <wParam>����; <lParam>����; <����>������(HFONT). */
//#define WM_SETHOTKEY                    0x0032
//#define WM_GETHOTKEY                    0x0033
//#define WM_QUERYDRAGICON                0x0037
//#define WM_COMPAREITEM                  0x0039
//#define WM_GETOBJECT                    0x003D
//#define WM_COMPACTING                   0x0041
//#define WM_COMMNOTIFY                   0x0044


#define WM_CAPTURECHANGED               0x0065


#define WM_NOTIFY                       0x004E /* [�Ӵ����¼�֪ͨ]: <wParam>HIWORD:�¼�֪ͨ��;LOWORD:��������Ϣ���Ӵ���ID; <lParam>��չ��Ϣ(HMHDR�ṹ��ָ��); <����>����. */
//#define WM_INPUTLANGCHANGEREQUEST       0x0050
//#define WM_INPUTLANGCHANGE              0x0051
//#define WM_TCARD                        0x0052
//#define WM_HELP                         0x0053
//#define WM_USERCHANGED                  0x0054
//#define WM_NOTIFYFORMAT                 0x0055

//#define WM_CONTEXTMENU                  0x007B
//#define WM_STYLECHANGING                0x007C
//#define WM_STYLECHANGED                 0x007D
//#define WM_DISPLAYCHANGE                0x007E
//#define WM_GETICON                      0x007F
//#define WM_SETICON                      0x0080

#define WM_NCCREATE                     0x0081
#define WM_NCDESTROY                    0x0082
#define WM_NCCALCSIZE                   0x0083
#define WM_NCHITTEST                    0x0084
#define WM_NCPAINT                      0x0085 /* [�ǿͻ���������Ϣ]: <wParam>HDC; <lParam>NCPAINT_STRUCT�ṹ��ָ��; <����>TURE:�ɹ�;FALSE:ʧ��(����ϵͳĬ�ϻ���). */
//#define WM_NCACTIVATE                   0x0086

//#define WM_GETDLGCODE                   0x0087
//#define WM_SYNCPAINT                    0x0088

//#define WM_SYSCOMMAND                   0x0090
#define WM_TIMER                        0x0091 /* [��ʱ����Ϣ]: <wParam>��������Ϣ�Ķ�ʱ��ID; <lParam>����; <����>����. */
#define WM_CTLCOLOR               		0x0092 /* [�ؼ���ɫ����]: <wParam>��������Ϣ�Ŀؼ�ID; <lParam>��Ҫ���õĿؼ���ɫ����(CTLCOLOR�ṹ��ָ��); <����>TRUE���ɹ�;FALSE:ʧ��. */

#define WM_NCMOUSEMOVE                  0x00A0
#define WM_NCLBUTTONDOWN                0x00A1
#define WM_NCLBUTTONUP                  0x00A2
#define WM_NCLBUTTONDBLCLK              0x00A3
#define WM_NCRBUTTONDOWN                0x00A4
#define WM_NCRBUTTONUP                  0x00A5
#define WM_NCRBUTTONDBLCLK              0x00A6
#define WM_NCMBUTTONDOWN                0x00A7
#define WM_NCMBUTTONUP                  0x00A8
#define WM_NCMBUTTONDBLCLK              0x00A9

#define WM_KEYFIRST                     0x0100
#define WM_KEYDOWN                      0x0100 /* wParam:KeyValue, lParam:NONE */
#define WM_KEYUP                        0x0101 /* wParam:KeyValue, lParam:NONE */
#define WM_CHAR                         0x0102 /* wParam:char, lParam:NONE */
//#define WM_DEADCHAR                     0x0103
//#define WM_SYSKEYDOWN                   0x0104
//#define WM_SYSKEYUP                     0x0105
//#define WM_SYSCHAR                      0x0106
//#define WM_SYSDEADCHAR                  0x0107
#define WM_KEYLAST                      0x0108

//#define WM_IME_STARTCOMPOSITION         0x010D
//#define WM_IME_ENDCOMPOSITION           0x010E
//#define WM_IME_COMPOSITION              0x010F
//#define WM_IME_KEYLAST                  0x010F

//#define WM_INITDIALOG                   0x0110
//#define WM_COMMAND                      0x0111 /* wParam:HIWORD:event,LOWORD:id; lParam:hwnd */
//#define WM_HSCROLL                      0x0114 /* wParam:HIWORD:pos,LOWORD: SB commands; lParam:hwnd */
//#define WM_VSCROLL                      0x0115 /* wParam:HIWORD:pos,LOWORD: SB commands; lParam:hwnd */
//#define WM_INITMENU                     0x0116
//#define WM_INITMENUPOPUP                0x0117
//#define WM_MENUSELECT                   0x011F
//#define WM_MENUCHAR                     0x0120
//#define WM_ENTERIDLE                    0x0121
//#define WM_MENURBUTTONUP                0x0122
//#define WM_MENUDRAG                     0x0123
//#define WM_MENUGETOBJECT                0x0124
//#define WM_UNINITMENUPOPUP              0x0125
//#define WM_MENUCOMMAND                  0x0126


#define WM_MOUSEFIRST                   0x0200  /* First Mouse Message */

#define WM_MOUSEMOVE                    0x0200	// [�ͻ�������ƶ�]: <wParam>LO16:����״̬; <lParam>λ��(�ͻ�������),H16:Y,LO16:X; <����>����.
#define WM_LBUTTONDOWN                  0x0201	// [�ͻ�������������]: <wParam>LO16:����״̬; <lParam>λ��(�ͻ�������),H16:Y,LO16:X; <����>����.
#define WM_LBUTTONUP                    0x0202	// [�ͻ���������̧��]: <wParam>LO16:����״̬; <lParam>λ��(�ͻ�������),H16:Y,LO16:X; <����>����.
#define WM_LBUTTONDBLCLK                0x0203  // [�ͻ���������˫��]: <wParam>LO16:����״̬; <lParam>λ��(�ͻ�������),H16:Y,LO16:X; <����>����.
#define WM_RBUTTONDOWN                  0x0204	// [�ͻ�������Ҽ�����]: <wParam>LO16:����״̬; <lParam>λ��(�ͻ�������),H16:Y,LO16:X; <����>����.
#define WM_RBUTTONUP                    0x0205	// [�ͻ�������Ҽ�̧��]: <wParam>LO16:����״̬; <lParam>λ��(�ͻ�������),H16:Y,LO16:X; <����>����.
#define WM_RBUTTONDBLCLK                0x0206  // [�ͻ�������Ҽ�˫��]: <wParam>LO16:����״̬; <lParam>λ��(�ͻ�������),H16:Y,LO16:X; <����>����.
#define WM_MBUTTONDOWN                  0x0207  // [�ͻ�������м�����]: <wParam>LO16:����״̬; <lParam>λ��(�ͻ�������),H16:Y,LO16:X; <����>����.
#define WM_MBUTTONUP                    0x0208  // [�ͻ�������м�̧��]: <wParam>LO16:����״̬; <lParam>λ��(�ͻ�������),H16:Y,LO16:X; <����>����.
#define WM_MBUTTONDBLCLK                0x0209  // [�ͻ�������м�˫��]: <wParam>LO16:����״̬; <lParam>λ��(�ͻ�������),H16:Y,LO16:X; <����>����.
#define WM_MOUSEWHEEL                   0x020A
#define WM_MOUSEHOVER                   0x020B
#define WM_MOUSELEAVE                   0x020C  // [����뿪�ͻ���]: <wParam>LO16:����״̬; <lParam>λ��(��Ļ����),H16:Y,LO16:X; <����>����.

#define WM_MOUSELAST                    0x020C /* Last Mouse Message */

//#define WHEEL_DELTA                     120     /* Value for rolling one detent */
//#define WHEEL_PAGESCROLL                (UINT_MAX) /* Scroll one page */

//#define WM_PARENTNOTIFY                 0x0210
//#define WM_ENTERMENULOOP                0x0211
//#define WM_EXITMENULOOP                 0x0212

//#define WM_NEXTMENU                     0x0213
//
/*
typedef struct tagMDINEXTMENU
{
    HMENU   hmenuIn;
    HMENU   hmenuNext;
    HWND    hwndNext;
} MDINEXTMENU, * PMDINEXTMENU,  * LPMDINEXTMENU;
*/


//#define WM_DEVICECHANGE                 0x0219

/*
#define WM_MDICREATE                    0x0220
#define WM_MDIDESTROY                   0x0221
#define WM_MDIACTIVATE                  0x0222
#define WM_MDIRESTORE                   0x0223
#define WM_MDINEXT                      0x0224
#define WM_MDIMAXIMIZE                  0x0225
#define WM_MDITILE                      0x0226
#define WM_MDICASCADE                   0x0227
#define WM_MDIICONARRANGE               0x0228
#define WM_MDIGETACTIVE                 0x0229
*/

/*
#define WM_MDISETMENU                   0x0230
#define WM_ENTERSIZEMOVE                0x0231
#define WM_EXITSIZEMOVE                 0x0232
#define WM_DROPFILES                    0x0233
#define WM_MDIREFRESHMENU               0x0234
*/

/*
#define WM_IME_SETCONTEXT               0x0281
#define WM_IME_NOTIFY                   0x0282
#define WM_IME_CONTROL                  0x0283
#define WM_IME_COMPOSITIONFULL          0x0284
#define WM_IME_SELECT                   0x0285
#define WM_IME_CHAR                     0x0286
#define WM_IME_REQUEST                  0x0288
#define WM_IME_KEYDOWN                  0x0290
#define WM_IME_KEYUP                    0x0291
*/


/*
#define WM_CUT                          0x0300
#define WM_COPY                         0x0301
#define WM_PASTE                        0x0302
#define WM_CLEAR                        0x0303
#define WM_UNDO                         0x0304
#define WM_RENDERFORMAT                 0x0305
#define WM_RENDERALLFORMATS             0x0306
#define WM_DESTROYCLIPBOARD             0x0307
#define WM_DRAWCLIPBOARD                0x0308
#define WM_PAINTCLIPBOARD               0x0309
#define WM_VSCROLLCLIPBOARD             0x030A
#define WM_SIZECLIPBOARD                0x030B
#define WM_ASKCBFORMATNAME              0x030C
#define WM_CHANGECBCHAIN                0x030D
#define WM_HSCROLLCLIPBOARD             0x030E
#define WM_QUERYNEWPALETTE              0x030F
#define WM_PALETTEISCHANGING            0x0310
#define WM_PALETTECHANGED               0x0311
#define WM_HOTKEY                       0x0312
*/

/*
#define WM_PRINT                        0x0317
#define WM_PRINTCLIENT                  0x0318
#define	WM_NCFLUSH						0x0319
#define	WM_FLUSH						0x031A

#define WM_APP                          0x8000
*/

/*  wParam for WM_SIZING message  */
/*
#define WMSZ_LEFT           1
#define WMSZ_RIGHT          2
#define WMSZ_TOP            3
#define WMSZ_TOPLEFT        4
#define WMSZ_TOPRIGHT       5
#define WMSZ_BOTTOM         6
#define WMSZ_BOTTOMLEFT     7
#define WMSZ_BOTTOMRIGHT    8
*/

/*
 * WM_NCHITTEST and MOUSEHOOKSTRUCT Mouse Position Codes
 */
#define HTERROR             (-2)
#define HTTRANSPARENT       (-1)
#define HTNOWHERE           0
#define HTCLIENT            1
#define HTCAPTION           2
#define HTSYSMENU           3
#define HTGROWBOX           4
#define HTSIZE              HTGROWBOX
#define HTMENU              5
#define HTHSCROLL           6
#define HTVSCROLL           7
#define HTMINBUTTON         8
#define HTMAXBUTTON         9
#define HTLEFT              10
#define HTRIGHT             11
#define HTTOP               12
#define HTTOPLEFT           13
#define HTTOPRIGHT          14
#define HTBOTTOM            15
#define HTBOTTOMLEFT        16
#define HTBOTTOMRIGHT       17
#define HTBORDER            18
#define HTREDUCE            HTMINBUTTON
#define HTZOOM              HTMAXBUTTON
#define HTSIZEFIRST         HTLEFT
#define HTSIZELAST          HTBOTTOMRIGHT
#define HTOBJECT            19
#define HTCLOSE             20
#define HTHELP              21



/*============================================================================*/
/*
 *  ��ť / Button
 */

/*
 * ��ť�ؼ���˽�з���־(ʹ�ô��ڷ��ĵ�16λ) /Button private styles(Low 16bits)
 */

/*����ť��� / Button face */
#define	BS_FACE_MASK	(3<<0)
#define BS_FLAT         (1<<0) // ƽ����.
#define	BS_3D		    (2<<0) // ������.
#define	BS_ROUND	    (3<<0) // Բ�Ƿ��.

/*����ť���� / Button type */
#define	BS_TYPE_MASK	(3<<2)
#define	BS_PUSHBUTTON	(0<<2) // ���水ť.
#define	BS_CHECKBOX		(1<<2) // ��ѡ��.
#define	BS_RADIOBOX		(2<<2) // ��ѡ��.

/*���������־ / Misc flags */
#define	BS_PUSHLIKE		(1<<4)  // ����Ϊ��ѡ��/��ѡ������ʱ,ʹ�ó��水ť�����.
//#define	BS_LEFTTEXT		(1<<5)
#define BS_FOCUSFRAME 	(1<<6)  // ����ý���ʱ,����ʾ�������.
#define BS_NOTIFY       (1<<15) // ʹ�øñ�־ʱ�������������֪ͨ��Ϣ.

/*
 * ��ť״ֵ̬ / Button States
 */
#define BST_PUSHED         0x0001 // ����״̬(���水ť).
#define BST_CHECKED        0x0002 // ѡ��״̬(��ѡ��/��ѡ��).
//#define BST_INDETERMINATE  0x0004
//#define BST_FOCUS          0x0008

/*
 *����ť֪ͨ�� / Button Notification Codes
 */
#define	BN_CLICKED			0x00 // ����(����+����).
//�����֪ͨ�� / extend Notification Codes
#define BN_SETFOCUS         0x80 // ����˽���.
#define BN_KILLFOCUS        0x81 // ʧȥ�˽���.
#define	BN_PUSHED			0x82 // ����.
#define	BN_CHECKED			0x83 // ��ѡ��/��ѡ�򡡱�ѡ��.
//#define	BN_UNCHECKED		0x84

/*
 * ��ť��Ϣ / Button Messages
 */
//#define BM_GETCHECK      (WM_WIDGET+0)
//#define BM_SETCHECK      (WM_WIDGET+1)
#define BM_GETSTATE        (WM_WIDGET+2) // [���״̬]: <wParam>����; <lParam>����; <����>״ֵ̬.
#define BM_SETSTATE        (WM_WIDGET+3) // [����״̬]: <wParam>״ֵ̬; <lParam>����; <����>����.

/*============================================================================*/
/*** �ַ��� / Textbox ***/

//// �ַ��� �з���־ / Textbox private styles

#define	TBS_FLAT		(0<<0) //ƽ�����.
#define	TBS_SUNK		(1<<0) //�³����.

#define	TBS_H_MASK		(3<<2)
#define	TBS_LEFT		(0<<2) //����ˮƽ�����.
#define	TBS_CENTER		(1<<2) //����ˮƽ���ж���.
#define	TBS_RIGHT		(2<<2) //����ˮƽ�Ҷ���.

#define	TBS_V_MASK		(3<<4)
#define	TBS_VCENTER		(0<<4) //���ִ�ֱ���ж���.
#define	TBS_TOP			(1<<4) //���ִ�ֱ��������.
#define	TBS_BOTTOM		(2<<4) //���ִ�ֱ�ײ�����.

#define	TBS_SINGLELINE	(1<<6)  //������ʾ.
#define TBS_NOTIFY      (1<<15) //���������֪ͨ��.

////�ַ���֪ͨ�� /  Notification Codes.
#define TBN_CLICKED          0x00 //�����.

//�����֪ͨ�� / Extra Notification Codes.
#define TBN_SETFOCUS         0x80 //����˽���.
#define TBN_KILLFOCUS        0x81 //ʧȥ�˽���.

////�ַ�����Ϣ / Textbox Messages.
#define	TBM_SET_TEXTFLAG	(WM_WIDGET+0) // [�������ֻ��Ʊ�־]: <wParam>TRUE:�ػ洰��;FALSE:���ػ洰��; <lParam>���ֻ��Ʊ�־,��DrawText��־��ͬ; <����>����.
#define	TBM_GET_TEXTFLAG	(WM_WIDGET+1) // [������ֻ��Ʊ�־]: <wParam>����; <lParam>����; <����>���ֻ��Ʊ�־ֵ.

/*============================================================================*/
/*
 * ������ / Progressbar
 */

//// ����������־ / ProgressBar Style
#define	PBS_FACE_MASK		(3<<0)
#define	PBS_FLAT			(1<<0) //ƽ����۷���.
#define	PBS_3D				(2<<0) //����͹����۷���.
#define	PBS_TEXT			(1<<2) //��������ʾ.

#define	PBS_ALIGN_MASK		(3<<3)
#define	PBS_ALIGN_LEFT		(0<<3) //�����ʼ��ģʽ.
#define	PBS_ALIGN_RIGHT		(1<<3) //�ұ���ʼ��ģʽ.
#define	PBS_ALIGN_TOP		(2<<3) //������ʼ��ģʽ.
#define	PBS_ALIGN_BOTTOM	(3<<3) //�ײ���ʼ��ģʽ.

//// ��������������ֵ.
#define	PB_CFG_RANGLE	(1<<0)
#define	PB_CFG_VALUE	(1<<1)
#define	PB_CFG_TEXTFLAG (1<<2)

#define	PB_CFG_ALL	(PB_CFG_RANGLE | PB_CFG_VALUE  | PB_CFG_TEXTFLAG )

typedef	struct	{
	u16 cbSize;    //ʹ��ǰ����ָ��Ϊ PROGRESSBAR_CFG �ֽ����������ڲ��жϽṹ��ĺϷ���.
	u16 fMask;     //��������ֵ.
	u32 Rangle;    //����.
	u32 Value;     //��ǰֵ.
	u32 TextFlag;  //�ļ����Ʊ�־(��DrawText���Ʊ�־��ͬ).
}PROGRESSBAR_CFG;


//// ��������Ϣ / Progressbar Message
#define	PBM_SET_CFG		(WM_WIDGET+0) // [���ò���]: <wParam>TRUE���ػ�; FALSE:���ػ�; <lParam>PROGRESSBAR_CFGָ��; <����>����.
#define	PBM_GET_CFG		(WM_WIDGET+1) // [��ò���]: <wParam>����; <lParam>PROGRESSBAR_CFGָ��; <����>����.
#define	PBM_SET_RANGLE	(WM_WIDGET+2) // [��������]: <wParam>TRUE���ػ�; FALSE:���ػ�; <lParam>����; <����>����.
#define	PBM_GET_RANGLE	(WM_WIDGET+3) // [�������]: <wParam>����; <lParam>����; <����>����.
#define	PBM_SET_VALUE	(WM_WIDGET+4) // [���õ�ǰֵ]: <wParam>TRUE���ػ�; FALSE:���ػ�; <lParam>��ǰֵ; <����>����.
#define	PBM_GET_VALUE	(WM_WIDGET+5) // [��õ�ǰֵ]: <wParam>����; <lParam>����; <����>��ǰֵ.

//// ������֪ͨ�� / Porgressbar Notify codes.
#define	PBN_CLICKED		(0x00) //�������.
#define	PBN_CHANGE		(0x01) //��ǰֵ���ı���.


#if 0
/*
 * Static Control Mesages
 */
#define STM_SETICON         0x0170
#define STM_GETICON         0x0171
#define STM_SETIMAGE        0x0172
#define STM_GETIMAGE        0x0173
#define STM_MSGMAX          0x0174

/*
 * Static Control Notification Codes
 */

#define STN_CLICKED         0
#define STN_DBLCLK          1
#define STN_ENABLE          2
#define STN_DISABLE         3
#endif

/*============================================================================*/
#if 0
/**************/
/*   Edit     */
/**************/

/*
 * Edit Control Styles
 */
#define ES_LEFT             0x0000L
#define ES_CENTER           0x0001L
#define ES_RIGHT            0x0002L
#define ES_MULTILINE        0x0004L
#define ES_UPPERCASE        0x0008L
#define ES_LOWERCASE        0x0010L
#define ES_PASSWORD         0x0020L
#define ES_AUTOVSCROLL      0x0040L
#define ES_AUTOHSCROLL      0x0080L
#define ES_NOHIDESEL        0x0100L
#define ES_OEMCONVERT       0x0400L
#define ES_READONLY         0x0800L
#define ES_WANTRETURN       0x1000L
#define ES_NUMBER           0x2000L


/*
 * Edit Control Notification Codes
 */
#define EN_SETFOCUS         0x0100
#define EN_KILLFOCUS        0x0200
#define EN_CHANGE           0x0300
#define EN_UPDATE           0x0400
#define EN_ERRSPACE         0x0500
#define EN_MAXTEXT          0x0501
#define EN_HSCROLL          0x0601
#define EN_VSCROLL          0x0602

/* Edit control EM_SETMARGIN parameters */
#define EC_LEFTMARGIN       0x0001
#define EC_RIGHTMARGIN      0x0002
#define EC_USEFONTINFO      0xffff

/* wParam of EM_GET/SETIMESTATUS  */
#define EMSIS_COMPOSITIONSTRING        0x0001

/* lParam for EMSIS_COMPOSITIONSTRING  */
#define EIMES_GETCOMPSTRATONCE         0x0001
#define EIMES_CANCELCOMPSTRINFOCUS     0x0002
#define EIMES_COMPLETECOMPSTRKILLFOCUS 0x0004


/*
 * Edit Control Messages
 */
#define EM_GETSEL               0x00B0
#define EM_SETSEL               0x00B1
#define EM_GETRECT              0x00B2
#define EM_SETRECT              0x00B3
#define EM_SETRECTNP            0x00B4
#define EM_SCROLL               0x00B5
#define EM_LINESCROLL           0x00B6
#define EM_SCROLLCARET          0x00B7
#define EM_GETMODIFY            0x00B8
#define EM_SETMODIFY            0x00B9
#define EM_GETLINECOUNT         0x00BA
#define EM_LINEINDEX            0x00BB
#define EM_SETHANDLE            0x00BC
#define EM_GETHANDLE            0x00BD
#define EM_GETTHUMB             0x00BE
#define EM_LINELENGTH           0x00C1
#define EM_REPLACESEL           0x00C2
#define EM_GETLINE              0x00C4
#define EM_LIMITTEXT            0x00C5
#define EM_CANUNDO              0x00C6
#define EM_UNDO                 0x00C7
#define EM_FMTLINES             0x00C8
#define EM_LINEFROMCHAR         0x00C9
#define EM_SETTABSTOPS          0x00CB
#define EM_SETPASSWORDCHAR      0x00CC
#define EM_EMPTYUNDOBUFFER      0x00CD
#define EM_GETFIRSTVISIBLELINE  0x00CE
#define EM_SETREADONLY          0x00CF
#define EM_SETWORDBREAKPROC     0x00D0
#define EM_GETWORDBREAKPROC     0x00D1
#define EM_GETPASSWORDCHAR      0x00D2
#define EM_SETMARGINS           0x00D3
#define EM_GETMARGINS           0x00D4
#define EM_SETLIMITTEXT         EM_LIMITTEXT   /* ;win40 Name change */
#define EM_GETLIMITTEXT         0x00D5
#define EM_POSFROMCHAR          0x00D6
#define EM_CHARFROMPOS          0x00D7

#define EM_SETIMESTATUS         0x00D8
#define EM_GETIMESTATUS         0x00D9


/*
 * EDITWORDBREAKPROC code values
 */
#define WB_LEFT            0
#define WB_RIGHT           1
#define WB_ISDELIMITER     2
#endif

/*============================================================================*/
/*
 *   �б�� /  ListBox
 */

/// Listbox ˽�еķ���־
//#define LBS_SORT              0x0002L
//#define LBS_NOREDRAW          0x0004L
//#define LBS_MULTIPLESEL       0x0008L
#define LBS_LINE    		  0x0010L //�б���֮���з�������.
//#define LBS_OWNERDRAWVARIABLE 0x0020L
//#define LBS_HASSTRINGS        0x0040L
//#define LBS_USETABSTOPS       0x0080L
//#define LBS_NOINTEGRALHEIGHT  0x0100L
//#define LBS_MULTICOLUMN       0x0200L
//#define LBS_WANTKEYBOARDINPUT 0x0400L
//#define LBS_EXTENDEDSEL       0x0800L
//#define LBS_DISABLENOSCROLL   0x1000L
//#define LBS_NODATA            0x2000L
#define LBS_NOSEL             0x4000L //û�п�ѡ����.
#define LBS_NOTIFY            0x8000L //���������֪ͨ��(0x80ֵ���ϵ� LBN_xxx).

//#define LBS_STANDARD          (LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER)


/*
 * Listbox֪ͨ�� / Listbox Notification Codes
 */
#define LBN_CLICKED       	0x00 //�������.

//�����֪ͨ��(LBS_NOTIFY)
#define LBN_SETFOCUS        0x80 //��ý���.
#define LBN_KILLFOCUS       0x81 //ʧȥ����.
#define LBN_SELCHANGE       0x82 //ѡ����ı�.
#define	LBN_POSCHANGE		0x83 //λ�ñ��ı�.
//#define LBN_DBLCLK          0x83
//#define LBN_SELCANCEL       0x84
//#define LBN_ERRSPACE        0x85

/*
 * Listbox messages
 */
#define LB_ADDSTRING            0x0180	//[��������Ŀ]: <wParam>����ֵ(�ڵڼ���λ��,Ϊ0�����ӵ���һ��λ��); <lParam>����; <����>����.
#define LB_DELETESTRING         0x0182	//[ɾ������Ŀ]: <wParam>����ֵ; <lParam>����; <����>����.
//#define LB_SELITEMRANGEEX       0x0183
#define LB_RESETCONTENT         0x0184	//[ɾ����������Ŀ]: <wParam>����; <lParam>����; <����>����.
//#define LB_SETSEL               0x0185
#define LB_SETCURSEL            0x0186	//[���õ�ǰѡ�е�����Ŀ]: <wParam>����ֵ; <lParam>����; <����>����.
//#define LB_GETSEL               0x0187
#define LB_GETCURSEL            0x0188	//[��õ�ǰѡ�е�����Ŀ]: <wParam>����ֵ; <lParam>����; <����>��ǰѡ�е�����Ŀ����ֵ.
#define LB_GETTEXT              0x0189	//[�������Ŀ����������]: <wParam>����ֵ; <lParam>���ֻ������ڴ�; <����>����.
#define LB_GETTEXTLEN           0x018A	//[�������Ŀ���������ݳ���]: <wParam>����ֵ; <lParam>����; <����>�������ݳ���.
#define LB_GETCOUNT             0x018B	//[�������Ŀ������]: <wParam>����; <lParam>����; <����>����Ŀ������.
#define LB_SETPOS               0x018C  //[������ʾλ��]: <wParam>TURE:�ػ�;FALSE:���ػ�; <lParam>λ��ֵ; <����>����.
#define LB_GETPOS               0x018D  //[�����ʾλ��]: <wParam>����; <lParam>����; <����>λ��ֵ.
#define LB_GETTOPINDEX          0x018E	//[����׸���ʾ������Ŀ]: <wParam>����; <lParam>����; <����>����Ŀ����ֵ.
//#define LB_FINDSTRING           0x018F
//#define LB_GETSELCOUNT          0x0190
//#define LB_GETSELITEMS          0x0191
//#define LB_SETTABSTOPS          0x0192
//#define LB_GETHORIZONTALEXTENT  0x0193
//#define LB_SETHORIZONTALEXTENT  0x0194
//#define LB_SETCOLUMNWIDTH       0x0195
#define	LB_LOCKCURSEL			0x0196  //[������ǰѡ�е�����Ŀ(���ܱ�����ı�,����Ȼ����ʹ��LB_SETCURSEL)]: <wParam>TRUE:����; FALSE:�������; <lParam>����; <����>����.
#define LB_SETTOPINDEX          0x0197	//[�����׸���ʾ������Ŀ]: <wParam>����Ŀ����ֵ; <lParam>����; <����>����.
#define LB_GETITEMRECT          0x0198	//[�������Ŀ�ľ��β���]: <wParam>����Ŀ����ֵ; <lParam>RECTָ��; <����>����.
#define LB_GETITEMDATA          0x0199	//[�������Ŀ������ֵ]: <wParam>����Ŀ����ֵ; <lParam>����; <����>����ֵ.
#define LB_SETITEMDATA          0x019A	//[��������Ŀ������ֵ]: <wParam>����Ŀ����ֵ; <lParam>����ֵ; <����>����.
//#define LB_SELITEMRANGE         0x019B
//#define LB_SETANCHORINDEX       0x019C
//#define LB_GETANCHORINDEX       0x019D
//#define LB_SETCARETINDEX        0x019E
//#define LB_GETCARETINDEX        0x019F
#define LB_SETITEMHEIGHT        0x01A0	//[��������Ŀ�ĸ߶�]: <wParam>����Ŀ����ֵ; <lParam>�߶�ֵ; <����>����.
#define LB_GETITEMHEIGHT        0x01A1	//[�������Ŀ�ĸ߶�]: <wParam>����Ŀ����ֵ; <lParam>����; <����>�߶�ֵ.
#define	LB_GETITEMOFFSET		0x01A2	//[�������Ŀ��λ��]: <wParam>����Ŀ����ֵ; <lParam>����; <����>λ��.
//#define LB_FINDSTRINGEXACT      0x01A2
//#define LB_SETLOCALE            0x01A5
//#define LB_GETLOCALE            0x01A6
//#define LB_SETCOUNT             0x01A7
//#define LB_INITSTORAGE          0x01A8
//#define LB_ITEMFROMPOINT        0x01A9
#define	LB_SETTEXT				0x01AA	//[��������Ŀ������]: <wParam>����Ŀ����ֵ; <lParam>��������(WCHAR���ַ���); <����>����.
#define LB_OFFSETPOS			0x01AB  //[�ƶ���ʾλ��]: <wParam>TURE:�ػ�;FALSE:���ػ�; <lParam>ƫ��ֵ(����Ϊ�����ƶ�;����Ϊ�����ƶ�); <����>����.
#define	LB_MOVEINDEX			0x01AC  //[�ƶ�ָ������Ŀ������ֵ]: <wParam>����Ŀ����ֵ; <lParam>�µ�����ֵ; <����>����.
////
#define	LB_INSERTSTRING		LB_ADDSTRING

/*
 * Listbox Return Values
 */
#define LB_OKAY             0
#define LB_ERR              (-1)
#define LB_ERRSPACE         (-2)

/*============================================================================*/
#if 0
/***************/
/*   ComboBox  */
/***************/

/*
 * Combo Box return Values
 */
#define CB_OKAY             0
#define CB_ERR              (-1)
#define CB_ERRSPACE         (-2)


/*
 * Combo Box Notification Codes
 */
#define CBN_ERRSPACE        (-1)
#define CBN_SELCHANGE       1
#define CBN_DBLCLK          2
#define CBN_SETFOCUS        3
#define CBN_KILLFOCUS       4
#define CBN_EDITCHANGE      5
#define CBN_EDITUPDATE      6
#define CBN_DROPDOWN        7
#define CBN_CLOSEUP         8
#define CBN_SELENDOK        9
#define CBN_SELENDCANCEL    10


/*
 * Combo Box styles
 */
#define CBS_SIMPLE            0x0001L
#define CBS_DROPDOWN          0x0002L
#define CBS_DROPDOWNLIST      0x0003L
#define CBS_OWNERDRAWFIXED    0x0010L
#define CBS_OWNERDRAWVARIABLE 0x0020L
#define CBS_AUTOHSCROLL       0x0040L
#define CBS_OEMCONVERT        0x0080L
#define CBS_SORT              0x0100L
#define CBS_HASSTRINGS        0x0200L
#define CBS_NOINTEGRALHEIGHT  0x0400L
#define CBS_DISABLENOSCROLL   0x0800L
#define CBS_UPPERCASE         0x2000L
#define CBS_LOWERCASE         0x4000L


/*
 * Combo Box messages
 */
#define CB_GETEDITSEL               0x0140
#define CB_LIMITTEXT                0x0141
#define CB_SETEDITSEL               0x0142
#define CB_ADDSTRING                0x0143
#define CB_DELETESTRING             0x0144
#define CB_DIR                      0x0145
#define CB_GETCOUNT                 0x0146
#define CB_GETCURSEL                0x0147
#define CB_GETLBTEXT                0x0148
#define CB_GETLBTEXTLEN             0x0149
#define CB_INSERTSTRING             0x014A
#define CB_RESETCONTENT             0x014B
#define CB_FINDSTRING               0x014C
#define CB_SELECTSTRING             0x014D
#define CB_SETCURSEL                0x014E
#define CB_SHOWDROPDOWN             0x014F
#define CB_GETITEMDATA              0x0150
#define CB_SETITEMDATA              0x0151
#define CB_GETDROPPEDCONTROLRECT    0x0152
#define CB_SETITEMHEIGHT            0x0153
#define CB_GETITEMHEIGHT            0x0154
#define CB_SETEXTENDEDUI            0x0155
#define CB_GETEXTENDEDUI            0x0156
#define CB_GETDROPPEDSTATE          0x0157
#define CB_FINDSTRINGEXACT          0x0158
#define CB_SETLOCALE                0x0159
#define CB_GETLOCALE                0x015A
#define CB_GETTOPINDEX              0x015b
#define CB_SETTOPINDEX              0x015c
#define CB_GETHORIZONTALEXTENT      0x015d
#define CB_SETHORIZONTALEXTENT      0x015e
#define CB_GETDROPPEDWIDTH          0x015f
#define CB_SETDROPPEDWIDTH          0x0160
#define CB_INITSTORAGE              0x0161
#define CB_MSGMAX                   0x0162
#endif

/*============================================================================*/
/*
 * ������ / ScrollBar
 */

//// ������˽�з���־ / ScrollBar Styles
#define SBS_HORZ                    ((u16)(0<<0))   // ˮƽ�������.
#define SBS_VERT                    ((u16)(1<<0))   // ��ֱ�������.

#define	SBS_LEFT_ALIGN				((u16)(0<<1))   // �����������ʽ.
#define	SBS_RIGHT_ALIGN				((u16)(1<<1))   // �Ҷ���������ʽ.
#define	SBS_NOARROWS				((u16)(1<<2))   // �޼�ͷ���.
#define	SBS_NOTIFY					((u16)(1<<15))  // ���������֪ͨ��Ϣ.

#define	SBS_TOP_ALIGN				SBS_LEFT_ALIGN  // ��������������ʽ.
#define	SBS_BOTTOM_ALIGN			SBS_RIGHT_ALIGN // �ײ�����������ʽ.

/*
#define SBS_SIZEBOXTOPLEFTALIGN     0x0002L
#define SBS_SIZEBOXBOTTOMRIGHTALIGN 0x0004L
#define SBS_SIZEBOX                 0x0008L
#define SBS_SIZEGRIP                0x0010L
*/


/*
 * ������֪ͨ�� / Scrollbar Notify Codes.
 */
#define SBN_CLICKED    (0x00) //�������.
#define SBN_THUMBTRACK (0x01) //����λ���ƶ���.

////������������Ĳ�λ���塣
#define SB_TRACK      (0) //����.
#define	SB_ARROWUP	  (1) //�ϼ�ͷ.
#define	SB_ARROWDOWN  (2) //�¼�ͷ.
#define SB_PAGEUP     (3) //��ҳ(�ϼ�ͷ�뻬��֮�������).
#define SB_PAGEDOWN   (4) //��ҳ(�¼�ͷ�뻬��֮�������).

#define	SB_ARROWLEFT  (5) //���ͷ.
#define	SB_ARROWRIGHT (6) //�Ҽ�ͷ.
#define SB_PAGELEFT   (7) //��ҳ(���ͷ�뻬��֮�������).
#define SB_PAGERIGHT  (8) //��ҳ(�Ҽ�ͷ�뻬��֮�������).
//#define SB_THUMBTRACK (9)

//������(SCROLLBAR)��֪ͨ�븽����Ϣ�ṹ��.
typedef struct
{
	NMHDR hdr;       //������ HMHDR Ϊ��ͷ.
	S32 nTrackValue; //�������ֵ.
	U32 cmd;         //������������Ĳ�λ.

}NM_SCROLLBAR;;

/////����������������ֵ.
#define SIF_RANGE           (u16)(1<<0) //����.
#define SIF_VALUE           (u16)(1<<1) //��ǰֵ.
#define SIF_TRACKSIZE       (u16)(1<<2) //�����С.
#define SIF_ARROWSIZE       (u16)(1<<3) //��ͷ��С.

#define SIF_ALL             (SIF_ARROWSIZE| SIF_TRACKSIZE | SIF_VALUE | SIF_RANGE)

typedef struct
{
    u16  cbSize;     // SCROLLINFO �ṹ����ֽ���(�ڲ��øó�Աֵ���жϽṹ��ĺϷ���).
    u16  fMask;      // ����������ֵ(��ز���������ֵ��Ч���ܱ����û��ȡ).
	s32  nMin;       // ������Сֵ.
	s32  nMax;       // �������ֵ.
	s32  nValue;     // ��ǰֵ.
	u16  TrackSize;  // �����С.
	u16  ArrowSize;  // ��ͷ��С.

}SCROLLINFO,SCROLLBAR_CFG;

typedef	union
{
	struct
	{
		RECT Track;        //�������.
		RECT LeftArrow;    //���ͷ����.
		RECT RightArrow;   //�Ҽ�ͷ����.
		RECT LeftPage;     //�󻬸˾���.
		RECT RightPage;    //�һ��˾���.
	}H; //ˮƽ���������β���.

	struct
	{
		RECT Track;        //�������.
		RECT TopArrow;     //�ϼ�ͷ����.
		RECT BottomArrow;  //�¼�ͷ����.
		RECT TopPage;      //�ϻ��˾���.
		RECT BottomPage;   //�»��˾���.
	}V; //��ֱ���������β���.

}SCROLLBAR_RECT;

/*
 * ������״ֵ̬ / Scrollbar State
 */
#define	SST_THUMBTRACK		(1<<0) //����״̬.
#define SST_LINELEFT        (1<<1) //���ͷ���״̬.
#define SST_LINERIGHT       (1<<2) //�Ҽ�ͷ���״̬.
#define SST_PAGELEFT        (1<<3) //��ҳ���״̬.
#define SST_PAGERIGHT       (1<<4) //��ҳ���״̬.

#define SST_LINEUP          SST_LINELEFT  //�ϼ�ͷ���״̬.
#define SST_LINEDOWN        SST_LINERIGHT //�¼�ͷ���״̬.
#define SST_PAGEUP          SST_PAGELEFT  //��ҳ���״̬.
#define SST_PAGEDOWN        SST_PAGERIGHT //��ҳ���״̬.

/*
 * ��������Ϣ / Scrollbar messages
 */
#define SBM_SETVALUE                0x00E0 //[���õ�ǰֵ]: <wParam>TRUE:�ػ�;FALSE:���ػ�; <lParam>��ǰֵ; <����>����.
#define SBM_GETVALUE                0x00E1 //[���õ�ǰֵ]: <wParam>����; <lParam>����; <����>��ǰֵ.
#define SBM_SETRANGE                0x00E2 //[��������]: <wParam>������Сֵ; <lParam>�������ֵ; <����>����.
#define SBM_GETRANGE                0x00E3 //[�������]: <wParam>������Сֵ(s32 ָ��); <lParam>�������ֵ(s32 ָ��); <����>����.
//#define SBM_ENABLE_ARROWS           0x00E4
#define SBM_GETSTATE           		0x00E5 //[���״̬]: <wParam>����; <lParam>����; <����>״ֵ̬.
#define SBM_SETSCROLLINFO           0x00E9 //[���ò���]: <wParam>TRUE:�ػ�;FALSE:���ػ�; <lParam>����(SCROLLINFOָ��,cbSize��Ա��Ԥ��������Чֵ); <����>����.
#define SBM_GETSCROLLINFO           0x00EA //[��ò���]: <wParam>����; <lParam>����(SCROLLINFOָ��,cbSize��Ա��Ԥ��������Чֵ); <����>����.
#define	SBM_GETTRACKRECT		    0x00EB //[��û�����β���]: <wParam>����; <lParam>RECTָ��; <����>����.
#define	SBM_GETRECT		  	 		0x00EC //[������о��β���]: <wParam>����; <lParam>SCROLLBAR_RECTָ��; <����>����.

#if 0
/*
 * Scroll Bar Constants
 */
#define SB_HORZ             0
#define SB_VERT             1
#define SB_CTL              2
#define SB_BOTH             3
#endif

/*===================================================================================*/

/*===================================================================================*/
/*===================================================================================*/

//// GDI API
int		GetSystemMetrics(int nIndex);

COLOR_RGB32 RGB32_AVG(COLOR_RGB32 c1,COLOR_RGB32 c2);
COLOR_RGB32 RGB32_Offset(COLOR_RGB32 c,int offR,int offG,int offB);


COLORREF	MapRGB332(HDC hdc,U8 rgb332);
COLORREF	MapRGB565(HDC hdc,U16 rgb565);
COLORREF	MapXRGB1555(HDC hdc,U16 xrgb1555);
COLORREF	MapARGB1555(HDC hdc,U16 argb1555);
COLORREF	MapXRGB4444(HDC hdc,U16 xrgb4444);
COLORREF	MapARGB4444(HDC hdc,U16 argb4444);
COLORREF	MapXRGB8888(HDC hdc,U32 xrgb8888);
COLORREF	MapARGB8888(HDC hdc,U32 argb8888);
COLORREF	MapRGB(HDC hdc,U8 r,U8 g,U8 b);
COLORREF	MapARGB(HDC hdc,U8 a,U8 r,U8 g,U8 b);
U32			ToARGB(HDC hdc,COLORREF color);
void		GetRGB(HDC hdc,COLORREF color,U8 *r,U8 *g,U8 *b);
void		GetARGB(HDC hdc,COLORREF color,U8 *a,U8 *r,U8 *g,U8 *b);

#define		MapRGB888(hdc,rgb888)	MapXRGB8888(hdc,rgb888)
#define		MapRGB32(hdc,rgb888)	MapXRGB8888(hdc,rgb888)


int		SetTextAlign(HDC hdc,int align);
int		GetTextAlign(HDC hdc);
COLORREF	SetTextColor(HDC hdc,COLORREF color);
COLORREF	GetTextColor(HDC hdc);
BOOL	GetDrawRect(HDC hdc,RECT *lprc);
BOOL	SetDrawRect(HDC hdc,const RECT *rc_new,RECT *rc_old);

SURFACE*	CreateSurface(SURF_FORMAT Format,U32 nWidth,U32 nHeight,int LineBytes,void *Bits);
void		DeleteSurface(const SURFACE *pSurf);
HDC  CreateDC(const SURFACE *pSurf,const RECT *lprc);
HDC  CreateMemoryDC(SURF_FORMAT Format,int nWidth,int nHeight);
BOOL DeleteDC(HDC hdc);


BOOL	CopyBits(HDC hdc,const RECT *rc_in,RECT *rc_out,int line_bytes,U8 *bits_out);
BOOL	DCtoBitmap(HDC hdc,BITMAP *bitmap);

void	LPtoDP(HDC hdc,POINT *lpPoint,int nCount);
void	DPtoLP(HDC hdc,POINT *lpPoint,int nCount);
BOOL	MoveToEx(HDC hdc,int x,int y,POINT *pt_old);


//int			SetPenStyle(HDC hdc,int style);
//int			GetPenStyle(HDC hdc);
COLORREF	SetPenColor(HDC hdc,COLORREF color);
COLORREF	GetPenColor(HDC hdc);
int			SetPenSize(HDC hdc,int size);
int			GetPenSize(HDC hdc);

//int			SetBrushStyle(HDC hdc,int style);
//int			GetBrushStyle(HDC hdc);
 
COLORREF	SetBrushColor(HDC hdc,COLORREF color);
COLORREF	GetBrushColor(HDC hdc);


HFONT   CreateFont(const FONT_OPS *ft_ops,const void *pdata);
void    DeleteFont(HFONT hFont);
HFONT	SetFont(HDC hdc,HFONT hFont);
HFONT	GetFont(HDC hdc);


BOOL	SetTextInterval(HDC hdc, S16 IntervalX,S16 IntervalY);
/* ����: SetTextInterval:�����ַ���ʾ�����С(ÿ���ַ���ʾ��ռ�Ŀ�Ⱥ͸߶�).
 * ����: IntervalX/IntervalY:����ֵ(���ش�С),�������Ϊ0,��ʹ�������ڲ�Ĭ��ֵ.
 */

BOOL	SetTextSpace(HDC hdc, S16 SpaceX,S16 SpaceY);
/* ����: SetTextSpace:�����ַ���ʾ�ռ��С(�����ַ����־༰�о�).
 * ����: SpaceX/SpaceY:���ֵ(���ش�С).
 */

BOOL 	GetFontInfo(HFONT hFont,FONT_INFO *ft_info);
int 	GetFontAveHeight(HFONT hFont);
int     GetTextWidth(HDC hdc, const WCHAR *lpString, int Count);
BOOL    GetTextExtent(HDC hdc, const WCHAR *lpString, int Count, SIZE16 *size_out);

BOOL	IsEnableAlpha(HDC hdc);
BOOL	EnableAlpha(HDC hdc,BOOL bEnable);
U8		SetAlpha(HDC hdc,U8 Alpha);

BOOL 	EnableAntiAlias(HDC hdc,BOOL bEnable);
U8 		SetAntiAliasRate(HDC hdc,U8 AA_Rate);

COLOR_RGB32		AlphaBlendColor(COLOR_RGB32 bk_c,COLOR_RGB32 fr_c,U8 alpha);
void	ClrDisplay(HDC hdc,const RECT *lpRect,COLORREF color);
void		SetPixel(HDC hdc,int x,int y,COLORREF color);
COLORREF	GetPixel(HDC hdc,int x,int y);
void  XorPixel(HDC hdc,int x,int y);
void  Line(HDC hdc,int sx,int sy,int ex,int ey);
BOOL  LineTo(HDC hdc,int x,int y);
void  HLine(HDC hdc,int sx,int sy,int ex);
void  VLine(HDC hdc,int sx,int sy,int ey);
void  PolyLine(HDC hdc,int xOff,int yOff,const POINT *pt,int cnt);

void 	DrawArc(HDC hdc,int cx, int cy, int r, int a0, int a1);
void	FillArc(HDC hdc,int cx, int cy, int ri, int ro, int a0, int a1);

void 	DrawEllipse(HDC hdc,int cx, int cy, int rx, int ry);
void 	FillEllipse(HDC hdc,int cx, int cy, int rx, int ry);

void	DrawCircle(HDC hdc,int cx,int  cy,int r);
void	FillCircle(HDC hdc,int cx,int  cy,int r);


void	DrawRect(HDC hdc,const RECT *lpRect);
void	FillRect(HDC hdc,const RECT *lpRect);
void	Draw3DRect(HDC hdc,const RECT *lpRect,COLORREF	Color0,COLORREF	Color1);
void	Fill3DRect(HDC hdc,const RECT *lpRect,COLORREF	Color0,COLORREF	Color1);

void	GradientFillRect(HDC hdc,const RECT *lpRect,COLORREF c1,COLORREF c2,BOOL bVert);
//void	DitheredFillRect(HDC hdc,const RECT *lpRect,COLORREF Color,U8 v);

void	DrawRoundRect(HDC hdc,const RECT *lpRect,int r);
void	FillRoundRect(HDC hdc,const RECT *lpRect,int r);
void	GradientFillRoundRect(HDC hdc,const RECT *lpRect,int r,COLORREF c1,COLORREF c2,BOOL bVert);

void	DrawPolygon(HDC hdc,int xOff,int yOff,const POINT *pt,int count);
void	FillPolygon(HDC hdc,int xOff,int yOff,const POINT *pt,int count);
void	FillFlood(HDC hdc,int x,int y,COLORREF color);

BOOL 	TextOut(HDC hdc,int x,int y,const WCHAR *pText,int nCount);
int 	DrawTextEx(HDC hdc, const WCHAR *pText, int nCount, const RECT *lpRect, UINT uDTFormat, const DRAWTEXTPARAMS *pDTParams);



BOOL	BitBlt(	HDC dst_hdc,int dst_x,int dst_y,UINT dst_w,UINT dst_h,
				HDC src_hdc,int src_x,int src_y,U32 dwRop);
BOOL	StretchBlt(	HDC dst_hdc,int dst_x,int dst_y,UINT dst_w,UINT dst_h,
					HDC src_hdc,int src_x,int src_y,UINT src_w,UINT src_h,
					U32 dwRop);
BOOL	TransparentBlt(	HDC dst_hdc,int dst_x,int dst_y,UINT dst_w,UINT dst_h,
						HDC src_hdc,int src_x,int src_y,UINT src_w,UINT src_h,
						COLORREF key_color);
BOOL	AlphaBlend(	HDC dst_hdc,int dst_x,int dst_y,UINT dst_w,UINT dst_h,
					HDC src_hdc,int src_x,int src_y,UINT src_w,UINT src_h,
					BLENDFUNCTION bf);


void 	AA_DrawLine(HDC hdc, int sx, int sy, int ex, int ey,COLORREF c);
void 	AA_DrawBoldLine(HDC hdc, int sx, int sy, int ex, int ey,COLORREF c);
//void	AA_DrawPolygon(HDC hdc,int xOff,int yOff,const POINT *ps,int count);
//void	AA_FillPolygon(HDC hdc,int xOff,int yOff,const POINT *ps,int count);
//void	AA_DrawCircle(HDC hdc,int cx,int cy,int r);
//void	AA_FillCircle(HDC hdc,int cx,int cy,int r);

//BITMAP*	LoadBitmap(rwctx_t *rwctx,CONST LPRECT lprc);
BOOL	DrawBitmap(HDC hdc,int x,int y,const BITMAP *bitmap,const RECT *lpRect);
BOOL 	ScaleBitmap(HDC hdc, int dst_x, int dst_y, int dst_w,int dst_h,const BITMAP *bitmap);
BOOL	RotateBitmap(HDC hdc,int cx,int cy,const BITMAP *bitmap,int angle);

//////////// BMP
BOOL	BMP_GetInfo(BITMAPINFO *bm_info,const void *pBMPData);
BOOL	BMP_GetInfoEx(BITMAPINFO *bm_info,GUI_GET_DATA *read_data);
BOOL	BMP_Draw(HDC hdc,int x,int y,const void *pBMPData,const RECT *lprc);
BOOL	BMP_DrawEx(HDC hdc,int x,int y,GUI_GET_DATA *read_data,const RECT *lprc);

//////////// GIF
typedef struct	tagGIF_DECODE* HGIF;
typedef	S32	GIF_DELAY;

HGIF    GIF_Open(const void *dat);
BOOL    GIF_GetInfo(HGIF hGIF,IMAGE_INFO *Info);
UINT    GIF_GetFrameCount(HGIF hGIF);
GIF_DELAY    GIF_GetFrameDelay(HGIF gif_dec,UINT frame_idx);
GIF_DELAY    GIF_DrawFrame(HDC hdc,int x,int y,COLORREF bk_color,HGIF hGIF,UINT frame_idx);
void    GIF_Close(HGIF hGIF);


//// Region
void 	OffsetPoint(POINT *pt,int count,int dx,int dy);
void 	RotatePoint(POINT *pt,int count,int cx,int cy,float angle);

int		RectX(const RECT *rc);
int		RectY(const RECT *rc);
int		RectW(const RECT *rc);
int		RectH(const RECT *rc);
void	SetRectW(RECT *rc,int w);
void	SetRectH(RECT *rc,int h);
void	SetRectEmpty(RECT *rc);
BOOL	IsRectEmpty(const RECT *rc);
BOOL	PtInRect(const RECT *rc,const POINT *pt);
void	SetRect(RECT *rc,LONG left,LONG top,LONG right,LONG bottom);
void	SetRectArea(RECT *rc,int x,int y,int w,int h);

void	CopyRect(RECT *dst_rc,const RECT *src_rc);
void	OffsetRect(RECT *rc,int xoffset,int yoffset);
BOOL	InflateRect(RECT *lprc,int dx,int dy);
BOOL	InflateRectEx(RECT *lprc,int l,int t,int r,int b);
BOOL	IsEqualRect(const RECT *rc0,const RECT *rc1);
BOOL	IsCoveredRect(const RECT *rc0,const RECT *rc1);
BOOL	IsIntersectRect(const RECT *rc0,const RECT *rc1);
BOOL 	IntersectRect(RECT *dst, const RECT *rc1, const RECT *rc2);
void	GetBoundRect(RECT *dst,const RECT *rc1,const RECT *rc2);
int		SubtractRectEx(RECT *dst,const RECT *rc1,const RECT *rc2);
int		UnionRect(RECT *dst,const RECT *rc1,const RECT *rc2);
int		XorRect(RECT *dst,const RECT *rc1,const RECT *rc2);

void	MakeMatrixRect(RECT *dst,const RECT*lpRect,int x_space,int y_space,int	x_num,int y_num);
BOOL 	MakeProgressRect(RECT *dst, const RECT *lpRect, U32 Range,U32 Val,ePB_ORG Org);

HRGN	CreateRectRgnIndirect(const RECT *lpRect);
HRGN 	CreateRectRgn(int x, int y, int w, int h);
BOOL	DeleteRgn(HRGN hRgn);

HRGN	SelectClipRgn(HDC hdc,HRGN hRgn);
HRGN	GetClipRgn(HDC hdc);

//// Message
LRESULT	SendMessage(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
LONG	DispatchMessage(const MSG *pMsg);
BOOL	PostAsyncMessage(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
BOOL	PostMessage(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL 	PostCloseMessage(HWND hwnd);
BOOL	PostQuitMessage(HWND hwnd);
BOOL	PeekMessage(MSG *pMsg,HWND hwnd ,UINT mode);
int		GetMessage(MSG *pMsg,HWND hwnd);
void	TranslateMessage(const MSG *pMsg);
int		NotifyParent(HWND hwnd,UINT code);
int		NotifyParentEx(HWND hwnd,UINT code,NMHDR *nr);


//// Window
BOOL	SetWindowText(HWND hwnd,LPCWSTR lpString);
int		GetWindowText(HWND hwnd,LPWSTR lpString,int MaxCount);
int		GetWindowTextLength(HWND hwnd);
HFONT 	SetWindowFont(HWND hwnd, HFONT hFont);
HFONT 	GetWindowFont(HWND hwnd);

LONG	SetWindowLong(HWND hwnd,int index,LONG dwNewLong);
LONG	GetWindowLong(HWND hwnd,int index);


WNDERASEBKGND	SetWindowErase(HWND hwnd,WNDERASEBKGND cb);
WNDERASEBKGND	GetWindowErase(HWND hwnd);
void	SetWindowEraseEx(HWND hwnd,WNDERASEBKGND cb,BOOL bSetAllChildren);


WNDPROC	SetWindowProc(HWND hwnd,WNDPROC cb);
WNDPROC	GetWindowProc(HWND hwnd);

HWND 	GetDlgItem(HWND hwnd,int nID);
BOOL	IsWindowVisible(HWND hwnd);

void	EnableDragWindow(HWND hwnd,BOOL bEnable);
BOOL	IsEnableDragWindow(HWND hwnd);

BOOL	ScreenToClient(HWND hwnd,POINT *lpPoint,int count);
BOOL	ClientToScreen(HWND hwnd,POINT *lpPoint,int count);
BOOL	ScreenToWindow(HWND hwnd,POINT *lpPoint,int count);
BOOL	WindowToScreen(HWND hwnd,POINT *lpPoint,int count);
HWND	GetDesktopWindow(void);
BOOL	GetWindowRect(HWND hwnd,RECT *lpRect);
BOOL	GetClientRect(HWND  hwnd,RECT *lpRect);
BOOL	GetClientRectToScreen(HWND  hwnd,RECT *lpRect);
BOOL 	IsWindowEnabled(HWND hwnd);
BOOL 	EnableWindow(HWND hwnd,BOOL bEnable);
BOOL	InvalidateRect(HWND hwnd ,const RECT *lpRect,BOOL bErase);
BOOL	SetWindowPos(HWND hwnd, HWND hwndInsertAfter, int x, int y, int cx, int cy,UINT Flags);
BOOL	MoveWindow(HWND hwnd,int x,int y,int nWidth,int nHeight,BOOL bRepaint);
BOOL	ShowWindow(HWND hwnd,int nCmdShow);
BOOL    RedrawWindow(HWND hwnd, const RECT *lprcUpdate, UINT Flags);

HWND 	GetWindow(HWND hwnd,UINT uCmd);

HDC  	GetDCEx(HWND hwnd, HRGN hrgnClip, U32 flags);
HDC  	GetDC(HWND hwnd);
HDC  	GetWindowDC(HWND hwnd);
BOOL 	ReleaseDC(HWND hwnd,HDC hdc);

HDC		BeginPaint(HWND hwnd,PAINTSTRUCT *lpPaint);
BOOL	EndPaint(HWND hwnd,const PAINTSTRUCT *lpPaint);
BOOL	UpdateWindow(HWND hwnd);

HWND 	GetWindowFromPoint(HWND hwnd, const POINT *pt);
HWND	SetFocus(HWND hwnd);
HWND	GetFocus(void);
HWND	SetCapture(HWND hwnd);
HWND	GetCapture(void);
BOOL	ReleaseCapture(void);
HWND	GetForegroundWindow(void);
BOOL	SetForegroundWindow(HWND hwnd);

HWND	GetParent(HWND hwnd);
BOOL	GetClassInfo(LPCWSTR lpClassName,WNDCLASS *WndClass);
LRESULT	DefWindowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL	AdjustWindowRect(RECT *pRect,U32 dwStyle);
BOOL	AdjustWindowRectEx(RECT *pRect,U32 dwStyle,U32 dwExStyle);

HWND    CreateWindowEx(U32 ExStyle, const WNDCLASS *pClass, LPCWSTR lpWindowName,
		  U32 dwStyle, int x, int y, int nWidth, int nHeight,
		  HWND hwndParent, U32 WinId,HINSTANCE hInstance,LPVOID lpParam);

int	DestroyWindow(HWND hwnd);


//// Timer Flags
#define	TMR_SINGLE  (1<<0) //���ָ���ñ��,��Ϊ���ζ�ʱ����,����Ϊѭ����ʱ����.
#define	TMR_START   (1<<1) //���ָ���ñ��,��������ʱ��.


HTMR 	SetTimer(HWND hwnd,UINT TMR_Id,U32 IntervalMS,U32 Flags,TIMERPROC Proc);
BOOL	StartTimer(HWND hwnd,UINT TMR_Id,BOOL bStart);
BOOL	ResetTimer(HWND hwnd,UINT TMR_Id,U32 IntervalMS,U32 Flags,TIMERPROC Proc);
BOOL 	KillTimer(HWND hwnd,UINT TMR_Id);

U16	    GetKeyState(int Vkey);
U16 	GetAsyncKeyState(int Vkey);
BOOL	GetKeyboardState(U8 *lpKeyState);

int	DialogWindowEx(U32 ExStyle, const WNDCLASS *wcex, LPCWSTR lpWindowName,
                   U32 Style, int x, int y, int Width, int Height,
                   HWND hWndParent, UINT WinId,HINSTANCE hInstance,LPVOID lpParam);

int	MessageBoxEx(U32 ExStyle,U32 Style,HWND hwndParent,int x,int y,int w,int h,
		         const WCHAR *pText,const WCHAR *pCaption,const MSGBOX_OPTIONS *opt);

/*===================================================================================*/

#define	DrawText(hdc, lpString, nCount, lpRect, uFormat)\
		DrawTextEx(hdc,lpString,nCount,lpRect,uFormat,NULL)

#define	CreateWindow(lpClass, lpWindowName, dwStyle, x, y, nWidth, nHeight, hwndParent, WinId, hInstance, lpParam)\
		CreateWindowEx(0,lpClass,lpWindowName,dwStyle,x,y,nWidth,nHeight,hwndParent,WinId,hInstance,lpParam)

#define	DialogWindow(wcex, lpWindowName, dwStyle, x, y, nWidth, nHeight, hwndParent, WinId, hInstance, lpParam)\
		DialogWindowEx(0,wcex,lpWindowName,dwStyle,x,y,nWidth,nHeight,hwndParent,WinId,hInstance,lpParam)

#define	MessageBox(hwndParent, x, y, w, h, pText, pCaption, opt)\
		MessageBoxEx(0,WS_OVERLAPPEDWINDOW,hwndParent,x,y,w,h,pText,pCaption,opt)
	
/*===================================================================================*/
//// Cursor
int 	ShowCursor(BOOL bShow);
HCURSOR SetCursor(HCURSOR hCursor);
BOOL 	ClipCursor(CONST RECT *lpRect);
BOOL  	SetCursorPos(int x, int y);
BOOL	GetCursorPos(POINT *lpPoint);

void MouseInput(int x,int y,u16 mouse_key);
void PostKeyMessage(U8 key,BOOL IsKeyDown);

/*===================================================================================*/
/*===================================================================================*/

typedef	int (FN_XFT_GetData)(void *buf,int offset,int size,LONG lParam);
HFONT	XFT_CreateFont(const void *xft_dat);
HFONT	XFT_CreateFontEx(FN_XFT_GetData *pfnGetData,LONG lParam);

/*===================================================================================*/
#include "gui_os_port.h"
#include "emXGUI_Arch.h"
#include "gui_drv.h"
#include "web_color.h"


/*===================================================================================*/

#ifdef	__cplusplus
}
#endif
#endif
