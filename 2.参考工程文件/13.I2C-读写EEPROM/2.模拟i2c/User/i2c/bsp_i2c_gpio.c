#include "bsp_i2c_gpio.h"
#include <stm32f10x.h>

static void i2c_CfgGpio(void); // 初始化模拟I2C引脚

// 模拟I2C总线位延迟，最快400KHz
static void i2c_Delay(void)
{
	uint8_t i;
	/*　
	 	下面的时间是通过逻辑分析仪测试得到的。
    	工作条件：CPU主频72MHz ，MDK编译环境，1级优化
  
		循环次数为10时，SCL频率 = 205KHz 
		循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us 
	 	循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us 
	*/
	for (i = 0; i < 10; i++);
}

// CPU发起I2C总线启动信号
void i2c_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	EEPROM_I2C_SDA_1(); // 拉高SDA
	EEPROM_I2C_SCL_1(); // 拉高SCL
	i2c_Delay();
	EEPROM_I2C_SDA_0(); // 拉低SDA，产生启动信号
	i2c_Delay();
	EEPROM_I2C_SCL_0(); // 拉低SCL，产生一个时钟
	i2c_Delay();
}

// CPU发起I2C总线停止信号
void i2c_Stop(void)
{
	// 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号
	EEPROM_I2C_SDA_0(); // 拉低SDA
	EEPROM_I2C_SCL_1(); // 拉高SCL
	i2c_Delay();
	EEPROM_I2C_SDA_1(); // 拉高SDA，产生停止信号
}

// CPU向I2C总线设备发送8bit数据
void i2c_SendByte(uint8_t _ucByte) // 函数参数：要发送的8bit数据
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			EEPROM_I2C_SDA_1(); // 发送高电平
		}
		else
		{
			EEPROM_I2C_SDA_0(); // 发送低电平
		}
		i2c_Delay();
		EEPROM_I2C_SCL_1(); // 拉高SCL
		i2c_Delay();	
		EEPROM_I2C_SCL_0(); // 拉低SCL，产生一个时钟
		if (i == 7)
		{
			 EEPROM_I2C_SDA_1(); // 释放总线
		}
		_ucByte <<= 1;	// 左移一个bit
		i2c_Delay();
	}
}

// CPU从I2C总线设备读取8bit数据
uint8_t i2c_ReadByte(void)
{
	uint8_t i; // 要读取的bit位数
	uint8_t value; // 读取到的8bit数据

	// 读到第1个bit为数据的bit7 
	value = 0;
	for(i = 0; i < 8; i++)
	{
		value <<= 1;
		EEPROM_I2C_SCL_1();
		i2c_Delay();
		if (EEPROM_I2C_SDA_READ())
		{
			value++;
		}
		EEPROM_I2C_SCL_0();
		i2c_Delay();
	}
	return value;
}

// CPU产生一个时钟，并读取器件的ACK应答信号
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	EEPROM_I2C_SDA_1();	// CPU释放SDA总线
	i2c_Delay();
	EEPROM_I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	i2c_Delay();
	if (EEPROM_I2C_SDA_READ()) // CPU读取SDA口线状态
	{
		re = 1; // 返回正常
	}
	else
	{
		re = 0; // 返回异常
	}
	EEPROM_I2C_SCL_0(); // 拉低SCL，产生一个时钟
	i2c_Delay();
	return re; // 返回ACK应答结果
}

// CPU产生一个ACK信号
void i2c_Ack(void)
{
	EEPROM_I2C_SDA_0();	/* CPU驱动SDA = 0 */
	i2c_Delay();
	EEPROM_I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	EEPROM_I2C_SCL_0();
	i2c_Delay();
	EEPROM_I2C_SDA_1();	/* CPU释放SDA总线 */
}

// CPU产生1个NACK信号
void i2c_NAck(void)
{
	EEPROM_I2C_SDA_1();	/* CPU驱动SDA = 1 */
	i2c_Delay();
	EEPROM_I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	EEPROM_I2C_SCL_0();
	i2c_Delay();	
}

// 配置I2C总线的GPIO，采用模拟IO的方式实现
static void i2c_CfgGpio(void)
{
	// 1.开头第一步，先定义一个结构体
	GPIO_InitTypeDef GPIO_InitStructure;

	// 2.使能GPIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	// 3.配置GPIO引脚
	GPIO_InitStructure.GPIO_Pin = EEPROM_I2C_SCL_PIN | EEPROM_I2C_SDA_PIN;

	// 4.设置GPIO为开漏输出模式，速度50MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  	/* 开漏输出 */

	// 5.初始化GPIO
	GPIO_Init(EEPROM_GPIO_PORT_I2C, &GPIO_InitStructure);

	// 6.给一个停止信号，目的是恢复到初始状态
	i2c_Stop();
}

// 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
uint8_t i2c_CheckDevice(uint8_t _Address) // 函数参数：要检测的设备地址
{
	uint8_t ucAck; // 设备应答结果

	i2c_CfgGpio(); // GPIO初始化
	i2c_Start();   // 发送启动信号

	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	i2c_SendByte(_Address | EEPROM_I2C_WR);

	ucAck = i2c_WaitAck(); // 检测设备应答

	i2c_Stop(); // 传输完成，产生停止信号

	return ucAck; // 返回设备应答结果
}