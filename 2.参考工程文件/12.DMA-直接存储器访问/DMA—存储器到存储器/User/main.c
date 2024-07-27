
// DMA-�洢�����洢��ģʽ �����ڲ���FLASH���ڲ���SRAM
	
#include "stm32f10x.h"
#include "./led/bsp_led.h"


// ��ʹ�ô洢�����洢��ģʽʱ��ͨ���������ѡ��û��Ӳ�ԵĹ涨
#define DMA_CHANNEL     DMA1_Channel6
#define DMA_CLOCK       RCC_AHBPeriph_DMA1

// ������ɱ�־
#define DMA_FLAG_TC     DMA1_FLAG_TC6

// Ҫ���͵����ݴ�С
#define BUFFER_SIZE     32

/* ����aSRC_Const_Buffer������ΪDMA��������Դ
 * const�ؼ��ֽ�aSRC_Const_Buffer�����������Ϊ��������
 * ��ʾ���ݴ洢���ڲ���FLASH��
 */
const uint32_t aSRC_Const_Buffer[BUFFER_SIZE]= {
                                    0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
                                    0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
                                    0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
                                    0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
                                    0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
                                    0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
                                    0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
                                    0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80};
/* ����DMA����Ŀ��洢��
 * �洢���ڲ���SRAM��																		
 */
uint32_t aDST_Buffer[BUFFER_SIZE];

#define SOFT_DELAY Delay(0x0FFFFF);
void Delay(__IO u32 nCount); 
uint8_t Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength);
void DMA_Config(void);																	
																		
/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
  /* �����űȽϽ������ */
  uint8_t TransferStatus;
  
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
    
  /* ����RGB��ɫ��Ϊ��ɫ */
  LED_PURPLE;  
  
  /* ����ʱ���� */
  Delay(0xFFFFFF);  
  
  /* DMA�������� */
  DMA_Config(); 
  
  /* �ȴ�DMA������� */
  while(DMA_GetFlagStatus(DMA_FLAG_TC)==RESET)
  {
    
  }   
  
  /* �Ƚ�Դ�����봫������� */
  TransferStatus=Buffercmp(aSRC_Const_Buffer, aDST_Buffer, BUFFER_SIZE);
  
  /* �ж�Դ�����봫������ݱȽϽ��*/
  if(TransferStatus==0)  
  {
    /* Դ�����봫������ݲ����ʱRGB��ɫ����ʾ��ɫ */
    LED_RED;
  }
  else
  { 
    /* Դ�����봫����������ʱRGB��ɫ����ʾ��ɫ */
    LED_BLUE;
  }

	while (1)
	{		
	}
}

void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}

void DMA_Config(void)
{
	  DMA_InitTypeDef DMA_InitStructure;
	
		// ����DMAʱ��
		RCC_AHBPeriphClockCmd(DMA_CLOCK, ENABLE);
		// Դ���ݵ�ַ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)aSRC_Const_Buffer;
		// Ŀ���ַ
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)aDST_Buffer;
		// �������赽�洢����������������ڲ���FLASH��	
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		// �����С	
		DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;
		// ���裨�ڲ���FLASH����ַ����	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
		// �ڴ��ַ����
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		// �������ݵ�λ	
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
		// �ڴ����ݵ�λ
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;	 
		// DMAģʽ��һ�λ���ѭ��ģʽ
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
		//DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  
		// ���ȼ�����	
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		// ʹ���ڴ浽�ڴ�Ĵ���
		DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
		// ����DMAͨ��		   
		DMA_Init(DMA_CHANNEL, &DMA_InitStructure);
    //���DMA������������ɱ�־λ
    DMA_ClearFlag(DMA_FLAG_TC);
		// ʹ��DMA
		DMA_Cmd(DMA_CHANNEL,ENABLE);
}

/**
  * �ж�ָ�����ȵ���������Դ�Ƿ���ȫ��ȣ�
  * �����ȫ��ȷ���1��ֻҪ����һ�����ݲ���ȷ���0
  */
uint8_t Buffercmp(const uint32_t* pBuffer, 
                  uint32_t* pBuffer1, uint16_t BufferLength)
{
  /* ���ݳ��ȵݼ� */
  while(BufferLength--)
  {
    /* �ж���������Դ�Ƿ��Ӧ��� */
    if(*pBuffer != *pBuffer1)
    {
      /* ��Ӧ����Դ����������˳�������������0 */
      return 0;
    }
    /* ������������Դ�ĵ�ַָ�� */
    pBuffer++;
    pBuffer1++;
  }
  /* ����жϲ��Ҷ�Ӧ������� */
  return 1;  
}

//typedef struct
//{
//  uint32_t DMA_PeripheralBaseAddr;   // �����ַ
//  uint32_t DMA_MemoryBaseAddr;       // �洢����ַ
//  uint32_t DMA_DIR;                  // ���䷽��
//  uint32_t DMA_BufferSize;           // ������Ŀ
//  uint32_t DMA_PeripheralInc;        // �����ַ����ģʽ
//  uint32_t DMA_MemoryInc;            // �洢����ַ����ģʽ
//  uint32_t DMA_PeripheralDataSize;   // �������ݿ��
//  uint32_t DMA_MemoryDataSize;       // �洢�����ݿ��
//  uint32_t DMA_Mode;                 // ģʽѡ��
//  uint32_t DMA_Priority;             // ͨ�����ȼ�
//  uint32_t DMA_M2M;                  // �洢�����洢��ģʽ
//}DMA_InitTypeDef;

/*********************************************END OF FILE**********************/
