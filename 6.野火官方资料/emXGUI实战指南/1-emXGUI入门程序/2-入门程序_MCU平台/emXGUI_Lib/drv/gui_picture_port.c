/**
  *********************************************************************
  * @file    gui_picture_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ͼƬ��ʾ�ӿ�
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */ 

#include <string.h>
#include	"emXGUI.h"
#include  "gui_drv_cfg.h"
#include  "gui_mem_port.h"
#include  "gui_resource_port.h"
#include  "gui_picture_port.h"




/*===================================================================================*/
/**
  * @brief  ����ý��������ݵĻص�������FLASH ���ļ�ϵͳ����
  * @param  buf[out] �洢��ȡ�������ݻ�����
  * @param  offset Ҫ��ȡ��λ��
  * @param  size Ҫ��ȡ�����ݴ�С
  * @param  lParam ���ú���ʱ���Զ���������û�������
  * @retval ��ȡ�������ݴ�С
  */
static int bmp_read_data_Res(void *buf,int offset,int size,LPARAM lParam)
{
	offset += lParam;
	RES_DevRead(buf,offset,size);
	return size;
}

 /**
  * @brief  ��ʾ�ļ�ϵͳ�е�BMPͼƬ��FLASH ���ļ�ϵͳ����
  * @param  hdc�����룩����ͼ������
  * @param  x��y�����룩: ���Ƶ�Ŀ�������
  * @param 	lprc�����룩:Ҫ���Ƶ�BMPͼ���������������øò���ΪNULL�����������BMPͼ������
  * @retval FALSE:ʧ��; TRUE:�ɹ�
  */
BOOL PIC_BMP_Draw_Res(HDC hdc, int x, int y, char *file_name, const RECT *lprc) 
{	 
    int pic_offset;
    GUI_GET_DATA  get_data;
  
    pic_offset =RES_GetOffset(file_name);
    if(pic_offset > 0)
    {    
      get_data.lParam = pic_offset;
      get_data.pfReadData = bmp_read_data_Res;

      return BMP_DrawEx(hdc,x,y,&get_data,lprc);
    }
    
    return FALSE;
}

 /**
  * @brief  ���BMPͼ�����Ϣ��FLASH ���ļ�ϵͳ����
  * @param  bm_info����������洢�õ���ͼ����Ϣ
  * @param  file_name�����룩: ���Ƶ�Ŀ�������
  * @retval FALSE:ʧ��; TRUE:�ɹ�
  */
BOOL PIC_BMP_GetInfo_Res(BITMAPINFO *bm_info, char *file_name) 
{	 
    int pic_offset;
    GUI_GET_DATA  get_data;
  
    pic_offset =RES_GetOffset(file_name);
    if(pic_offset > 0)
    {    
      get_data.lParam = pic_offset;
      get_data.pfReadData = bmp_read_data_Res;

      return BMP_GetInfoEx(bm_info,&get_data);
    }
    
    return FALSE;
}

/* �ļ�ϵͳ�ӿ� */
#if(GUI_PIC_FS_EN)

#include  "ff.h"

/**
  * @brief  ����ý��������ݵĻص�����(�ļ�ϵͳ)
  * @param  buf[out] �洢��ȡ�������ݻ�����
  * @param  offset Ҫ��ȡ��λ��
  * @param  size Ҫ��ȡ�����ݴ�С
  * @param  lParam ���ú���ʱ���Զ���������û�������
  * @retval ��ȡ�������ݴ�С
  */
static int bmp_read_data_fs(void *buf,int offset,int size,LPARAM lParam)
{
  int    rw;
  
  /* ���ص�������lParam�Ƕ�Ӧ���ļ�ָ��*/
  FIL *p_file = (FIL*)lParam;

  /* ƫ�Ƶ�ָ��λ�� */
  f_lseek(p_file, offset);
	/* ��ȡ���ݵ������� */
	 f_read(p_file, buf, (UINT)size, (UINT *)&rw);
	/* ���ض�ȡ�������ݴ�С */
	return rw;
}



 /**
  * @brief  ��ʾ�ļ�ϵͳ�е�BMPͼƬ(�ļ�ϵͳ)
  * @param  hdc�����룩����ͼ������
  * @param  x��y�����룩: ���Ƶ�Ŀ�������
  * @param 	lprc�����룩:Ҫ���Ƶ�BMPͼ���������������øò���ΪNULL�����������BMPͼ������
  * @retval FALSE:ʧ��; TRUE:�ɹ�
  */
