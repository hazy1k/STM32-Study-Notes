/**
  ******************************************************************************
  * @file    bsp_internalFlash.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   �ڲ�FLASH��д�������Է���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 �Ե� ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./protect/bsp_readWriteProtect.h"   
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"   

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

#define WRITE_PROTECTION_DISABLE

#define FLASH_USER_START_ADDR       ADDR_FLASH_PAGE_32   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR         ADDR_FLASH_PAGE_48   /* End @ of user Flash area */
#define FLASH_PAGE_TO_BE_PROTECTED (OB_WRP_PAGES0TO1 | OB_WRP_PAGES2TO3)  

#define DATA_32                     ((uint32_t)0x12345678)


#if !defined(WRITE_PROTECTION_ENABLE)&&!defined(WRITE_PROTECTION_DISABLE)
#define WRITE_PROTECTION_DISABLE
#endif /* !WRITE_PROTECTION_ENABLE && !WRITE_PROTECTION_DISABLE */

#if defined(WRITE_PROTECTION_ENABLE)&&defined(WRITE_PROTECTION_DISABLE)
#error "Switches WRITE_PROTECTION_ENABLE & WRITE_PROTECTION_DISABLE cannot be enabled in the time!"
#endif /* WRITE_PROTECTION_ENABLE && WRITE_PROTECTION_DISABLE */

uint32_t Address = 0;
uint32_t PageError = 0;
__IO TestStatus MemoryProgramStatus = PASSED;
/*���ڲ������̵ı���*/
 FLASH_EraseInitTypeDef EraseInitStruct;
/*���ڴ���ѡ���ֽڵı���*/
 FLASH_OBProgramInitTypeDef OptionsBytesStruct;




/**
  * @brief  FLASH_Test,��ͨ��д��������
  * @param  ���б�������������FLASH_WRP_SECTORS����д���������ظ�һ�λ���н�д����
  * @retval None
  */
void FLASH_Test(void)
{
	printf("��ʼ����������ʼ����\r\n");
	  /* ��ʼ������״̬ */
  MemoryProgramStatus = PASSED;
  
  /* ����Flash������������ƼĴ�������*/ 
  HAL_FLASH_Unlock();

  /* ����ѡ���ֽ�*/
  HAL_FLASH_OB_Unlock();

  /* ��ȡҳ��д����״̬*/
  HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);

#ifdef WRITE_PROTECTION_DISABLE
  /* �������ҳ���Ƿ��ѱ�д����*/
  if((OptionsBytesStruct.WRPPage & FLASH_PAGE_TO_BE_PROTECTED) != FLASH_PAGE_TO_BE_PROTECTED)
  {
		printf("ҳ���ѱ�д���������ڻָ�д����ҳ��\r\n");
    /*�ָ�д����ҳ�� */
    OptionsBytesStruct.OptionType   = OPTIONBYTE_WRP;
    OptionsBytesStruct.WRPState     = OB_WRPSTATE_DISABLE;
    OptionsBytesStruct.WRPPage = FLASH_PAGE_TO_BE_PROTECTED;
    if(HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK)
    {
      while (1)
      {
       LED1_ON
       printf("����ʧ��\r\n");
      }
    }

    /* ����ϵͳ�����Լ�����ѡ���ֽ�ֵ*/
    HAL_FLASH_OB_Launch();
  }
#elif defined WRITE_PROTECTION_ENABLE
  /* �������ҳ���Ƿ���δд����*/
  if(((~OptionsBytesStruct.WRPPage) & FLASH_PAGE_TO_BE_PROTECTED )!= FLASH_PAGE_TO_BE_PROTECTED)
  {
	  printf("ҳ��δ��д��������������д����ҳ��\r\n");
    /* ����ҳ��д����*/
    OptionsBytesStruct.OptionType = OPTIONBYTE_WRP;
    OptionsBytesStruct.WRPState   = OB_WRPSTATE_ENABLE;
    OptionsBytesStruct.WRPPage    = FLASH_PAGE_TO_BE_PROTECTED;
    if(HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK)
    {
      while (1)
      {
        LED1_ON
        printf("����ʧ��\r\n");
      }
    }

    /* ����ϵͳ�����Լ�����ѡ���ֽ�ֵ*/
    HAL_FLASH_OB_Launch();
  }
#endif /* WRITE_PROTECTION_DISABLE */

  /*����ѡ���ֽ�*/
  HAL_FLASH_OB_Lock();

  /* ��ѡҳ��δ��д����*/
  if ((OptionsBytesStruct.WRPPage & FLASH_PAGE_TO_BE_PROTECTED) != 0x00)
  {
		printf("ҳ��δ��д���������ڲ���ҳ��\r\n");
    /*��дEraseInit�ṹ*/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
    EraseInitStruct.NbPages     = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR)/FLASH_PAGE_SIZE;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
      /* 
       ҳ�����ʱ�������� �û��������������һЩ����������������� PageError�������������ҳ�棬Ȼ��֪����ҳ���ϵĴ������
       �û����Ե��ú���'HAL_FLASH_GetError����'
      */
      while (1)
      {
        LED1_ON
        printf("����ʧ��\r\n");
      }
    }

    /*��FLASH_USER_START_ADDR��FLASH_USER_END_ADDR����ĵ�ַ����DATA_32 FLASH�ֳ��� */
    Address = FLASH_USER_START_ADDR;
    while (Address < FLASH_USER_END_ADDR)
    {
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, DATA_32) == HAL_OK)
      {
        Address = Address + 4;
      }
      else
      {
        while (1)
        {
          LED1_ON
           printf("����ʧ��\r\n");
        }
      }
    }

    /*����������ݵ���ȷ��*/
    Address = FLASH_USER_START_ADDR;

    while (Address < FLASH_USER_END_ADDR)
    {
      if((*(__IO uint32_t*) Address) != DATA_32)
      {
        MemoryProgramStatus = FAILED;
      }
      Address += 4;
    }
  }
  else
  { 
    /*����ҳ����д���� */ 
    /* ����Ƿ������ڴ�ҳ����д�� */
    Address = FLASH_USER_START_ADDR;
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, DATA_32) != HAL_OK)
    {
      /* ����ڼ䷵�ش��� */
      /* ���WRPERR��־�Ƿ��������� */
      if (HAL_FLASH_GetError() == HAL_FLASH_ERROR_WRP) 
      {
        MemoryProgramStatus = FAILED;
      }
      else
      {
        while (1)
        {
          LED1_ON;
          
          printf("����ʧ��\r\n");
        }
      }
    }
    else
    {
      while (1)
      {
        LED1_ON
        printf("����ʧ��\r\n");
      }
    }
  }
  HAL_FLASH_Lock();

  /*�����������Ƿ��������*/
  if (MemoryProgramStatus == PASSED)
  {
    LED2_ON
		printf("��� --> ������������\r\n");
  }
  else
  {
    while (1)
    {
      LED1_ON
      printf("����ʧ��\r\n");
    }
  }
	printf("���Խ���\r\n");

  while (1)
  {
  }
}






