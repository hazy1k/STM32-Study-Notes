/**
  ******************************************************************************
  * @file    bsp_ds18b20.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   DS18B20温度传感器应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  i.MXRT1052开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "./ds18B20/bsp_ds18b20.h"
#include "./systick/bsp_SysTick.h"
#include "./delay/core_delay.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"  
#include "pad_config.h"  
/*******************************************************************************
 * 宏
 ******************************************************************************/
/* 所有引脚均使用同样的PAD配置 */
#define DS18B20_PAD_CONFIG_DATA            (SRE_0_SLOW_SLEW_RATE| \
                                        DSE_6_R0_6| \
                                        SPEED_2_MEDIUM_100MHz| \
                                        ODE_0_OPEN_DRAIN_DISABLED| \
                                        PKE_0_PULL_KEEPER_DISABLED| \
                                        PUE_0_KEEPER_SELECTED| \
                                        PUS_0_100K_OHM_PULL_DOWN| \
                                        HYS_0_HYSTERESIS_DISABLED)   
    /* 配置说明 : */
    /* 转换速率: 转换速率慢
      驱动强度: R0/6 
      带宽配置 : medium(100MHz)
      开漏配置: 关闭 
      拉/保持器配置: 关闭
      拉/保持器选择: 保持器（上面已关闭，配置无效）
      上拉/下拉选择: 100K欧姆下拉（上面已关闭，配置无效）
      滞回器配置: 关闭 */     

/*******************************************************************************
 * 声明
 ******************************************************************************/
static void  DS18B20_IOMUXC_MUX_Config(void);
static void  DS18B20_IOMUXC_PAD_Config(void);
static void  DS18B20_GPIO_Mode_Config(void);

/* 可以在下面的宏定义中把后面的延时函数替换换SysTick的延时函数，就是想用那个就换成那个的 */

#define  DS18B20_DELAY_US(us)  CPU_TS_Tmr_Delay_US(us)
#define  DS18B20_DELAY_MS(ms)  CPU_TS_Tmr_Delay_MS(ms)
 /**
* @brief  DS18B20_Mode_IPU
* @param  无
* @retval 无
*/
static void DS18B20_Mode_IPU(void)
{
	  /* 定义gpio初始化配置结构体 */
  gpio_pin_config_t ds18b20_config;      
   /** GPIO配置 **/       
  ds18b20_config.direction = kGPIO_DigitalInput; //输入模式	
  ds18b20_config.outputLogic =  1;                //默认低电平
  ds18b20_config.interruptMode = kGPIO_NoIntmode; //不使用中断
  /* 初始化 GPIO  */
  GPIO_PinInit(DS18B20_GPIO, DS18B20_GPIO_PIN, &ds18b20_config);
}

/**
* @brief  DS18B20_Mode_Out_PP
* @param  无
* @retval 无
*/
static void  DS18B20_Mode_Out_PP(void)
{
	  /* 定义gpio初始化配置结构体 */
  gpio_pin_config_t ds18b20_config;      
   /** GPIO配置 **/       
  ds18b20_config.direction = kGPIO_DigitalOutput; //输出模式	
  ds18b20_config.outputLogic =  1;                //默认低电平
  ds18b20_config.interruptMode = kGPIO_NoIntmode; //不使用中断
  /* 初始化 GPIO */
  GPIO_PinInit(DS18B20_GPIO, DS18B20_GPIO_PIN, &ds18b20_config);
		 
}


/**
* @brief  初始化DS18B20相关IOMUXC的MUX复用配置
* @param  无
* @retval 无
*/
static void DS18B20_IOMUXC_MUX_Config(void)
{ 
  /* 设置引脚的复用模式为GPIO，不使用SION功能 */
  IOMUXC_SetPinMux(DS18B20_IOMUXC, 0U);
}

