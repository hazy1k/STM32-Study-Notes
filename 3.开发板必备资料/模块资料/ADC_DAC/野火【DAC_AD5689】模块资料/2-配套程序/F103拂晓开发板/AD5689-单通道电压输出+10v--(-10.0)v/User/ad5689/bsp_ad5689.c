#include "./ad5689/bsp_ad5689.h"
#include "./usart/bsp_debug_usart.h"


SPI_HandleTypeDef hspi_AD5689;
/**
  * @brief   AD5689引脚和SPI初始化
  * @param   	 
  * @retval  
  */
void AD5689_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  /* 使能SPI外设以及SPI引脚时钟 */
//  AD5689_SPIx_CLK_ENABLE();
//  AD5689_GPIO_CLK_ENABLE();
		AD5689_SYNC_CLK_ENABLE();
		AD5689_SCK_CLK_ENABLE();
		AD5689_MISO_CLK_ENABLE();
		AD5689_MOSI_CLK_ENABLE();
	
//  GPIO_InitStruct.Pin = AD5689_LDAC_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
//  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(AD5689_LDAC_GPIO_Port, &GPIO_InitStruct);	
//	
//  GPIO_InitStruct.Pin = AD5689_RESET_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(AD5689_RESET_GPIO_Port, &GPIO_InitStruct);		
	
  GPIO_InitStruct.Pin = AD5689_SCK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(AD5689_SCK_GPIO_Port, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = AD5689_MISO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(AD5689_MISO_GPIO_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = AD5689_MOSI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(AD5689_MOSI_GPIO_Port, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = AD5689_SYNC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(AD5689_SYNC_GPIO_Port, &GPIO_InitStruct);  
  
	  /* 片选使能 */
  AD5689_SYNC_L;

	AD5689_SCLK_H;			      //时钟拉高，时钟拉高要干写入的事

}

/**
  * @brief   延时函数
  * @param   	 
  * @retval  
  */
static void delay(void)
{
  uint16_t i;
  for(i=0;i<50;i++)
  {
    __NOP(); 
  }  
}

/**
  * @brief   设置AD5689寄存器值
  * @param   command：指令
  *          channel：通道
  *          data   ：数据
  * @retval 
  */
void AD5689_SetRegisterValue(uint8_t command,uint16_t channel,uint16_t data)
{
  uint8_t reg[3]={0};
  
  reg[0]=(command<<4)|channel;
  reg[1]=(data>>8)&0xFF;
  reg[2]=data&0xFF;

  AD5689_SYNC_L;
  //delay();
  //HAL_SPI_Transmit(&hspi_AD5689,reg,3,0xF);
	spi_send_byte(reg[0]);
	spi_send_byte(reg[1]);
	spi_send_byte(reg[2]);
  //delay();
  AD5689_SYNC_H;
}

static void spi_send_byte(uint8_t data)
{
    uint8_t i;
    AD5689_SCLK_H;//时钟拉高
    for(i = 0; i < 8; i++)
    {
        AD5689_SCLK_H;
        //delay();
        if(0x80 == (data & 0x80))
            HAL_GPIO_WritePin(AD5689_MOSI_GPIO_Port, AD5689_MOSI_Pin,GPIO_PIN_SET);	      //Send one to SDI pin   发送1至SDI引脚
        else
            HAL_GPIO_WritePin(AD5689_MOSI_GPIO_Port, AD5689_MOSI_Pin,GPIO_PIN_RESET);	    //Send zero to SDI pin   发送0至SDI引脚
        //delay();
        data <<= 1;
				AD5689_SCLK_L;
    }
		AD5689_SCLK_H;//时钟拉高
}

/**
  * @brief  LDAC屏蔽寄存器设置
  * @param  LDAC功能选择
  * @retval 无
  */
void AD5689_LDAC_MASK(uint16_t ldac1,uint16_t ldac2)
{
  uint8_t data=ldac1;
  data |=(ldac2<<3);
  AD5689_SetRegisterValue(CMD_LDAC_MASK,DAC_ALL,data);
}

/**
  * @brief  软件复位
  * @param  无
  * @retval 无
  */
void AD5689_Reset(void)
{
  AD5689_SetRegisterValue(CMD_RESET,DAC_ALL,0);
}


/**
  * @brief  掉电设置
  * @param  channel1：通道1的掉电模式
  *         channel2：通道2的掉电模式
  * @retval 无
  */
void AD5689_PowerDown(uint16_t channel1,uint16_t channel2)
{
  uint8_t data=0x3C;
  data |=((channel2<<6)|channel1);
  AD5689_SetRegisterValue(CMD_POWER_DOWN,DAC_ALL,data);
}


/**
  * @brief  写入和更新DAC通道n(与LDAC无关)
  * @param  channel：通道
  *         data   ：数值
  * @retval 无
  */
void AD5689_WriteUpdate_DACREG(uint16_t channel,uint16_t data)
{
  AD5689_SetRegisterValue(CMD_WRITE_UPDATEn,channel,data);
}

/**
  * @brief  写入输入寄存器n(取决于LDAC)
  * @param  channel：通道
  *         data   ：数值
  * @retval 无
  */
void AD5689_Write_InputREG(uint16_t channel,uint16_t data)
{
  AD5689_SetRegisterValue(CMD_WRITEn,channel,data);
}

/**
  * @brief  以输入寄存器n的内容更新DAC寄存器n
  * @param  channel：通道
  *         data   ：数值
  * @retval 无
  */
void AD5689_InputREG_toUpdate_DACREG(uint16_t channel,uint16_t data)
{
  AD5689_SetRegisterValue(CMD_WRITEn_UPDATEn,channel,data);
}

/**
  * @brief  LDAC屏蔽寄存器设置
  * @param  菊花链操作
  * @retval 无
  */
void AD5689_DAISYCHAIN_OPERATION(uint16_t decn)
{
  AD5689_SetRegisterValue(CMD_DCEN_REG,DAC_ALL,decn);
}

/**
  * @brief  AD5689初始化
  * @param  无
  * @retval 无
  */
void AD5689_Init(void)
{
	AD5689_GPIO_Config();
  AD5689_RESET_L;
  HAL_Delay(5);
  AD5689_RESET_H;
  HAL_Delay(5);
  AD5689_Reset();
  AD5689_LDAC_MASK(LDAC_Disable,LDAC_Disable);
}
