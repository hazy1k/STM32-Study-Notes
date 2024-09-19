
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stdint.h"

#include "stm32f1xx.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./tjpgd/src/tjpgd.h"
#include "./FatFS/ff.h"
#include "./FatFS/ff_gen_drv.h"
#include "./FatFS/drivers/sd_diskio.h"


char SDPath[4];                   /* SD���߼��豸·�� */
FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FIL file;									        /* �ļ����� */
FRESULT f_res;                    /* �ļ�������� */

void printf_fatfs_error(FRESULT FRESULT);

/* User defined device identifier */
typedef struct {
    FIL *fp;      /* File pointer for input function */
    BYTE *fbuf;    /* Pointer to the frame buffer for output function */
    UINT wfbuf;    /* Width of the frame buffer [pix] */
} IODEV;


/* tiny jpg�����ӿڣ�in_func��out_func�ӿڣ� ���û�ʵ��*/

/*��SD����ȡͼƬ�����������Ҫ��SRAM���ȡ�����������޸Ĵ˺���*/
UINT in_func (JDEC* jd, BYTE* buff, UINT nbyte)
{
		UINT rwb;
		FRESULT res;
	
    IODEV *dev = (IODEV*)jd->device;   /* Device identifier for the session (5th argument of jd_prepare function) */


    if (buff) {
        /* Read bytes from input stream */
			  res = f_read(dev->fp,buff,nbyte,&rwb);
				if(res == FR_OK)
					return rwb;
				
    } else {
        /* Remove bytes from input stream */
			return (f_lseek(dev->fp, f_tell(dev->fp)+nbyte) == FR_OK) ? nbyte:0;
    }	
		
}



/*���������������*/
UINT out_func (JDEC* jd, void* bitmap, JRECT* rect)
{
    //IODEV *dev = (IODEV*)jd->device;
    BYTE *src;
    UINT x,y;


    /* Put progress indicator */
    if (rect->left == 0) {
        printf("\r%lu%%", (rect->top << jd->scale) * 100UL / jd->height);
    }
    
		src = (BYTE*)bitmap;	//������ͼ������
			
		/* ���� ��������С�Ĵ��� */
		ILI9341_OpenWindow(rect->left,rect->top,(rect->right - rect->left)+1,(rect->bottom - rect->top)+1);	
    ILI9341_Write_Cmd ( CMD_SetPixel );		
		for (y = rect->top; y <= rect->bottom; y++) 
		{	
			//��ʾһ����Ԫ��һ�У�������ԣ�8*8~16*16����ࣩ�ľ���Ϊһ�����뵥Ԫ
		for(x=0;x<(rect->right - rect->left)+1;x++)						
			{				
				ILI9341_Write_Data(*(WORD*)src);
				src += 2;	
			}				
		}

    return 1;    /* Continue to decompress */
}

/* ��ʾͼƬ������ 
 * pic_name:jpg�ļ�����ȫ·��		
 */
void jpgDisplay()
{
  void *work;       /* Pointer to the decompressor work area */
  JDEC jdec;        /* Decompression object */
  JRESULT res;      /* Result code of TJpgDec API */
  IODEV devid;      /* User defined device identifier */
	
  /* Open a JPEG file */
  /* ע��һ��FatFS�豸��SD�� */
  FATFS_LinkDriver(&SD_Driver, SDPath); 

  //��SD�������ļ�ϵͳ���ļ�ϵͳ����ʱ���SD����ʼ��
  f_res = f_mount(&fs,(TCHAR const*)SDPath,1);
  printf_fatfs_error(f_res);
  
  ILI9341_GramScan ( 6 );
	/*��ʾSD���ڵ�JPG�ļ�����ȷ��SD����·�����и��ļ���
	���ڹ���Ŀ¼�µġ���Ҫ������SD�������ļ����и��Ƶ�SD���ĸ�Ŀ¼*/
   f_res = f_open(&file,"one_piece.jpg",FA_READ|FA_OPEN_EXISTING);	
   printf_fatfs_error(f_res);     
   if(f_res == FR_OK)
   {
     devid.fp = &file; 
  }
		
		/* �������ռ� */
    /* Allocate a work area for TJpgDec */
    work = (char *)malloc(sizeof(char)*7*1024);
		
    /* Prepare to decompress */
    res = jd_prepare(&jdec, in_func, work, 7*1024, &devid);
    if (res == JDR_OK) {
        /* Ready to dcompress. Image info is available here. */
        printf("Image dimensions: %u by %u. %u bytes used.\n", jdec.width, jdec.height, 7*1024 - jdec.sz_pool);

				/*���õ���������������������*/
        res = jd_decomp(&jdec, out_func, 0);   /* ���벢��ʾͼƬ ��Start to decompress with 1/1 scaling */
       
				if (res == JDR_OK) {
	
            printf("\rOK  \n");				

        } else {
            printf("Failed to decompress: rc=%d\n", res);
        }

    } else {
        printf("Failed to prepare: rc=%d\n", res);
    }

    free(work);             /* Discard work area */

    /* ����ʹ�ã�ȡ������ */
    f_mount(NULL,(TCHAR const*)SDPath,1);	    
}


/**
  * @brief  ��ӡ�����Ϣ
  * @param  ��
  * @retval ��
  */
 void printf_fatfs_error(FRESULT fresult)
{
  switch(fresult)
  {
    case FR_OK:                   
      printf("�������ɹ���\r\n");
    break;
    case FR_DISK_ERR:             
      printf("����Ӳ�����������������\r\n");
    break;
    case FR_INT_ERR:              
      printf("�������Դ���\r\n");
    break;
    case FR_NOT_READY:            
      printf("���������豸�޷�������\r\n");
    break;
    case FR_NO_FILE:              
      printf("�����޷��ҵ��ļ���\r\n");
    break;
    case FR_NO_PATH:             
      printf("�����޷��ҵ�·����\r\n");
    break;
    case FR_INVALID_NAME:         
      printf("������Ч��·������\r\n");
    break;
    case FR_DENIED:               
    case FR_EXIST:                
      printf("�����ܾ����ʡ�\r\n");
    break;
    case FR_INVALID_OBJECT:       
      printf("������Ч���ļ���·����\r\n");
    break;
    case FR_WRITE_PROTECTED:      
      printf("�����߼��豸д������\r\n");
    break;
    case FR_INVALID_DRIVE:        
      printf("������Ч���߼��豸��\r\n");
    break;
    case FR_NOT_ENABLED:          
      printf("������Ч�Ĺ�������\r\n");
    break;
    case FR_NO_FILESYSTEM:        
      printf("������Ч���ļ�ϵͳ��\r\n");
    break;
    case FR_MKFS_ABORTED:         
      printf("���������������⵼��f_mkfs��������ʧ�ܡ�\r\n");
    break;
    case FR_TIMEOUT:              
      printf("����������ʱ��\r\n");
    break;
    case FR_LOCKED:               
      printf("�����ļ���������\r\n");
    break;
    case FR_NOT_ENOUGH_CORE:      
      printf("�������ļ���֧�ֻ�ȡ�ѿռ�ʧ�ܡ�\r\n");
    break;
    case FR_TOO_MANY_OPEN_FILES:  
      printf("������̫���ļ���\r\n");
    break;
    case FR_INVALID_PARAMETER:    
      printf("����������Ч��\r\n");
    break;
  }
}

