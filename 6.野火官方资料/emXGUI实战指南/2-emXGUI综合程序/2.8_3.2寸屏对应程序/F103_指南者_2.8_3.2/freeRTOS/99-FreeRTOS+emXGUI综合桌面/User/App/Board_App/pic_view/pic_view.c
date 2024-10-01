#include <emXGUI.h>
#include "pic_view.h"
#include	"CListMenu.h"
#include "GUI_AppDef.h"
#include "Widget.h"
#include "ff.h"
#include "x_libc.h"
#include <string.h>
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"

//ͼ���������
icon_S GUI_PicViewer_Icon[12] = 
{
  {"Pic_Name",           {100,0,600,35},        FALSE},
  {"Pic_MSGBOX",         {200,240,400,70},      FALSE},
  {"Pic_Res",            {130,35,100,35},       FALSE},
  {"Pic_Res_Value",      {230,35,100,35},       FALSE},
  {"Pic_Time",           {330,35,70,35},       FALSE}, 
  {"Pic_Time_Value",     {400,35,90,35},       FALSE},
  {"Pic_FPS",            {490,35,70,35},       FALSE}, 
  {"Pic_FPS_Value",      {560,35,90,35},       FALSE},  
  {"In_Flash",           {0,420,250,60},       FALSE},
  {"Ex_Flash",           {250,420,250,60},       FALSE}, 
  {"SD_Card",            {500,420,300,60},       FALSE}, 
  
};

static void dummy(HWND hwnd)
{

}
//�ļ�·���б�
static const struct __obj_list path_list_1[] = {

      L"�ڲ�FLASH",	NULL,   L"e", RGB_WHITE,				dummy,
      L"�ⲿFLASH",		NULL,	  L"f", RGB_WHITE,				dummy,  
      L"SD��",	NULL,	  L"g", RGB_WHITE,				dummy, 
        NULL,	NULL,	NULL,NULL, NULL,//������־!

};


static char path[100] = "0:";//�ļ���Ŀ¼
/* JPGͼƬ���ݣ�������ڲ�FLASH--��ǰ�汾��ȡʧ�� */
extern char tiger_jpg[];
/* �ⲿͼƬ���ݴ�С */
extern unsigned int tiger_jpg_size(void);

/* �ⲿͼƬ���� */
extern char redfish[];
/* �ⲿͼƬ���ݴ�С */
extern unsigned int redfish_size(void);

extern const unsigned char gImage_0[];
//GIFͼƬ���ݣ�������ڲ�FLASH
extern char king[];
/* GIFͼƬ���ݴ�С */
extern unsigned int minion_gif_size(void);
//ͼƬ���������ṹ��
PicViewer_Master_Struct PicViewer = 
{
  .pic_nums = 0,//ͼƬ��Ŀ
  .show_index = 0,//��ǰͼƬ���
  .Menu_State = 0,//�˵�״̬�����ã�
  .SecMenu_State = 0,//�����ã�
  .cur_type = 0,//��ǰ����
  .cur_path = eID_Pic_SDCARD,//��ǰ·��
};
//�˳���ť�ػ���
static void PicViewer_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc,hdc_mem;
	RECT rc;
 // RECT rc_top={0,0,800,70};
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.
//  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
//  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��
   
  hdc_mem = CreateMemoryDC(SURF_ARGB4444,rc.w,rc.h);
  ClrDisplay(hdc_mem, &rc, MapARGB(hdc_mem,255,0,0,0));
  ClrDisplay(hdc, &rc,  MapRGB(hdc,0,0,0));
//  ClrDisplay(hdc_mem, 
  SetBrushColor(hdc_mem, MapARGB(hdc_mem, 155,105,105,105));
  FillRect(hdc_mem, &rc);  
  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_mem, 0, 0, SRCCOPY);
  DeleteDC(hdc_mem);
	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));
   
  FillCircle(hdc, rc.x+rc.w, rc.y, rc.w);
	//FillRect(hdc, &rc); //�þ�����䱳��

  if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
//    GUI_DEBUG("ds->ID=%d,BST_PUSHED",ds->ID);
//		SetBrushColor(hdc,MapRGB(hdc,150,200,250)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
//		SetPenColor(hdc,MapRGB(hdc,250,0,0));        //���û���ɫ(PenColor��������Draw���͵Ļ�ͼ����)
		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬
//		SetBrushColor(hdc,MapRGB(hdc,255,255,255));
//		SetPenColor(hdc,MapRGB(hdc,0,250,0));
		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}

	  /* ʹ�ÿ���ͼ������ */
	SetFont(hdc, controlFont_64);
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
  rc.y = -10;
  rc.x = 16;
	DrawText(hdc, wbuf, -1, &rc, NULL);//��������(���ж��뷽ʽ)


  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);

}
//��ͨ��ť�ػ���
static void PicViewer_Button_OwnerDraw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc, rc_tmp;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.

  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��

  //BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
//  SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
//  FillRect(hdc, &rc);
	if (IsWindowEnabled(hwnd) == FALSE)
	{
		SetTextColor(hdc, MapRGB(hdc, COLOR_INVALID));
	}
	else if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
//    GUI_DEBUG("ds->ID=%d,BST_PUSHED",ds->ID);
//		SetBrushColor(hdc,MapRGB(hdc,150,200,250)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
//		SetPenColor(hdc,MapRGB(hdc,250,0,0));        //���û���ɫ(PenColor��������Draw���͵Ļ�ͼ����)
		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬
//		SetBrushColor(hdc,MapRGB(hdc,255,255,255));
//		SetPenColor(hdc,MapRGB(hdc,0,250,0));
		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}


	//	SetBrushColor(hdc,COLOR_BACK_GROUND);

	//	FillRect(hdc,&rc); //�þ�����䱳��
	//	DrawRect(hdc,&rc); //���������
	//  
	//  FillCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //�þ�����䱳��FillCircle
	//	DrawCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //���������

	  /* ʹ�ÿ���ͼ������ */
//	SetFont(hdc, controlFont_64);
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ�������

	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//��������(���ж��뷽ʽ)


//  /* �ָ�Ĭ������ */
//	SetFont(hdc, defaultFont);
}
//͸���ı�
static void PicViewer_TBOX_OwnerDraw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc, rc_tmp;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��
  
//  SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
//  FillRect(hdc, &rc);
  //BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc_tmp.x, rc_tmp.y, SRCCOPY);
  SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));


  GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������

  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)

}
//���ذ����ػ���
static void Pic_ReturnBTN_Ownerdraw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.
  if(ds->State & BST_PUSHED) //��ť�ǰ���״̬
	{

		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //��������ɫ
	}
	else//��ť�ǵ���״̬
	{ 

		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}

	/* ʹ�ÿ���ͼ������ */
	SetFont(hdc, controlFont_32);

	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������

	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER);//��������(���ж��뷽ʽ)
   rc.x = 25; 
  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);
  DrawText(hdc, L"����", -1, &rc, DT_VCENTER);
}
//����JPGͼƬ
//i�����Ŵ�����j������С����
//�Ŵ�ͼƬ��������Ϊ800*480��
//��С��ͼƬ��������Ϊ320*240
//ͨ��StretchBlt����ʵ��
void Draw_Pic_JPG(char *file_name)
{
  BOOL res;
  RECT rc = {0,0,800,480};
  u8 *jpeg_buf;
  static int i,j = 0;//�Ŵ�������С����
  u32 jpeg_size;
  JPG_DEC *dec;
  u16 wid = 0, high = 0;
  if(PicViewer.cur_path == eID_Pic_EXTFLASH)
    res= RES_Load_Content(JPG_FILE_NAME, (char**)&jpeg_buf, &jpeg_size);
  else
    res= FS_Load_Content(file_name, (char**)&jpeg_buf, &jpeg_size);
  if(res)
  {
    /* ����ͼƬ���ݴ���JPG_DEC��� */
    dec = JPG_Open(jpeg_buf, jpeg_size);
    /* ��ȡͼƬ�ļ���Ϣ */
    JPG_GetImageSize(&wid, &high,dec);
    switch(PicViewer.scale_state)
    {
      case 0://ԭʼ״̬
      {
        GUI_DEBUG("ԭʼ״̬");
        PicViewer.pic_width = wid;
        PicViewer.pic_height = high; 
        i = 0; 
        j = 0;
        break;
      }
      case 1://�Ŵ�
      {
        if(PicViewer.is_scale)
        {
          PicViewer.is_scale = 1;
        }
        else
          i++;
        j = 0;
        if(PicViewer.pic_width == 800 && PicViewer.pic_height == 480) i--;
          
        else if(PicViewer.pic_width < 800 && PicViewer.pic_height == 480)
        {
          PicViewer.pic_width = PicViewer.pic_width + 20 * i ;
          if(PicViewer.pic_width > 800) PicViewer.pic_width = 800;
        }
        else if(PicViewer.pic_width == 800 && PicViewer.pic_height < 480)
        {
           PicViewer.pic_height = PicViewer.pic_height + 20 *i ;
          if(PicViewer.pic_height > 480) PicViewer.pic_height = 480;         
        }
        else
        {
          PicViewer.pic_width = PicViewer.pic_width+20*i;
          PicViewer.pic_height = PicViewer.pic_height+20*i; 
          if(PicViewer.pic_height > 480) PicViewer.pic_height = 480;  
          if(PicViewer.pic_width > 800) PicViewer.pic_width = 800;
        }
        
        break;  
      }
      case 2://��С
      {
        if(PicViewer.is_scale)
        {          
          PicViewer.is_scale = 1;
        } 
        else
          j++;
        i = 0;
        if(PicViewer.pic_width > 320 && PicViewer.pic_height > 240)
        {
          PicViewer.pic_width = PicViewer.pic_width-20*j;
          PicViewer.pic_height = PicViewer.pic_height-20*j; 
          if(PicViewer.pic_height < 240) PicViewer.pic_height = 240;  
          if(PicViewer.pic_width < 320) PicViewer.pic_width = 320;          
        }         
        else if(PicViewer.pic_width > 320 && PicViewer.pic_height <= 240)
        {
          PicViewer.pic_width = PicViewer.pic_width - 20 * j ;
          if(PicViewer.pic_width < 320) PicViewer.pic_width = 320;
        }
        else if(PicViewer.pic_width <= 320 && PicViewer.pic_height > 240)
        {
           PicViewer.pic_height = PicViewer.pic_height - 20 *j;
          if(PicViewer.pic_height < 240) PicViewer.pic_height = 240;         
        }
        else
        {
          j--;
        }
        
        break;  
      
      }
    }    
    PicViewer.mhdc_pic = CreateMemoryDC(SURF_SCREEN, 800, 480);
    SetBrushColor(PicViewer.mhdc_pic, MapRGB(PicViewer.mhdc_pic, 0, 0, 0));
    FillRect(PicViewer.mhdc_pic, &rc);     
    
    HDC hdc_tmp;
//    int scale_w = (PicViewer.pic_width>660)?660:PicViewer.pic_width;
//    int scale_h = (PicViewer.pic_height>410)?410:PicViewer.pic_height;
//    
//    PicViewer.scale_x = (scale_w == PicViewer.pic_width)?1:(float)660/PicViewer.pic_width;
//    PicViewer.scale_y = (scale_h == PicViewer.pic_height)?1:(float)410/PicViewer.pic_height;
    
    hdc_tmp = CreateMemoryDC(SURF_SCREEN, 800, 480);
    
    JPG_Draw(hdc_tmp, 0, 0, dec);
    StretchBlt(PicViewer.mhdc_pic,400-PicViewer.pic_width/2,240 - PicViewer.pic_height/2,
    PicViewer.pic_width,PicViewer.pic_height,
               hdc_tmp,0,0,wid,high,SRCCOPY);
    DeleteDC(hdc_tmp);
    /* �ر�JPG_DEC��� */
    JPG_Close(dec);
  }
  /* �ͷ�ͼƬ���ݿռ� */
  RES_Release_Content((char **)&jpeg_buf);  
}
//�����ڲ�JPGͼƬ--ʧ��

