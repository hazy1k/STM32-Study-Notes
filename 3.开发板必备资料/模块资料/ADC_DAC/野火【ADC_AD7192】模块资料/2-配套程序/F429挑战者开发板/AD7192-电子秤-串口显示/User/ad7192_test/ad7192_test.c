#include "./ad7192_test/ad7192_test.h"
#include "stm32f4xx_hal.h"
#include "./ad7192/bsp_ad7192.h"
#include "./usart/bsp_debug_usart.h"
#include ".\key\bsp_key.h"

/**
  * @brief  电子秤实验
  * @param  无
  * @retval 无
  */
void electronic_scale_test(void)
{
  static float coefficient = 173.6f;     // 转换系数
  int32_t ad_data_init = 0;
  int32_t ad_data = 0;
  uint32_t mode = 0, cfg = 0;
  
  printf("野火 AD9172 电子秤实验\r\n");

  /* 读 AD7192 寄存器 */
  ReadFromAD7192ViaSPI(REG_COM_STA, 8, AD7192Registers, REG_COM_STA);
  for(int i=0; i < 8; i++)
  {
    printf("AD7192Register[%d] = 0x%06X \r\n", i+REG_COM_STA , AD7192Registers[i+REG_COM_STA]);
  }
  
       /* 单次转换|禁用状态传输|外部时钟|sinc4滤波器|禁用奇偶校验|时钟不分频|禁用单周期转换|使能60Hz陷波|128 */
  mode = MODE_SING|DAT_STA_DIS|EXT_XTAL|SINC_4|ENPAR_DIS|CLK_DIV_DIS|SINGLECYCLE_DIS|REJ60_EN|0x3FF;
  cfg  = CHOP_EN|REF_IN1|AIN1_AIN2|BURN_DIS|REFDET_DIS|BUF_DIS|UB_BI|GAIN_128;
       /*使能斩波|外部基准电压1|12差分通道(单)|禁用激励电流|禁用基准电压检测|禁用模拟输入缓冲|双极性模式|增益为128 */
  
  ad7192_mode_cfg_reg(mode, cfg);    // 配置模式寄存器和配置寄存器

  /* 校准 */
  printf("内部校准中\r\n");
  AD7192InternalZeroScaleCalibration();
  AD7192InternalFullScaleCalibration();	
  printf("内部校准完成\r\n");
  
  /* 差分输入 */
  AD7192StartContinuousConvertion(AIN1_AIN2);    // 启动连续转换

  /* 记录初始值 */
  ad_data_init = AD7192ReadConvertingData();
  
	while(1)
	{
    /* 读转换数据 */
    ad_data = AD7192ReadConvertingData();

    printf("ad_data - ad_data_init = %d\n", ad_data - ad_data_init);
    printf("重量为：%.2fg\n", (ad_data - ad_data_init) / coefficient);

    
    if (Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON)
    {
      /* 去皮 清零 */
      ad_data_init = AD7192ReadConvertingData();
    }
    
    if (Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON)
    {
      /* 校准，由于不懂的传感器的变化率不一样所以有时可能需要校准，*
       * 放上 50g 的砝码然后按下 KEY2                              */
      ad_data = AD7192ReadConvertingData();
      coefficient = (ad_data - ad_data_init)/50.0;
      printf("coefficient = %f\r\n", coefficient);
    }

    HAL_Delay(100);
	}
}
