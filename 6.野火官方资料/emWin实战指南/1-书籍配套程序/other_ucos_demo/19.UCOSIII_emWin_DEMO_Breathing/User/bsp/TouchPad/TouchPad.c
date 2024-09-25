#include "..\User\bsp\TouchPad\TouchPad.h"
#include "includes.h"

#define TPAD_ARR_MAX_VAL 	0XFFFF	//����ARRֵ

volatile  uint16_t tpad_default_val=0;//���ص�ʱ��(û���ְ���),��������Ҫ��ʱ��

void TPAD_Reset(void);
uint16_t  TPAD_Get_Val(void);
uint16_t  TPAD_Get_MaxVal(uint8_t n);
void TIMx_CHx_Cap_Init(uint16_t arr,uint16_t psc); 

//��ʼ����������
//��ÿ��ص�ʱ����������ȡֵ.
//����ֵ:0,��ʼ���ɹ�;1,��ʼ��ʧ��
uint8_t TPAD_Init(void)
{
	uint16_t buf[10];
	uint16_t temp;
	uint8_t j,i;
	TIMx_CHx_Cap_Init(TPAD_ARR_MAX_VAL,SystemCoreClock/1000000-1);//��1Mhz��Ƶ�ʼ��� 
	for(i=0;i<10;i++)//������ȡ10��
	{				 
		buf[i]=TPAD_Get_Val();
		bsp_DelayUS(10*1000);	    
	}				    
	for(i=0;i<9;i++)//����
	{
		for(j=i+1;j<10;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	temp=0;
	for(i=2;i<8;i++)temp+=buf[i];//ȡ�м��8�����ݽ���ƽ��
	tpad_default_val=temp/6;
	//printf("tpad_default_val:%d\r\n",tpad_default_val);	
	if(tpad_default_val>TPAD_ARR_MAX_VAL/2)return 1;//��ʼ����������TPAD_ARR_MAX_VAL/2����ֵ,������!
	return 0;		     	    					   
}


//��λһ��
void TPAD_Reset(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
	
	
	//����GPIOA.1Ϊ����ʹ��
	macPAD_TIM_GPIO_APBxClock_FUN ( macPAD_TIM_GPIO_CLK, ENABLE );	 //ʹ��PA�˿�ʱ��
 	GPIO_InitStructure.GPIO_Pin = macPAD_TIM_CH_PIN;				 //PA1 �˿�����
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(macPAD_TIM_CH_PORT, &GPIO_InitStructure);
	
 	GPIO_ResetBits ( macPAD_TIM_CH_PORT, macPAD_TIM_CH_PIN );						 //PA.1���0,�ŵ�

	bsp_DelayUS(5*1000);

	TIM_SetCounter(macPAD_TIMx,0);		//��0
	TIM_ClearITPendingBit(macPAD_TIMx, TIM_IT_CC2|TIM_IT_Update); //����жϱ�־
	
	//����GPIOA.1Ϊ��������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;	 //��������
	GPIO_Init(macPAD_TIM_CH_PORT, &GPIO_InitStructure);

}


//�õ���ʱ������ֵ
//�����ʱ,��ֱ�ӷ��ض�ʱ���ļ���ֵ.
uint16_t TPAD_Get_Val(void)
{				   
	TPAD_Reset();
	while(TIM_GetFlagStatus ( macPAD_TIMx, macPAD_TIM_IT_CCx ) == RESET)//�ȴ�����������
	{
		if(TIM_GetCounter(macPAD_TIMx)>TPAD_ARR_MAX_VAL-500)return TIM_GetCounter(macPAD_TIMx);//��ʱ��,ֱ�ӷ���CNT��ֵ
	};	
	return macPAD_TIM_GetCaptureX ( macPAD_TIMx );	  
} 	 


//��ȡn��,ȡ���ֵ
//n��������ȡ�Ĵ���
//����ֵ��n�ζ������������������ֵ
uint16_t TPAD_Get_MaxVal(uint8_t n)
{
	uint16_t temp=0;
	uint16_t res=0;
	while(n--)
	{
		temp=TPAD_Get_Val();//�õ�һ��ֵ
		if(temp>res)res=temp;
	};
	return res;
}  


//ɨ�败������
//mode:0,��֧����������(����һ�α����ɿ����ܰ���һ��);1,֧����������(����һֱ����)
//����ֵ:0,û�а���;1,�а���;										  
#define TPAD_GATE_VAL 	80	//����������ֵ,Ҳ���Ǳ������tpad_default_val+TPAD_GATE_VAL,����Ϊ����Ч����.
uint8_t TPAD_Scan(uint8_t mode)
{
	static uint8_t keyen=0;	//0,���Կ�ʼ���;>0,�����ܿ�ʼ���	 
	uint8_t res=0;
	uint8_t sample=3;		//Ĭ�ϲ�������Ϊ3��	 
	uint16_t rval;
	if(mode)
	{
		sample=6;	//֧��������ʱ�����ò�������Ϊ6��
		keyen=0;	//֧������	  
	}
	rval=TPAD_Get_MaxVal(sample); 
//	printf("scan_rval=%d\n",rval);
	if(rval>(tpad_default_val+TPAD_GATE_VAL))//����tpad_default_val+TPAD_GATE_VAL,��Ч
	{						
  		rval=TPAD_Get_MaxVal(sample);		 
		if((keyen==0)&&(rval>(tpad_default_val+TPAD_GATE_VAL)))//����tpad_default_val+TPAD_GATE_VAL,��Ч
		{
			res=1;
		}	   
		//printf("r:%d\r\n",rval);			     	    					   
		keyen=5;				//����Ҫ�ٹ�5��֮����ܰ�����Ч   
	}else if(keyen>2)keyen=2; 	//�����⵽�����ɿ�,��ֱ�ӽ�������Ϊ2,�������Ӧ�ٶ�
	if(keyen)keyen--;		   							   		     	    					   
	return res;
}	 


//��ʱ��2ͨ��2���벶������
void TIMx_CHx_Cap_Init(uint16_t arr,uint16_t psc)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	

	//����GPIOA.1Ϊ��������
	macPAD_TIM_GPIO_APBxClock_FUN ( macPAD_TIM_GPIO_CLK, ENABLE );	 //ʹ��PA�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = macPAD_TIM_CH_PIN;				 //PA1 �˿�����
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;	 //��������
	GPIO_Init(macPAD_TIM_CH_PORT, &GPIO_InitStructure);	 //����Ϊ��������
	
	//��ʼ��TIM5
	macPAD_TIM_APBxClock_FUN ( macPAD_TIM_CLK, ENABLE );	 //ʹ��TIM5ʱ��
	TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ   
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//Ԥ��Ƶ�� 	   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(macPAD_TIMx, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	//��ʼ��ͨ��2 
	TIM_ICInitStructure.TIM_Channel = macPAD_TIM_Channel_X; //CC1S=01 	ѡ������� IC2ӳ�䵽TI5��
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
	TIM_ICInitStructure.TIM_ICFilter = 0x03;//IC2F=0011 ���������˲��� 8����ʱ��ʱ�������˲�
	TIM_ICInit(TIM5, &TIM_ICInitStructure);//��ʼ��I5 IC2

	TIM_Cmd ( macPAD_TIMx, ENABLE ); 	//ʹ�ܶ�ʱ��5
		
}


/*********************************************END OF FILE**********************/
