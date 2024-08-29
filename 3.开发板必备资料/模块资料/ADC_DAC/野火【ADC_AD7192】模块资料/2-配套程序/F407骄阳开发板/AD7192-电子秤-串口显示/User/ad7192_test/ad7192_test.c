#include "./ad7192_test/ad7192_test.h"
#include "stm32f4xx_hal.h"
#include "./ad7192/bsp_ad7192.h"
#include "./usart/bsp_debug_usart.h"
#include ".\key\bsp_key.h"

/**
  * @brief  ���ӳ�ʵ��
  * @param  ��
  * @retval ��
  */
void electronic_scale_test(void)
{
  static float coefficient = 173.6f;     // ת��ϵ��
  int32_t ad_data_init = 0;
  int32_t ad_data = 0;
  uint32_t mode = 0, cfg = 0;
  
  printf("Ұ�� AD9172 ���ӳ�ʵ��\r\n");

  /* �� AD7192 �Ĵ��� */
  ReadFromAD7192ViaSPI(REG_COM_STA, 8, AD7192Registers, REG_COM_STA);
  for(int i=0; i < 8; i++)
  {
    printf("AD7192Register[%d] = 0x%06X \r\n", i+REG_COM_STA , AD7192Registers[i+REG_COM_STA]);
  }
  
       /* ����ת��|����״̬����|�ⲿʱ��|sinc4�˲���|������żУ��|ʱ�Ӳ���Ƶ|���õ�����ת��|ʹ��60Hz�ݲ�|128 */
  mode = MODE_SING|DAT_STA_DIS|EXT_XTAL|SINC_4|ENPAR_DIS|CLK_DIV_DIS|SINGLECYCLE_DIS|REJ60_EN|0x3FF;
  cfg  = CHOP_EN|REF_IN1|AIN1_AIN2|BURN_DIS|REFDET_DIS|BUF_DIS|UB_BI|GAIN_128;
       /*ʹ��ն��|�ⲿ��׼��ѹ1|12���ͨ��(��)|���ü�������|���û�׼��ѹ���|����ģ�����뻺��|˫����ģʽ|����Ϊ128 */
  
  ad7192_mode_cfg_reg(mode, cfg);    // ����ģʽ�Ĵ��������üĴ���

  /* У׼ */
  printf("�ڲ�У׼��\r\n");
  AD7192InternalZeroScaleCalibration();
  AD7192InternalFullScaleCalibration();	
  printf("�ڲ�У׼���\r\n");
  
  /* ������� */
  AD7192StartContinuousConvertion(AIN1_AIN2);    // ��������ת��

  /* ��¼��ʼֵ */
  ad_data_init = AD7192ReadConvertingData();
  
	while(1)
	{
    /* ��ת������ */
    ad_data = AD7192ReadConvertingData();

    printf("ad_data - ad_data_init = %d\n", ad_data - ad_data_init);
    printf("����Ϊ��%.2fg\n", (ad_data - ad_data_init) / coefficient);

    
    if (Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON)
    {
      /* ȥƤ ���� */
      ad_data_init = AD7192ReadConvertingData();
    }
    
    if (Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON)
    {
      /* У׼�����ڲ����Ĵ������ı仯�ʲ�һ��������ʱ������ҪУ׼��*
       * ���� 50g ������Ȼ���� KEY2                              */
      ad_data = AD7192ReadConvertingData();
      coefficient = (ad_data - ad_data_init)/50.0;
      printf("coefficient = %f\r\n", coefficient);
    }

    HAL_Delay(100);
	}
}
