#include "i2c.h"
#include "usart.h"

uint16_t EEPROM_ADDRESS;
uint32_t I2CTimeout = I2CT_LONG_TIMEOUT; // 超时时间初始化

// I2C GPIO配置
void I2C_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE); // I2C时钟使能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // GPIOB时钟使能
    GPIO_InitStructure.GPIO_Pin = I2C_SCL_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // 复用开漏输出
    GPIO_Init(I2C_SCL_Port, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = I2C_SDA_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // 复用开漏输出
    GPIO_Init(I2C_SDA_Port, &GPIO_InitStructure);
}

// I2C工作模式配置
void I2C_Mode_Config(void)
{
    I2C_InitTypeDef  I2C_InitStructure;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C; // I2C模式
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; // 时钟占空比
    I2C_InitStructure.I2C_OwnAddress1 = I2Cx_OWM_ADDRESS; // 自己的地址
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; // 使能应答
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // 7位地址模式
    I2C_InitStructure.I2C_ClockSpeed = I2C_Speend; // 高速模式-400KHz
    I2C_Init(I2Cx, &I2C_InitStructure);
    I2C_Cmd(I2Cx, ENABLE);
}

// I2C外设初始化（EEPROM）
void I2C_EEPROM_Init(void)
{
    I2C_GPIO_Config();
    I2C_Mode_Config();
    // 根据头文件i2c.h中的电压来选择EEPROM的地址
    #ifdef EEPROM_Block0_ADDRESS
        EEPROM_ADDRESS = EEPROM_Block0_ADDRESS;
    #endif
    #ifdef EEPROM_Block1_ADDRESS
        EEPROM_ADDRESS = EEPROM_Block1_ADDRESS;
    #endif
    #ifdef EEPROM_Block2_ADDRESS
        EEPROM_ADDRESS = EEPROM_Block2_ADDRESS;
    #endif
    #ifdef EEPROM_Block3_ADDRESS
        EEPROM_ADDRESS = EEPROM_Block3_ADDRESS;
    #endif
}

// 自定义超时时间回调函数，返回错误码
// 函数参数：errorCode：错误码
uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
    EEPROM_ERROR("I2C TIMEOUT ERROR! errorCode = %d\r\n", errorCode);
    return 0;
}

// 等待EEPROM进入待机模式
void I2C_EEPROM_WaitEepromStandbyState(void)
{
    vu16 Temp_SR1 = 0;
    do
    {
        I2C_GenerateSTART(I2Cx, ENABLE); // I2C1开始信号
        Temp_SR1 = I2C_ReadRegister(I2Cx, I2C_Register_SR1); // 读取I2C1的状态寄存器1
        I2C_Send7bitAddress(I2Cx, EEPROM_ADDRESS, I2C_Direction_Receiver); // 发送EEPROM的地址
    } while(!(I2C_ReadRegister(I2Cx, I2C_Register_SR1) & 0x0002)); // 等待EEPROM进入待机模式
    I2C_ClearFlag(I2Cx, I2C_FLAG_AF); // 清除I2C1的中断标志位
    I2C_GenerateSTOP(I2Cx, ENABLE); // I2C1停止信号
}

// 通过I2C写一个字节到EEPROM中
// 函数参数：pBuffer：写入的数据缓冲区；WriteAddr：写入的起始地址
uint32_t I2C_EEPROM_ByetWrite(uint8_t* pBuffer, uint8_t WriteAddr)
{
    I2C_GenerateSTART(I2Cx, ENABLE); // I2C1开始信号
    I2CTimeout = I2CT_FLAG_TIMEOUT; // 超时时间初始化
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) // 等待从机响应
    {
        if((I2CTimeout--) == 0)
        {
            return I2C_TIMEOUT_UserCallback(0); // 超时处理,返回错误码0，代表I2C没有启动成功
        }
    }
    I2CTimeout = I2CT_FLAG_TIMEOUT; // 超时时间初始化
    I2C_Send7bitAddress(I2Cx, EEPROM_ADDRESS, I2C_Direction_Transmitter); // 发送EEPROM的地址和写命令
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) // 等待从机响应
    {
        if((I2CTimeout--) == 0)
        {
            return I2C_TIMEOUT_UserCallback(1); // 超时处理,返回错误码1，代表发送EEPROM地址失败
        }
    }
    I2C_SendData(I2Cx, WriteAddr); // 发送写入的起始地址
    I2CTimeout = I2CT_FLAG_TIMEOUT; // 超时时间初始化
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // 等待从机响应
    {
        if((I2CTimeout--) == 0)
        {
            return I2C_TIMEOUT_UserCallback(2); // 超时处理,返回错误码2，代表发送写入地址失败
        }     
    }
    I2C_SendData(I2Cx, *pBuffer); // 发送写入的数据
    I2CTimeout = I2CT_FLAG_TIMEOUT; // 超时时间初始化
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // 等待从机响应
    {
        if((I2CTimeout--) == 0)
        {
            return I2C_TIMEOUT_UserCallback(3); // 超时处理,返回错误码3，代表发送写入数据失败
        }     
    }
    I2C_GenerateSTOP(I2Cx, ENABLE); // I2C1停止信号
    return 1; // 写入成功
}