void Draw_Pic_JPG_INTFLASH()
{
//  RECT rc = {0,0,800,480};
//  u8 *jpeg_buf;
//  JPG_DEC *dec;
  //if(res)
  {
    /* ����ͼƬ���ݴ���JPG_DEC��� */
//    dec = JPG_Open(tiger_jpg, tiger_jpg_size());
//    /* ��ȡͼƬ�ļ���Ϣ */
//    JPG_GetImageSize(&PicViewer.pic_width, &PicViewer.pic_height,dec);
//    
//    PicViewer.mhdc_pic = CreateMemoryDC(SURF_SCREEN, 800, 480);
//    SetBrushColor(PicViewer.mhdc_pic, MapRGB(PicViewer.mhdc_pic, 0, 0, 0));
//    FillRect(PicViewer.mhdc_pic, &rc);     
//    
//    HDC hdc_tmp;
////    int scale_w = (PicViewer.pic_width>660)?660:PicViewer.pic_width;
////    int scale_h = (PicViewer.pic_height>410)?410:PicViewer.pic_height;
////    
////    PicViewer.scale_x = (scale_w == PicViewer.pic_width)?1:(float)660/PicViewer.pic_width;
////    PicViewer.scale_y = (scale_h == PicViewer.pic_height)?1:(float)410/PicViewer.pic_height;
//    
//    hdc_tmp = CreateMemoryDC(SURF_SCREEN, 800, 480);
//    JPG_Draw(hdc_tmp, 0, 0, dec);
//    StretchBlt(PicViewer.mhdc_pic,400-PicViewer.pic_width/2,240 - PicViewer.pic_height/2,
//    PicViewer.pic_width,PicViewer.pic_height,
//               hdc_tmp,0,0,PicViewer.pic_width,PicViewer.pic_height,SRCCOPY);
//    DeleteDC(hdc_tmp);
    /* �ر�JPG_DEC��� */
    //JPG_Close(dec);
  }
//  /* �ͷ�ͼƬ���ݿռ� */
//  RES_Release_Content((char **)&jpeg_buf);  
}
//����PNGͼƬ
void Draw_Pic_PNG(char *file_name)
{
  BOOL res;
  BITMAP png_bm;
  u8 *png_buf;
  u32 png_size;
  PNG_DEC *png_dec;
  HDC hdc_tmp;
  static int i,j;
  RECT rc = {0,0,800,480};
  if(PicViewer.cur_path == eID_Pic_EXTFLASH)
    res= RES_Load_Content(PNG_FILE_NAME, (char**)&png_buf, &png_size);
  else  
    res= FS_Load_Content(file_name, (char**)&png_buf, &png_size);
  if(res)
  {
    png_dec = PNG_Open(png_buf, png_size);
    PNG_GetBitmap(png_dec, &png_bm);
    
    PicViewer.mhdc_pic = CreateMemoryDC(SURF_SCREEN, 800, 480);
    SetBrushColor(PicViewer.mhdc_pic, MapRGB(PicViewer.mhdc_pic, 0, 0, 0));
    FillRect(PicViewer.mhdc_pic, &rc);  
    hdc_tmp = CreateMemoryDC(SURF_SCREEN, 800, 480); 
    SetBrushColor(hdc_tmp, MapRGB(hdc_tmp, 0, 0, 0));
    FillRect(hdc_tmp, &rc); 
    switch(PicViewer.scale_state)
    {
      case 0:
      {
        GUI_DEBUG("ԭʼ״̬");
        PicViewer.pic_width = png_bm.Width;
        PicViewer.pic_height = png_bm.Height; 
        i = 0; 
        j = 0;
        break;
      }
      case 1:
      {
        i++;
        j = 0;
        if(PicViewer.pic_width == 800 && PicViewer.pic_height == 480) i--;
          
        else if(PicViewer.pic_width < 800 && PicViewer.pic_height == 480)
        {
          PicViewer.pic_width = PicViewer.pic_width + 20 * i ;
          if(PicViewer.pic_width > 800) PicViewer.pic_width = 800;
        }
        else if(PicViewer.pic_width == 800 && PicViewer.pic_height < 480)
        {
           PicViewer.pic_height = PicViewer.pic_height + 20 *i ;
          if(PicViewer.pic_height > 480) PicViewer.pic_height = 480;         
        }
        else
        {
          PicViewer.pic_width = PicViewer.pic_width+20*i;
          PicViewer.pic_height = PicViewer.pic_height+20*i; 
          if(PicViewer.pic_height > 480) PicViewer.pic_height = 480;  
          if(PicViewer.pic_width > 800) PicViewer.pic_width = 800;
        }
        
        break;  
      }
      case 2:
      {
        j++;
        i = 0;
        if(PicViewer.pic_width > 80 && PicViewer.pic_height > 80)
        {
          PicViewer.pic_width = PicViewer.pic_width-20*j;
          PicViewer.pic_height = PicViewer.pic_height-20*j; 
          if(PicViewer.pic_height < 80) PicViewer.pic_height = 80;  
          if(PicViewer.pic_width < 80) PicViewer.pic_width = 80;          
        }         
        else if(PicViewer.pic_width > 80 && PicViewer.pic_height <= 80)
        {
          PicViewer.pic_width = PicViewer.pic_width - 20 * j ;
          if(PicViewer.pic_width < 80) PicViewer.pic_width = 80;
        }
        else if(PicViewer.pic_width <= 80 && PicViewer.pic_height > 80)
        {
           PicViewer.pic_height -= 20 *j;
          if(PicViewer.pic_height < 80) PicViewer.pic_height = 80;         
        }
        else
        {
          j--;
        }
        
        break;  
      
      }
    } 
    
    DrawBitmap(hdc_tmp, 0,0, &png_bm, NULL);
    
    StretchBlt(PicViewer.mhdc_pic,400-PicViewer.pic_width/2,240 - PicViewer.pic_height/2,
               PicViewer.pic_width,PicViewer.pic_height,
               hdc_tmp,0,0,png_bm.Width,png_bm.Height,SRCCOPY);    
    DeleteDC(hdc_tmp);
    PNG_Close(png_dec);
  }
  RES_Release_Content((char **)&png_buf);
}
//�����ڲ�PNGͼƬ
void Draw_Pic_PNG_INTFLASH()
{
  BITMAP png_bm;
//  u8 *png_buf;
//  u32 png_size;
  PNG_DEC *png_dec;
  HDC hdc_tmp;
  static int i,j;
  RECT rc = {0,0,800,480};
  //if(res)
  {
    png_dec = PNG_Open((u8 *)redfish, redfish_size());
    PNG_GetBitmap(png_dec, &png_bm);
    
    PicViewer.mhdc_pic = CreateMemoryDC(SURF_SCREEN, 800, 480);
    
    SetBrushColor(PicViewer.mhdc_pic, MapRGB(PicViewer.mhdc_pic, 0, 0, 0));
    FillRect(PicViewer.mhdc_pic, &rc);  
    hdc_tmp = CreateMemoryDC(SURF_SCREEN, 800, 480); 
    SetBrushColor(hdc_tmp, MapRGB(hdc_tmp, 0, 0, 0));
    FillRect(hdc_tmp, &rc); 
    switch(PicViewer.scale_state)
    {
      case 0:
      {
        GUI_DEBUG("ԭʼ״̬");
        PicViewer.pic_width = png_bm.Width;
        PicViewer.pic_height = png_bm.Height; 
        i = 0; 
        j = 0;
        break;
      }
      case 1:
      {
        i++;
        j = 0;
        if(PicViewer.pic_width == 800 && PicViewer.pic_height == 480) i--;
          
        else if(PicViewer.pic_width < 800 && PicViewer.pic_height == 480)
        {
          PicViewer.pic_width = PicViewer.pic_width + 20 * i ;
          if(PicViewer.pic_width > 800) PicViewer.pic_width = 800;
        }
        else if(PicViewer.pic_width == 800 && PicViewer.pic_height < 480)
        {
           PicViewer.pic_height = PicViewer.pic_height + 20 *i ;
          if(PicViewer.pic_height > 480) PicViewer.pic_height = 480;         
        }
        else
        {
          PicViewer.pic_width = PicViewer.pic_width+20*i;
          PicViewer.pic_height = PicViewer.pic_height+20*i; 
          if(PicViewer.pic_height > 480) PicViewer.pic_height = 480;  
          if(PicViewer.pic_width > 800) PicViewer.pic_width = 800;
        }
        
        break;  
      }
      case 2:
      {
        j++;
        i = 0;
        if(PicViewer.pic_width > 80 && PicViewer.pic_height > 80)
        {
          PicViewer.pic_width = PicViewer.pic_width-20*j;
          PicViewer.pic_height = PicViewer.pic_height-20*j; 
          if(PicViewer.pic_height < 80) PicViewer.pic_height = 80;  
          if(PicViewer.pic_width < 80) PicViewer.pic_width = 80;          
        }         
        else if(PicViewer.pic_width > 80 && PicViewer.pic_height <= 80)
        {
          PicViewer.pic_width = PicViewer.pic_width - 20 * j ;
          if(PicViewer.pic_width < 80) PicViewer.pic_width = 80;
        }
        else if(PicViewer.pic_width <= 80 && PicViewer.pic_height > 80)
        {
           PicViewer.pic_height -= 20 *j;
          if(PicViewer.pic_height < 80) PicViewer.pic_height = 80;         
        }
        else
        {
          j--;
        }
        
        break;  
      
      }
    } 
    
    DrawBitmap(hdc_tmp, 0,0, &png_bm, NULL);
    
    StretchBlt(PicViewer.mhdc_pic,400-PicViewer.pic_width/2,240 - PicViewer.pic_height/2,
               PicViewer.pic_width,PicViewer.pic_height,
               hdc_tmp,0,0,png_bm.Width,png_bm.Height,SRCCOPY);    
    DeleteDC(hdc_tmp);
    PNG_Close(png_dec);
  }
  //RES_Release_Content((char **)&png_buf);
}

