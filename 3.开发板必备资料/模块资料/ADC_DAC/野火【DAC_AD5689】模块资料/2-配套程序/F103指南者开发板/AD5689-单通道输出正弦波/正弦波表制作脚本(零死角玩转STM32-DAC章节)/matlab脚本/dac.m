
%����ͨ���趨 period��prescaler���DAC����������ź�Ƶ��

TIM_Period    = 19;                %��ʱ������ 
TIM_Prescaler = 0;                 %��ʱʱ�ӷ�Ƶ
point =32;                         %�������źŵ��� 
Tsysclk=1/72000000;                 %ϵͳʱ������
Ttim = Tsysclk * (TIM_Prescaler+1); %��ʱʱ������
Tupdate = Ttim * (TIM_Period +1);   %��ʱ����������
Tsin = Tupdate * point;             %һ�����Ҳ������ڵ�dac����
fsin = 1/Tsin                       %���Ҳ�Ƶ��


