/**
  ******************************************************************************
  * @file    bsp_ili9341_lcd.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./lcd/bsp_xpt2046_lcd.h"
#include "./lcd/bsp_ili9341_lcd_emwin.h"
#include "./flash/bsp_spi_flash.h"
#include <stdio.h> 
#include <string.h>

/******************************* ���� XPT2046 ��صľ�̬���� ***************************/
static void                   XPT2046_DelayUS                       ( __IO uint32_t ulCount );
static void                   XPT2046_WriteCMD                      ( uint8_t ucCmd );
static uint16_t               XPT2046_ReadCMD                       ( void );

/**
  * @brief  XPT2046 ��ʼ������
  * @param  ��
  * @retval ��
  */	
void XPT2046_Init ( void )
{

  GPIO_InitTypeDef  GPIO_InitStructure = {0};
	

  /* ����GPIOʱ�� */
  RCC_APB2PeriphClockCmd ( XPT2046_SPI_GPIO_CLK|XPT2046_PENIRQ_GPIO_CLK, ENABLE );

  /* ģ��SPI GPIO��ʼ�� */          
  GPIO_InitStructure.GPIO_Pin=XPT2046_SPI_CLK_PIN;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz ;	  
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_Init(XPT2046_SPI_CLK_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_MOSI_PIN;
  GPIO_Init(XPT2046_SPI_MOSI_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_MISO_PIN; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      
  GPIO_Init(XPT2046_SPI_MISO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_CS_PIN; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      
  GPIO_Init(XPT2046_SPI_CS_PORT, &GPIO_InitStructure); 
   
  /* ����Ƭѡ��ѡ��XPT2046 */
  XPT2046_CS_DISABLE();		
								
	//�����������ź�ָʾ����
  GPIO_InitStructure.GPIO_Pin = XPT2046_PENIRQ_GPIO_PIN;       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // ��������
  GPIO_Init(XPT2046_PENIRQ_GPIO_PORT, &GPIO_InitStructure);	
}


/**
  * @brief  ���� XPT2046 �ļ�΢�뼶��ʱ����
  * @param  nCount ����ʱ����ֵ����λΪ΢��
  * @retval ��
  */	
static void XPT2046_DelayUS ( __IO uint32_t ulCount )
{
	uint32_t i;


	for ( i = 0; i < ulCount; i ++ )
	{
		uint8_t uc = 12;     //����ֵΪ12����Լ��1΢��  
	      
		while ( uc -- );     //��1΢��	

	}
	
}

/**
  * @brief  XPT2046 ��д������
  * @param  ucCmd ������
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg 0x90 :ͨ��Y+��ѡ�������
  *     @arg 0xd0 :ͨ��X+��ѡ�������
  * @retval ��
  */
static void XPT2046_WriteCMD ( uint8_t ucCmd ) 
{
	uint8_t i;


	XPT2046_MOSI_0();
	
	XPT2046_CLK_LOW();

	for ( i = 0; i < 8; i ++ ) 
	{
		( ( ucCmd >> ( 7 - i ) ) & 0x01 ) ? XPT2046_MOSI_1() : XPT2046_MOSI_0();
		
	  XPT2046_DelayUS ( 5 );
		
		XPT2046_CLK_HIGH();

	  XPT2046_DelayUS ( 5 );

		XPT2046_CLK_LOW();
	}
}

/**
  * @brief  XPT2046 �Ķ�ȡ����
  * @param  ��
  * @retval ��ȡ��������
  */
static uint16_t XPT2046_ReadCMD ( void ) 
{
	uint8_t i;
	uint16_t usBuf=0, usTemp;
	
  
	XPT2046_MOSI_0();

	XPT2046_CLK_HIGH();

	for ( i=0;i<12;i++ ) 
	{
		XPT2046_CLK_LOW();    
	
		usTemp = XPT2046_MISO();
		
		usBuf |= usTemp << ( 11 - i );
	
		XPT2046_CLK_HIGH();
	}
	return usBuf;
}

#define SAMP_CNT 4
#define SAMP_CNT_DIV2 2
/*
*********************************************************************************************************
*	�� �� ��: XPT2046_ReadAdc_Fliter
*	����˵��: ѡ��һ��ģ��ͨ��������ADC��������ADC�������
*	��    �Σ�_ucCh = 0x90 ��ʾYͨ���� 0xd0 ��ʾXͨ��
*	�� �� ֵ: �˲����12λADCֵ
*********************************************************************************************************
*/
uint16_t XPT2046_ReadAdc_Fliter(uint8_t channel)
{
  u8 i, j, min;
  u16 temp;
  u16 tempXY[SAMP_CNT];
  
  static uint16_t adc_x = 0,adc_y = 0;

  if(XPT2046_PENIRQ_Read()== XPT2046_PENIRQ_ActiveLevel) 
  {
    for(i = 0;i < SAMP_CNT;i++)
    {
      XPT2046_WriteCMD(channel);
      tempXY[i] = XPT2046_ReadCMD();     
    }
    // ��������
    for(i=0; i<SAMP_CNT-1; i++)
    {
      min=i;
      for (j=i+1; j<SAMP_CNT; j++)
      {
        if (tempXY[min] > tempXY[j]) min=j;
      }
      temp = tempXY[i];
      tempXY[i] = tempXY[min];
      tempXY[min] = temp;
    }
   
    // �趨��ֵ
    if((tempXY[SAMP_CNT_DIV2]-tempXY[SAMP_CNT_DIV2-1]) > 5)
    {
      /* �������м�ֵ���̫Զ����������������ݣ�������һ�εĴ�������*/
      if(channel == macXPT2046_CHANNEL_Y )
        return adc_x; //xͨ��
      else
        return adc_y; //yͨ��
    
    }
    
   // ���м�ֵ�ľ�ֵ   
   if(channel == macXPT2046_CHANNEL_Y)
   {
     adc_x = (tempXY[SAMP_CNT_DIV2]+tempXY[SAMP_CNT_DIV2-1]) / 2;
      return adc_x;
    }      
    else
    {
      adc_y = (tempXY[SAMP_CNT_DIV2]+tempXY[SAMP_CNT_DIV2-1]) / 2;
      return adc_y;
    }
       
  }
  else
  {
    return 0;   //û�д���������0
  }
}


///**
//	* @brief   ��⵽�����ж�ʱ���õĴ�����,ͨ��������tp_down ��tp_up�㱨������
//	*	@note 	 ��������Ҫ��whileѭ���ﱻ���ã�Ҳ��ʹ�ö�ʱ����ʱ����
//	*			���磬����ÿ��5ms����һ�Σ�������ֵ��DURIATION_TIME������Ϊ2������ÿ�������Լ��100���㡣
//	*						����XPT2046_TouchDown��XPT2046_TouchUp�����б�д�Լ��Ĵ���Ӧ��
//	* @param   none
//	* @retval  none
//	*/
//void XPT2046_TouchEvenHandler(void )
//{
//	  static strType_XPT2046_Coordinate cinfo={-1,-1,-1,-1};
//	
//		if(XPT2046_TouchDetect() == TOUCH_PRESSED)
//		{
//			LED_GREEN;
//			
//			//��ȡ��������
//			XPT2046_Get_TouchedPoint(&cinfo,strXPT2046_TouchPara);
//			
//			//���������Ϣ������
//			XPT2046_DEBUG("x=%d,y=%d",cinfo.x,cinfo.y);
//			
//			//���ô���������ʱ�Ĵ����������ڸú�����д�Լ��Ĵ������´������
//			XPT2046_TouchDown(&cinfo);
//			
//			/*���´�����Ϣ��pre xy*/
//			cinfo.pre_x = cinfo.x; cinfo.pre_y = cinfo.y;  

//		}
//		else
//		{
//			LED_BLUE;
//			
//			//���ô������ͷ�ʱ�Ĵ����������ڸú�����д�Լ��Ĵ����ͷŴ������
//			XPT2046_TouchUp(&cinfo); 
//			
//			/*�����ͷţ��� xy ����Ϊ��*/
//			cinfo.x = -1;
//			cinfo.y = -1; 
//			cinfo.pre_x = -1;
//			cinfo.pre_y = -1;
//		}

//}


/***************************end of file*****************************/


