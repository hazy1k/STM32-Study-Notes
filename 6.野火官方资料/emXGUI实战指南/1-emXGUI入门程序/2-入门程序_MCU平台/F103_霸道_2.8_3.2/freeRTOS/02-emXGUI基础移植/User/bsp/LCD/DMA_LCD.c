/*
 * DMA_LCD.c
 *
 *  Created on: 2018��6��28��
 *      Author: liuwei
 */

#include "BSP.h"
/*=========================================================================================*/

#define	LCD_DMA_CLK_EN()		  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE)
//#define	LCD_DMA_Stream		  DMA2_Stream0
#define	LCD_DMA_Channel		    DMA1_Channel6
#define	LCD_DMA_IRQn          DMA1_Channel6_IRQn 
#define	LCD_DMA_IRQHandler		DMA1_Channel6_IRQHandler
// ������ɱ�־
#define DMA_FLAG_TC     DMA1_FLAG_TC6


static	const int LCD_DMA_MAX_TRANS	= 0xFFF0;

/*=========================================================================================*/

static GUI_SEM *sem_rdy = NULL;

static volatile int dma_busy=FALSE;

////static void dma_isr(void)
////{
////	DMA_ClearFlag(LCD_DMA_Stream,DMA_FLAG_TCIF0);//�����������ж�
////	dma_busy=FALSE;
////	SYS_sem_post(sem_rdy);
////}

#if 1
void LCD_DMA_IRQHandler(void)
{
  uint32_t ulReturn;
  BaseType_t pxHigherPriorityTaskWoken;
  
  /* �����ٽ�Σ��ٽ�ο���Ƕ�� */
  ulReturn = taskENTER_CRITICAL_FROM_ISR();
  
  DMA_ClearFlag(DMA1_FLAG_TC6);//�����������ж�
	dma_busy=FALSE;
  xSemaphoreGiveFromISR(sem_rdy, &pxHigherPriorityTaskWoken);
  
  //�����Ҫ�Ļ�����һ�������л���ϵͳ���ж��Ƿ���Ҫ�����л�
  portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
	//GUI_SemPost(sem_rdy);
  
  /* �˳��ٽ�� */
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}
#else
void LCD_DMA_IRQHandler(void)
{
  uint32_t ulReturn;
  
  /* �����ٽ�Σ��ٽ�ο���Ƕ�� */
  ulReturn = taskENTER_CRITICAL_FROM_ISR();
  
  DMA_ClearFlag(DMA1_FLAG_TC6);//�����������ж�
	dma_busy=FALSE;
  
  /* �˳��ٽ�� */
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}
#endif
/*=========================================================================================*/

static u32 LCD_DAT_ADR=0;

void LCD_DMA_Init(u32 lcd_dat_addr)
{
	DMA_InitTypeDef  DMA_InitStructure;

	LCD_DAT_ADR =lcd_dat_addr;

	sem_rdy = GUI_SemCreate(0,1);

	LCD_DMA_CLK_EN(); //DMA2ʱ��ʹ��


	/* ���� DMA Stream */
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)lcd_dat_addr;//&LCD->LCD_RAM;;//DMA �洢��0��ַ
	DMA_InitStructure.DMA_PeripheralBaseAddr=0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//DMA_DIR_PeripheralDST;//DMA_DIR_MemoryToMemory;    // �洢�����洢��ģʽ
	DMA_InitStructure.DMA_BufferSize = 0;                                          // ���ݴ�����
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;          // ��������ģʽ
	DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Disable;              // �洢��������ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;    // �������ݳ���:16λ
	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;        // �洢�����ݳ��� 16λ
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;                    // ʹ����ͨģʽ
	DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;              // �����ȼ�
  
  DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;                                    // ʹ���ڴ浽�ڴ�Ĵ���

	DMA_Init(LCD_DMA_Channel, &DMA_InitStructure);//��ʼ��DMA Stream

	DMA_ITConfig(LCD_DMA_Channel,DMA_IT_TC,ENABLE);//������������ж�
  
  DMA_ClearFlag(DMA_FLAG_TC);                   //���DMA������������ɱ�־λ
//	DMA_SetIRQHandler(LCD_DMA_Stream,dma_isr);
	if(1)
	{
		NVIC_InitTypeDef   nvic_init;
		nvic_init.NVIC_IRQChannel = LCD_DMA_IRQn;
		nvic_init.NVIC_IRQChannelPreemptionPriority =0x00;//��ռ���ȼ�0
		nvic_init.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�1
		nvic_init.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
		NVIC_Init(&nvic_init);//����
	}

}


static u16 dat_v;

void LCD_DMA_WaitBusy(void)
{
	while(dma_busy==TRUE);

//	DMA_Cmd(LCD_DMA_Stream,DISABLE);//�ر�DMA2,Stream0
//	while (DMA_GetCmdStatus(LCD_DMA_Stream) != DISABLE){}//�ȴ�DMA2_Stream0������
//	DMA_ClearFlag(LCD_DMA_Stream,DMA_FLAG_TCIF0);//�����������ж�

}

static void LCD_DMA_Execu(u32 dst,u32 src,int count,int src_inc)
{
	LCD_DMA_Channel->CMAR =dst;
	LCD_DMA_Channel->CPAR  =src;

	while(count > 0)
	{
		if(count > (int)LCD_DMA_MAX_TRANS)
		{
      LCD_DMA_Channel->CCR &= ~(uint32_t)DMA_CCR6_EN;  //����DMA
      
			LCD_DMA_Channel->CNDTR =LCD_DMA_MAX_TRANS; //���ô��䳤��
			count -= LCD_DMA_MAX_TRANS;

			dma_busy=TRUE;
			LCD_DMA_Channel->CCR |= (uint32_t)DMA_CCR6_EN;  //����DMA
//      while(dma_busy);
			GUI_SemWait(sem_rdy,1000);

			if(src_inc)
			{
				src += LCD_DMA_MAX_TRANS*2; //halfword.
				LCD_DMA_Channel->CPAR  =src;
			}
		}
		else
		{

      LCD_DMA_Channel->CCR &= ~(uint32_t)DMA_CCR6_EN;  //����DMA
			LCD_DMA_Channel->CNDTR =count; //���ô��䳤��
			count =0;

			dma_busy=TRUE;
			LCD_DMA_Channel->CCR |= (uint32_t)DMA_CCR6_EN;  //����DMA
//      while(dma_busy);
			GUI_SemWait(sem_rdy,1000);
			break;

		}

	}

}


BOOL LCD_DMA_WritePixels(u16 val,s32 count)
{

	if(count < (20*20))
	{
		return FALSE;
	}

	LCD_DMA_WaitBusy();

	dat_v =val;
	LCD_DMA_Channel->CCR &= ~DMA_MemoryInc_Enable;
	LCD_DMA_Channel->CCR &= ~DMA_PeripheralInc_Enable;

	LCD_DMA_Execu(LCD_DAT_ADR,(u32)&dat_v,count,FALSE);

	return TRUE;

}


BOOL LCD_DMA_WriteBitmap(u16 *src,int count)
{

	if(count < (20*20))
	{
		return FALSE;
	}

	LCD_DMA_WaitBusy();

	LCD_DMA_Channel->CCR &= ~DMA_MemoryInc_Enable;
	LCD_DMA_Channel->CCR |=  DMA_PeripheralInc_Enable;

	LCD_DMA_Execu(LCD_DAT_ADR,(u32)src,count,TRUE);

	return TRUE;
}


/*=========================================================================================*/
/*=========================================================================================*/