BOOL PIC_BMP_Draw_FS(HDC hdc, int x, int y, char *file_name, const RECT *lprc) 
{	 	
    /* file objects */
    FIL     *file;												
    FRESULT fresult;  
    BOOL res = TRUE;
    GUI_GET_DATA  get_data;

    file =(FIL*)GUI_VMEM_Alloc(sizeof(FIL));
      
    /* ���ļ� */		
    fresult = f_open(file, file_name, FA_OPEN_EXISTING | FA_READ );
    if (fresult != FR_OK)
    {      
      GUI_ERROR("Open Pic failed!");
      GUI_VMEM_Free(file);
      return FALSE;
    }    
  
    /* ���ļ�ָ����ΪlParam����*/
    get_data.lParam = (LPARAM)file;
    /* ��ȡ���ݵĻص����� */
    get_data.pfReadData = bmp_read_data_fs;
    /* ��ʾͼƬ */
    res = BMP_DrawEx(hdc,x,y,&get_data,lprc);
    
    /* �ر��ļ� */
    f_close(file);
    
    /* �ͷſռ� */
    GUI_VMEM_Free(file);
    
    return res;
}

 /**
  * @brief  ���BMPͼ�����Ϣ(�ļ�ϵͳ)
  * @param  bm_info����������洢�õ���ͼ����Ϣ
  * @param  file_name�����룩: ���Ƶ�Ŀ�������
  * @retval FALSE:ʧ��; TRUE:�ɹ�
  */
BOOL PIC_BMP_GetInfo_FS(BITMAPINFO *bm_info, char *file_name) 
{	 
    /* file objects */
    FIL     *file;												
    FRESULT fresult;  
    BOOL res = TRUE;
    GUI_GET_DATA  get_data;

    file =(FIL*)GUI_VMEM_Alloc(sizeof(FIL));
      
    /* ���ļ� */		
    fresult = f_open(file, file_name, FA_OPEN_EXISTING | FA_READ );
    if (fresult != FR_OK)
    {   
      GUI_ERROR("Open Pic failed!");      
      GUI_VMEM_Free(file);
      return FALSE;
    }      
    /* ���ļ�ָ����ΪlParam����*/
    get_data.lParam = (LPARAM)file;
    /* ��ȡ���ݵĻص����� */
    get_data.pfReadData = bmp_read_data_fs;
    /* ��ȡͼƬ��Ϣ */
    res = BMP_GetInfoEx(bm_info,&get_data);
    f_close(file);
    
    /* �ͷſռ� */
    GUI_VMEM_Free(file);
    
    return res;
}

#endif /* GUI_PIC_FS_EN */


#if(GUI_PIC_CAPTURE_SCREEN_EN)

#pragma pack(1)
struct  tagBMP_HEADER  /*   λͼ�ļ���Ϣͷ�ṹ   */
{

	U16 bfType; /*   �ļ�����,   ����Ϊ   "BM "   (0x4D42)   */
	U32 bfSize; /*   �ļ��Ĵ�С(�ֽ�)   */
	U16 bfRsvd1; /*   ����,   ����Ϊ   0   */
	U16 bfRsvd2; /*   ����,   ����Ϊ   0   */
	U32 bfOffBits; /*   λͼ����������ļ�ͷ��ƫ����(�ֽ�)   */

	U32 biSize; /*   size   of   BITMAPINFOHEADER   */
	U32 biWidth; /*   λͼ���(����)   */
	U32 biHeight; /*   λͼ�߶�(����)   */
	U16 biPlanes; /*   Ŀ���豸��λƽ����,   ������Ϊ1   */
	U16 biBitCount; /*   ÿ�����ص�λ��,   1,4,8��24   */
	U32 biCompress; /*   λͼ���е�ѹ������,0=��ѹ��   */
	U32 biSizeImage; /*   ͼ���С(�ֽ�)   */
	U32 biXPelsPerMeter; /*   Ŀ���豸ˮƽÿ�����ظ���   */
	U32 biYPelsPerMeter; /*   Ŀ���豸��ֱÿ�����ظ���   */
	U32 biColorUsed; /*   λͼʵ��ʹ�õ���ɫ�����ɫ��   */
	U32 biColorImportant; /*   ��Ҫ��ɫ�����ĸ���   */
//	U32 biRedMask;
//	U32 biGreenMask;
//	U32 biBlueMask;
//	U32 biAlphaMask;

};
#pragma pack()

 /**
  * @brief  ��ʾ�ļ�ϵͳ�е�JPEGͼƬ(�ļ�ϵͳ)
  * @param  out_file ��ͼ���ļ���
  * @retval FALSE or TRUE
  */