BITMAPINFO bm_info;
void Draw_Pic_BMP(HDC hdc, char *file_name)
{
  RECT rc = {0,0,800,480};
  static int i,j;
  if(PicViewer.cur_path == eID_Pic_EXTFLASH)
  {
    PIC_BMP_GetInfo_Res(&bm_info, BMP_FILE_NAME);

  }
  else
    PIC_BMP_GetInfo_FS(&bm_info,file_name);

    switch(PicViewer.scale_state)
    {
      case 0:
      {
        GUI_DEBUG("ԭʼ״̬");
        PicViewer.pic_width = bm_info.Width;
        PicViewer.pic_height =bm_info.Height; 
        i = 0; 
        j = 0;
        break;
      }
      case 1:
      {
        i++;
        j = 0;
        if(PicViewer.pic_width == 800 && PicViewer.pic_height == 480) i--;
          
        else if(PicViewer.pic_width < 800 && PicViewer.pic_height == 480)
        {
          PicViewer.pic_width = PicViewer.pic_width + 20 * i ;
          if(PicViewer.pic_width > 800) PicViewer.pic_width = 800;
        }
        else if(PicViewer.pic_width == 800 && PicViewer.pic_height < 480)
        {
           PicViewer.pic_height = PicViewer.pic_height + 20 *i ;
          if(PicViewer.pic_height > 480) PicViewer.pic_height = 480;         
        }
        else
        {
          PicViewer.pic_width = PicViewer.pic_width+20*i;
          PicViewer.pic_height = PicViewer.pic_height+20*i; 
          if(PicViewer.pic_height > 480) PicViewer.pic_height = 480;  
          if(PicViewer.pic_width > 800) PicViewer.pic_width = 800;
        }
        
        break;  
      }
      case 2:
      {
        j++;
        i = 0;
        if(PicViewer.pic_width > 80 && PicViewer.pic_height > 80)
        {
          PicViewer.pic_width = PicViewer.pic_width-20*j;
          PicViewer.pic_height = PicViewer.pic_height-20*j; 
          if(PicViewer.pic_height < 80) PicViewer.pic_height = 80;  
          if(PicViewer.pic_width < 80) PicViewer.pic_width = 80;          
        }         
        else if(PicViewer.pic_width > 80 && PicViewer.pic_height <= 80)
        {
          PicViewer.pic_width = PicViewer.pic_width - 20 * j ;
          if(PicViewer.pic_width < 80) PicViewer.pic_width = 80;
        }
        else if(PicViewer.pic_width <= 80 && PicViewer.pic_height > 80)
        {
           PicViewer.pic_height -= 20 *j;
          if(PicViewer.pic_height < 80) PicViewer.pic_height = 80;         
        }
        else
        {
          j--;
        }
        
        break;  
      
      }
    }   
  
  PicViewer.mhdc_pic = CreateMemoryDC(SURF_SCREEN, 800, 480);
  SetBrushColor(PicViewer.mhdc_pic, MapRGB(PicViewer.mhdc_pic, 0, 0, 0));
  FillRect(PicViewer.mhdc_pic, &rc);     
    
  HDC hdc_tmp;
    
  hdc_tmp = CreateMemoryDC(SURF_SCREEN, 800, 480);
  SetBrushColor(hdc_tmp, MapRGB(hdc_tmp, 0, 0, 0));
  FillRect(hdc_tmp, &rc);   
  if(PicViewer.cur_path == eID_Pic_EXTFLASH)
  {  
    /* ����bmp��hdc */
    PIC_BMP_Draw_Res(hdc_tmp, 0,0, BMP_FILE_NAME, NULL); 
  }
  else
    PIC_BMP_Draw_FS(hdc_tmp, 0,0, file_name, NULL); 
  
  StretchBlt(PicViewer.mhdc_pic,400-PicViewer.pic_width/2,240 - PicViewer.pic_height/2,
  PicViewer.pic_width,PicViewer.pic_height,
             hdc_tmp,0,0,bm_info.Width, bm_info.Height,SRCCOPY);
  DeleteDC(hdc_tmp);  
}
static BITMAP bm_0;
void Draw_Pic_BMP_INTFLASH(HDC hdc)
{
  RECT rc = {0,0,800,480};
  

  //����λͼ�ṹ����
  bm_0.Format	= BM_ARGB8888;     //λͼ��ʽ
  bm_0.Width  = 92;              //���
  bm_0.Height = 184;             //�߶�
  bm_0.WidthBytes =bm_0.Width*4; //ÿ���ֽ���
  bm_0.LUT =NULL;                //���ұ�(RGB/ARGB��ʽ��ʹ�øò���)
  bm_0.Bits =(void*)gImage_0;    //λͼ����  
  static int i,j;
  switch(PicViewer.scale_state)
  {
    case 0:
    {
      GUI_DEBUG("ԭʼ״̬");
      PicViewer.pic_width = bm_0.Width;
      PicViewer.pic_height =bm_0.Height; 
      i = 0; 
      j = 0;
      break;
    }
    case 1:
    {
      i++;
      j = 0;
      if(PicViewer.pic_width == 800 && PicViewer.pic_height == 480) i--;
        
      else if(PicViewer.pic_width < 800 && PicViewer.pic_height == 480)
      {
        PicViewer.pic_width = PicViewer.pic_width + 20 * i ;
        if(PicViewer.pic_width > 800) PicViewer.pic_width = 800;
      }
      else if(PicViewer.pic_width == 800 && PicViewer.pic_height < 480)
      {
         PicViewer.pic_height = PicViewer.pic_height + 20 *i ;
        if(PicViewer.pic_height > 480) PicViewer.pic_height = 480;         
      }
      else
      {
        PicViewer.pic_width = PicViewer.pic_width+20*i;
        PicViewer.pic_height = PicViewer.pic_height+20*i; 
        if(PicViewer.pic_height > 480) PicViewer.pic_height = 480;  
        if(PicViewer.pic_width > 800) PicViewer.pic_width = 800;
      }
      
      break;  
    }
    case 2:
    {
      j++;
      i = 0;
      if(PicViewer.pic_width > 80 && PicViewer.pic_height > 80)
      {
        PicViewer.pic_width = PicViewer.pic_width-20*j;
        PicViewer.pic_height = PicViewer.pic_height-20*j; 
        if(PicViewer.pic_height < 80) PicViewer.pic_height = 80;  
        if(PicViewer.pic_width < 80) PicViewer.pic_width = 80;          
      }         
      else if(PicViewer.pic_width > 80 && PicViewer.pic_height <= 80)
      {
        PicViewer.pic_width = PicViewer.pic_width - 20 * j ;
        if(PicViewer.pic_width < 80) PicViewer.pic_width = 80;
      }
      else if(PicViewer.pic_width <= 80 && PicViewer.pic_height > 80)
      {
         PicViewer.pic_height -= 20 *j;
        if(PicViewer.pic_height < 80) PicViewer.pic_height = 80;         
      }
      else
      {
        j--;
      }
      
      break;  
    
    }
  }   
  
  PicViewer.mhdc_pic = CreateMemoryDC(SURF_SCREEN, 800, 480);
  SetBrushColor(PicViewer.mhdc_pic, MapRGB(PicViewer.mhdc_pic, 0, 0, 0));
  FillRect(PicViewer.mhdc_pic, &rc);     
    
  HDC hdc_tmp;
    
  hdc_tmp = CreateMemoryDC(SURF_SCREEN, 800, 480);
  SetBrushColor(hdc_tmp, MapRGB(hdc_tmp, 0, 0, 0));
  FillRect(hdc_tmp, &rc);   
  
  DrawBitmap(hdc_tmp,0,0,&bm_0,NULL);
  
  StretchBlt(PicViewer.mhdc_pic,400-PicViewer.pic_width/2,240 - PicViewer.pic_height/2,
  PicViewer.pic_width,PicViewer.pic_height,
             hdc_tmp,0,0,bm_0.Width, bm_0.Height,SRCCOPY);
  DeleteDC(hdc_tmp);  
}

IMAGE_INFO img_info; 
HANDLE hgif;
void Draw_Pic_GIF(char *file_name, HDC hdc)
{
  static int i = 0;
  RECT rc = {0,0,800,480};
  //GUI_DEBUG("%s", file_name);
//  if(res)
  {
    switch(PicViewer.gif_state)
    {
      case 0:
      {
        GUI_DEBUG("%s", file_name);
        GUI_DEBUG("0");
        u8 *gif_buf;
        u32 gif_size;
        i = 0;//�������
        PicViewer.gif_state = 1;
        if(PicViewer.cur_path == eID_Pic_EXTFLASH)
          RES_Load_Content(GIF_FILE_NAME, (char **)&gif_buf, &gif_size);
        else
          FS_Load_Content(file_name, (char**)&gif_buf, &gif_size);
        hgif = GIF_Open(gif_buf);
        GIF_GetInfo(hgif,&img_info);
        PicViewer.frame_num = GIF_GetFrameCount(hgif); 

        PicViewer.pic_width = img_info.Width;
        PicViewer.pic_height = img_info.Height;
           
        RES_Release_Content((char **)&gif_buf);
      }
      case 1:
      {
        if(i>=PicViewer.frame_num)
        {
           i=0;
        } 
        PicViewer.mhdc_pic = CreateMemoryDC(SURF_SCREEN, 800, 480);
        SetBrushColor(PicViewer.mhdc_pic, MapRGB(PicViewer.mhdc_pic, 0, 0, 0));
        FillRect(PicViewer.mhdc_pic, &rc);     
          
        HDC hdc_tmp;
          
        hdc_tmp = CreateMemoryDC(SURF_SCREEN, 800, 480);
        SetBrushColor(hdc_tmp, MapRGB(hdc_tmp, 0, 0, 0));
        FillRect(hdc_tmp, &rc);        
        PicViewer.delay = GIF_DrawFrame(hdc_tmp,0,0,MapRGB(hdc,0,0,0),hgif,i); 
        StretchBlt(PicViewer.mhdc_pic,400-PicViewer.pic_width/2,240 - PicViewer.pic_height/2,
        PicViewer.pic_width,PicViewer.pic_height,
                   hdc_tmp,0,0,PicViewer.pic_width,PicViewer.pic_height,SRCCOPY);     
        DeleteDC(hdc_tmp);         
        i++;
        break;
      }
    } 
  }
}

void Draw_Pic_GIF_INTFLASH(char *file_name, HDC hdc)
{
  static int i = 0;
  RECT rc = {0,0,800,480};
	static HANDLE hgif;
	static IMAGE_INFO img_info;
  {
    switch(PicViewer.gif_state)
    {
      case 0:
      {
//        GUI_DEBUG("%s", file_name);
//        GUI_DEBUG("0");
//        u8 *gif_buf;
//        u32 gif_size;
        i = 0;//�������
        PicViewer.gif_state = 1;
        hgif = GIF_Open(king);
        GIF_GetInfo(hgif,&img_info);
        PicViewer.frame_num = GIF_GetFrameCount(hgif); 

        PicViewer.pic_width = img_info.Width;
        PicViewer.pic_height = img_info.Height;
           
      }
      case 1:
      {
        if(i>=PicViewer.frame_num)
        {
           i=0;
        } 
        PicViewer.mhdc_pic = CreateMemoryDC(SURF_SCREEN, 800, 480);
        SetBrushColor(PicViewer.mhdc_pic, MapRGB(PicViewer.mhdc_pic, 0, 0, 0));
        FillRect(PicViewer.mhdc_pic, &rc);     
          
        HDC hdc_tmp;
          
        hdc_tmp = CreateMemoryDC(SURF_SCREEN, 800, 480);
        SetBrushColor(hdc_tmp, MapRGB(hdc_tmp, 0, 0, 0));
        FillRect(hdc_tmp, &rc);        
        PicViewer.delay = GIF_DrawFrame(hdc_tmp,0,0,MapRGB(hdc,0,0,0),hgif,i); 
        StretchBlt(PicViewer.mhdc_pic,400-PicViewer.pic_width/2,240 - PicViewer.pic_height/2,
        PicViewer.pic_width,PicViewer.pic_height,
                   hdc_tmp,0,0,PicViewer.pic_width,PicViewer.pic_height,SRCCOPY);     
        DeleteDC(hdc_tmp);         
        i++;
        break;
      }
    } 
  }
}

