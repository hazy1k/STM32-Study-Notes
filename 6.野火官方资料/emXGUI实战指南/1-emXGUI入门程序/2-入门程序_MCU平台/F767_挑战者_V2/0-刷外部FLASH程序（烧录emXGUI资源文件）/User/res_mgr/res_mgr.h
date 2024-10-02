
#ifndef __RES_MGR__
#define __RES_MGR__

#ifdef	__cplusplus
extern "C"{
#endif

#include "stm32f7xx.h"
#include "ff.h"
  
//SD����flash�ĸ�Ŀ¼
#define SD_ROOT       "0:"
#define FLASH_ROOT    "1:"

/* ��Դ��SD���е�·�� */
#define RESOURCE_DIR         "0:/srcdata"
/* ��Դ��¼����FLASH����ַ��Ŀ¼��ַ�� */
#define RESOURCE_BASE_ADDR    (16*1024*1024)
/* �洢��FLASH�е���ԴĿ¼��С */
#define CATALOG_SIZE           (8*1024)
  
  
/* ���ɵ���¼��Ϣ�ļ��� */
#define BURN_INFO_NAME        "burn_info.txt"
/* ������¼��Ϣ�ļ���*/  
#define BURN_INFO_NAME_FULL   (RESOURCE_DIR "/" BURN_INFO_NAME)
/* ��¼���Ե��ļ��� */
#define IGNORE_NAME           "ignore.txt"  
/* ���������ļ���*/    
#define IGNORE_NAME_FULL      (RESOURCE_DIR "/" IGNORE_NAME)

/*=========================================================================================*/
/* Ŀ¼��Ϣ���� */
typedef struct 
{
	char 	      name[40];  /* ��Դ������ */
	uint32_t  	size;      /* ��Դ�Ĵ�С */ 
	uint32_t 	  offset;    /* ��Դ����ڻ���ַ��ƫ�� */
}CatalogTypeDef;

/*=========================================================================================*/

/*��Ϣ���*/
#define BURN_DEBUG_ON         1
#define BURN_DEBUG_FUNC_ON    1
#define BURN_DEBUG_ARRAY_ON    1

#define BURN_INFO(fmt,arg...)           printf("<<-BURN-INFO->> "fmt"\n",##arg)
#define BURN_ERROR(fmt,arg...)          printf("<<-BURN-ERROR->> "fmt"\n",##arg)
#define BURN_DEBUG(fmt,arg...)          do{\
                                          if(BURN_DEBUG_ON)\
                                          printf("<<-BURN-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

#define BURN_DEBUG_ARRAY(array, num)    do{\
                                         int32_t i;\
                                         uint8_t* a = array;\
                                         if(BURN_DEBUG_ARRAY_ON)\
                                         {\
                                            printf("<<-BURN-DEBUG-ARRAY->>\n");\
                                            for (i = 0; i < (num); i++)\
                                            {\
                                                printf("%02x   ", (a)[i]);\
                                                if ((i + 1 ) %10 == 0)\
                                                {\
                                                    printf("\n");\
                                                }\
                                            }\
                                            printf("\n");\
                                        }\
                                       }while(0)                                          

#define BURN_DEBUG_FUNC()               do{\
                                         if(BURN_DEBUG_FUNC_ON)\
                                         printf("<<-BURN-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)
/*=========================================================================================*/

int GetResOffset(const char *res_name);
FRESULT Make_Catalog (char* path, uint8_t clear)  ;
uint8_t Read_CatalogInfo( uint32_t file_index,
                            CatalogTypeDef *dir,
                            char *full_name);
void Burn_Catalog(void);
FRESULT Burn_Content(void);   
FRESULT Check_Resource(void);
                            

/*=========================================================================================*/

#ifdef	__cplusplus
}
#endif

#endif /*__RES_MGR__*/

