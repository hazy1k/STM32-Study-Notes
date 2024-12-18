#include "stm32f10x.h"

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x02000000+((addr & 0x00FFFFFF)<<5)+(bitnum<<2)) // ��λ����������ַת����ʵ�ʵ�ַ 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) // �ѵ�ַת����ָ��
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) // ��λ����������ַת����ʵ�ʵ�ַ
// GPIO ODR �� IDR �Ĵ�����ַӳ��  
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C           
#define GPIOB_IDR_Addr    (GPIOB_BASE+8)  //0x40010C08   
// �������� GPIO��ĳһ��IO�ڣ�n(0,1,2...15),n��ʾ��������һ������
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  // ���   
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  // ����    
// ��������
void SOFT_Delay(__IO uint32_t nCount);	
void LED_GPIO_Config(void);

int main(void)
{	
	
	LED_GPIO_Config(); // LED��ʼ��
	// ���濪ʼ����������-ʵ��ѭ����������
	while( 1 )
	{
		// PB0 = 0,����LED
		PBout(0)= 0;		
		SOFT_Delay(0x0FFFFF);
		// PB0 = 1,Ϩ��LED		
		PBout(0)= 1;
		SOFT_Delay(0x0FFFFF);		
        // PB1 = 0,����LED
		PBout(1)= 0;		
		SOFT_Delay(0x0FFFFF);
		// PB1 = 1,Ϩ��LED		
		PBout(1)= 1;
		SOFT_Delay(0x0FFFFF);	
        // PB5 = 0,����LED
		PBout(5)= 0;		
		SOFT_Delay(0x0FFFFF);
		// PB5 = 1,Ϩ��LED		
		PBout(5)= 1;
		SOFT_Delay(0x0FFFFF);
	}
}

void LED_GPIO_Config(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 										   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_5|GPIO_Pin_1;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);		  
}

void SOFT_Delay(__IO uint32_t nCount)	
{
	for(; nCount != 0; nCount--);
}