//��������SD��
static FRESULT scan_Picfiles(char* path, char* type) 
{ 
  FRESULT res; 		//�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���	
  FILINFO fno; 
  DIR dir; 
  int i; 
  char *fn; 
  char file_name[100];	
	
#if _USE_LFN 
  static char lfn[_MAX_LFN * (0x81 ? 2 : 1) + 1]; 	//���ļ���֧��
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif  
  res = f_opendir(&dir, path); //��Ŀ¼
  if (res == FR_OK) 
  { 
    i = strlen(path); 
    for (;;) 
    { 
      res = f_readdir(&dir, &fno); 										//��ȡĿ¼�µ�����
     if (res != FR_OK || fno.fname[0] == 0) break; 	//Ϊ��ʱ��ʾ������Ŀ��ȡ��ϣ�����
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      if (*fn == '.') continue; 											//���ʾ��ǰĿ¼������			
      if (fno.fattrib & AM_DIR) 
			{ 																							//Ŀ¼���ݹ��ȡ
        sprintf(&path[i], "/%s", fn); 							//�ϳ�����Ŀ¼��
        res = scan_Picfiles(path,type);											//�ݹ���� 
        if (res != FR_OK) 
					break; 																		//��ʧ�ܣ�����ѭ��
        path[i] = 0; 
      } 
      else 
		{ 
				//printf("%s%s\r\n", path, fn);								//����ļ���
				if(strstr(fn, type))
				{
					if ((strlen(path)+strlen(fn)<100)&&(PicViewer.pic_nums<100))
					{
						sprintf(file_name, "%s/%s", path, fn);						
						memcpy(PicViewer.pic_list[PicViewer.pic_nums],file_name,strlen(file_name));//��·��
            printf("%s\r\n", PicViewer.pic_list[PicViewer.pic_nums]);
						memcpy(PicViewer.pic_lcdlist[PicViewer.pic_nums],fn,strlen(fn));						//�ļ���
						PicViewer.pic_nums++;//��¼�ļ�����
					}
				}//if
      }//else
     } //for
  } 
  return res; 
}
//��ʼ��ͼƬ�����
//
void PicViewer_Init(void)
{
  int i = 0, j = 0;
  
  //Step1:�����ڴ�ռ�  
  PicViewer.pic_list = (char **)GUI_VMEM_Alloc(sizeof(char*) * PICFILE_NUM_MAX);//�����пռ�
  for(i = 0; i < PICFILE_NUM_MAX; i++)
  {
    PicViewer.pic_list[i] = (char *)GUI_VMEM_Alloc(sizeof(char) * PICFILE_NAME_MAXLEN);//�����пռ�
  }
  
  PicViewer.pic_lcdlist = (char **)GUI_VMEM_Alloc(sizeof(char*) * PICFILE_NUM_MAX);//�����пռ�
  for(i = 0; i < PICFILE_NUM_MAX; i++)
  {
    PicViewer.pic_lcdlist[i] = (char *)GUI_VMEM_Alloc(sizeof(char) * PICFILE_NAME_MAXLEN);//�����пռ�
  }  
  for(i = 0; i < PICFILE_NUM_MAX; i++)
  {
    for(j = 0; j < PICFILE_NAME_MAXLEN; j++)
    {
      PicViewer.pic_lcdlist[i][j] = '\0';
      PicViewer.pic_list[i][j] = '\0';
    }
  }
  //Step2��ɨ��ͼƬ�ļ�
  scan_Picfiles(path,".jpg");
}
//�˳��ļ������
void PicViewer_Quit(void)
{
  int i = 0;
  //Step1:�ͷ��ڴ�
  for(;i < PICFILE_NUM_MAX; i++)
  {
    GUI_VMEM_Free(PicViewer.pic_list[i]);
    GUI_VMEM_Free(PicViewer.pic_lcdlist[i]);
  }
  GUI_VMEM_Free(PicViewer.pic_list);
  GUI_VMEM_Free(PicViewer.pic_lcdlist);
  //����״̬����Ϊԭʼ״̬
  PicViewer.scale_state = 0;
  //ͼƬ��Ŀ����ǰ����Ϊ0
  PicViewer.pic_nums = 0;
  PicViewer.show_index = 0;
//  PicViewer.cur_type = eID_Pic_JPG;
//  PicViewer.cur_path = eID_Pic_SDCARD;
  //DeleteDC(PicViewer.mhdc_bk);  
}
//�ڲ�FLASHͼƬ��ʾ���ڻص�����
static	LRESULT DlgINTFLASH_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      PicViewer.cur_type = 0;
      GetClientRect(hwnd, &rc); 
      CreateWindow(BUTTON, L"ͼƬ����", WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE, 
                   GUI_PicViewer_Icon[0].rc.x, GUI_PicViewer_Icon[0].rc.y, 
                   GUI_PicViewer_Icon[0].rc.w, GUI_PicViewer_Icon[0].rc.h,          
                   hwnd, eID_Pic_Name, NULL, NULL);  
      CreateWindow(BUTTON, L"L", BS_FLAT | BS_NOTIFY |WS_TRANSPARENT| WS_OWNERDRAW |WS_VISIBLE,
                   0, rc.h * 1 / 2, 70, 70, hwnd, eID_Pic_PREV, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_Pic_PREV), controlFont_64); 
      CreateWindow(BUTTON, L"K", BS_FLAT | BS_NOTIFY |WS_TRANSPARENT| WS_OWNERDRAW | WS_VISIBLE,
                   rc.w - 65, rc.h * 1 / 2, 70, 70, hwnd, eID_Pic_NEXT, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_Pic_NEXT), controlFont_64);   
      
      CreateWindow(BUTTON, L"F", BS_FLAT | BS_NOTIFY|WS_TRANSPARENT|WS_OWNERDRAW |WS_VISIBLE,
                    0, 10, 75, 50, hwnd, eID_Pic_Return, NULL, NULL);  
      
      CreateWindow(BUTTON, L"JPG", WS_TRANSPARENT| WS_OWNERDRAW |WS_VISIBLE,
                   0, 420, 60, 60, hwnd, eID_Pic_JPG, NULL, NULL);
 
      CreateWindow(BUTTON, L"(", BS_FLAT | BS_NOTIFY | WS_TRANSPARENT|WS_OWNERDRAW | WS_VISIBLE,
                   675, 420, 60, 60, hwnd, eID_ZOOMIN, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_ZOOMIN), controlFont_48);  
      CreateWindow(BUTTON, L")", BS_FLAT | BS_NOTIFY | WS_TRANSPARENT|WS_OWNERDRAW| WS_VISIBLE,
                   740, 420, 60, 60, hwnd, eID_ZOOMOUT, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_ZOOMOUT), controlFont_48);               
      CreateWindow(TEXTBOX, L" ", NULL, 
                   GUI_PicViewer_Icon[1].rc.x, GUI_PicViewer_Icon[1].rc.y, 
                   GUI_PicViewer_Icon[1].rc.w, GUI_PicViewer_Icon[1].rc.h,          
                   hwnd, eID_Pic_MsgBOX, NULL, NULL);
      SendMessage(GetDlgItem(hwnd,eID_Pic_MsgBOX),TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BORDER|DT_BKGND); 
      CreateWindow(BUTTON, L"�ֱ��ʣ�", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[2].rc.x, GUI_PicViewer_Icon[2].rc.y, 
                   GUI_PicViewer_Icon[2].rc.w, GUI_PicViewer_Icon[2].rc.h,          
                   hwnd, eID_Pic_Res, NULL, NULL);

      CreateWindow(BUTTON, L" ", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[3].rc.x, GUI_PicViewer_Icon[3].rc.y, 
                   GUI_PicViewer_Icon[3].rc.w, GUI_PicViewer_Icon[3].rc.h,          
                   hwnd, eID_Pic_Res_Value, NULL, NULL);
                   
      CreateWindow(BUTTON, L"ʱ�䣺", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[4].rc.x, GUI_PicViewer_Icon[4].rc.y, 
                   GUI_PicViewer_Icon[4].rc.w, GUI_PicViewer_Icon[4].rc.h,          
                   hwnd, eID_Pic_Time, NULL, NULL);

      CreateWindow(BUTTON, L" ", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[5].rc.x, GUI_PicViewer_Icon[5].rc.y, 
                   GUI_PicViewer_Icon[5].rc.w, GUI_PicViewer_Icon[5].rc.h,          
                   hwnd, eID_Pic_Time_Value, NULL, NULL);
    
      CreateWindow(BUTTON, L"֡�ʣ�", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[6].rc.x, GUI_PicViewer_Icon[6].rc.y, 
                   GUI_PicViewer_Icon[6].rc.w, GUI_PicViewer_Icon[6].rc.h,          
                   hwnd, eID_Pic_FPS, NULL, NULL);

      CreateWindow(BUTTON, L" ", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[7].rc.x, GUI_PicViewer_Icon[7].rc.y, 
                   GUI_PicViewer_Icon[7].rc.w, GUI_PicViewer_Icon[7].rc.h,          
                   hwnd, eID_Pic_FPS_Value, NULL, NULL);      
      
      
      SetTimer(hwnd,1,0,TMR_SINGLE,NULL);//��������
      SetTimer(hwnd,2,0,TMR_SINGLE,NULL);//��������
      //PicViewer_Init();
      break;
    } 
    case WM_TIMER:
    {
      
      switch((UINT)wParam)
      {
        case 1:
        {
          ShowWindow(GetDlgItem(hwnd, eID_Pic_MsgBOX), SW_HIDE);
          RedrawWindow(hwnd, NULL, RDW_ALLCHILDREN|RDW_ERASE|RDW_INVALIDATE);
          PicViewer.scale_state = 4;            
          break;
        }
        case 2:
        {
          InvalidateRect(hwnd,NULL,TRUE);
          break;
        }
      }
      break;
    }    
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       if(ds->ID == eID_Pic_Time || ds->ID == eID_Pic_Time_Value || 
          ds->ID == eID_Pic_FPS || ds->ID == eID_Pic_FPS_Value  ||
          ds->ID == eID_Pic_Res || ds->ID == eID_Pic_Res_Value)
       
       {
          PicViewer_TBOX_OwnerDraw(ds);
          return TRUE;            
       }      
       switch(ds->ID)
       {
          case eID_Pic_Name:
          {              
             PicViewer_TBOX_OwnerDraw(ds);
             return TRUE;              
          } 
          case eID_Pic_PREV:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          }            
          case eID_Pic_NEXT:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          } 
          case eID_Pic_JPG:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          }            
          case eID_ZOOMIN:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          } 
          case eID_ZOOMOUT:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          }
          case eID_Pic_Return:
          {
            Pic_ReturnBTN_Ownerdraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          }          
                     
       }
       break;
    }
    case WM_PAINT:
    {
       HDC hdc, hdc_mem;
       PAINTSTRUCT ps;
       RECT rc = {0,0,800,70};
       hdc_mem = CreateMemoryDC(SURF_ARGB4444, 800,70);
       
       hdc = BeginPaint(hwnd, &ps);
       
       SetBrushColor(hdc_mem, MapARGB(hdc_mem,155,105, 105, 105));
       FillRect(hdc_mem, &rc);
       
       BitBlt(hdc, 0,0,800,70,hdc_mem,0,0,SRCCOPY);
       DeleteDC(hdc_mem);
       EndPaint(hwnd, &ps);
       break;
    }       
    case WM_ERASEBKGND:
    {
      HDC hdc =(HDC)wParam;
      RECT rc =*(RECT*)lParam;
      WCHAR wbuf[128];
      rt_tick_t tick;
      switch(PicViewer.cur_type)
      {
        case 0:
        {
//          GUI_DEBUG("JPG");
            //Draw_Pic_JPG_INTFLASH();
          RECT rc = {0,0,800,480};
          SetBrushColor(hdc, MapRGB(hdc,0,0,0));
          
          FillRect(hdc, &rc);
          SetTextColor(hdc, MapRGB(hdc,250,250,250));
          DrawText(hdc, L"�ù�����δʵ��", -1, &rc, DT_VCENTER|DT_CENTER|DT_SINGLELINE);          
//          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY);
//          DeleteDC(PicViewer.mhdc_pic);
          break;
        }
        case 1:
        {          
          GUI_DEBUG("PNG");
          
          tick = rt_tick_get();
          Draw_Pic_PNG_INTFLASH();

          float time = (float)(rt_tick_get() - tick)/1000;
          x_wsprintf(wbuf, L"%.2fs", time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_Time_Value), wbuf);
          x_wsprintf(wbuf, L"%.1fFPS", 1/time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_FPS_Value), wbuf);            
          
          x_mbstowcs_cp936(wbuf, PNG_FILE_NAME_INT, PICFILE_NAME_MAXLEN);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_Name), wbuf);          
          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY);
          DeleteDC(PicViewer.mhdc_pic);         

          break;
        }
        case 2:
        {
          tick = rt_tick_get();
          Draw_Pic_BMP_INTFLASH(hdc);
          
          float time = (float)(rt_tick_get() - tick)/1000;
          x_wsprintf(wbuf, L"%.2fs", time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_Time_Value), wbuf);
          x_wsprintf(wbuf, L"%.1fFPS", 1/time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_FPS_Value), wbuf);            
          
          
          x_mbstowcs_cp936(wbuf, BMP_FILE_NAME_INT, PICFILE_NAME_MAXLEN);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_Name), wbuf);
          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY);
          DeleteDC(PicViewer.mhdc_pic);
          break;
        }        
        case 3:
        {
          tick = rt_tick_get();
          
          x_mbstowcs_cp936(wbuf, GIF_FILE_NAME_INT, PICFILE_NAME_MAXLEN);
          
          float time = (float)(rt_tick_get() - tick)/1000;
          x_wsprintf(wbuf, L"%.2fs", time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_Time_Value), wbuf);
          x_wsprintf(wbuf, L"%.1fFPS", 1/time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_FPS_Value), wbuf);              
          x_mbstowcs_cp936(wbuf, GIF_FILE_NAME_INT, PICFILE_NAME_MAXLEN);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_Name), wbuf);     
          Draw_Pic_GIF_INTFLASH(PicViewer.pic_list[PicViewer.show_index], hdc);
          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY);
          DeleteDC(PicViewer.mhdc_pic);          
          ResetTimer(hwnd,2,PicViewer.delay,TMR_SINGLE|TMR_START,NULL);          
        }          
      }
      //
      