/**
* @brief  初始化DS18B20相关IOMUXC的MUX复用配置
* @param  无
* @retval 无
*/
static void DS18B20_IOMUXC_PAD_Config(void)
{
  /* 开发板的DS18B20接口 */    
  IOMUXC_SetPinConfig(DS18B20_IOMUXC, DS18B20_PAD_CONFIG_DATA);  
}


 /**
  * @brief  初始化DS18B20相关的GPIO模式
  * @param  无
  * @retval 无
  */
static void DS18B20_GPIO_Mode_Config(void)
{     
  /* 定义gpio初始化配置结构体 */
  gpio_pin_config_t ds18b20_config;      
    
   /** 开发板的DS18B20接口，GPIO配置 **/       
  ds18b20_config.direction = kGPIO_DigitalOutput; //输出模式
  ds18b20_config.outputLogic =  1;                //默认高电平
  ds18b20_config.interruptMode = kGPIO_NoIntmode; //不使用中断
  
  /* 初始化 DS18B20 GPIO. */
  GPIO_PinInit(DS18B20_GPIO, DS18B20_GPIO_PIN, &ds18b20_config);
}



/**
  * @brief  初始化控制DS18B20的IO
  * @param  无
  * @retval 无
  */
void DS18B20_GPIO_Config(void)
{
  /* 初始化GPIO复用、属性、模式 */
  DS18B20_IOMUXC_MUX_Config();
  DS18B20_IOMUXC_PAD_Config();
  DS18B20_GPIO_Mode_Config();
}
/*
 *主机给从机发送复位脉冲
 */
static void DS18B20_Rst(void)
{
	/* 主机设置为推挽输出 */
	DS18B20_Mode_Out_PP();
	
	DS18B20_DATA_OUT(DS18B20_LOW);
	
	/* 主机至少产生480us的低电平复位信号 */
	DS18B20_DELAY_US(750);

	/* 主机在产生复位信号后，需将总线拉高 */
	DS18B20_DATA_OUT(DS18B20_HIGH);
	
	/*从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲*/
	DS18B20_DELAY_US(15);
}

/**
  * @brief  检测从机给主机返回的存在脉冲
  * @param  dat 要写入的字节
  * @retval 0 成功
	* @retval 1 失败
  */
static uint8_t DS18B20_Presence(void)
{
	uint8_t pulse_time = 0;
	
	/* 主机设置为上拉输入 */
	DS18B20_Mode_IPU();
	
	/* 等待存在脉冲的到来，存在脉冲为一个60~240us的低电平信号 
	 * 如果存在脉冲没有来则做超时处理，从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲
	 */
	while( DS18B20_DATA_IN() && pulse_time<100 )
	{
		pulse_time++;
		DS18B20_DELAY_US(1);
	}	
	/* 经过100us后，存在脉冲都还没有到来*/
	if( pulse_time >=100 )
		return 1;
	else
		pulse_time = 0;
	
	/* 存在脉冲到来，且存在的时间不能超过240us */
	while( !DS18B20_DATA_IN() && pulse_time<240 )
	{
		pulse_time++;
		DS18B20_DELAY_US(1);
	}	
	if( pulse_time >=240 )
		return 1;
	else
		return 0;
}

/**
  * @brief  DS18B20读一个位
  * @retval 无
  */
static uint8_t DS18B20_Read_Bit(void)
{
	uint8_t dat;
	
	/* 读0和读1的时间至少要大于60us */	
	DS18B20_Mode_Out_PP();
	/* 读时间的起始：必须由主机产生 >1us <15us 的低电平信号 */
	DS18B20_DATA_OUT(DS18B20_LOW);
	DS18B20_DELAY_US(10);
	DS18B20_DATA_OUT(DS18B20_HIGH);
	/* 设置成输入，释放总线，由外部上拉电阻将总线拉高 */
	DS18B20_Mode_IPU();
	DS18B20_DELAY_US(12);
	
	if( DS18B20_DATA_IN() == SET )
		dat = 1;
	else
		dat = 0;
	
	/* 这个延时参数请参考时序图 */
	DS18B20_DELAY_US(50);//45
	
	return dat;
}

