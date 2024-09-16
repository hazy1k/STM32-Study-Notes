#include "./pvd/bsp_pvd.h" 

// ����PCD
void PVD_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  // ʹ�� PWR ʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  // ʹ�� PVD �ж�
  NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
      
  // ���� EXTI16��(PVD ���) �����������½����ж�
  EXTI_ClearITPendingBit(EXTI_Line16); // ����жϱ�־λ
  EXTI_InitStructure.EXTI_Line = EXTI_Line16; // ѡ�� EXTI ��
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; // �ж�ģʽ
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // �����غ��½��ش����ж�
  EXTI_InitStructure.EXTI_LineCmd = ENABLE; // ʹ���ж���
  EXTI_Init(&EXTI_InitStructure);

  // ����PVD����PWR_PVDLevel_2V6 (PVD����ѹ����ֵΪ2.6V��VDD��ѹ����2.6Vʱ����PVD�ж�)
	// ���弶������Լ���ʵ��Ӧ��Ҫ������
  PWR_PVDLevelConfig(PWR_PVDLevel_2V6); // ����PVD����ѹ��ֵ

  // ʹ��PVD���
  PWR_PVDCmd(ENABLE);
}