//      x_mbstowcs_cp936(wbuf, PicViewer.pic_lcdlist[PicViewer.show_index], PICFILE_NAME_MAXLEN);
//      SetWindowText(GetDlgItem(hwnd, eID_Pic_Name), wbuf); 
      x_wsprintf(wbuf, L"%d*%d", PicViewer.pic_width, PicViewer.pic_height);
      SetWindowText(GetDlgItem(hwnd, eID_Pic_Res_Value), wbuf); 
      //x_mbstowcs_cp936(wbuf, PicViewer.pic_lcdlist[PicViewer.show_index], PICFILE_NAME_MAXLEN);
//      SetBrushColor(PicViewer.mhdc_bk, MapRGB(PicViewer.mhdc_bk, 0, 0, 0));
//      FillRect(PicViewer.mhdc_bk, &rc);
      //PIC_BMP_Draw_FS(hdc, 70, 70, PicViewer.pic_list[PicViewer.show_index], NULL); 
      //BitBlt(PicViewer.mhdc_bk, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, 0, 0, SRCCOPY);
      
      
      
      return TRUE;
    }
    case WM_NOTIFY:
    {
      u16 code, id;
      id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
      code=HIWORD(wParam);//��ȡ��Ϣ������
      if(code == BN_CLICKED)
      { 
        switch(id)
        {
          case eID_Pic_NEXT:
          {
            PicViewer.scale_state = 0;
            SetWindowText(GetDlgItem(hwnd, eID_Pic_MsgBOX), L"����Ƭ�Ѿ������һ����");
            ResetTimer(hwnd,1,1000,TMR_START|TMR_SINGLE,NULL);
            ShowWindow(GetDlgItem(hwnd, eID_Pic_MsgBOX), SW_SHOW);    
                 
            break;
          }
          case eID_Pic_PREV:
          {
            PicViewer.scale_state = 0;
            SetWindowText(GetDlgItem(hwnd, eID_Pic_MsgBOX), L"����Ƭ�Ѿ��ǵ�һ����");
            ResetTimer(hwnd,1,1000,TMR_START|TMR_SINGLE,NULL);
            ShowWindow(GetDlgItem(hwnd, eID_Pic_MsgBOX), SW_SHOW);   
            break;
          }
          case eID_Pic_JPG:
          {
//            int i, j;
            PicViewer.cur_type++;
            PicViewer.scale_state = 0;            
            if(PicViewer.cur_type > 3)
              PicViewer.cur_type = 0; 
            if(PicViewer.gif_state != 0 && PicViewer.cur_type != 3)
            {
              PicViewer.gif_state = 0;
              GIF_Close(hgif);
              
            }            
            switch(PicViewer.cur_type)
            {
              case 0:
              {
                SetWindowText(GetDlgItem(hwnd, eID_Pic_JPG), L"JPG");
                InvalidateRect(PicViewer.mPicViewer, NULL, TRUE);
                break;
              }
              case 1:
              {
                InvalidateRect(PicViewer.mPicViewer, NULL, TRUE);
                SetWindowText(GetDlgItem(hwnd, eID_Pic_JPG), L"PNG");
                
                break;                
              }
              case 2:
              {               
                SetWindowText(GetDlgItem(hwnd, eID_Pic_JPG), L"BMP");
                InvalidateRect(PicViewer.mPicViewer, NULL, TRUE);
                break;
              }
              case 3:
              {                
                SetWindowText(GetDlgItem(hwnd, eID_Pic_JPG), L"GIF");
                InvalidateRect(PicViewer.mPicViewer, NULL, TRUE);
                break;                
              }              
            
            }
            break;
          }
          case eID_ZOOMIN:
          {
            GUI_DEBUG("�Ŵ�");
            InvalidateRect(hwnd,NULL,TRUE);
            PicViewer.scale_state = 1;
            
            break;
          }   
          case eID_ZOOMOUT:
          {
            PicViewer.scale_state = 2; 
            InvalidateRect(hwnd,NULL,TRUE);
            GUI_DEBUG("��С");
            break;
          }          
          case eID_Pic_Return:
          {
            PostCloseMessage(hwnd);
            break;
          }           
        }
        
      }
      break;
    }  
    case WM_DESTROY:
    {
      //PicViewer_Quit();
      PicViewer.scale_state = 0;
      if(PicViewer.cur_type == 3)
      {
        PicViewer.gif_state = 0;
        GIF_Close(hgif);
        
      }         
      PicViewer.pic_nums = 0;
      PicViewer.show_index = 0;      
      return PostQuitMessage(hwnd);	
    }     
    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }    
  return WM_NULL;
}
//�ⲿFLASHͼƬ��ʾ���ڻص�����
static	LRESULT DlgEXTFLASH_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      PicViewer.cur_type = 0;
      RECT rc;
      GetClientRect(hwnd, &rc); 
      CreateWindow(BUTTON, L"ͼƬ����", WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE, 
                   GUI_PicViewer_Icon[0].rc.x, GUI_PicViewer_Icon[0].rc.y, 
                   GUI_PicViewer_Icon[0].rc.w, GUI_PicViewer_Icon[0].rc.h,          
                   hwnd, eID_Pic_Name, NULL, NULL);  
      CreateWindow(BUTTON, L"L", BS_FLAT | BS_NOTIFY |WS_TRANSPARENT| WS_OWNERDRAW |WS_VISIBLE,
                   0, rc.h * 1 / 2, 70, 70, hwnd, eID_Pic_PREV, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_Pic_PREV), controlFont_64); 
      CreateWindow(BUTTON, L"K", BS_FLAT | BS_NOTIFY |WS_TRANSPARENT| WS_OWNERDRAW | WS_VISIBLE,
                   rc.w - 65, rc.h * 1 / 2, 70, 70, hwnd, eID_Pic_NEXT, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_Pic_NEXT), controlFont_64);   
      
      CreateWindow(BUTTON, L"F", BS_FLAT | BS_NOTIFY|WS_TRANSPARENT|WS_OWNERDRAW |WS_VISIBLE,
                    0, 10, 75, 50, hwnd, eID_Pic_Return, NULL, NULL);  
      
      CreateWindow(BUTTON, L"JPG", WS_TRANSPARENT| WS_OWNERDRAW |WS_VISIBLE,
                   0, 420, 60, 60, hwnd, eID_Pic_JPG, NULL, NULL);
 
      CreateWindow(BUTTON, L"(", BS_FLAT | BS_NOTIFY | WS_TRANSPARENT|WS_OWNERDRAW | WS_VISIBLE,
                   675, 420, 60, 60, hwnd, eID_ZOOMIN, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_ZOOMIN), controlFont_48);  
      CreateWindow(BUTTON, L")", BS_FLAT | BS_NOTIFY | WS_TRANSPARENT|WS_OWNERDRAW| WS_VISIBLE,
                   740, 420, 60, 60, hwnd, eID_ZOOMOUT, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_ZOOMOUT), controlFont_48);
                   
      CreateWindow(TEXTBOX, L" ", NULL, 
                   GUI_PicViewer_Icon[1].rc.x, GUI_PicViewer_Icon[1].rc.y, 
                   GUI_PicViewer_Icon[1].rc.w, GUI_PicViewer_Icon[1].rc.h,          
                   hwnd, eID_Pic_MsgBOX, NULL, NULL);
      SendMessage(GetDlgItem(hwnd,eID_Pic_MsgBOX),TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BORDER|DT_BKGND); 
      
      CreateWindow(BUTTON, L"�ֱ��ʣ�", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[2].rc.x, GUI_PicViewer_Icon[2].rc.y, 
                   GUI_PicViewer_Icon[2].rc.w, GUI_PicViewer_Icon[2].rc.h,          
                   hwnd, eID_Pic_Res, NULL, NULL);

      CreateWindow(BUTTON, L" ", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[3].rc.x, GUI_PicViewer_Icon[3].rc.y, 
                   GUI_PicViewer_Icon[3].rc.w, GUI_PicViewer_Icon[3].rc.h,          
                   hwnd, eID_Pic_Res_Value, NULL, NULL);
                   
      CreateWindow(BUTTON, L"ʱ�䣺", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[4].rc.x, GUI_PicViewer_Icon[4].rc.y, 
                   GUI_PicViewer_Icon[4].rc.w, GUI_PicViewer_Icon[4].rc.h,          
                   hwnd, eID_Pic_Time, NULL, NULL);

      CreateWindow(BUTTON, L" ", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[5].rc.x, GUI_PicViewer_Icon[5].rc.y, 
                   GUI_PicViewer_Icon[5].rc.w, GUI_PicViewer_Icon[5].rc.h,          
                   hwnd, eID_Pic_Time_Value, NULL, NULL);
    
      CreateWindow(BUTTON, L"֡�ʣ�", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[6].rc.x, GUI_PicViewer_Icon[6].rc.y, 
                   GUI_PicViewer_Icon[6].rc.w, GUI_PicViewer_Icon[6].rc.h,          
                   hwnd, eID_Pic_FPS, NULL, NULL);

      CreateWindow(BUTTON, L" ", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[7].rc.x, GUI_PicViewer_Icon[7].rc.y, 
                   GUI_PicViewer_Icon[7].rc.w, GUI_PicViewer_Icon[7].rc.h,          
                   hwnd, eID_Pic_FPS_Value, NULL, NULL);      
      
      SetTimer(hwnd,1,0,TMR_SINGLE,NULL);//��������
      SetTimer(hwnd,2,0,TMR_SINGLE,NULL);//��������
      //PicViewer_Init();
      break;
    } 
    case WM_TIMER:
    {
      
      switch((UINT)wParam)
      {
        case 1:
        {
          ShowWindow(GetDlgItem(hwnd, eID_Pic_MsgBOX), SW_HIDE);
          RedrawWindow(hwnd, NULL, RDW_ALLCHILDREN|RDW_ERASE|RDW_INVALIDATE);
          PicViewer.scale_state = 4;          
          break;
        }
        case 2:
        {
          InvalidateRect(hwnd,NULL,TRUE);
          break;
        }
      }
      break;
    } 
    case WM_NOTIFY:
    {
      u16 code, id;
      id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
      code=HIWORD(wParam);//��ȡ��Ϣ������
      if(code == BN_CLICKED)
      { 
        switch(id)
        {
          case eID_Pic_NEXT:
          {
            PicViewer.scale_state = 0;
            SetWindowText(GetDlgItem(hwnd, eID_Pic_MsgBOX), L"����Ƭ�Ѿ������һ����");
            ResetTimer(hwnd,1,1000,TMR_START|TMR_SINGLE,NULL);
            ShowWindow(GetDlgItem(hwnd, eID_Pic_MsgBOX), SW_SHOW);    
                 
            break;
          }
          case eID_Pic_PREV:
          {
            PicViewer.scale_state = 0;
            SetWindowText(GetDlgItem(hwnd, eID_Pic_MsgBOX), L"����Ƭ�Ѿ��ǵ�һ����");
            ResetTimer(hwnd,1,1000,TMR_START|TMR_SINGLE,NULL);
            ShowWindow(GetDlgItem(hwnd, eID_Pic_MsgBOX), SW_SHOW);   
            break;
          }
          case eID_Pic_JPG:
          {
//            int i, j;
            PicViewer.cur_type++;
            PicViewer.scale_state = 0;            
            if(PicViewer.cur_type > 3)
              PicViewer.cur_type = 0; 
            if(PicViewer.gif_state != 0 && PicViewer.cur_type != 3)
            {
              PicViewer.gif_state = 0;
              GIF_Close(hgif);
              
            }            
            switch(PicViewer.cur_type)
            {
              case 0:
              {
                SetWindowText(GetDlgItem(hwnd, eID_Pic_JPG), L"JPG");
                InvalidateRect(PicViewer.mPicViewer, NULL, TRUE);
                break;
              }
              case 1:
              {
                InvalidateRect(PicViewer.mPicViewer, NULL, TRUE);
                SetWindowText(GetDlgItem(hwnd, eID_Pic_JPG), L"PNG");
                
                break;                
              }
              case 2:
              {               
                SetWindowText(GetDlgItem(hwnd, eID_Pic_JPG), L"BMP");
                InvalidateRect(PicViewer.mPicViewer, NULL, TRUE);
                break;
              }
              case 3:
              {                
                SetWindowText(GetDlgItem(hwnd, eID_Pic_JPG), L"GIF");
                InvalidateRect(PicViewer.mPicViewer, NULL, TRUE);
                break;                
              }              
            
            }
            break;
          }
          case eID_ZOOMIN:
          {
            GUI_DEBUG("�Ŵ�");
            InvalidateRect(hwnd,NULL,TRUE);
            PicViewer.scale_state = 1;
            
            break;
          }   
          case eID_ZOOMOUT:
          {
            PicViewer.scale_state = 2; 
            InvalidateRect(hwnd,NULL,TRUE);
            GUI_DEBUG("��С");
            break;
          }          
          case eID_Pic_Return:
          {
            PostCloseMessage(hwnd);
            break;
          }           
        }
        
      }
      break;
    } 
    case WM_PAINT:
    {
       HDC hdc, hdc_mem;
       PAINTSTRUCT ps;
       RECT rc = {0,0,800,70};
       hdc_mem = CreateMemoryDC(SURF_ARGB4444, 800,70);
       
       hdc = BeginPaint(hwnd, &ps);
       
       SetBrushColor(hdc_mem, MapARGB(hdc_mem,155,105, 105, 105));
       FillRect(hdc_mem, &rc);
       
       BitBlt(hdc, 0,0,800,70,hdc_mem,0,0,SRCCOPY);
       DeleteDC(hdc_mem);
       EndPaint(hwnd, &ps);
       break;
    }       
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
      if(ds->ID == eID_Pic_Time || ds->ID == eID_Pic_Time_Value|| 
          ds->ID == eID_Pic_FPS || ds->ID == eID_Pic_FPS_Value ||
          ds->ID == eID_Pic_Res || ds->ID == eID_Pic_Res_Value )
       {
          PicViewer_TBOX_OwnerDraw(ds);
          return TRUE;            
       }
       switch(ds->ID)
       {
          case eID_Pic_Name:
          {              
             PicViewer_TBOX_OwnerDraw(ds);
             return TRUE;              
          } 
          case eID_Pic_PREV:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          }            
          case eID_Pic_NEXT:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          } 
          case eID_Pic_JPG:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          }            
          case eID_ZOOMIN:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          } 
          case eID_ZOOMOUT:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          }
          case eID_Pic_Return:
          {
            Pic_ReturnBTN_Ownerdraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          }          
                     
       }
       break;
    }
    case WM_ERASEBKGND:
    {
      HDC hdc =(HDC)wParam;
      RECT rc =*(RECT*)lParam;
      WCHAR wbuf[128];
      switch(PicViewer.cur_type)
      {
        rt_tick_t tick;
        case 0:
        {
          GUI_DEBUG("JPG");
          tick = rt_tick_get();
          Draw_Pic_JPG("NULL");
       
          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY); 
          
          float time = (float)(rt_tick_get() - tick)/1000;
          x_wsprintf(wbuf, L"%.2fs", time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_Time_Value), wbuf);
          x_wsprintf(wbuf, L"%.1fFPS", 1/time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_FPS_Value), wbuf);            
          x_mbstowcs_cp936(wbuf, JPG_FILE_NAME, PICFILE_NAME_MAXLEN);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_Name), wbuf);
          DeleteDC(PicViewer.mhdc_pic);
          break;
        }
        case 1:
        {          
          GUI_DEBUG("PNG");
          tick = rt_tick_get();
          Draw_Pic_PNG("NULL");
          float time = (float)(rt_tick_get() - tick)/1000;
          x_wsprintf(wbuf, L"%.2fs", time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_Time_Value), wbuf);
          x_wsprintf(wbuf, L"%.1fFPS", 1/time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_FPS_Value), wbuf);  
          
          x_mbstowcs_cp936(wbuf, PNG_FILE_NAME, PICFILE_NAME_MAXLEN);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_Name),wbuf);
          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY);
          DeleteDC(PicViewer.mhdc_pic);         

          break;
        }
        case 2:
        {
          tick = rt_tick_get();
          Draw_Pic_BMP(hdc, "NULL");
          float time = (float)(rt_tick_get() - tick)/1000;
          x_wsprintf(wbuf, L"%.2fs", time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_Time_Value), wbuf);
          x_wsprintf(wbuf, L"%.1fFPS", 1/time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_FPS_Value), wbuf);            
          

          
          GUI_DEBUG("BMP");
          x_mbstowcs_cp936(wbuf, BMP_FILE_NAME, PICFILE_NAME_MAXLEN);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_Name), wbuf);
          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY);
          DeleteDC(PicViewer.mhdc_pic);
          break;
        }        
        case 3:
        {
          GUI_DEBUG("GIF");
          tick = rt_tick_get();
          Draw_Pic_GIF("NULL", hdc);
          float time = (float)(rt_tick_get() - tick)/1000;
          x_wsprintf(wbuf, L"%.2fs", time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_Time_Value), wbuf);
          x_wsprintf(wbuf, L"%.1fFPS", 1/time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_FPS_Value), wbuf);           
          
          x_mbstowcs_cp936(wbuf, GIF_FILE_NAME, PICFILE_NAME_MAXLEN);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_Name), wbuf);
          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY);
          DeleteDC(PicViewer.mhdc_pic);          
          ResetTimer(hwnd,2,PicViewer.delay,TMR_SINGLE|TMR_START,NULL);          
        }          
      }
      //
      
