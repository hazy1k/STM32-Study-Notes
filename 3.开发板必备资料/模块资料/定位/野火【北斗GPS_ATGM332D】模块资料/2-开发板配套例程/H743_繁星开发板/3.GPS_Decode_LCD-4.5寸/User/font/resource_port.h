#ifndef __GUI_RESOURCE_PORT_H
#define	__GUI_RESOURCE_PORT_H

#include "stm32h7xx.h"

#ifdef __cplusplus
extern "C" {
#endif  

typedef int         BOOL;

/* ��Դ���ڵĻ���ַ */
#define GUI_RES_BASE             4096

/* �洢��FLASH�е���ԴĿ¼��С */
#define GUI_CATALOG_SIZE         (8*1024)
  
/* Ŀ¼��Ϣ���� */
typedef struct 
{
	char 	name[40];  /* ��Դ������ */
	uint32_t  	size;      /* ��Դ�Ĵ�С */ 
	uint32_t 	offset;    /* ��Դ����ڻ���ַ��ƫ�� */
}CatalogTypeDef;


/* ���ڱ���Ƿ�����Դ�ļ��޷��ҵ� */
extern BOOL res_not_found_flag;

BOOL RES_DevRead(uint8_t *buf,uint32_t addr,uint32_t size);
int RES_DevEraseSector(uint32_t addr);
void RES_DevTest(void);

int RES_GetOffset(const char *res_name);
int RES_GetInfo_AbsAddr(const char *res_name, CatalogTypeDef *dir);

#ifdef	__cplusplus
}
#endif
#endif /* __GUI_RESOURCE_PORT_H */
