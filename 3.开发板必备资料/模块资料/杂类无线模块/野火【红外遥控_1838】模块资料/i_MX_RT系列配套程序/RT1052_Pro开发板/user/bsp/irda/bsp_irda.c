/**
  ******************************************************************************
  * @file    bsp_irda.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   红外遥控器接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  i.MXRT1052开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "./irda/bsp_irda.h"
#include "./systick/bsp_SysTick.h"
#include "./bsp/nvic/bsp_nvic.h"
#include "./delay/core_delay.h"  
#include "./led/bsp_led.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"  
#include "pad_config.h"  

uint32_t frame_data=0;    /* 一帧数据缓存 */
uint8_t  frame_cnt=0;
uint8_t  frame_flag=0;    /* 一帧数据接收完成标志 */

/*******************************************************************************
 * 宏
 ******************************************************************************/
/* 所有引脚均使用同样的PAD配置 */
#define IRDA_PAD_CONFIG_DATA            (SRE_0_SLOW_SLEW_RATE| \
                                        DSE_0_OUTPUT_DRIVER_DISABLED| \
                                        SPEED_2_MEDIUM_100MHz| \
                                        ODE_0_OPEN_DRAIN_DISABLED| \
                                        PKE_1_PULL_KEEPER_ENABLED| \
                                        PUE_1_PULL_SELECTED| \
                                        PUS_3_22K_OHM_PULL_UP| \
                                        HYS_1_HYSTERESIS_ENABLED)   

    /* 配置说明 : */
    /* 转换速率: 转换速率慢
      驱动强度: 关闭
      速度配置 : medium(100MHz)
      开漏配置: 关闭 
      拉/保持器配置: 使能
      拉/保持器选择: 上下拉
      上拉/下拉选择: 22K欧姆上拉
      滞回器配置: 开启 （仅输入时有效，施密特触发器，使能后可以过滤输入噪声）*/
/*******************************************************************************
 * 声明
 ******************************************************************************/
static void  Irda_IOMUXC_MUX_Config(void);
static void  Irda_IOMUXC_PAD_Config(void);
static void  Irda_GPIO_Mode_Config(void);

/* 可以在下面的宏定义中把后面的延时函数替换换SysTick的延时函数，就是想用那个就换成那个的 */

#define  Delay_us(us)  CPU_TS_Tmr_Delay_US(us)
#define  Delay_ms(ms)  CPU_TS_Tmr_Delay_MS(ms)

/**
* @brief  初始化IRDA相关IOMUXC的MUX复用配置
* @param  无
* @retval 无
*/
static void Irda_IOMUXC_MUX_Config(void)
{ 
  /* 设置引脚的复用模式为GPIO，不使用SION功能 */
  IOMUXC_SetPinMux(IRDA_IOMUXC, 0U);
}

/**
* @brief  初始化IRDA相关IOMUXC的MUX复用配置
* @param  无
* @retval 无
*/
static void Irda_IOMUXC_PAD_Config(void)
{
  /* 开发板的IRDA接口 */    
  IOMUXC_SetPinConfig(IRDA_IOMUXC, IRDA_PAD_CONFIG_DATA);  
}

 /**
  * @brief  初始化IRDA相关的GPIO模式
  * @param  无
  * @retval 无
  */
static void Irda_GPIO_Mode_Config(void)
{     
  /* 定义gpio初始化配置结构体 */
  gpio_pin_config_t irda_config;      
   
  irda_config.direction = kGPIO_DigitalInput; //输入模式
  irda_config.outputLogic =  1;                //默认高电平
	irda_config.interruptMode = kGPIO_IntFallingEdge; //下降沿中断
	
  /* 初始化 IRDA GPIO. */
  GPIO_PinInit(IRDA_GPIO, IRDA_GPIO_PIN, &irda_config);
}

/**
 * @brief  初始化中断相关的内容
 * @param  无
 * @retval 无
 */
static void Irda_Interrupt_Config(void)   
{
  /* 开启GPIO引脚的中断 */
  GPIO_PortEnableInterrupts(IRDA_GPIO, 1U << IRDA_GPIO_PIN);                             
  /*设置中断优先级,*/
	set_IRQn_Priority(IRDA_IRQ,Group4_PreemptPriority_6, Group4_SubPriority_1);  
  /* 开启GPIO端口中断 */
  EnableIRQ(IRDA_IRQ);
}


/**
  * @brief  初始化控制Irda的IO
  * @param  无
  * @retval 无
  */
