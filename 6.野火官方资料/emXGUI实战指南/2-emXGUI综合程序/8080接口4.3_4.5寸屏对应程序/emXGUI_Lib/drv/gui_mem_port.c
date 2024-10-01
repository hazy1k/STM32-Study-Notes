/**
  *********************************************************************
  * @file    gui_mem_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ��̬�ڴ����ӿ�
  *          
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */ 
#include "x_libc.h" // x_libc��ṩ��ͨ�õ��ڴ�ѹ�����(x_heap)...
#include "board.h"
#include "gui_drv.h"
#include	"emXGUI_Arch.h"
#include "gui_mem_port.h"


/**
* @note  vmem/vmalloc/vfree �ṩ��Ӧ�ó�������ʹ�ã�
*        ͨ��ʹ��һ��ϴ���ⲿSRAM��SDRAM�ռ���з��䡣
*        
*        GUI_MEM_Init/Alloc/Free �ṩ��GUI�ں˶���ʹ�ã�
*        ͨ��ʹ�ÿ��������ڲ�SRAM���з��䡣
*
*        GUI_GRAM_Alloc/Free ������Һ���Դ�ռ��ʱ��ʹ�ã�
*        �������Դ�ռ�̶�������û���õ�
*
*/

#if(GUI_VMEM_EN)    
/* VMEM�ڴ���� */
/* �����ź��� */
static GUI_MUTEX *mutex_vmem = NULL;

/* �ڴ�ѹ����� */
heap_t heap_vmem;

/* VMEM������ */
static uint8_t buff_vmem[VMEM_SIZE]    __attribute__((at(VMEM_BASE)));
#endif

/* GUI_CORE�ڴ���� */
//static GUI_MUTEX *mutex_core_mem = NULL;
//static	heap_t heap_core_mem;
//static uint8_t buff_core_mem[GUI_CORE_MEM_SIZE] __attribute__((at(GUI_CORE_MEM_BASE)));


/**
  * @brief  ����һ���ڴ��
  * @note  ʹ��vmallocǰ������ñ�������ʼ���ڴ�Ѿ��
  * @retval ��
  */
void GUI_VMEM_Init(void)
{
#if(GUI_VMEM_EN)      
	mutex_vmem = GUI_MutexCreate();
	x_heap_init(&heap_vmem,
                (void*)buff_vmem,
                  VMEM_SIZE,
                  VMEM_ALLOC_UNIT);	 /* ����һ���ڴ�� */
#endif
}
  
/**
* @brief  ���ڴ��������ռ�
* @param  size Ҫ������ڴ��С
* @retval ���뵽���ڴ�ָ��
*/
void* GUI_VMEM_Alloc(u32 size)
{
	void *p=NULL;
  printf("VMEM - ʣ�ࣺ%d�ֽ�\n��Ҫ��%d�ֽ�\n", xPortGetFreeHeapSize(), size);
	p =OS_MALLOC(size);
	if(p==NULL)
	{
	  GUI_ERROR("GUI_VMEM_Alloc Failed.\r\n");
		while(1);
	}

	return p;
#if(GUI_VMEM_EN)      

	u8 *p;
	
	GUI_MutexLock(mutex_vmem,5000);
	p =x_heap_alloc(&heap_vmem,size);
	GUI_MutexUnlock(mutex_vmem);
  if(p==NULL)
	{
	    GUI_ERROR("GUI_VMEM_Alloc,no enough space(for %d byte, residue %d byte)", size, heap_vmem.total_size - heap_vmem.used_cur);
	}
  
	return p;
#endif
}
  
/**
* @brief  �ͷ��ڴ�
* @param  p:��Ҫ�ͷŵ��ڴ���ַ
* @retval ��
*/
void GUI_VMEM_Free(void *p)
{
	OS_FREE(p);
#if(GUI_VMEM_EN)      
	GUI_MutexLock(mutex_vmem,5000);
	x_heap_free(&heap_vmem,p);
	GUI_MutexUnlock(mutex_vmem);
#endif

}

/**
  * @brief  ����һ���ڴ�ѣ�����GUI�ں˶���,�ɲο�vmem����
  * @retval ��
  */
void	GUI_MEM_Init(void)
{
  /* ��ʾ���е�GUI�ں˶���ʹ�� pvPortMalloc ��
    ������FreeRTOSϵͳ��ʼ��*/

	return ;
}

/* OS_MALLOC�� OS_FREE��gui_os_port.h�ж���*/
/**
* @brief  ��̬�ڴ�����(����GUI�ں˶���)
* @param  size Ҫ������ڴ��С
* @retval ���뵽���ڴ�ָ��
*/
void*	GUI_MEM_Alloc(U32 size)
{
	void *p=NULL;
	p =OS_MALLOC(size);
	if(p==NULL)
	{
		printf("MEM - ʣ�ࣺ%d�ֽ�\n��Ҫ��%d�ֽ�\n", xPortGetFreeHeapSize(), size);
	  GUI_ERROR("GUI_MEM_Alloc Failed!\r\n.");
		while(1);
	}

	return p;
}

/*===================================================================================*/

/**
* @brief  �ͷ��ڴ�(����GUI�ں˶���)
* @param  p:��Ҫ�ͷŵ��ڴ���ַ
* @retval ��
*/
void	GUI_MEM_Free(void *p)
{
	OS_FREE(p);
//  printf("�ͷź�ʣ�ࣺ%d�ֽ�\n",xPortGetFreeHeapSize());
}

/*===================================================================================*/

/**
* @brief  ��ʾ��̬�ڴ�����(����GUI��ʾ������)
* @param  size Ҫ������ڴ��С
* @retval ���뵽���ڴ�ָ��
*/
void*	GUI_GRAM_Alloc(U32 size)
{
  
	return GUI_VMEM_Alloc(size);
}

/*===================================================================================*/

/**
* @brief  ��ʾ��̬�ڴ�����(����GUI��ʾ������)
* @param  p:��Ҫ�ͷŵ��ڴ���ַ
* @retval ��
*/
void	GUI_GRAM_Free(void *p)
{
	GUI_VMEM_Free(p);
}


void* vmalloc(u32 size)
{
	return GUI_VMEM_Alloc(size);
}

void vfree(void *p)
{
	GUI_VMEM_Free(p);
}

/********************************END OF FILE****************************/

