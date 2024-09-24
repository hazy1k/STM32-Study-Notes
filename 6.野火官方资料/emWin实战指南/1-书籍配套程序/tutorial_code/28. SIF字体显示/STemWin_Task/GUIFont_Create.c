/**
  ******************************************************************************
  * @file    GUIFont_Create.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   XBF��ʽ����emwin�����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
	
#include "./flash/bsp_spi_flash.h"
#include "GUIFont_Create.h"
#include "ff.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"

#include <string.h>
#include <stdlib.h>


/* �ֿ�ṹ�� */
GUI_FONT     	FONT_SIYUANHEITI_20_4BPP;
GUI_FONT     	FONT_XINSONGTI_16_4BPP;

/* �ֿ⻺���� */
uint8_t *SIFbuffer20;
uint8_t *SIFbuffer16;

/* �ֿ�洢·�� */
#if (SIF_FONT_SOURCE == USE_FLASH_FONT)

	/* ��Դ��¼����FLASH����ַ��Ŀ¼��ַ�� */
	#define RESOURCE_BASE_ADDR	(16*1024*1024)
	/* �洢��FLASH�е���ԴĿ¼��С */
	#define CATALOG_SIZE				(8*1024)

	/* �ֿ�Ŀ¼��Ϣ���� */
	typedef struct 
	{
		char 	      name[40];  /* ��Դ������ */
		uint32_t  	size;      /* ��Դ�Ĵ�С */ 
		uint32_t 	  offset;    /* ��Դ����ڻ���ַ��ƫ�� */
	}CatalogTypeDef;
	
	CatalogTypeDef Catalog;
	
	static const char  FONT_XINSONGTI_18_ADDR[]	 = "xinsongti18_4bpp.sif";
	static const char  FONT_SIYUANHEITI_36_ADDR[]	 = "siyuanheit36_4bpp.sif";

#elif (SIF_FONT_SOURCE == USE_SDCARD_FONT)

	static const char FONT_STORAGE_ROOT_DIR[]  =   "0:";
	static const char FONT_XINSONGTI_16_ADDR[] = 	 "0:/Font/16_4bpp.sif";
	static const char FONT_SIYUANHEITI_20_ADDR[] = 	 "0:/Font/20_4bpp.sif";

#endif

/* �洢����ʼ����־ */
static uint8_t storage_init_flag = 0;

/* �ֿ�洢���ļ�ϵͳʱ��Ҫʹ�õı��� */
#if (SIF_FONT_SOURCE == USE_SDCARD_FONT)
	static FIL fnew;									  /* file objects */
	static FATFS fs;									  /* Work area (file system object) for logical drives */
	static FRESULT res;
	static UINT br;            			    /* File R/W count */
#endif

#if (SIF_FONT_SOURCE == USE_FLASH_FONT)
/**
  * @brief  ��FLASH�е�Ŀ¼������Ӧ����Դλ��
  * @param  res_base Ŀ¼��FLASH�еĻ���ַ
  * @param  res_name[in] Ҫ���ҵ���Դ����
  * @retval -1��ʾ�Ҳ���������ֵ��ʾ��Դ��FLASH�еĻ���ַ
  */
int GetResOffset(const char *res_name)
{
	int i,len;

	len =strlen(res_name);
	for(i=0;i<CATALOG_SIZE;i+=sizeof(CatalogTypeDef))
	{
		taskENTER_CRITICAL();//�����ٽ���
		SPI_FLASH_BufferRead((uint8_t*)&Catalog,RESOURCE_BASE_ADDR+i,sizeof(CatalogTypeDef));
    taskEXIT_CRITICAL();//�˳��ٽ���
		
		if(strncasecmp(Catalog.name,res_name,len)==0)
		{
			return Catalog.offset;
		}
	}
	return -1;
}
#endif

/**
  * @brief  �����������ݵ�SDRAM
  * @note ��
  * @param  res_name��Ҫ���ص��ֿ��ļ���
  * @retval Fontbuffer���Ѽ��غõ��ֿ�����
  */
void *FONT_SIF_GetData(const char *res_name)
{
	uint8_t *Fontbuffer;
	GUI_HMEM hFontMem;
	
#if (SIF_FONT_SOURCE == USE_FLASH_FONT)
	
	int32_t offset;
	
	if (storage_init_flag == 0)
	{
		/* ��ʼ��SPI FLASH */
		SPI_FLASH_Init();
		storage_init_flag = 1;
	}
	
	/* ��ȡ�ֿ��ļ���FLASH�е�ƫ���� */
	offset = GetResOffset(res_name);
	if(offset == -1)
	{
		printf("�޷���FLASH���ҵ��ֿ��ļ�\r\n");
		while(1);
	}
	
	/* ����һ�鶯̬�ڴ�ռ� */
	hFontMem = GUI_ALLOC_AllocZero(Catalog.size);
	/* ת����̬�ڴ�ľ��Ϊָ�� */
	Fontbuffer = GUI_ALLOC_h2p(hFontMem);
	
	/* ��ȡ���� */
	SPI_FLASH_BufferRead(Fontbuffer, RESOURCE_BASE_ADDR+offset, Catalog.size);
	
	return Fontbuffer; 
	
#elif (SIF_FONT_SOURCE == USE_SDCARD_FONT)
	
	if (storage_init_flag == 0)
	{
		/* ����sd���ļ�ϵͳ */
		res = f_mount(&fs,FONT_STORAGE_ROOT_DIR,1);
		storage_init_flag = 1;
	}
	
	/* ���ֿ� */
	res = f_open(&fnew , res_name, FA_OPEN_EXISTING | FA_READ);
	if(res != FR_OK)
	{
		printf("Open font failed! res = %d\r\n", res);
		while(1);
	}
	
	/* ����һ�鶯̬�ڴ�ռ� */
	hFontMem = GUI_ALLOC_AllocZero(fnew.fsize);
	/* ת����̬�ڴ�ľ��Ϊָ�� */
	Fontbuffer = GUI_ALLOC_h2p(hFontMem);

	/* ��ȡ���� */
	res = f_read(&fnew, Fontbuffer, fnew.fsize, &br);
	if(res != FR_OK)
	{
		printf("Read font failed! res = %d\r\n", res);
		while(1);
	}
	f_close(&fnew);
	
	return Fontbuffer;  
#endif
}

/**
  * @brief  ����SIF����
  * @param  ��
  * @retval ��
  */
void Create_SIF_Font(void) 
{
	/* ��ȡ�������� */
	SIFbuffer16 = FONT_SIF_GetData(FONT_XINSONGTI_16_ADDR);
	SIFbuffer20 = FONT_SIF_GetData(FONT_SIYUANHEITI_20_ADDR);
	
	/* ������18 */
	GUI_SIF_CreateFont(SIFbuffer16,               /* �Ѽ��ص��ڴ��е��������� */
	                   &FONT_XINSONGTI_16_4BPP,   /* GUI_FONT ����ṹ��ָ�� */
										 GUI_SIF_TYPE_PROP_AA4_EXT);/* �������� */
	/* ˼Դ����36 */
	GUI_SIF_CreateFont(SIFbuffer20,               /* �Ѽ��ص��ڴ��е��������� */
	                   &FONT_SIYUANHEITI_20_4BPP, /* GUI_FONT ����ṹ��ָ�� */
										 GUI_SIF_TYPE_PROP_AA4_EXT);/* �������� */

}

/*********************************************END OF FILE**********************/