void Irda_GPIO_Config(void)
{
  /* 初始化GPIO复用、属性、模式 以及中断*/
  Irda_IOMUXC_MUX_Config();
  Irda_IOMUXC_PAD_Config();
  Irda_GPIO_Mode_Config();
	Irda_Interrupt_Config();
}
 
/* 初始化红外接收头1838用到的IO */
void IrDa_Init(void)
{
	Irda_GPIO_Config();
}

/* 获取高电平的时间 */
uint8_t Get_Pulse_Time(void)
{
  uint8_t time = 0;
  while( IrDa_DATA_IN() )
  {
    time ++;
    Delay_us(20);     // 延时 20us
    if(time == 250)
      return time;   // 超时溢出   
  }
  return time;
}

/*
 * 帧数据有4个字节，第一个字节是遥控的ID，第二个字节是第一个字节的反码
 * 第三个数据是遥控的真正的键值，第四个字节是第三个字节的反码
 */
uint8_t IrDa_Process(void)
{
  uint8_t first_byte, sec_byte, tir_byte, fou_byte;  
  
  first_byte = frame_data >> 24;
  sec_byte = (frame_data>>16) & 0xff;
  tir_byte = frame_data >> 8;
  fou_byte = frame_data;
  
  /* 记得清标志位 */
  frame_flag = 0;
  
  if( (first_byte==(uint8_t)~sec_byte) && (first_byte==IRDA_ID) )
  {
    if( tir_byte == (uint8_t)~fou_byte )
      return tir_byte;
  }
  
  return 0;   /* 错误返回 */
}
/********************中断服务函数**************************/
/**
 * @brief  GPIO 输入中断服务函数
 *         IRDA_IRQHandler只是一个宏，
 *         中断服务函数名是固定的，可以在启动文件中找到。
 * @param  中断服务函数不能有输入参数
 * @note   中断函数一般只使用标志位进行指示，完成后尽快退出，
 *         具体操作或延时尽量不放在中断服务函数中
 * @retval 中断服务函数不能有返回值
 */
uint8_t isr_cnt;  /* 用于计算进了多少次中断 */  
void IRDA_IRQHandler(void)
{ 
		uint8_t pulse_time = 0;
		uint8_t leader_code_flag = 0; /* 引导码标志位，当引导码出现时，表示一帧数据开始 */
		uint8_t irda_data = 0;        /* 数据暂存位 */
    /* 清除中断标志位 */
    RGB_RED_LED_TOGGLE
		while(1)
		{
			if( IrDa_DATA_IN()== SET )        /* 只测量高电平的时间 */
			{       
				pulse_time = Get_Pulse_Time();
				
				/* >=5ms 不是有用信号 当出现干扰或者连发码时，也会break跳出while(1)循环 */
				if( pulse_time >= 250 )                
				{
					break; /* 跳出while(1)循环 */
				}
				
				if(pulse_time>=200 && pulse_time<250)         /* 获得前导位 4ms~4.5ms */
				{
					leader_code_flag = 1;
				}
				else if(pulse_time>=10 && pulse_time<50)      /* 0.56ms: 0.2ms~1ms */
				{
					irda_data = 0;
				}
				else if(pulse_time>=50 && pulse_time<100)     /* 1.68ms：1ms~2ms */
				{
					irda_data =1 ; 
				}        
				else if( pulse_time>=100 && pulse_time<=200 ) /* 2.1ms：2ms~4ms */
				{/* 连发码，在第二次中断出现 */
					frame_flag = 1;               /* 一帧数据接收完成 */
					frame_cnt++;                  /* 按键次数加1 */
					isr_cnt ++;                   /* 进中断一次加1 */
					break;                        /* 跳出while(1)循环 */
				}
				
				if( leader_code_flag == 1 )
				{/* 在第一次中断中完成 */
					frame_data <<= 1;
					frame_data += irda_data;
					frame_cnt = 0;
					isr_cnt = 1;
				}
			}      
			GPIO_PortClearInterruptFlags(IRDA_GPIO,1U << IRDA_GPIO_PIN);  
	}  
	
	    /* 以下部分是为 ARM 的勘误838869添加的, 
       该错误影响 Cortex-M4, Cortex-M4F内核，       
       立即存储覆盖重叠异常，导致返回操作可能会指向错误的中断
        CM7不受影响，此处保留该代码
    */
	 /* 原注释：Add for ARM errata 838869, affects Cortex-M4,
       Cortex-M4F Store immediate overlapping
       exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
/*********************************************END OF FILE**********************/