// 在EEPROM一次写入多个字节
// 但一次写入的字节数不能超过EEPROM页的大小，AT24C02每页有8个字节
// 函数参数：pBuffer：写入的数据缓冲区；WriteAddr：写入的起始地址；NumByteToWrite：写入的字节数
uint32_t I2C_EEPROM_PageWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite)
{
    I2CTimeout = I2CT_LONG_TIMEOUT; // 超时时间初始化
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) // 等待EEPROM空闲
    {
        if((I2CTimeout--) == 0)
        {
            return I2C_TIMEOUT_UserCallback(4); // 超时处理,返回错误码4，代表EEPROM忙
        }
    }
    I2C_GenerateSTART(I2Cx, ENABLE); // I2C1开始信号
    I2CTimeout = I2CT_LONG_TIMEOUT; // 超时时间初始化
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) // 等待从机响应
    {
        if((I2CTimeout--) == 0)
        {
            return I2C_TIMEOUT_UserCallback(5); // 超时处理,返回错误码5，代表I2C没有启动成功
        }
    }
    I2C_Send7bitAddress(I2Cx, EEPROM_ADDRESS, I2C_Direction_Transmitter); // 发送EEPROM的地址和写命令
    I2CTimeout = I2CT_FLAG_TIMEOUT; // 超时时间初始化
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) // 等待从机响应
    {
        if((I2CTimeout--) == 0)
        {
            return I2C_TIMEOUT_UserCallback(6); // 超时处理,返回错误码6，代表发送EEPROM地址失败
        }
    }
    I2C_SendData(I2Cx, WriteAddr); // 发送写入的起始地址
    I2CTimeout = I2CT_FLAG_TIMEOUT; // 超时时间初始化
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // 等待从机响应
    {
        if((I2CTimeout--) == 0)
        {
            return I2C_TIMEOUT_UserCallback(7); // 超时处理,返回错误码7，代表发送写入地址失败
        }
    }
    while(NumByteToWrite--) // 循环写入数据
    {
        I2C_SendData(I2Cx, *pBuffer); // 发送写入的数据
        pBuffer++; // 数据缓冲区指针递增
        I2CTimeout = I2CT_FLAG_TIMEOUT; // 超时时间初始化
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // 等待从机响应
        {
            if((I2CTimeout--) == 0)
            {
                return I2C_TIMEOUT_UserCallback(8); // 超时处理,返回错误码8，代表发送写入数据失败
            }
        }
    }
    I2C_GenerateSTOP(I2Cx, ENABLE); // I2C1停止信号
    return 1; // 写入成功
}

// 将缓冲区的数据写入到EEPROM中
// 函数参数：pBuffer：写入的数据缓冲区；WriteAddr：写入的起始地址；NumByteToWrite：写入的字节数
void I2C_EEPROM_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite)
{
    u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0; // 定义变量
    Addr = WriteAddr % I2C_PageSize; // 计算起始地址所在的页的偏移地址
    count = I2C_PageSize - Addr; // 计算起始地址所在的页剩余的字节数
    NumOfPage = NumByteToWrite / I2C_PageSize; // 计算需要写入的页数
    NumOfSingle = NumByteToWrite % I2C_PageSize; // 计算剩余的单字节数

    // 如果WriteAddr是I2C_PageSize的整数倍，则不需要写入起始地址所在的页
    if(Addr == 0)
    {
        // 如果NumByteTOWrite < I2C_PageSiZe
        if(NumOfPage == 0)
        {
            I2C_EEPROM_PageWrite(pBuffer, WriteAddr, NumOfSingle); // 写入剩余的单字节数
            I2C_EEPROM_WaitEepromStandbyState(); // 等待EEPROM进入待机模式
        }
        // 如果NumByteTOWrite > I2C_PageSiZe
        else
        {
            while(NumOfPage--)
            {
                I2C_EEPROM_PageWrite(pBuffer, WriteAddr, I2C_PageSize); // 写入I2C_PageSize字节
                I2C_EEPROM_WaitEepromStandbyState(); // 等待EEPROM进入待机模式
                WriteAddr += I2C_PageSize; // 页地址递增
                pBuffer += I2C_PageSize; // 数据缓冲区指针递增
            }
            if(NumOfSingle != 0) // 如果剩余的字节数不为0
            {
                I2C_EEPROM_PageWrite(pBuffer, WriteAddr, NumOfSingle); // 写入剩余的单字节数
                I2C_EEPROM_WaitEepromStandbyState(); // 等待EEPROM进入待机模式
            }
        }
    }
    else // 如果WriteAddr不是I2C_PageSize的整数倍
    {
        if(NumOfPage == 0)
        {
            I2C_EEPROM_PageWrite(pBuffer, WriteAddr, NumOfSingle); // 写入剩余的单字节数
            I2C_EEPROM_WaitEepromStandbyState(); // 等待EEPROM进入待机模式
        }
        else
        {
            NumByteToWrite -= count; // 计算剩余的字节数
            NumOfPage = NumByteToWrite / I2C_PageSize; // 计算需要写入的页数
            NumOfSingle = NumByteToWrite % I2C_PageSize; // 计算剩余的单字节数

            if(count != 0) // 如果起始地址所在的页的字节数不为0
            {
                I2C_EEPROM_PageWrite(pBuffer, WriteAddr, count); // 写入起始地址所在的页的字节数
                I2C_EEPROM_WaitEepromStandbyState(); // 等待EEPROM进入待机模式
                WriteAddr += count; // 页地址递增
                pBuffer += count; // 数据缓冲区指针递增
            }   
            while(NumOfPage--)
            {
                I2C_EEPROM_PageWrite(pBuffer, WriteAddr, I2C_PageSize); // 写入I2C_PageSize字节
                I2C_EEPROM_WaitEepromStandbyState(); // 等待EEPROM进入待机模式
                WriteAddr += I2C_PageSize; // 页地址递增
                pBuffer += I2C_PageSize; // 数据缓冲区指针递增
            }
            if(NumOfSingle != 0)
            {
                I2C_EEPROM_PageWrite(pBuffer, WriteAddr, NumOfSingle); // 写入剩余的单字节数
                I2C_EEPROM_WaitEepromStandbyState(); // 等待EEPROM进入待机模式
            }
        }       
    }
}

