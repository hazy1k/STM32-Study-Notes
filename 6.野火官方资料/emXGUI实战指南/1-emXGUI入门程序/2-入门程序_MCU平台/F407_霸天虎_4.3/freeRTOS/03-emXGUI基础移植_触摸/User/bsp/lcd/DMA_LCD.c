/*
 * DMA_LCD.c
 *
 *  Created on: 2018��6��28��
 *      Author: liuwei
 */

#include "def.h"
#include "DMA_LCD.h"
#include "FreeRTOS.h"
#include "board.h"
#include "semphr.h"
/*=========================================================================================*/

#define	LCD_DMA_CLK_EN()		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE)
#define	LCD_DMA_Stream		DMA2_Stream0
#define	LCD_DMA_Channel		DMA_Channel_0
#define	LCD_DMA_IRQn		DMA2_Stream0_IRQn


static	const int LCD_DMA_MAX_TRANS	= 0xFFF0;

/*=========================================================================================*/

static GUI_SEM* sem_rdy = NULL;

static volatile int dma_busy=FALSE;

//static void dma_isr(void)
//{
//	DMA_ClearFlag(LCD_DMA_Stream,DMA_FLAG_TCIF0);//�����������ж�
//	dma_busy=FALSE;
//	SYS_sem_post(sem_rdy);
//}

void DMA2_Stream0_IRQHandler(void)
{
  uint32_t ulReturn;
  BaseType_t pxHigherPriorityTaskWoken;
  
  /* �����ٽ�Σ��ٽ�ο���Ƕ�� */
  ulReturn = taskENTER_CRITICAL_FROM_ISR();
  
  if(DMA_GetITStatus(DMA2_Stream0,DMA_IT_TCIF0)==SET)
  {
    DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TCIF0);//�����������ж�
    dma_busy=FALSE;
    
    xSemaphoreGiveFromISR(sem_rdy, &pxHigherPriorityTaskWoken);
    //�����Ҫ�Ļ�����һ�������л���ϵͳ���ж��Ƿ���Ҫ�����л�
    portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
//    GUI_SemPost(sem_rdy);
  }
  
  /* �˳��ٽ�� */
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}

//void DMA2_Stream0_IRQHandler(void)
//{
//	mk_isr_enter();

//	if(DMA_GetITStatus(DMA2_Stream0,DMA_IT_TCIF0)==SET)
//	{
//		DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TCIF0);

//		if(pfnDMA2Isr[0]!=NULL)
//			pfnDMA2Isr[0]();
//	}

//	mk_isr_exit();
//}

/*=========================================================================================*/

static u32 LCD_DAT_ADR=0;

void LCD_DMA_Init(u32 lcd_dat_addr)
{
	DMA_InitTypeDef  DMA_InitStructure;

	LCD_DAT_ADR =lcd_dat_addr;

	sem_rdy = GUI_SemCreate(0,1);

	LCD_DMA_CLK_EN(); //DMA2ʱ��ʹ��

	while (DMA_GetCmdStatus(LCD_DMA_Stream) != DISABLE){}//�ȴ�DMA2_Stream1������
	DMA_DeInit(LCD_DMA_Stream);

	/* ���� DMA Stream */
	DMA_InitStructure.DMA_Channel         = LCD_DMA_Channel;  //ͨ��0
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)lcd_dat_addr;//&LCD->LCD_RAM;;//DMA �洢��0��ַ
	DMA_InitStructure.DMA_PeripheralBaseAddr=0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;//�洢�����洢��ģʽ
	DMA_InitStructure.DMA_BufferSize = 0;//���ݴ�����
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;//��������ģʽ
	DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Disable;//�洢��������ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�������ݳ���:16λ
	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;//�洢�����ݳ��� 16λ
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;// ʹ����ͨģʽ
	DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;//�����ȼ�

	DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Enable; //FIFOģʽ
	DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull;

	DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_INC4;//����ͻ�����δ���
	DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_INC8;//�洢��ͻ�����δ���
	DMA_Init(LCD_DMA_Stream, &DMA_InitStructure);//��ʼ��DMA Stream

	DMA_ITConfig(LCD_DMA_Stream,DMA_IT_TC,ENABLE);//������������ж�
//	DMA_SetIRQHandler(LCD_DMA_Stream,dma_isr);
	if(1)
	{
		NVIC_InitTypeDef   nvic_init;
		nvic_init.NVIC_IRQChannel = LCD_DMA_IRQn;
		nvic_init.NVIC_IRQChannelPreemptionPriority =0x05;//��ռ���ȼ�0
		nvic_init.NVIC_IRQChannelSubPriority = 0x01;//�����ȼ�1
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
	LCD_DMA_Stream->M0AR =dst;
	LCD_DMA_Stream->PAR  =src;

	while(count > 0)
	{
		if(count > (int)LCD_DMA_MAX_TRANS)
		{
			LCD_DMA_Stream->NDTR =LCD_DMA_MAX_TRANS; //���ô��䳤��
			count -= LCD_DMA_MAX_TRANS;

			dma_busy=TRUE;
			LCD_DMA_Stream->CR |= (uint32_t)DMA_SxCR_EN;  //����DMA
			GUI_SemWait(sem_rdy,1000);

			if(src_inc)
			{
				src += LCD_DMA_MAX_TRANS*2; //halfword.
				LCD_DMA_Stream->PAR  =src;
			}
		}
		else
		{

			LCD_DMA_Stream->NDTR =count; //���ô��䳤��
			count =0;

			dma_busy=TRUE;
			LCD_DMA_Stream->CR |= (uint32_t)DMA_SxCR_EN;  //����DMA
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
	LCD_DMA_Stream->CR &= ~DMA_MemoryInc_Enable;
	LCD_DMA_Stream->CR &= ~DMA_PeripheralInc_Enable;

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

	LCD_DMA_Stream->CR &= ~DMA_MemoryInc_Enable;
	LCD_DMA_Stream->CR |=  DMA_PeripheralInc_Enable;

	LCD_DMA_Execu(LCD_DAT_ADR,(u32)src,count,TRUE);

	return TRUE;
}



/*=========================================================================================*/
/*=========================================================================================*/
