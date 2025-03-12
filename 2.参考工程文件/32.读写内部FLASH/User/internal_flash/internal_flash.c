#include "internal_flash.h"   
#include "usart.h"

// ���ڲ�FLASH���ж�д����
int InternalFlash_Test(void)
{
	uint32_t EraseCounter = 0x00; // ��¼Ҫ��������ҳ
	uint32_t Address = 0x00;			// ��¼д��ĵ�ַ
	uint32_t Data = 0x3210ABCD;		// ��¼д�������
	uint32_t NbrOfPage = 0x00;		// ��¼д�����ҳ
	FLASH_Status FLASHStatus = FLASH_COMPLETE; // ��¼ÿ�β����Ľ��	
	TestStatus MemoryProgramStatus = PASSED;   // ��¼�������Խ��

  FLASH_Unlock(); // ����FLASH
  /* ����Ҫ��������ҳ */
  NbrOfPage = (WRITE_END_ADDR - WRITE_START_ADDR) / FLASH_PAGE_SIZE;
  /* ������б�־λ */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
  /* ��ҳ����*/
  for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
  {
    FLASHStatus = FLASH_ErasePage(WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));
	}
  /* ���ڲ�FLASHд������ */
  Address = WRITE_START_ADDR;
  while((Address < WRITE_END_ADDR) && (FLASHStatus == FLASH_COMPLETE))
  {
    FLASHStatus = FLASH_ProgramWord(Address, Data);
    Address = Address + 4;
  }
  FLASH_Lock(); // ����FLASH
  /* ���д��������Ƿ���ȷ */
  Address = WRITE_START_ADDR;
  while((Address < WRITE_END_ADDR) && (MemoryProgramStatus != FAILED))
  {
    if((*(__IO uint32_t*) Address) != Data)
    {
      MemoryProgramStatus = FAILED;
    }
    Address += 4;
  }
	return MemoryProgramStatus;
}