// 从EEPROM中读取数据
// 函数参数：pBuffer：读取的数据缓冲区；ReadAddr：读取的起始地址；NumByteToRead：读取的字节数
uint32_t I2C_EEPROM_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint8_t NumByteToRead)
{
    I2CTimeout = I2CT_LONG_TIMEOUT; // 超时时间初始化
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) // 等待EEPROM空闲
    {
        if((I2CTimeout--) == 0)
        {
            return I2C_TIMEOUT_UserCallback(9); // 超时处理,返回错误码9，代表EEPROM忙
        }
    }
    I2C_GenerateSTART(I2Cx, ENABLE); // I2C1开始信号
    I2CTimeout = I2CT_FLAG_TIMEOUT; // 超时时间初始化
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) // 等待从机响应
    {
        if((I2CTimeout--) == 0)
        {
            return I2C_TIMEOUT_UserCallback(10); // 超时处理,返回错误码10，代表I2C没有启动成功
        }
    }
    I2C_Send7bitAddress(I2Cx, EEPROM_ADDRESS, I2C_Direction_Transmitter); // 发送EEPROM的地址和读命令
    I2CTimeout = I2CT_FLAG_TIMEOUT; // 超时时间初始化
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) // 等待从机响应
    {
        if((I2CTimeout--) == 0)
        {
            return I2C_TIMEOUT_UserCallback(11); // 超时处理,返回错误码11，代表发送EEPROM地址失败
        }
    }
    I2C_Cmd(I2Cx, ENABLE); // 使能I2C1
    I2C_SendData(I2Cx, ReadAddr); // 发送读取的起始地址
    I2CTimeout = I2CT_FLAG_TIMEOUT; // 超时时间初始化
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // 等待从机响应
    {
        if((I2CTimeout--) == 0)
        {
            return I2C_TIMEOUT_UserCallback(12); // 超时处理,返回错误码12，代表发送读取地址失败
        }
    }
    I2C_GenerateSTART(I2Cx, ENABLE); // I2C1开始信号
    I2CTimeout = I2CT_FLAG_TIMEOUT; // 超时时间初始化
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) // 等待从机响应
    {
        if((I2CTimeout--) == 0)
        {
            return I2C_TIMEOUT_UserCallback(13); // 超时处理,返回错误码13，代表I2C没有启动成功
        }
    }
    I2C_Send7bitAddress(I2Cx, EEPROM_ADDRESS, I2C_Direction_Receiver); // 发送EEPROM的地址和读命令
    I2CTimeout = I2CT_FLAG_TIMEOUT; // 超时时间初始化
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) // 
    {
        if((I2CTimeout--) == 0)
        {
            return I2C_TIMEOUT_UserCallback(14); // 超时处理,返回错误码14，代表发送EEPROM地址失败
        }
    }
    while(NumByteToRead)
    {
        if(NumByteToRead == 1) // 如果读取的字节数为1
        {
            I2C_AcknowledgeConfig(I2Cx, DISABLE); // 禁止ACK
            I2C_GenerateSTOP(I2Cx, ENABLE); // I2C1停止信号
        }
        I2CTimeout = I2CT_LONG_TIMEOUT; // 超时时间初始化
        while(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) == RESET) // 等待从机响应
        {
            if((I2CTimeout--) == 0)
            {
                return I2C_TIMEOUT_UserCallback(15); // 超时处理,返回错误码15，代表读取数据失败
            }
        }
        {
        *pBuffer = I2C_ReceiveData(I2Cx); // 读取数据
        pBuffer++; // 数据缓冲区指针递增
        NumByteToRead--; // 读取字节数递减
        }
    }
    I2C_AcknowledgeConfig(I2Cx, ENABLE); // 使能ACK
    return 1; // 读取成功
}
