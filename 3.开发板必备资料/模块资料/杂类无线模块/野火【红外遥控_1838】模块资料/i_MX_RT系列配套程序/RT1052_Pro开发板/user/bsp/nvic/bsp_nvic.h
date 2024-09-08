#ifndef __NVIC_H
#define __NVIC_H

#include "fsl_common.h"

/*定义中断优先级分组 */
typedef enum PriorityGroup{
  Group_4 = 4,
  Group_5,
  Group_6,
  Group_7,
}PriorityGroup_Type;

/*定义抢占优先级*/
typedef enum PreemptPriority {
  Group4_PreemptPriority_0 = 0,
  Group4_PreemptPriority_2,
  Group4_PreemptPriority_3,
  Group4_PreemptPriority_4,
  Group4_PreemptPriority_5,
  Group4_PreemptPriority_6,
  Group4_PreemptPriority_7,
  
  Group5_PreemptPriority_0 = 0,
  Group5_PreemptPriority_1,
  Group5_PreemptPriority_2,
  Group5_PreemptPriority_3,
  
  Group6_PreemptPriority_0 = 0,
  Group6_PreemptPriority_1,
  
  Group7_PreemptPriority_0 = 0
}PreemptPriority_Type;

/*定义子优先级*/
typedef enum SubPriority {
  Group4_SubPriority_0 = 0,
  Group4_SubPriority_1,
  
  Group5_SubPriority_0 = 0,
  Group5_SubPriority_1,
  Group5_SubPriority_2,
  Group5_SubPriority_3,
  
  
  Group6_SubPriority_0 = 0,
  Group6_SubPriority_2,
  Group6_SubPriority_3,
  Group6_SubPriority_4,
  Group6_SubPriority_5,
  Group6_SubPriority_6,
  Group6_SubPriority_7,
  
  Group7_SubPriority_0 = 0,
  Group7_SubPriority_1,
  Group7_SubPriority_2,
  Group7_SubPriority_3,
  Group7_SubPriority_4,
  Group7_SubPriority_5,
  Group7_SubPriority_6,
  Group7_SubPriority_7,
  Group7_SubPriority_8,
  Group7_SubPriority_9,
  Group7_SubPriority_10,
  Group7_SubPriority_11,
  Group7_SubPriority_12,
  Group7_SubPriority_13,
  Group7_SubPriority_14,
  Group7_SubPriority_15
  
}SubPriority_Type;


void Set_NVIC_PriorityGroup(PriorityGroup_Type PriorityGroup) ;
void set_IRQn_Priority(IRQn_Type IRQn,PreemptPriority_Type PreemptPriority, SubPriority_Type SubPriorit);


#endif /* __NVIC_H */

