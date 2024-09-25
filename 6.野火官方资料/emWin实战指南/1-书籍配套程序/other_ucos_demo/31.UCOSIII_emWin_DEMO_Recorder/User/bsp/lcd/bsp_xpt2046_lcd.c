#include "..\User\bsp\lcd\bsp_ili9341_lcd.h"
#include "..\User\bsp\lcd\bsp_xpt2046_lcd.h"
#include <stdio.h> 
#include <string.h>



/******************************* ���� XPT2046 ��صľ�̬���� ***************************/
static void                   XPT2046_GPIO_SPI_Config               ( void );

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
	XPT2046_GPIO_SPI_Config ();		
}



/**
  * @brief  ���� XPT2046 ��ģ��SPI
  * @param  ��
  * @retval ��
  */	
static void XPT2046_GPIO_SPI_Config ( void ) 
{ 
  GPIO_InitTypeDef  GPIO_InitStructure;
	

  /* ����GPIOʱ�� */
  RCC_APB2PeriphClockCmd ( macXPT2046_SPI_GPIO_CLK | macXPT2046_INT_GPIO_CLK, ENABLE );

  /* ģ��SPI GPIO��ʼ�� */          
  GPIO_InitStructure.GPIO_Pin=macXPT2046_SPI_CLK_PIN;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz ;	  
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_Init(macXPT2046_SPI_CLK_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = macXPT2046_SPI_MOSI_PIN;
  GPIO_Init(macXPT2046_SPI_MOSI_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = macXPT2046_SPI_MISO_PIN; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      
  GPIO_Init(macXPT2046_SPI_MISO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = macXPT2046_SPI_CS_PIN; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      
  GPIO_Init(macXPT2046_SPI_CS_PORT, &GPIO_InitStructure); 
   
	GPIO_InitStructure.GPIO_Pin = macXPT2046_INT_GPIO_PIN;       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // ��������
  GPIO_Init(macXPT2046_INT_GPIO_PORT, &GPIO_InitStructure); 
  /* ����Ƭѡ��ѡ��XPT2046 */
   macXPT2046_CS_ENABLE();
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


	macXPT2046_MOSI_0();
	
	macXPT2046_CLK_LOW();

	for ( i = 0; i < 8; i ++ ) 
	{
		( ( ucCmd >> ( 7 - i ) ) & 0x01 ) ? macXPT2046_MOSI_1() : macXPT2046_MOSI_0();
		
	  XPT2046_DelayUS ( 5 );
		
		macXPT2046_CLK_HIGH();

	  XPT2046_DelayUS ( 5 );

		macXPT2046_CLK_LOW();
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
	


	macXPT2046_MOSI_0();

	macXPT2046_CLK_HIGH();

	for ( i=0;i<12;i++ ) 
	{
		macXPT2046_CLK_LOW();    
	
		usTemp = macXPT2046_MISO();
		
		usBuf |= usTemp << ( 11 - i );
	
		macXPT2046_CLK_HIGH();
		
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

  if(macXPT2046_INT_Read()== macXPT2046_INT_ActiveLevel) 
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
















