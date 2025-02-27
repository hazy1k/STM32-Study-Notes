
 
#include <string.h>
#include <stdlib.h>

#include "./flash/bsp_spi_flash.h"
#include "RES_MGR.h"



/*=========================================================================================*/

/**
  * @brief  从FLASH中的目录查找相应的资源位置
  * @param  res_base 目录在FLASH中的基地址
  * @param  res_name[in] 要查找的资源名字
  * @retval -1表示找不到，其余值表示资源在FLASH中的基地址
  */
int GetResOffset(const char *res_name)
{
  
	int i,len;
	CatalogTypeDef dir;

	len =strlen(res_name);
	for(i=0;i<CATALOG_SIZE;i+=sizeof(CatalogTypeDef))
	{
		SPI_FLASH_BufferRead((u8*)&dir,RESOURCE_BASE_ADDR+i,sizeof(CatalogTypeDef));
    
		if(strncasecmp(dir.name,res_name,len)==0)
		{
			return dir.offset;
		}
	}

	return -1;
}

/*********************************************END OF FILE**********************/
