/**
  *********************************************************************
  * @file    gui_resource_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   �����ⲿ��Դ�Ľӿ�
  *          �������������ⲿFLASH�е���Դ
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */ 
  
#include <string.h>
#include <stdio.h>
#include	"emXGUI.h"
#include	"emXGUI_Arch.h"
#include "x_libc.h" 
#include "gui_resource_port.h"
#include "gui_mem_port.h"



/*=========================================================================================*/
/*������Դ�豸�Ļ����ź���*/
/*PC��ֱ��ʹ���ļ�ϵͳ�ķ�ʽʵ��RES��Դ�豸�����ݽӿ�*/
static GUI_MUTEX *mutex_lock=NULL;
/* ��Դ�豸ʹ�õ��ļ���� */
FILE *res_fp = NULL;
/**
  * @brief  ��ʼ����Դ�豸���ⲿFLASH��
  * @param  ��
  * @retval �Ƿ��ʼ������
  */
BOOL RES_DevInit(void)
{  
	mutex_lock=GUI_MutexCreate();

	//if(SPI_FLASH_Init() == 0)
 //   return TRUE;
 // else
 //   return FALSE;
	return TRUE;

//	RES_DevTest();
}

/**
  * @brief  ��ȡ�豸ID
  * @param  ��
  * @retval �豸ID
  */
U32 RES_DevGetID(void)
{
	//U32 id;
	//GUI_MutexLock(mutex_lock,5000);

	//id =SPI_FLASH_ReadID();
	//GUI_MutexUnlock(mutex_lock);
	//return id;
	return 1;
}

/**
  * @brief  ���豸д������
  * @param  buf Ҫд�������
  * @param  addr д���Ŀ���ַ
  * @param  size д�����������size��Ӧ����BLOCK��С��
  * @retval �Ƿ�д������
  */
BOOL RES_DevWrite(u8 *buf,u32 addr,u32 size)
{
	//GUI_MutexLock(mutex_lock,5000);
	//SPI_FLASH_SectorErase(addr&0xFFFFF000);
	//SPI_FLASH_BufferWrite(buf,addr,size);
	//GUI_MutexUnlock(mutex_lock);
	return TRUE;
}

/**
  * @brief  ��res_fp�ļ�����ж�ȡ����
  * @param  buf �洢��ȡ��������
  * @param  addr ��ȡ��Ŀ���ַ
  * @param  size ��ȡ��������
  * @retval �Ƿ��ȡ����
  */
BOOL RES_DevRead(u8 *buf,u32 addr,u32 size)
{
	GUI_MutexLock(mutex_lock,5000);

	if (res_fp != NULL)
	{
		fseek(res_fp, addr, SEEK_SET);
		fread((u8*)buf, size, 1, res_fp);
	}

	GUI_MutexUnlock(mutex_lock);
	return TRUE;
}

/**
  * @brief  ��������
  * @param  addr Ҫ������������ַ
  * @retval �������ֽ���
  */
int RES_DevEraseSector(u32 addr)
{
	//GUI_MutexLock(mutex_lock,5000);
	//SPI_FLASH_SectorErase(addr&0xFFFFF000);
	//GUI_MutexUnlock(mutex_lock);
	//return SPI_FLASH_SectorSize;
	return 1;
}

/*=========================================================================================*/
#if 0
/**
  * @brief  ��Դ�豸����
  * @param  ��
  * @retval ��
  */
void RES_DevTest(void)
{
	char *buf;
	volatile u32 id;

	buf =GUI_VMEM_Alloc(4096);
	RES_DevInit();
	id =RES_DevGetID();
  
//  GUI_Printf("0x%x",id);

#if 0  
	while(1)
	{	
    int i = 0;

		RES_DevEraseSector(i);

		memset(buf,i>>12,4096);
		RES_DevWrite(buf,i,4096);

		memset(buf,0,4096);
		RES_DevRead((u8 *)buf,i,4096);

		i += 4096;
		GUI_msleep(100);

	}
#endif  
	GUI_VMEM_Free(buf);
}
#endif


