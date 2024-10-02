/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ledӦ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F429 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
 
#include <string.h>
#include <stdlib.h>

#include "./flash/bsp_qspi_flash.h"
#include "RES_MGR.h"



/*=========================================================================================*/

/**
  * @brief  ��FLASH�е�Ŀ¼������Ӧ����Դλ��
  * @param  res_base Ŀ¼��FLASH�еĻ���ַ
  * @param  res_name[in] Ҫ���ҵ���Դ����
  * @retval -1��ʾ�Ҳ���������ֵ��ʾ��Դ��FLASH�еĻ���ַ
  */
int GetResOffset(const char *res_name)
{
  
	int i,len;
	CatalogTypeDef dir;

	len =strlen(res_name);
	for(i=0;i<CATALOG_SIZE;i+=sizeof(CatalogTypeDef))
	{
		BSP_QSPI_FastRead((uint8_t*)&dir,RESOURCE_BASE_ADDR+i,sizeof(CatalogTypeDef));
    
		if(strncasecmp(dir.name,res_name,len)==0)
		{
			return dir.offset;
		}
	}

	return -1;
}

/*********************************************END OF FILE**********************/