/**
  * @brief  DS18B20读字节
  * @retval 无
  */
uint8_t DS18B20_Read_Byte(void)
{
	uint8_t i, j, dat = 0;	
	
	for(i=0; i<8; i++) 
	{
		j = DS18B20_Read_Bit();		
		dat = (dat) | (j<<i);
	}
	
	return dat;
}

/**
  * @brief  DS18B20写字节
  * @param  dat 要写入的字节
  * @retval 无
  */
void DS18B20_Write_Byte(uint8_t dat)
{
	uint8_t i, testb;
	/*设置为输出模式*/
	DS18B20_Mode_Out_PP();
	
	for( i=0; i<8; i++ )
	{
		testb = dat&0x01;
		dat = dat>>1;		
		/* 写0和写1的时间至少要大于60us */
		if (testb)
		{			
			DS18B20_DATA_OUT(DS18B20_LOW);
			/* 1us < 这个延时 < 15us */
			DS18B20_DELAY_US(8);
			
			DS18B20_DATA_OUT(DS18B20_HIGH);
			DS18B20_DELAY_US(58);
		}		
		else
		{			
			DS18B20_DATA_OUT(DS18B20_LOW);
			/* 60us < Tx 0 < 120us */
			DS18B20_DELAY_US(70);
			
			DS18B20_DATA_OUT(DS18B20_HIGH);			
			/* 1us < Trec(恢复时间) < 无穷大*/
			DS18B20_DELAY_US(2);
		}
	}
}
 /**
  * @brief  初始化DS18B20
  * @param  无
  * @retval 无
  */
uint8_t DS18B20_Init(void)
{
	DS18B20_GPIO_Config();	
	DS18B20_Rst();
	
	return DS18B20_Presence();
}
/*
 * 存储的温度是16 位的带符号扩展的二进制补码形式
 * 当工作在12位分辨率时，其中5个符号位，7个整数位，4个小数位
 *
 *         |---------整数----------|-----小数 分辨率 1/(2^4)=0.0625----|
 * 低字节  | 2^3 | 2^2 | 2^1 | 2^0 | 2^(-1) | 2^(-2) | 2^(-3) | 2^(-4) |
 *
 *
 *         |-----符号位：0->正  1->负-------|-----------整数-----------|
 * 高字节  |  s  |  s  |  s  |  s  |    s   |   2^6  |   2^5  |   2^4  |
 *
 * 
 * 温度 = 符号位 + 整数 + 小数*0.0625
 */
float DS18B20_Get_Temp(void)
{
	uint8_t tpmsb, tplsb;
	short s_tem;
	float f_tem;
	
	DS18B20_Rst();	   
	DS18B20_Presence();	 
	DS18B20_Write_Byte(0XCC);				/* 跳过 ROM */
	DS18B20_Write_Byte(0X44);				/* 开始转换 */
	
	DS18B20_Rst();
	DS18B20_Presence();
	DS18B20_Write_Byte(0XCC);				/* 跳过 ROM */
	DS18B20_Write_Byte(0XBE);				/* 读温度值 */
	
	tplsb = DS18B20_Read_Byte();		 
	tpmsb = DS18B20_Read_Byte(); 
	
	s_tem = tpmsb<<8;
	s_tem = s_tem | tplsb;
	
	if( s_tem < 0 )		/* 负温度 */
		f_tem = (~s_tem+1) * 0.0625;	
	else
		f_tem = s_tem * 0.0625;
	
	return f_tem; 	
}

 /**
  * @brief  在匹配 ROM 情况下获取 DS18B20 温度值 
  * @param  ds18b20_id：用于存放 DS18B20 序列号的数组的首地址
  * @retval 无
  */
void DS18B20_ReadId ( uint8_t * ds18b20_id )
{
	uint8_t uc;
	
	
	DS18B20_Write_Byte(0x33);       //读取序列号
	
	for ( uc = 0; uc < 8; uc ++ )
	  ds18b20_id [ uc ] = DS18B20_Read_Byte();
	
}