/**
  * @brief  �������ִ���Դ�ļ���res_fpָ��
  * @param  res_name[in] Ҫ���ҵ���Դ����
  * @retval -1��ʾ�Ҳ���������ֵ��ʾres_fp
  */
s32 RES_GetOffset(const char *res_name)
{
	char full_name[100];

	sprintf(full_name,"%s%s",RES_DIR,res_name);

	res_fp = fopen(full_name, "rb+");
	if (res_fp != NULL)
		return (s32)res_fp;
	else
		return -1;
}

/**
  * @brief  ��FLASH�е�Ŀ¼������Ӧ��Դ����Ϣ
  * @param  res_base Ŀ¼��FLASH�еĻ���ַ
  * @param  res_name[in] Ҫ���ҵ���Դ����
  * @param  dir[out] �������Դ��Ϣ
  * @note   �˴�dir.offset�ᱻ��ֵΪres_fp
  * @retval -1��ʾ�Ҳ���������ֵ��ʾres_fp
  */
s32 RES_GetInfo_AbsAddr(const char *res_name, CatalogTypeDef *dir)
{
	char full_name[100];

	x_sprintf(full_name, "%s%s", RES_DIR, res_name);

	res_fp = fopen(full_name, "rb+");
	if (res_fp != NULL)
	{
		int fsize;

		x_strcpy(dir->name, res_name);
		fseek(res_fp, 0L, SEEK_END);
    	fsize = ftell(res_fp);
		fseek(res_fp, 0L, SEEK_SET);
		dir->size = fsize;
		dir->offset = (u32)res_fp;
			
		return (s32)res_fp;
	}
	else
		return -1;
}


/**
  * @brief  ����Դ�豸��������
  * @param  file_name[in]: �ļ���
  * @param  buf[out]�����غ�õ��Ļ�����
  * @param  size[out]�����ݵĴ�С
  * @note   buf�Ǹ������ݵĴ�С��̬��VMEM������ģ�
  *         ʹ�����buf����Ҫ����h�ļ��е�Release_Content�����ͷ�buf�Ŀռ�
  * @retval FALSE:ʧ��; TRUE:�ɹ�
*/
BOOL RES_Load_Content(char *file_name, char** buf, u32* size) 
{	 
    int content_offset;
    CatalogTypeDef dir;
    BOOL result = TRUE;
  
    content_offset = RES_GetInfo_AbsAddr(file_name, &dir);
    if(content_offset > 0)
    {    
      /* �ļ����ݿռ� */
      *buf = (char *)GUI_VMEM_Alloc(dir.size);
      if(*buf != NULL)
      {
        /* ��������*/
        RES_DevRead((u8 *)*buf,0,dir.size); 
          
        *size = dir.size;
      }
      else
        result = FALSE;
    }
    else 
      result = FALSE;
   
    return result;
}

#if(GUI_RES_FS_EN)
/**
  * @brief  ���ļ�ϵͳ��������
  * @param  file_name[in]: �ļ�·��
  * @param  buf[out]�����غ�õ��Ļ�����
  * @param  size[out]�����ݵĴ�С
  * @note   buf�Ǹ������ݵĴ�С��̬��VMEM������ģ�
  *         ʹ�����buf����Ҫ����h�ļ��е�Release_Content�����ͷ�buf�Ŀռ�
  * @retval FALSE:ʧ��; TRUE:�ɹ�
*/
BOOL FS_Load_Content(char *file_name, char** buf, u32* size) 
{
	/* file objects */
	FILE     *fp;
	UINT br;

	/* ���ļ� */
	fp = fopen( file_name, "rb+");
	if (fp == NULL)
		return FALSE;

	fseek(fp, 0L, SEEK_END);
	*size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	/* �ļ����ݿռ� */
	*buf = (char *)GUI_VMEM_Alloc(*size);
	if (*buf != NULL)
	{
		/* ��������ͼƬ�ļ� */
		fread((u8*)*buf, *size  ,1,fp);
		/* �ر��ļ� */
		fclose(fp);
	}
	else
		return FALSE;

	return TRUE;
}
#endif




/********************************END OF FILE****************************/