BOOL PIC_Capture_Screen_To_BMP(const char *out_file)
{
	HDC hdc,hdc_mem;
	int i,w,h,fsize,line_bytes;
	struct  tagBMP_HEADER *bmp;
	FIL *file;
  FRESULT fresult;  
  BOOL res = TRUE;
  UINT bw;
  
	u8 *buf;
	////////

	w =GUI_XSIZE;
	h =GUI_YSIZE;
  
  /* �ļ�����ռ� */
  file =(FIL*)GUI_VMEM_Alloc(sizeof(FIL));

	hdc_mem =CreateMemoryDC(SURF_XRGB8888,w,h); //����һ������Ļ��С��ͬ���ڴ���DC��XRGB8888��ʽ��.

	if(hdc_mem!=NULL)
	{
		hdc =GetDC(NULL); //�����ĻDC��
		if(hdc!=NULL)
		{
			BitBlt(hdc_mem,0,0,w,h,hdc,0,0,SRCCOPY); //����ĻDC�����ݸ��Ƶ��ڴ�DC��.
			ReleaseDC(NULL,hdc); //�ͷ���ĻDC��

			line_bytes =w*3;
			fsize =54+(h*line_bytes); //����BMP�ļ���С(54B �ļ�ͷ+λͼ����)

			buf=(u8*)GUI_VMEM_Alloc(fsize);  //����һƬ�ڴ���Ϊ�ļ���������
			if(buf!=NULL)
			{
				u8 *dst;
				int x,y;

				bmp=(struct tagBMP_HEADER*)buf;
				memset(bmp,0,128);

				bmp->bfType    =0x4D42;
				bmp->bfSize    =54 + h*line_bytes;
				bmp->bfOffBits =54;

				bmp->biSize   =40;
				bmp->biWidth  =w;
				bmp->biHeight =h;
				bmp->biPlanes =1;
				bmp->biBitCount =24;
				bmp->biCompress =0;
				bmp->biSizeImage =h*line_bytes;
				bmp->biColorUsed =0;
				bmp->biColorImportant =0;

				dst =buf + bmp->bfOffBits;
				//src =(u8*)out+(h*line_bytes)-line_bytes;

				for(y=h-1;y>=0;y--)
				{
					i=0;
					for(x=0;x<w;x++)
					{
						U32 c;
						u8 r,g,b;
            
//            u8 a;
//						a=0xFF;

						c=GetPixel(hdc_mem,x,y);     //���ڴ�DC�������ֵ��
						GetRGB(hdc_mem,c,&r,&g,&b);  //������ֵת����8λ��R,G,B����������д��BMP�ļ�λͼ������.
						dst[i++] =b;
						dst[i++] =g;
						dst[i++] =r;
						//dst[i++] =a;
					}

					dst += line_bytes;
				}

        fresult = f_open(file, out_file, FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
				if(fresult == FR_OK)
				{
					u8 *p;
					int a;

					i=fsize;
					p=buf;
					while(i > 0)
					{
						a=MIN(i,(64*1024));
						fresult = f_write(file,p,a,&bw);
            if(fresult != FR_OK)
            { 
              res = FALSE;
              break;
            }              
            
						i -= a;
						p += a;
					}
					f_close(file);
				}
        else
          res = FALSE;
        
				GUI_VMEM_Free(file);
        GUI_VMEM_Free(buf);
			}
      else
        res = FALSE;
		}
    else 
      res = FALSE;
    
		DeleteDC(hdc_mem); //ɾ���ڴ�DC��
	}
  else 
    res = FALSE;
  
  return res;
}

#endif /* GUI_PIC_CAPTURE_SCREEN_EN */



/********************************END OF FILE****************************/
