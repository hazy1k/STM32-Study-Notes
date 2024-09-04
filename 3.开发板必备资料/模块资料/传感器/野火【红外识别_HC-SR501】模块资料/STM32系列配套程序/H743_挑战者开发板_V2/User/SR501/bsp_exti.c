/**
  ******************************************************************************
  * @file    bsp_exti.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   I/O线中断应用bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 H743 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./SR501/bsp_exti.h"


 /**
  * @brief  配置 PE2 为线中断口，并设置中断优先级
  * @param  无
  * @retval 无
  */
void EXTI_SR501_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    /*开启按键GPIO口的时钟*/
    SR501_INT_GPIO_CLK_ENABLE();

    /* 选择SR501的引脚 */ 
    GPIO_InitStructure.Pin = SR501_INT_GPIO_PIN;
    /* 设置引脚为输入模式 */ 
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;	    		
    /* 设置引脚不上拉也不下拉 */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* 使用上面的结构体初始化按键 */
    HAL_GPIO_Init(SR501_INT_GPIO_PORT, &GPIO_InitStructure); 
    /* 配置 EXTI 中断源 到SR501 引脚、配置中断优先级*/
    HAL_NVIC_SetPriority(SR501_INT_EXTI_IRQ, 0, 0);
    /* 使能中断 */
    HAL_NVIC_EnableIRQ(SR501_INT_EXTI_IRQ);

}
/*********************************************END OF FILE**********************/
