/**
  *********************************************************************
  * @file    gui_resource_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   管理外部资源的接口
  *          例如用来管理外部FLASH中的资源
  *********************************************************************
  * @attention
  * 官网    :www.emXGUI.com
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
/*访问资源设备的互斥信号量*/
/*PC上直接使用文件系统的方式实现RES资源设备，兼容接口*/
static GUI_MUTEX *mutex_lock=NULL;
/* 资源设备使用的文件句柄 */
FILE *res_fp = NULL;
/**
  * @brief  初始化资源设备（外部FLASH）
  * @param  无
  * @retval 是否初始化正常
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
  * @brief  读取设备ID
  * @param  无
  * @retval 设备ID
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
  * @brief  向设备写入内容
  * @param  buf 要写入的内容
  * @param  addr 写入的目标地址
  * @param  size 写入的数据量（size不应超过BLOCK大小）
  * @retval 是否写入正常
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
  * @brief  从res_fp文件句柄中读取内容
  * @param  buf 存储读取到的内容
  * @param  addr 读取的目标地址
  * @param  size 读取的数据量
  * @retval 是否读取正常
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
  * @brief  擦除扇区
  * @param  addr 要擦除的扇区地址
  * @retval 扇区的字节数
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
  * @brief  资源设备测试
  * @param  无
  * @retval 无
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
  * @brief  根据名字打开资源文件到res_fp指针
  * @param  res_name[in] 要查找的资源名字
  * @retval -1表示找不到，其余值表示res_fp
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
  * @brief  从FLASH中的目录查找相应资源的信息
  * @param  res_base 目录在FLASH中的基地址
  * @param  res_name[in] 要查找的资源名字
  * @param  dir[out] 输出的资源信息
  * @note   此处dir.offset会被赋值为res_fp
  * @retval -1表示找不到，其余值表示res_fp
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
  * @brief  从资源设备加载内容
  * @param  file_name[in]: 文件名
  * @param  buf[out]：加载后得到的缓冲区
  * @param  size[out]：内容的大小
  * @note   buf是根据内容的大小动态从VMEM中申请的，
  *         使用完毕buf后，需要调用h文件中的Release_Content函数释放buf的空间
  * @retval FALSE:失败; TRUE:成功
*/
BOOL RES_Load_Content(char *file_name, char** buf, u32* size) 
{	 
    int content_offset;
    CatalogTypeDef dir;
    BOOL result = TRUE;
  
    content_offset = RES_GetInfo_AbsAddr(file_name, &dir);
    if(content_offset > 0)
    {    
      /* 文件内容空间 */
      *buf = (char *)GUI_VMEM_Alloc(dir.size);
      if(*buf != NULL)
      {
        /* 加载数据*/
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
  * @brief  从文件系统加载内容
  * @param  file_name[in]: 文件路径
  * @param  buf[out]：加载后得到的缓冲区
  * @param  size[out]：内容的大小
  * @note   buf是根据内容的大小动态从VMEM中申请的，
  *         使用完毕buf后，需要调用h文件中的Release_Content函数释放buf的空间
  * @retval FALSE:失败; TRUE:成功
*/
BOOL FS_Load_Content(char *file_name, char** buf, u32* size) 
{
	/* file objects */
	FILE     *fp;
	UINT br;

	/* 打开文件 */
	fp = fopen( file_name, "rb+");
	if (fp == NULL)
		return FALSE;

	fseek(fp, 0L, SEEK_END);
	*size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	/* 文件内容空间 */
	*buf = (char *)GUI_VMEM_Alloc(*size);
	if (*buf != NULL)
	{
		/* 加载整个图片文件 */
		fread((u8*)*buf, *size  ,1,fp);
		/* 关闭文件 */
		fclose(fp);
	}
	else
		return FALSE;

	return TRUE;
}
#endif




/********************************END OF FILE****************************/

