/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 ����ģ��
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
#include "board.h"
#include "rtthread.h"


/*
*************************************************************************
*                               ����
*************************************************************************
*/
	typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

	/* ��ȡ�������ĳ��� */
	#define TxBufferSize1   (countof(TxBuffer1) - 1)
	#define RxBufferSize1   (countof(TxBuffer1) - 1)
	#define countof(a)      (sizeof(a) / sizeof(*(a)))
	#define  BufferSize (countof(Tx_Buffer)-1)

	#define  FLASH_WriteAddress     0x00000
	#define  FLASH_ReadAddress      FLASH_WriteAddress
	#define  FLASH_SectorToErase    FLASH_WriteAddress

			 

	/* ���ͻ�������ʼ�� */
	uint8_t Tx_Buffer[] = "��л��ѡ�ñ���stm32������\r\n";
	uint8_t Rx_Buffer[BufferSize];

	__IO uint32_t DeviceID = 0;
	__IO uint32_t FlashID = 0;
	__IO TestStatus TransferStatus1 = FAILED;
	


/* �����߳̿��ƿ� */
static rt_thread_t led1_thread = RT_NULL;
static rt_thread_t test_thread = RT_NULL;
/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void led1_thread_entry(void* parameter);
static void test_thread_entry(void* parameter);

TestStatus Buffercmp(uint8_t* pBuffer1,uint8_t* pBuffer2, uint16_t BufferLength);
void spi_test(void);
/*
*************************************************************************
*                             main ����
*************************************************************************
*/
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
    /* 
	 * ������Ӳ����ʼ����RTTϵͳ��ʼ���Ѿ���main����֮ǰ��ɣ�
	 * ����component.c�ļ��е�rtthread_startup()����������ˡ�
	 * ������main�����У�ֻ��Ҫ�����̺߳������̼߳��ɡ�
	 */
	
	led1_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "led1",              /* �߳����� */
                      led1_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      2,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (led1_thread != RT_NULL)
        rt_thread_startup(led1_thread);
    else
        return -1;
		
	test_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "test",              /* �߳����� */
                      test_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      1024,                 /* �߳�ջ��С */
                      3,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (test_thread != RT_NULL)
        rt_thread_startup(test_thread);
    else
        return -1;
}

/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/

static void led1_thread_entry(void* parameter)
{	
    while (1)
    {
        LED1_ON;
        rt_thread_delay(500);   /* ��ʱ500��tick */
        rt_kprintf("led1_thread running,LED1_ON\r\n");
        
        LED1_OFF;     
        rt_thread_delay(500);   /* ��ʱ500��tick */		 		
        rt_kprintf("led1_thread running,LED1_OFF\r\n");
    }
}

static void test_thread_entry(void* parameter)
{	

	while (1)
	{
		spi_test();
		rt_thread_delay(500);   /* ��ʱ500��tick */		 		

	}
}


/*
 * ��������Buffercmp
 * ����  ���Ƚ������������е������Ƿ����
 * ����  ��-pBuffer1     src������ָ��
 *         -pBuffer2     dst������ָ��
 *         -BufferLength ����������
 * ���  ����
 * ����  ��-PASSED pBuffer1 ����   pBuffer2
 *         -FAILED pBuffer1 ��ͬ�� pBuffer2
 */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
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

void spi_test(void)
{
		/* ��ȡ Flash Device ID */
	DeviceID = SPI_FLASH_ReadDeviceID();	
	/* ��ȡ SPI Flash ID */
	FlashID = SPI_FLASH_ReadID();	
	rt_kprintf("\r\n FlashID is 0x%X,\
	Manufacturer Device ID is 0x%X\r\n", FlashID, DeviceID);
	/* ���� SPI Flash ID */
	if (FlashID == sFLASH_ID)
	{	
		printf("\r\n ��⵽����flash W25Q64 !\r\n");
		
		/* ������Ҫд��� SPI FLASH ������FLASHд��ǰҪ�Ȳ��� */
		// �������4K����һ����������������С��λ������
		SPI_FLASH_SectorErase(FLASH_SectorToErase);	 	 
		
		/* �����ͻ�����������д��flash�� */
		// ����дһҳ��һҳ�Ĵ�СΪ256���ֽ�
		SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);		
		printf("\r\n д�������Ϊ��%s \r\t", Tx_Buffer);
		
		/* ���ո�д������ݶ������ŵ����ջ������� */
		SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);
		printf("\r\n ����������Ϊ��%s \r\n", Rx_Buffer);
		
		/* ���д�������������������Ƿ���� */
		TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);
		
		if( PASSED == TransferStatus1 )
		{ 
			LED_GREEN;
			printf("\r\n 8M����flash(W25Q64)���Գɹ�!\n\r");
		}
		else
		{        
			LED_RED;
			printf("\r\n 8M����flash(W25Q64)����ʧ��!\n\r");
		}
	}// if (FlashID == sFLASH_ID)
	else// if (FlashID == sFLASH_ID)
	{ 
		LED_RED;
		printf("\r\n ��ȡ���� W25Q64 ID!\n\r");
	}
}


/********************************END OF FILE****************************/