//      x_mbstowcs_cp936(wbuf, PicViewer.pic_lcdlist[PicViewer.show_index], PICFILE_NAME_MAXLEN);
//      SetWindowText(GetDlgItem(hwnd, eID_Pic_Name), wbuf); 
      x_wsprintf(wbuf, L"%d*%d", PicViewer.pic_width, PicViewer.pic_height);
      SetWindowText(GetDlgItem(hwnd, eID_Pic_Res_Value), wbuf); 
      //x_mbstowcs_cp936(wbuf, PicViewer.pic_lcdlist[PicViewer.show_index], PICFILE_NAME_MAXLEN);
//      SetBrushColor(PicViewer.mhdc_bk, MapRGB(PicViewer.mhdc_bk, 0, 0, 0));
//      FillRect(PicViewer.mhdc_bk, &rc);
      //PIC_BMP_Draw_FS(hdc, 70, 70, PicViewer.pic_list[PicViewer.show_index], NULL); 
      //BitBlt(PicViewer.mhdc_bk, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, 0, 0, SRCCOPY);
      
      
      
      return TRUE;
    }
    case WM_DESTROY:
    {
      //PicViewer_Quit();
      PicViewer.scale_state = 0;
      if(PicViewer.cur_type == 3)
      {
        PicViewer.gif_state = 0;
        GIF_Close(hgif);
        
      }        
      PicViewer.pic_nums = 0;
      PicViewer.show_index = 0;      
      return PostQuitMessage(hwnd);	
    }   
    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }    
  return WM_NULL;
}
//SD��ͼƬ��ʾ���ڻص�����
static	LRESULT DlgSDCARD_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
//  static int fps = 0;
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      PicViewer.cur_type = 0;
      GetClientRect(hwnd, &rc); 
      CreateWindow(BUTTON, L"ͼƬ����", WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE, 
                   GUI_PicViewer_Icon[0].rc.x, GUI_PicViewer_Icon[0].rc.y, 
                   GUI_PicViewer_Icon[0].rc.w, GUI_PicViewer_Icon[0].rc.h,          
                   hwnd, eID_Pic_Name, NULL, NULL);  
      CreateWindow(BUTTON, L"L", BS_FLAT | BS_NOTIFY |WS_TRANSPARENT| WS_OWNERDRAW |WS_VISIBLE,
                   0, rc.h * 1 / 2, 70, 70, hwnd, eID_Pic_PREV, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_Pic_PREV), controlFont_64); 
      CreateWindow(BUTTON, L"K", BS_FLAT | BS_NOTIFY |WS_TRANSPARENT| WS_OWNERDRAW | WS_VISIBLE,
                   rc.w - 65, rc.h * 1 / 2, 70, 70, hwnd, eID_Pic_NEXT, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_Pic_NEXT), controlFont_64);   
      
      CreateWindow(BUTTON, L"F", BS_FLAT | BS_NOTIFY|WS_TRANSPARENT|WS_OWNERDRAW |WS_VISIBLE,
                    0, 10, 75, 50, hwnd, eID_Pic_Return, NULL, NULL);  
      
      CreateWindow(BUTTON, L"JPG", WS_TRANSPARENT| WS_OWNERDRAW |WS_VISIBLE,
                   0, 420, 60, 60, hwnd, eID_Pic_JPG, NULL, NULL);
 
      CreateWindow(BUTTON, L"(", BS_FLAT | BS_NOTIFY | WS_TRANSPARENT|WS_OWNERDRAW | WS_VISIBLE,
                   675, 420, 60, 60, hwnd, eID_ZOOMIN, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_ZOOMIN), controlFont_48);  
      CreateWindow(BUTTON, L")", BS_FLAT | BS_NOTIFY | WS_TRANSPARENT|WS_OWNERDRAW| WS_VISIBLE,
                   740, 420, 60, 60, hwnd, eID_ZOOMOUT, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_ZOOMOUT), controlFont_48);
                   
      CreateWindow(TEXTBOX, L" ", NULL, 
                   GUI_PicViewer_Icon[1].rc.x, GUI_PicViewer_Icon[1].rc.y, 
                   GUI_PicViewer_Icon[1].rc.w, GUI_PicViewer_Icon[1].rc.h,          
                   hwnd, eID_Pic_MsgBOX, NULL, NULL);
      SendMessage(GetDlgItem(hwnd,eID_Pic_MsgBOX),TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BORDER|DT_BKGND);
      
      CreateWindow(BUTTON, L"�ֱ��ʣ�", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[2].rc.x, GUI_PicViewer_Icon[2].rc.y, 
                   GUI_PicViewer_Icon[2].rc.w, GUI_PicViewer_Icon[2].rc.h,          
                   hwnd, eID_Pic_Res, NULL, NULL);

      CreateWindow(BUTTON, L" ", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[3].rc.x, GUI_PicViewer_Icon[3].rc.y, 
                   GUI_PicViewer_Icon[3].rc.w, GUI_PicViewer_Icon[3].rc.h,          
                   hwnd, eID_Pic_Res_Value, NULL, NULL);
                   
      CreateWindow(BUTTON, L"ʱ�䣺", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[4].rc.x, GUI_PicViewer_Icon[4].rc.y, 
                   GUI_PicViewer_Icon[4].rc.w, GUI_PicViewer_Icon[4].rc.h,          
                   hwnd, eID_Pic_Time, NULL, NULL);

      CreateWindow(BUTTON, L" ", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[5].rc.x, GUI_PicViewer_Icon[5].rc.y, 
                   GUI_PicViewer_Icon[5].rc.w, GUI_PicViewer_Icon[5].rc.h,          
                   hwnd, eID_Pic_Time_Value, NULL, NULL);
    
      CreateWindow(BUTTON, L"֡�ʣ�", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[6].rc.x, GUI_PicViewer_Icon[6].rc.y, 
                   GUI_PicViewer_Icon[6].rc.w, GUI_PicViewer_Icon[6].rc.h,          
                   hwnd, eID_Pic_FPS, NULL, NULL);

      CreateWindow(BUTTON, L" ", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[7].rc.x, GUI_PicViewer_Icon[7].rc.y, 
                   GUI_PicViewer_Icon[7].rc.w, GUI_PicViewer_Icon[7].rc.h,          
                   hwnd, eID_Pic_FPS_Value, NULL, NULL);      
      SetTimer(hwnd,1,0,TMR_SINGLE,NULL);//��������
      SetTimer(hwnd,2,0,TMR_SINGLE,NULL);//��������
      //SetTimer(hwnd,3,0,TMR_SINGLE|TMR_START,NULL);//��������
      PicViewer_Init();
      break;
    }
    case WM_TIMER:
    {
      
      switch((UINT)wParam)
      {
        case 1:
        {
          ShowWindow(GetDlgItem(hwnd, eID_Pic_MsgBOX), SW_HIDE);
          RedrawWindow(hwnd, NULL, RDW_ALLCHILDREN|RDW_ERASE|RDW_INVALIDATE);
          PicViewer.scale_state = 4;
          //InvalidateRect(GetDlgItem(hwnd, eID_Pic_Time_Value), NULL, TRUE);
          break;
        }
        case 2:
        {
          InvalidateRect(hwnd,NULL,TRUE);
          break;
        }
//        case 3:
//        {
////          GUI_DEBUG("%d", fps);
//          break;
//        }
      }
      break;
    }    
    case WM_NOTIFY:
    {
      u16 code, id;
      id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
      code=HIWORD(wParam);//��ȡ��Ϣ������
      if(code == BN_CLICKED)
      { 
        switch(id)
        {
          case eID_Pic_NEXT:
          {
            PicViewer.show_index++;

            if(PicViewer.show_index < PicViewer.pic_nums)
            {
              if(PicViewer.cur_type == 3)
              {
                PicViewer.gif_state = 0;
              //if(PicViewer.cur_type == 3)
                GIF_Close(hgif);
              }
              RedrawWindow(hwnd, NULL, RDW_ALLCHILDREN|RDW_ERASE|RDW_INVALIDATE);
              PicViewer.scale_state = 0;
              PicViewer.scale_time = 0;
            }
            else
            {
              PicViewer.show_index--;
              SetWindowText(GetDlgItem(hwnd, eID_Pic_MsgBOX), L"����Ƭ�Ѿ������һ����");
              ResetTimer(hwnd,1,1000,TMR_START|TMR_SINGLE,NULL);
              //
              ShowWindow(GetDlgItem(hwnd, eID_Pic_MsgBOX), SW_SHOW);
              //
            }

            
            break;
          }
          case eID_Pic_PREV:
          {
            PicViewer.show_index--;
//            if(PicViewer.cur_type == 3)
//              GIF_Close(hgif);
            if(PicViewer.show_index >= 0)
            {
              if(PicViewer.cur_type == 3)
              {
                PicViewer.gif_state = 0;
              //if(PicViewer.cur_type == 3)
                GIF_Close(hgif);
              }            
              RedrawWindow(hwnd, NULL, RDW_ALLCHILDREN|RDW_ERASE|RDW_INVALIDATE);
              PicViewer.scale_state = 0;
              PicViewer.scale_time = 0;
            }
            else
            {
              PicViewer.show_index++;
              SetWindowText(GetDlgItem(hwnd, eID_Pic_MsgBOX), L"����Ƭ�Ѿ��ǵ�һ����");
              ResetTimer(hwnd,1,1000,TMR_START|TMR_SINGLE,NULL);
              ShowWindow(GetDlgItem(hwnd, eID_Pic_MsgBOX), SW_SHOW);    
            }

            //InvalidateRect(hwnd, NULL, TRUE);
            break;
          }
          case eID_Pic_JPG:
          {
            int i, j;
            PicViewer.cur_type++;
            PicViewer.show_index = 0;
            PicViewer.pic_nums= 0;
            PicViewer.scale_state = 0;
            PicViewer.scale_time = 0;            
            for(i = 0; i < PICFILE_NUM_MAX; i++)
            {
              for(j = 0; j < PICFILE_NAME_MAXLEN; j++)
              {
                PicViewer.pic_lcdlist[i][j] = '\0';
                PicViewer.pic_list[i][j] = '\0';
              }
            }  
            if(PicViewer.cur_type > 3)
              PicViewer.cur_type = 0; 
            if(PicViewer.gif_state != 0 && PicViewer.cur_type != 3)
            {
              PicViewer.gif_state = 0;
              GIF_Close(hgif);
              
            }            
            switch(PicViewer.cur_type)
            {
              case 0:
              {
                scan_Picfiles(path,".jpg");
                SetWindowText(GetDlgItem(hwnd, eID_Pic_JPG), L"JPG");
                InvalidateRect(PicViewer.mPicViewer, NULL, TRUE);
                break;
              }//JPG
              case 1:
              {
                scan_Picfiles(path,".png");
                InvalidateRect(PicViewer.mPicViewer, NULL, TRUE);
                SetWindowText(GetDlgItem(hwnd, eID_Pic_JPG), L"PNG");
                
                break;                
              }
              case 2:
              {
                scan_Picfiles(path,".bmp");                
                SetWindowText(GetDlgItem(hwnd, eID_Pic_JPG), L"BMP");
                InvalidateRect(PicViewer.mPicViewer, NULL, TRUE);
                break;
              }
              case 3:
              {
                scan_Picfiles(path,".gif");
                
                SetWindowText(GetDlgItem(hwnd, eID_Pic_JPG), L"GIF");
                InvalidateRect(PicViewer.mPicViewer, NULL, TRUE);
                break;                
              }              
            
            }
            break;
          }
          case eID_Pic_Return:
          {
            PostCloseMessage(hwnd);
            break;
          }          
          case eID_ZOOMIN:
          {
            GUI_DEBUG("�Ŵ�");
            InvalidateRect(hwnd,NULL,TRUE);
            PicViewer.scale_state = 1;
            
            break;
          }   
          case eID_ZOOMOUT:
          {
            PicViewer.scale_state = 2; 
            InvalidateRect(hwnd,NULL,TRUE);
            GUI_DEBUG("��С");
            break;
          }          
        }
        
      }
      break;
    }      
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       if(ds->ID == eID_Pic_Time || ds->ID == eID_Pic_Time_Value || 
          ds->ID == eID_Pic_FPS || ds->ID == eID_Pic_FPS_Value)
       {
          PicViewer_TBOX_OwnerDraw(ds);
          return TRUE;            
       }
       switch(ds->ID)
       {
          case eID_Pic_Name:
          {              
             PicViewer_TBOX_OwnerDraw(ds);
             return TRUE;              
          } 
          case eID_Pic_Res:
          {              
             PicViewer_TBOX_OwnerDraw(ds);
             return TRUE;              
          }          
          case eID_Pic_Res_Value:
          {              
             PicViewer_TBOX_OwnerDraw(ds);
             return TRUE;              
          }           
          case eID_Pic_PREV:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          }            
          case eID_Pic_NEXT:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          } 
          case eID_Pic_JPG:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          }            
          case eID_ZOOMIN:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          } 
          case eID_ZOOMOUT:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          }
          case eID_Pic_Return:
          {
            Pic_ReturnBTN_Ownerdraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          }          
                     
       }
       break;
    }
    case WM_ERASEBKGND:
    {
      HDC hdc =(HDC)wParam;
      RECT rc =*(RECT*)lParam;
      WCHAR wbuf[128];
      rt_tick_t tick;
      switch(PicViewer.cur_type)
      {
        case 0:
        {
          tick = rt_tick_get();
          Draw_Pic_JPG(PicViewer.pic_list[PicViewer.show_index]);
          
          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY);       
          float time = (float)(rt_tick_get() - tick)/1000;
          x_wsprintf(wbuf, L"%.2fs", time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_Time_Value), wbuf);
          x_wsprintf(wbuf, L"%.1fFPS", 1/time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_FPS_Value), wbuf);         
          DeleteDC(PicViewer.mhdc_pic);
          break;
        }
