#include "stm32f10x.h"
#include "breathing.h"

// ����RGB�Ƶ���ɫ����
__IO uint32_t rgb_color = 0x00FFFF;

int main(void)
{			
	breathing_Init();
	while(1)
	{
	}		
}
