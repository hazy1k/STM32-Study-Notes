#ifndef __GUI_RESOURCE_PORT_H
#define	__GUI_RESOURCE_PORT_H

#include	"emXGUI.h"
#include	"gui_drv_cfg.h"
#include	<stdio.h>

#ifdef __cplusplus
extern "C" {
#endif  

/* 资源设备使用的文件句柄 */
extern	FILE *res_fp;

/* 目录信息类型 */
typedef struct 
{
	char 	name[24];  /* 资源的名字 */
	u32  	size;      /* 资源的大小 */ 
	u32 	offset;    /* 资源相对于基地址的偏移 */
}CatalogTypeDef;


BOOL RES_DevInit(void);
U32 RES_DevGetID(void);
BOOL RES_DevWrite(u8 *buf,u32 addr,u32 size);
BOOL RES_DevRead(u8 *buf,u32 addr,u32 size);
int RES_DevEraseSector(u32 addr);
void RES_DevTest(void);

s32 RES_GetOffset(const char *res_name);
s32 RES_GetInfo_AbsAddr(const char *res_name, CatalogTypeDef *dir);

BOOL FS_Load_Content(char *file_name, char** buf, u32* size); 
BOOL RES_Load_Content(char *file_name, char** buf,u32* size);


/**
 * @brief  关闭res_fp打开的文件
 * @retval 无
 */
static __inline void RES_Release_fp(void)
{
	if (res_fp)
		fclose(res_fp);
}
 /**
  * @brief  释放动态申请的buf空间,并关闭res_fp
  * @retval 无
  */
static __inline void RES_Release_Content(char **buf)
{
	if(res_fp)
		fclose(res_fp);
    /* 释放空间 */
    GUI_VMEM_Free(*buf);
}

#ifdef	__cplusplus
}
#endif
#endif /* __GUI_RESOURCE_PORT_H */