//        case eID_Pic_BMP:
//        {
////          SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
////          FillRect(hdc, &rc); 
//          Draw_Pic_BMP(PicViewer.mhdc_pic, PicViewer.pic_list[PicViewer.show_index]);
//          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY);
//          DeleteDC(PicViewer.mhdc_pic);          
//          GUI_DEBUG("BMP");

//          break;
//        }
        case 1:
        {  
          tick = rt_tick_get();          
          Draw_Pic_PNG(PicViewer.pic_list[PicViewer.show_index]);
          float time = (float)(rt_tick_get() - tick)/1000;
          x_wsprintf(wbuf, L"%.2fs", time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_Time_Value), wbuf);
          x_wsprintf(wbuf, L"%.1fFPS", 1/time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_FPS_Value), wbuf);
          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY);
          DeleteDC(PicViewer.mhdc_pic);         

          break;
        }
        case 2:
        {
          tick = rt_tick_get();
          Draw_Pic_BMP(hdc, PicViewer.pic_list[PicViewer.show_index]);
          float time = (float)(rt_tick_get() - tick)/1000;
          x_wsprintf(wbuf, L"%.2fs", time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_Time_Value), wbuf);
          x_wsprintf(wbuf, L"%.1fFPS", 1/time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_FPS_Value), wbuf); 
          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY);
          DeleteDC(PicViewer.mhdc_pic);
          break;
        }        
        case 3:
        {
          //GUI_DEBUG("GIF");
          tick = rt_tick_get();
          Draw_Pic_GIF(PicViewer.pic_list[PicViewer.show_index], hdc);
          float time = (float)(rt_tick_get() - tick)/1000;
          x_wsprintf(wbuf, L"%.2fs", time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_Time_Value), wbuf);
          x_wsprintf(wbuf, L"%.1fFPS", 1/time);
          SetWindowText(GetDlgItem(hwnd, eID_Pic_FPS_Value), wbuf); 
          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY);
          DeleteDC(PicViewer.mhdc_pic);          
          ResetTimer(hwnd,2,PicViewer.delay,TMR_SINGLE|TMR_START,NULL);          
        }          
      }
      //
      
      x_mbstowcs_cp936(wbuf, PicViewer.pic_lcdlist[PicViewer.show_index], PICFILE_NAME_MAXLEN);
      SetWindowText(GetDlgItem(hwnd, eID_Pic_Name), wbuf); 
      x_wsprintf(wbuf, L"%d*%d", PicViewer.pic_width, PicViewer.pic_height);
      SetWindowText(GetDlgItem(hwnd, eID_Pic_Res_Value), wbuf); 
      
