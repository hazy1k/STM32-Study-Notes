/**
  *********************************************************************
  * @file    resource_port.c
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
  
#include "stm32h7xx.h"
#include <string.h>
#include "./font/resource_port.h"
#include "./flash/bsp_spi_flash.h"

#if defined(CPU_MIMXRT1052DVL6B)
#define QSPIFLASH_ADDR 0x60000000         //�ⲿFLASH��ӳ���ַ
#endif
/*=========================================================================================*/
/*������Դ�豸�Ļ����ź���*/

uint8_t buf[100];

/**
  * @brief  ���豸�ж�ȡ����
  * @param  buf �洢��ȡ��������
  * @param  addr ��ȡ��Ŀ���ַ
  * @param  size ��ȡ��������
  * @retval �Ƿ��ȡ����
  */
BOOL RES_DevRead(uint8_t *buf,uint32_t addr,uint32_t size)
{

	SPI_FLASH_BufferRead(buf,addr,size);

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
	volatile uint32_t id;

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
		RES_DevRead((uint8_t *)buf,i,4096);

		i += 4096;
		GUI_msleep(100);

	}
#endif  
	GUI_VMEM_Free(buf);
}
#endif

#if 0
#define countof(a)      (sizeof(a) / sizeof(*(a)))
#define  BufferSize     (countof(Tx_Buffer123)-1)
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;
uint8_t Tx_Buffer[] = "��stm32������\r\nhttp://firestm32.taobao.c��л��ѡ��Ұ��stm32������\r\nhttp://firestm32.taobao.com��л��ѡ��Ұ��stm32������\r\nhttp://firestm32.taobao.com��л��ѡ��Ұ��stm32������\r\nhttp://firestm32.taobao.com��л��ѡ��Ұ��stm32������\r\nhttp://firestm32.taobao.com";
uint8_t Rx_Buffer[BufferSize];
#define  FLASH_WriteAddress     0x00000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress
__IO TestStatus TransferStatus1 = FAILED;

TestStatus Buffercmp1(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return PASSED;
}
CatalogTypeDef dir;
int font_base; 
void RES_DevTest(void)
{
  uint8_t *Rx_Buffer = (uint8_t*)GUI_VMEM_Alloc(BufferSize);
  //BSP_QSPI_Erase_Chip();
//  HAL_InitTick(5);
  BSP_QSPI_Write(Tx_Buffer123, FLASH_WriteAddress, BufferSize);
  HAL_InitTick(5);
  BSP_QSPI_Read(Rx_Buffer, FLASH_ReadAddress, BufferSize);
  TransferStatus1 = Buffercmp1(Tx_Buffer123, Rx_Buffer, BufferSize);
  if( PASSED == TransferStatus1 )
  {    
    LED_GREEN;
    GUI_DEBUG("\r\n16M����flash(W25Q256)���Գɹ�!\n\r");
  }
  else
  {        
    LED_RED;
    GUI_DEBUG("\r\n16M����flash(W25Q256)����ʧ��!\n\r");
  }
  GUI_VMEM_Free(Rx_Buffer);  
}
#endif

/**
  * @brief  ��FLASH�е�Ŀ¼������Ӧ����Դλ��
  * @param  res_base Ŀ¼��FLASH�еĻ���ַ
  * @param  res_name[in] Ҫ���ҵ���Դ����
  * @retval -1��ʾ�Ҳ���������ֵ��ʾ��Դ��FLASH�еĻ���ַ
  */
int RES_GetOffset(const char *res_name)
{
	int i,len;
	CatalogTypeDef dir;

	len =strlen(res_name);
	for(i=0;i<GUI_CATALOG_SIZE;i+=sizeof(CatalogTypeDef))
	{
		RES_DevRead((uint8_t*)&dir,GUI_RES_BASE+i,sizeof(CatalogTypeDef));

		if(strncasecmp(dir.name,res_name,len)==0)
		{
      /* dir.offset����Ի���ַ��ƫ�ƣ��˴����ؾ��Ե�ַ */
			return dir.offset + GUI_RES_BASE;
		}
	}

	return -1;
}

/**
  * @brief  ��FLASH�е�Ŀ¼������Ӧ��Դ����Ϣ
  * @param  res_base Ŀ¼��FLASH�еĻ���ַ
  * @param  res_name[in] Ҫ���ҵ���Դ����
  * @param  dir[out] Ҫ���ҵ���Դ����
  * @note   �˴�dir.offset�ᱻ��ֵΪ��Դ�ľ��Ե�ַ����
  * @retval -1��ʾ�Ҳ���������ֵ��ʾ��Դ��FLASH�еľ��Ե�ַ
  */
int RES_GetInfo_AbsAddr(const char *res_name, CatalogTypeDef *dir)
{
	int i,len;
  
	len =strlen(res_name);
  /* �������ֱ���Ŀ¼ */
	for(i=0;i<GUI_CATALOG_SIZE;i+=sizeof(CatalogTypeDef))
	{
		RES_DevRead((uint8_t*)dir,GUI_RES_BASE+i,sizeof(CatalogTypeDef));

		if(strncasecmp(dir->name,res_name,len)==0)
		{
      /* dir.offset����Ի���ַ��ƫ�ƣ��˴����ؾ��Ե�ַ */
      dir->offset += GUI_RES_BASE;
			return dir->offset ;
		}
	}
	return -1;
}

/********************************END OF FILE****************************/

