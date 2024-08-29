#include "./ad7192_test/ad7192_test.h"
#include "stm32h7xx_hal.h"
#include "./ad7192/bsp_ad7192.h"
#include "./usart/bsp_debug_usart.h"
#include ".\key\bsp_key.h"
#include "./delay/core_delay.h"   

/**
  * @brief  2路差分连续转换电压实验
  * @param  无
  * @retval 无
  */
void difference_continuous_conversion_voltage(void)
{
  uint32_t ad_data = 0;
  float v = 0.0;
  uint32_t mode = 0, cfg = 0;
  
  printf("野火 AD9172 2路 差分连续转换读电压实验\r\n");
  
  /* 读 AD7192 寄存器 */
  ReadFromAD7192ViaSPI(REG_COM_STA, 8, AD7192Registers, REG_COM_STA);
  for(int i=0; i < 8; i++)
  {
    printf("AD7192Register[%d] = 0x%06X \r\n", i+REG_COM_STA , AD7192Registers[i+REG_COM_STA]);
  }
  
	HAL_Delay(1);

	/* 单次转换|使能状态传输|外部时钟|sinc4滤波器|禁用奇偶校验|时钟不分频|禁用单周期转换|禁用60Hz陷波|128 */
  mode = MODE_SING|DAT_STA_EN|EXT_XTAL|SINC_4|ENPAR_DIS|CLK_DIV_DIS|SINGLECYCLE_DIS|REJ60_DIS|1023;
  cfg  = CHOP_DIS|REF_IN1|AIN1_AIN2|BURN_DIS|REFDET_DIS|BUF_DIS|UB_BI|GAIN_1;
       /*禁用斩波|外部基准电压1|12差分通道(单)|禁用激励电流|禁用基准电压检测|禁用模拟输入缓冲|双极性模式|增益为128 */
  
  ad7192_mode_cfg_reg(mode, cfg);    // 配置模式寄存器和配置寄存器

  /* 校准 */
  printf("内部校准中，请等待\r\n");
  AD7192InternalZeroScaleCalibration();
  AD7192InternalFullScaleCalibration();	

	
  AD7192StartContinuousConvertion(AIN1_AIN2|AIN3_AIN4);    // 启动连续转换
  AD7192StartContinuousRead();	
	while(1)
	{
    ad_data = AD7192ReadConvertingData();

    switch(ad_data & 7)
    {
      case 0:
        v = ((ad_data >> 8) / 8388608.0 - 1)*3.3;
        printf("AIN1_AIN2 = %fV\n", v);
      break;
      
      case 1:
        v = ((ad_data >> 8) / 8388608.0 - 1)*3.3;
        printf("AIN3_AIN4 = %fV\n", v);
      break;
      
    }
	}
}
