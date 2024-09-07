#include "./bsp/nvic/bsp_nvic.h"



    
  /**
* @brief  设置中断优先级分组  
* @param  PriorityGroup:中断优先级分组
* @retval 无
*/   
void Set_NVIC_PriorityGroup(PriorityGroup_Type PriorityGroup) 
{
  NVIC_SetPriorityGrouping((uint32_t)PriorityGroup); //设置中断优先级分组
}
    
  /**
* @brief  设置中断编号的中断优先级  
* @param  IRQn:中断编号
* @param  PreemptPriority:抢占优先级
* @param  SubPriorit:子优先级
* @retval 无
*/
void set_IRQn_Priority(IRQn_Type IRQn,PreemptPriority_Type PreemptPriority, SubPriority_Type SubPriorit)
{
  uint32_t PriorityGroup = 0;   //保存中断优先级分组
  uint32_t Priority_Encode = 0; //保存中断优先级编码
  PriorityGroup = NVIC_GetPriorityGrouping();//获得当前中断优先级分组
  Priority_Encode = NVIC_EncodePriority (PriorityGroup,(uint32_t)PreemptPriority,(uint32_t)SubPriorit);//得到中断优先级编码
  
  NVIC_SetPriority(IRQn, Priority_Encode);//设置中断编号的中断优先级
}