//      x_wsprintf(wbuf, L"%d*%d",1000);
           
      //x_mbstowcs_cp936(wbuf, PicViewer.pic_lcdlist[PicViewer.show_index], PICFILE_NAME_MAXLEN);
//      SetBrushColor(PicViewer.mhdc_bk, MapRGB(PicViewer.mhdc_bk, 0, 0, 0));
//      FillRect(PicViewer.mhdc_bk, &rc);
      //PIC_BMP_Draw_FS(hdc, 70, 70, PicViewer.pic_list[PicViewer.show_index], NULL); 
      //BitBlt(PicViewer.mhdc_bk, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, 0, 0, SRCCOPY);
      
      
      
      return TRUE;
    }
    case WM_PAINT:
    {
       HDC hdc, hdc_mem;
       PAINTSTRUCT ps;
       RECT rc = {0,0,800,70};
       hdc_mem = CreateMemoryDC(SURF_ARGB4444, 800,70);
       
       hdc = BeginPaint(hwnd, &ps);
       
       SetBrushColor(hdc_mem, MapARGB(hdc_mem,155,105, 105, 105));
       FillRect(hdc_mem, &rc);
       
       BitBlt(hdc, 0,0,800,70,hdc_mem,0,0,SRCCOPY);
       DeleteDC(hdc_mem);
       EndPaint(hwnd, &ps);
       break;
    }   
    case WM_DESTROY:
    {
      PicViewer_Quit();
      if(PicViewer.cur_type == 3)
      {
        PicViewer.gif_state = 0;
        GIF_Close(hgif);
        
      }        
      return PostQuitMessage(hwnd);	
    }      
    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);    
  }
  return WM_NULL;
}
//��������ʾ����
static	LRESULT	PicViewer_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
//      RECT rc[3];
      list_menu_cfg_t cfg;
      CreateWindow(BUTTON, L"ͼƬ�����", WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE, 100,0,600,35,          
                   hwnd, eID_Pic_Title, NULL, NULL);
      CreateWindow(BUTTON, L"֧��jpg��bmp��png��gif��ʽ", WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE, 100,35,600,35,          
                   hwnd, eID_Pic_Def, NULL, NULL);
      
      cfg.list_objs = path_list_1; //ָ��list�б�.
      cfg.x_num = 3; //ˮƽ����.
      cfg.y_num = 1; //��ֱ����.
      cfg.bg_color = 0x000000;

      CreateWindow(&wcex_ListMenu,
                  L"ListMenu1",
                  WS_VISIBLE ,
                  70, 140, 660, 200,
                  hwnd,
                  eID_LIST_1,
                  NULL,
                  &cfg);      
     SendMessage(GetDlgItem(hwnd, eID_LIST_1), MSG_LIST_DRAG_ENABLE, FALSE, 0);
      
      CreateWindow(BUTTON, L"O", BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                   730, 0, 70, 70, hwnd, eID_Pic_EXIT, NULL, NULL);      
      break;
    }
    case WM_ERASEBKGND:
    {
      HDC hdc =(HDC)wParam;
      RECT rc =*(RECT*)lParam; 
      
      SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
      FillRect(hdc, &rc);       
      return TRUE;
    }
    case WM_PAINT:
    {
       HDC hdc, hdc_mem;
       PAINTSTRUCT ps;
       RECT rc = {0,0,800,70};
       hdc_mem = CreateMemoryDC(SURF_ARGB4444, 800,70);
       
       hdc = BeginPaint(hwnd, &ps);
       
       SetBrushColor(hdc_mem, MapARGB(hdc_mem,155,105, 105, 105));
       FillRect(hdc_mem, &rc);
       
       BitBlt(hdc, 0,0,800,70,hdc_mem,0,0,SRCCOPY);
       DeleteDC(hdc_mem);
       EndPaint(hwnd, &ps);
       break;
    }       
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_Pic_EXIT:
          {
            PicViewer_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }
          case eID_Pic_Title:
          {
            PicViewer_TBOX_OwnerDraw(ds);
            return TRUE;  
          }
          case eID_Pic_Def:
          {
            PicViewer_TBOX_OwnerDraw(ds);
            return TRUE;  
          }          
                    
       }
//       if(ds->ID == eID_Pic_INTFLASH || ds->ID == eID_Pic_EXTFLASH || ds->ID == eID_Pic_SDCARD)
//       {
//         
//         PicViewer_Button_OwnerDraw(ds);//��͸���ȿؼ�
//         return TRUE;
//       }
       break;
    }  
    case WM_NOTIFY:
    {
      u16 code, id;
      LM_NMHDR *nm;
      id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
      code=HIWORD(wParam);//��ȡ��Ϣ������    
      if(code == BN_CLICKED && id == eID_Pic_EXIT)
      {
        PostCloseMessage(hwnd);
        break;
      }       
      nm = (LM_NMHDR*)lParam;

      if(nm->idx==2&& code==LMN_CLICKED)
      {
   
//        if(SendMessage(nr->hwndFrom,BM_GETSTATE,0,0)&BST_CHECKED) //��ȡ��ǰ״̬
        { //��ѡ��ѡ��.
          WNDCLASS wcex;
          PicViewer.cur_path = eID_Pic_SDCARD;
          wcex.Tag	 		= WNDCLASS_TAG;
          wcex.Style			= CS_HREDRAW | CS_VREDRAW;
          wcex.lpfnWndProc	= (WNDPROC)DlgSDCARD_proc;
          wcex.cbClsExtra		= 0;
          wcex.cbWndExtra		= 0;
          wcex.hInstance		= NULL;
          wcex.hIcon			= NULL;
          wcex.hCursor		= NULL;
        
          PicViewer.mPicViewer = CreateWindowEx(WS_EX_FRAMEBUFFER,
                                 &wcex,L"SDCARD",
                                 WS_OVERLAPPED|WS_CLIPCHILDREN|WS_VISIBLE,
                                 0,0,800,480,
                                 hwnd,0,NULL,NULL);

        }                        
      }
      if(nm->idx==0&& code==LMN_CLICKED)
      {
        

//        if(SendMessage(nr->hwndFrom,BM_GETSTATE,0,0)&BST_CHECKED) //��ȡ��ǰ״̬
        { //��ѡ��ѡ��.
          WNDCLASS wcex;
          PicViewer.cur_path = eID_Pic_INTFLASH;
          wcex.Tag	 		= WNDCLASS_TAG;
          wcex.Style			= CS_HREDRAW | CS_VREDRAW;
          wcex.lpfnWndProc	= (WNDPROC)DlgINTFLASH_proc;
          wcex.cbClsExtra		= 0;
          wcex.cbWndExtra		= 0;
          wcex.hInstance		= NULL;
          wcex.hIcon			= NULL;
          wcex.hCursor		= NULL;
        
          PicViewer.mPicViewer = CreateWindowEx(WS_EX_FRAMEBUFFER,
                                 &wcex,L"INTFLASH",
                                 WS_OVERLAPPED|WS_CLIPCHILDREN|WS_VISIBLE,
                                 0,0,800,480,
                                 hwnd,1,NULL,NULL);

        }                        
      }      
      
      if(nm->idx==1&& code==LMN_CLICKED)
      {
        
//        if(SendMessage(nr->hwndFrom,BM_GETSTATE,0,0)&BST_CHECKED) //��ȡ��ǰ״̬
        { //��ѡ��ѡ��.
          WNDCLASS wcex;
          PicViewer.cur_path = eID_Pic_EXTFLASH;
          wcex.Tag	 		= WNDCLASS_TAG;
          wcex.Style			= CS_HREDRAW | CS_VREDRAW;
          wcex.lpfnWndProc	= (WNDPROC)DlgEXTFLASH_proc;
          wcex.cbClsExtra		= 0;
          wcex.cbWndExtra		= 0;
          wcex.hInstance		= NULL;
          wcex.hIcon			= NULL;
          wcex.hCursor		= NULL;
        
          PicViewer.mPicViewer = CreateWindowEx(WS_EX_FRAMEBUFFER,
                                 &wcex,L"EXTFLASH",
                                 WS_OVERLAPPED|WS_CLIPCHILDREN|WS_VISIBLE,
                                 0,0,800,480,
                                 hwnd,2,NULL,NULL);

        }                        
      }      
      break;      
    }            
		case	WM_CTLCOLOR:
		{
			u16 id;
			id =LOWORD(wParam);
//			if(id== eID_Pic_Title || id == eID_Pic_Def)
//			{
//				CTLCOLOR *cr;
//				cr =(CTLCOLOR*)lParam;
//				cr->TextColor =RGB888(255,255,255);//������ɫ��RGB888��ɫ��ʽ)
//				cr->BackColor =ARGB8888(0,105,105,105);//������ɫ��RGB888��ɫ��ʽ)
////				cr->BorderColor =RGB888(255,0,0);//�߿���ɫ��RGB888��ɫ��ʽ)
//				return TRUE;
//			}
			if(id== eID_Pic_INTFLASH || id== eID_Pic_EXTFLASH || id== eID_Pic_SDCARD)
			{
        CTLCOLOR *cr;
        cr =(CTLCOLOR*)lParam;
				if(SendMessage(GetDlgItem(hwnd,id),BM_GETSTATE,0,0)&BST_CHECKED)
        {
					cr->TextColor =RGB888(105,105,105);
         //���ñ�����ɫ
					cr->BackColor =RGB888(0,0,0);
          //���ñ߿���ɫ
					cr->BorderColor =RGB888(0,0,0);
          return TRUE; 
				}
				else
				{
					cr->TextColor =RGB888(250,250,250);
					cr->BackColor =RGB888(0,0,0);
          cr->BorderColor =RGB888(0,0,0);
          return TRUE; 
        }
			}      
			else
			{
				return FALSE;
			}
      
      
		}    
    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);
  }
	return	WM_NULL;
}


void GUI_PicViewer_DIALOG(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;


	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = PicViewer_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//����������
	hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                        &wcex,
                        L"GUI_LED_DIALOG",
                        WS_CLIPCHILDREN,
                        0, 0, GUI_XSIZE, GUI_YSIZE,
                        NULL, NULL, NULL, NULL);
   //��ʾ������
	ShowWindow(hwnd, SW_SHOW);
	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}  
}
