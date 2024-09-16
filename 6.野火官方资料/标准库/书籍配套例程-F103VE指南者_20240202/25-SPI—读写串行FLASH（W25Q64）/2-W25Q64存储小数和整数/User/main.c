/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   W25Q64�洢С���������ķ���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_usart.h"
#include "./flash/bsp_spi_flash.h" 
 


//��ȡ��ID�洢λ��
__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;

// ����ԭ������
void Delay(__IO uint32_t nCount);

/*
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  ����
 * ��ʾ  ����Ҫ�Ҹ�PC0��ñ����
 */
int main(void)
{ 	
	uint8_t cal_flag = 0;
	uint8_t k;
	
	/*�洢С�������������飬��7��*/
	long double double_buffer[7] = {0};
	int int_bufffer[7] = {0};
	
	LED_GPIO_Config();
	LED_BLUE;
	
	/* ���ô���1Ϊ��115200 8-N-1 */
	USART_Config();
  
	printf("\r\n����һ��FLASH ��дС���ͳ�����ʵ�� \r\n");
	printf("\r\n ʹ��ָ���ߵװ�ʱ ���Ͻ�����λ�� ��Ҫ��PC0������ñ ��ֹӰ��PC0��SPIFLASHƬѡ�� \r\n");
	
	/* 16M����flash W25Q128��ʼ�� */
	SPI_FLASH_Init();
	
	/* ��ȡ Flash Device ID */
	DeviceID = SPI_FLASH_ReadDeviceID();
	
	Delay( 200 );
	
	/* ��ȡ SPI Flash ID */
	FlashID = SPI_FLASH_ReadID();
	
	printf("\r\nFlashID is 0x%X,  Manufacturer Device ID is 0x%X\r\n", FlashID, DeviceID);
	
	/* ���� SPI Flash ID */
	if (FlashID == sFLASH_ID) 
	{	
		LED_GREEN;
		
		printf("\r\n��⵽SPI FLASH W25Q64 !\r\n");
		
		/*��ȡ���ݱ�־λ*/
    SPI_FLASH_BufferRead(&cal_flag, SPI_FLASH_PageSize*0, 1);
    
		if( cal_flag == 0xCD )	/*����־����0xcd����ʾ֮ǰ����д������*/
    {
			  printf("\r\n��⵽���ݱ�־\r\n");

				/*��ȡС������*/
        SPI_FLASH_BufferRead((void*)double_buffer, SPI_FLASH_PageSize*1, sizeof(double_buffer));
				
				/*��ȡ��������*/
        SPI_FLASH_BufferRead((void*)int_bufffer, SPI_FLASH_PageSize*2, sizeof(int_bufffer));
	
			
				printf("\r\n��оƬ�������ݣ�\r\n");			
        for( k=0; k<7; k++ )
				{
					printf("С�� rx = %LF \r\n",double_buffer[k]);
					printf("���� rx = %d \r\n",int_bufffer[k]);				
				}
    }    
    else
    {
      printf("\r\nû�м�⵽���ݱ�־��FLASHû�д洢���ݣ���������С��д��ʵ��\r\n");
			cal_flag =0xCD;
			/*��������*/
      SPI_FLASH_SectorErase(0);
			
			/*д���־����0ҳ*/
      SPI_FLASH_BufferWrite(&cal_flag, SPI_FLASH_PageSize*0, 1); 
      
			/*����Ҫд�������*/
      for( k=0; k<7; k++ )
			{
			   double_buffer[k] = k +0.1;
				 int_bufffer[k]=k*500+1 ;
			}

			/*д��С�����ݵ���һҳ*/
      SPI_FLASH_BufferWrite((void*)double_buffer, SPI_FLASH_PageSize*1, sizeof(double_buffer));
			/*д���������ݵ��ڶ�ҳ*/
      SPI_FLASH_BufferWrite((void*)int_bufffer, SPI_FLASH_PageSize*2, sizeof(int_bufffer));
       			
			printf("��оƬд�����ݣ�");
			/*��ӡ������*/
      for( k=0; k<7; k++ )
			{
        printf("С��tx = %LF\r\n",double_buffer[k]);
				printf("����tx = %d\r\n",int_bufffer[k]);
			}
			
			printf("\r\n�븴λ�����壬�Զ�ȡ���ݽ��м���\r\n");
    }   

	}// if (FlashID == sFLASH_ID)
	else
	{    
		LED_RED;
		printf("\r\n��ȡ���� W25Q64 ID!\n\r");
	}
	
	SPI_Flash_PowerDown();  
	while(1);  
}



void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


/*********************************************END OF FILE**********************/

