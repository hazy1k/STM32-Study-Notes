#ifndef INC_FREERTOS_H
#define INC_FREERTOS_H

#include "FreeRTOSConfig.h"
#include "projdefs.h"
#include "portable.h"
#include "list.h"


//#if 0
//typedef struct xSTATIC_TCB
//{
//	void				*pxDummy1;      /* ����ջ�� */

//	StaticListItem_t	xDummy3[ 2 ];   /* �����б�ڵ� */
//	UBaseType_t			uxDummy5;       /* �������ȼ���0��ʾ������ȼ� */
//	void				*pxDummy6;      /* ����ջ��ʼ��ַ */
//	                                    /* �������ƣ��ַ�����ʽ */
//	uint8_t				ucDummy7[ configMAX_TASK_NAME_LEN ];                                    	

//} StaticTask_t;
//#endif 

//struct xSTATIC_LIST_ITEM
//{
//	TickType_t xDummy1;
//	void *pvDummy2[ 4 ];
//};
//typedef struct xSTATIC_LIST_ITEM StaticListItem_t;

//typedef struct xSTATIC_TCB
//{
//	void				*pxDummy1;      /* ����ջ�� */
//	StaticListItem_t	xDummy3[ 2 ];   /* �����б�ڵ� */
//    
//	void				*pxDummy6;      /* ����ջ��ʼ��ַ */
//	                                    /* �������ƣ��ַ�����ʽ */
//	uint8_t				ucDummy7[ configMAX_TASK_NAME_LEN ];                                    	

//} StaticTask_t;


typedef struct tskTaskControlBlock
{
	volatile StackType_t    *pxTopOfStack;    /* ջ�� */

	ListItem_t			    xStateListItem;   /* ����ڵ� */
    
    StackType_t             *pxStack;         /* ����ջ��ʼ��ַ */
	                                          /* �������ƣ��ַ�����ʽ */
	char                    pcTaskName[ configMAX_TASK_NAME_LEN ];

    TickType_t xTicksToDelay;
    UBaseType_t			uxPriority;    
} tskTCB;
typedef tskTCB TCB_t;

















#endif /* INC_FREERTOS_H */
