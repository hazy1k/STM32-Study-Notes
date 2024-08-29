#include "./ad7192_test/ad7192_test.h"
#include "stm32h7xx.h"
#include "./ad7192/bsp_ad7192.h"
#include "./usart/bsp_debug_usart.h"
#include "./key/bsp_key.h"

static void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/**
  * @brief  ����ת����ѹʵ��
  * @param  ��
  * @retval ��
  */
void single_conversion_voltage(void)
{
  uint32_t ad_data = 0;
  float v = 0.0;
  uint32_t mode = 0, cfg = 0;
  
  printf("Ұ�� AD9172 4 ·����ת������ѹʵ��\r\n");
  
  /* �� AD7192 �Ĵ��� */
  ReadFromAD7192ViaSPI(REG_COM_STA, 8, AD7192Registers, REG_COM_STA);
  for(int i=0; i < 8; i++)
  {
    printf("AD7192Register[%d] = 0x%06X \r\n", i+REG_COM_STA , AD7192Registers[i+REG_COM_STA]);
  }
  
       /* ����ת��|����״̬����|�ⲿʱ��|sinc4�˲���|������żУ��|ʱ�Ӳ���Ƶ|���õ�����ת��|����60Hz�ݲ�|128 */
  mode = MODE_SING|DAT_STA_DIS|EXT_XTAL|SINC_4|ENPAR_DIS|CLK_DIV_DIS|SINGLECYCLE_DIS|REJ60_DIS|1023;
  cfg  = CHOP_DIS|REF_IN1|AIN1_AIN2|BURN_DIS|REFDET_DIS|BUF_DIS|UB_BI|GAIN_1;
       /*����ն��|�ⲿ��׼��ѹ1|12���ͨ��(��)|���ü�������|���û�׼��ѹ���|����ģ�����뻺��|˫����ģʽ|����Ϊ128 */
  
  ad7192_mode_cfg_reg(mode, cfg);    // ����ģʽ�Ĵ��������üĴ���

  /* У׼ */
  printf("�ڲ�У׼��\r\n");
  AD7192InternalZeroScaleCalibration();
  AD7192InternalFullScaleCalibration();	
  printf("�ڲ�У׼���\r\n");
  
  while(1)
	{
    /* ��ͨ�� 1 ת������ */
    AD7192StartSingleConvertion(AIN1_COM);
		Delay(0xffff);
    ad_data = AD7192ReadConvertingData();
    v = (ad_data / 8388608.0 - 1)*3.3;
    printf("AIN1_COM = %fV\n", v);
    
    /* ��ͨ�� 2 ת������ */
    AD7192StartSingleConvertion(AIN2_COM);
		Delay(0xffff);
    ad_data = AD7192ReadConvertingData();
    v = (ad_data / 8388608.0 - 1)*3.3;
    printf("AIN2_COM = %fV\n", v);
    
    /* ��ͨ�� 3 ת������ */
    AD7192StartSingleConvertion(AIN3_COM);
		Delay(0xffff);
    ad_data = AD7192ReadConvertingData();
    v = (ad_data / 8388608.0 - 1)*3.3;
    printf("AIN3_COM = %fV\n", v);
    
    /* ��ͨ�� 4 ת������ */
    AD7192StartSingleConvertion(AIN4_COM);
		Delay(0xffff);
    ad_data = AD7192ReadConvertingData();
    v = (ad_data / 8388608.0 - 1)*3.3;
    printf("AIN4_COM = %fV\n", v);
	}
}
