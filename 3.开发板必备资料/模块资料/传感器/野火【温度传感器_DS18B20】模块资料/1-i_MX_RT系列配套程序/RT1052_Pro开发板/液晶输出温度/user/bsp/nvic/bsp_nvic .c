#include "./bsp/nvic/bsp_nvic.h"



    
  /**
* @brief  �����ж����ȼ�����  
* @param  PriorityGroup:�ж����ȼ�����
* @retval ��
*/   
void Set_NVIC_PriorityGroup(PriorityGroup_Type PriorityGroup) 
{
  NVIC_SetPriorityGrouping((uint32_t)PriorityGroup); //�����ж����ȼ�����
}
    
  /**
* @brief  �����жϱ�ŵ��ж����ȼ�  
* @param  IRQn:�жϱ��
* @param  PreemptPriority:��ռ���ȼ�
* @param  SubPriorit:�����ȼ�
* @retval ��
*/
void set_IRQn_Priority(IRQn_Type IRQn,PreemptPriority_Type PreemptPriority, SubPriority_Type SubPriorit)
{
  uint32_t PriorityGroup = 0;   //�����ж����ȼ�����
  uint32_t Priority_Encode = 0; //�����ж����ȼ�����
  PriorityGroup = NVIC_GetPriorityGrouping();//��õ�ǰ�ж����ȼ�����
  Priority_Encode = NVIC_EncodePriority (PriorityGroup,(uint32_t)PreemptPriority,(uint32_t)SubPriorit);//�õ��ж����ȼ�����
  
  NVIC_SetPriority(IRQn, Priority_Encode);//�����жϱ�ŵ��ж����ȼ�
}


