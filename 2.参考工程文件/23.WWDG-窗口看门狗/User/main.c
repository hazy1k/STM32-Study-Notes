#include "stm32f10x.h"
#include "led.h"
#include "key.h" 
#include "wwdg.h" 

void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
} 

int main(void)
{	
	uint8_t wwdg_tr, wwdg_wr;
	LED_Init();
	LED1_ON();
	Delay(0xFFFFFF);
	// ��ʼ��WWDG�����ü�������ʼֵ�������ϴ���ֵ������WWDG��ʹ����ǰ�����ж�
	WWDG_Mode_Init(0X7F, 0X5F, WWDG_Prescaler_8);
	wwdg_wr = WWDG->CFR & 0X7F;
	while(1)
	{	
		LED1_OFF();
		//-----------------------------------------------------
		// �ⲿ��Ӧ��д��Ҫ��WWDG��صĳ�����γ������е�ʱ��
		// �����˴���ֵӦ�����óɶ��
		//-----------------------------------------------------
		// ��ʱ��ֵ����ʼ�������0X7F��������WWDGʱ�����ֵ�᲻�ϼ�С
		// ����������ֵ���ڴ���ֵʱι���Ļ����Ḵλ�������������ٵ�0X40
		// ��û��ι���Ļ��ͷǳ��ǳ�Σ���ˣ��������ټ�һ�ε���0X3Fʱ�͸�λ
		// ����Ҫ����������ֵ�ڴ���ֵ��0X40֮���ʱ��ι��������0X40�ǹ̶��ġ�
		wwdg_tr = WWDG->CR & 0X7F;
		if(wwdg_tr < wwdg_wr)
		{
			// ι�����������ü�������ֵΪ���0X7F
			WWDG_Feed();
		}
	}
}
