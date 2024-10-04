/**
  ******************************************************************************
  * @file    bsp_creat_S_tab.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   S�Ӽ������ɱ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "./stepper/bsp_creat_S_tab.h"

/*

��������õ�����ѧ�ȱ��ٵ���ѧģ�ͣ������ٶ�Ҳ��

�������ٱ仯�ģ�����׼ȷ��˵�Ǽ��ٶȵ��ȱ���ѧģ�͡�

a-t �������� ��V-t���� �뿴 �ĵ���

	| 
 a|   /|\             
	|  / | \
	| /  |  \            
	|/___|___\_t 		
	0   t/2
		
���Ͼ��Ǽ��ٶȵ��ȱ�ģ�ͣ�Ĭ�ϳ�ʼ�ļ��ٶ�Ϊ0��

��ô�������ģ�;ͺ����׿������һЩ��Ϊ��֪�����ܣ�����

�����������ϵ�ģ�;Ϳ������ÿһʱ�̵ļ��ٶȣ������ȿ� 

0~t/2 ��� a=K*T ������KΪ���ٶ�a��б��

�����ٶȻ��־����ٶȣ����� V = a dt = K*T dt

�ã� V = K*T^2/2

���ٶȻ��־���λ�ƣ����� S = V dt = K*T^2/2 dt 

�ã� S = K*T^3/6

*/

Speed_s Speed ;
uint8_t print_flag=0;


/**
  * @brief  ������ٱ�
  * @param  Vo  ��ʼ�ٶ�
  * @param  Vt	tʱ�̵��ٶ�
  * @param  T	�������ʱ��
  *	@note 		��
  * @retval ��
  */

void CalculateSpeedTab(int Vo, int Vt, float T)
{
	int32_t i = 0;

	int32_t Vm =0;      // �м���ٶ�
	float K = 0;   		// �Ӽ��ٶ� ���ٶȵ�б��
	float Tn = 0;     	// Tnʱ��
	float DeltaV = 0; 	// ÿһʱ�̵��ٶ�
	float TimeDel = 0;	// ʱ����

	Speed.Vo = CONVER(Vo);    // ����:Step/s
	Speed.Vt = CONVER(Vt);    // ĩ��:Step/s
	
	T = T / 2;						//�Ӽ��ٶε�ʱ�䣨���ٶ�б��>0��ʱ�䣩
	
	Vm = (Speed.Vo + Speed.Vt) / 2;	//�����е���ٶ�
	
	K = ( ( 2 * ((Vm) - (Speed.Vo)) ) / pow((T),2) );// �����е��ٶȼ���Ӽ��ٶ�
	
	Speed.AccelHalfStep  = (int)( ( (K) * pow( (T) ,3) ) / 6 );// �Ӽ�����Ҫ�Ĳ���
	
	/* �����ڴ�ռ����ٶȱ� */
	Speed.AccelHalfStep  = abs(Speed.AccelHalfStep ); // ���ټ����ʱ���ֹ���ָ���
	if( Speed.AccelHalfStep  % 2 != 0)     // ���ڸ���������ת�����������ݴ��������,���������1
	{
		Speed.AccelHalfStep  += 1;
	}
	Speed.AccelTotalStep = Speed.AccelHalfStep * 2;           // ���ٶεĲ���
	
	if(FORM_LEN<Speed.AccelTotalStep)
	{
		printf("FORM_LEN ���泤�Ȳ���\r\n,�뽫 FORM_LEN �޸�Ϊ %d \r\n",Speed.AccelTotalStep);
		return ;
	}
	  
	/* Ŀ����ٶ������Ƕ�ʱ��ķ���,��ʱ���벽����Ӧ�������ڴ˽�ʱ��ֳ�
		�벽����Ӧ�ķ���,���Ҽ���� ,�����Ϳ��Լ������Ӧ���ٶ�*/
	TimeDel = T / Speed.AccelHalfStep;

	for(i = 0; i <= Speed.AccelHalfStep; i++)
	{
		Tn = i * TimeDel;						// ��i��ʱ�̵�Tn
		DeltaV = 0.5f * K * pow(Tn,2);        	// ��Tnʱ������Ӧ���ٶ�  dv = 1/2 * K * t^2;
		Speed.Form[i] = Speed.Vo + DeltaV;		// �õ�ÿһʱ�̶�Ӧ���ٶ�,���ڿ��ǵ����ٶȲ�Ϊ0�������������Vo������
																				
		Speed.Form [ Speed.AccelTotalStep - i] = Speed.Vt - DeltaV ;        // �Ӽ��ٹ���������������ĶԳ�,����ֱ����������ٶ�
																			// �����ٹ��̶ԳƵ���ٶ�
	}
	/* ����ٶȱ����� */
	for(i = 0; i <= Speed.AccelTotalStep ; i++)
	{
		printf("i,%.3f;speed,%.3f\n",(float)i,Speed.Form[i]);	
	}
	/* ��ձ� */
	memset(Speed.Form,0,FORM_LEN*sizeof(float));
}






