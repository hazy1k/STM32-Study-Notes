#include "./pvd/bsp_pvd.h" 

// 配置PCD
void PVD_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  // 使能 PWR 时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  // 使能 PVD 中断
  NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
      
  // 配置 EXTI16线(PVD 输出) 来产生上升下降沿中断
  EXTI_ClearITPendingBit(EXTI_Line16); // 清除中断标志位
  EXTI_InitStructure.EXTI_Line = EXTI_Line16; // 选择 EXTI 线
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; // 中断模式
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // 上升沿和下降沿触发中断
  EXTI_InitStructure.EXTI_LineCmd = ENABLE; // 使能中断线
  EXTI_Init(&EXTI_InitStructure);

  // 配置PVD级别PWR_PVDLevel_2V6 (PVD检测电压的阈值为2.6V，VDD电压低于2.6V时产生PVD中断)
	// 具体级别根据自己的实际应用要求配置
  PWR_PVDLevelConfig(PWR_PVDLevel_2V6); // 配置PVD检测电压阈值

  // 使能PVD输出
  PWR_PVDCmd(ENABLE);
}