#include "tpad.h"

uint16_t tpad_default_val;

// ���ݰ���GPIO��ʼ��
void TPAD_TIM_GPIO_Config(void) 
{
  	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(TPAD_TIM_CH_GPIO_CLK, ENABLE); 
    GPIO_InitStructure.GPIO_Pin =  TPAD_TIM_CH_PIN;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  	GPIO_Init(TPAD_TIM_CH_PORT, &GPIO_InitStructure);
}
// ǰ��֪ʶ
/*
	1.ARR���Զ����ؼĴ�����ֵ������������Ϊ0xFFFF���������������ֵ
	2.CNTxCLK����������ʱ��Ƶ�ʣ�APB1��ʱ��Ƶ��Ϊ72M������CNTxCLK=72/(psc+1) = 1MHz
	3.PWN�źŵ�����T��T = ARR*(psc+1)/72 = ARR*1
	4.ռ�ձ�P��P = CCR/(ARR+1)������CCRΪ�ȽϼĴ�����ֵ������ߵ�ƽ�����ڵ�ʱ��
*/
// ��ʱ��ģʽ����
static void TPAD_TIM_Mode_Config(void)
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TPAD_TIM_APBxClock_FUN(TPAD_TIM_CLK,ENABLE);
/*--------------------ʱ���ṹ���ʼ��-------------------------*/
	TIM_TimeBaseStructure.TIM_Period = TPAD_TIM_Period;	
	TIM_TimeBaseStructure.TIM_Prescaler = TPAD_TIM_Prescaler;	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;	
	TIM_TimeBaseInit(TPAD_TIM, &TIM_TimeBaseStructure);
/*--------------------���벶��ṹ���ʼ��----------------------*/		
	TIM_ICInitStructure.TIM_Channel = TPAD_TIM_CHANNEL_x;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInit(TPAD_TIM, &TIM_ICInitStructure);
	TIM_Cmd(TPAD_TIM, ENABLE);
}

void TPAD_Reset(void) // ���ݰ�����λ
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(TPAD_TIM_CH_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  TPAD_TIM_CH_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TPAD_TIM_CH_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(TPAD_TIM_CH_PORT, TPAD_TIM_CH_PIN);
	Delay_ms(5);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(TPAD_TIM_CH_PORT, &GPIO_InitStructure);
}

uint16_t TPAD_Get_Val(void) // ��ȡ��ʱ������ֵ
{		
	TPAD_Reset();
	// �����ݰ�����λ�ŵ�֮�󣬼�������0��ʼ����
  	TIM_SetCounter (TPAD_TIM, 0);
	TIM_ClearITPendingBit(TPAD_TIM, TPAD_TIM_IT_CCx | TIM_IT_Update);
	// �ȴ����������أ������ݰ�����絽1.8V���ҵ�ʱ�򣬾ͻᱻ��Ϊ��������
	while(TIM_GetFlagStatus (TPAD_TIM, TPAD_TIM_IT_CCx) == RESET)
	{
		if (TIM_GetCounter(TPAD_TIM) > TPAD_TIM_Period-100) // ��ʱ����
		{
			return TIM_GetCounter (TPAD_TIM);
		} 			
	}
	return TPAD_TIM_GetCapturex_FUN(TPAD_TIM); // ��ȡ����ֵ
} 

void TPAD_TIM_Init(void)
{
	TPAD_TIM_GPIO_Config();
	TPAD_TIM_Mode_Config();		
}

// ��ʼ��������������ÿ��ص�ʱ���������ĳ��ʱ��
uint8_t TPAD_Init(void)
{
	uint16_t temp;
	TPAD_TIM_Init();
	temp = TPAD_Get_Val();
	// ���Ե�ʱ����԰Ѳ����ֵ��ӡ����������Ĭ�ϵĳ��ʱ���Ƕ���
	printf("���ݰ���Ĭ�ϳ��ʱ��Ϊ: %d us\n",temp);
	if((TPAD_DEFAULT_VAL_MIN<temp) && (temp<TPAD_DEFAULT_VAL_MAX)) // ȷ��Ĭ�ϵĳ��ʱ���ں���Χ��
	{
		tpad_default_val = temp;
		return 0;  // �ɹ�
	}
	else
    {
		return 1; // ʧ��
    }
}

// ��ö���������������ֵ
uint16_t TPAD_Get_MaxVal( uint8_t num )
{
	uint16_t temp=0, res=0;
	while(num--)
	{
		temp = TPAD_Get_Val();
		if( temp > res ) 
			res = temp;
	}	
	return res;	
} 

// ����ɨ�躯��
uint8_t TPAD_Scan(void)
{
	static uint8_t keyen=0;	
	uint8_t res=0,sample=3; 
	uint16_t scan_val;		
	// ����sampleֵ������Σ���ȡ���ֵ��С��һ���Ǹ��Ż���������
	scan_val = TPAD_Get_MaxVal(sample); 
    // ��ɨ���ֵ���ڿ���ֵ����Ĭ�ϵ�����ֵ֮�󣬱�ʾ��������
	if(scan_val > (tpad_default_val+TPAD_GATE_VAL))
	{						
    	// �ٴμ�⣬�����ڻ�е������ȥ��
		scan_val = TPAD_Get_MaxVal(sample);		
		if((keyen == 0 )&&(scan_val > (tpad_default_val+TPAD_GATE_VAL)))
				res = 1;	// ��Ч�İ���
		keyen = 2;
	}
	if(keyen > 0)
		keyen--;
	return res;
}
