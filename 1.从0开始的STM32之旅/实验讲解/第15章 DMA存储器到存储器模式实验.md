# 第十五章 DMA存储器到存储器模式实验

存储器到存储器模式可以实现数据在两个内存的快速拷贝。我们先定义一个静态的源数据，存放在内部FLASH， 然后使用DMA传输把源数据拷贝到目标地址上（内部SRAM），最后对比源数据和目标地址的数据，看看是否传输准确 。

## 1. 硬件设计

DMA存储器到存储器实验不需要其他硬件要求，只用到RGB彩色灯用于指示程序状态。

## 2. 软件设计

### 2.1 编程目的

1. 使能DMA时钟；

2. 配置DMA数据参数；

3. 使能DMA，进行传输；

4. 等待传输完成，并对源数据和目标地址数据进行比较

### 2.2 代码分析

- DMA宏定义及相关变量定义

```c
// 当使用存储器到存储器模式时候，通道可以随便选，没有硬性的规定
#define DMA_CHANNEL     DMA1_Channel6 // 我们这里使用的通道是DMA1_Channel6
#define DMA_CLOCK       RCC_AHBPeriph_DMA1 // DMA1时钟

// 传输完成标志
#define DMA_FLAG_TC     DMA1_FLAG_TC6

// 要发送的数据大小
#define BUFFER_SIZE     32

/* 定义aSRC_Const_Buffer数组作为DMA传输数据源
 * const关键字将aSRC_Const_Buffer数组变量定义为常量类型
 * 表示数据存储在内部的FLASH中
 */
const uint32_t aSRC_Const_Buffer[BUFFER_SIZE]= {
                                    0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
                                    0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
                                    0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
                                    0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
                                    0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
                                    0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
                                    0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
                                    0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80};

// 定义DMA传输目标存储器-存储在内部的SRAM中																		
uint32_t aDST_Buffer[BUFFER_SIZE];
```

使用宏定义设置外设配置方便程序修改和升级。

存储器到存储器传输通道没有硬性规定，可以随意选择。

aSRC_Const_Buffer[BUFFER_SIZE]定义用来存放源数据，并且使用了const关键字修饰，即常量类型，使得变量是存储在内部flash空间上。

- DMA配置

```c
// DMA配置函数
void DMA_Config(void)
{
        // 1.DMA初始化结构体
	    DMA_InitTypeDef DMA_InitStructure;
	
		// 2.开启DMA时钟
		RCC_AHBPeriphClockCmd(DMA_CLOCK, ENABLE);

		// 3.源数据地址
        DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)aSRC_Const_Buffer;

		// 4.目标数据地址
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)aDST_Buffer;

		// 6.设置传输方向：外设到存储器（这里的外设是内部的FLASH）	
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;

		// 7.设置传输大小	
		DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;

		// 8.设置外设（内部的FLASH）地址递增    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;

		// 9.内存地址递增
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

		// 10.外设数据单位	
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;

		// 11.内存数据单位
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;

		// 12.设置DMA模式，一次或者循环模式
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
		//DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

		// 13.设置通道优先级：高	
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;

		// 14.使能内存到内存的传输
		DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;

		// 15.配置DMA通道		   
		DMA_Init(DMA_CHANNEL, &DMA_InitStructure);

       // 16.清除DMA数据流传输完成标志位
        DMA_ClearFlag(DMA_FLAG_TC);

		// 17.使能DMA
		DMA_Cmd(DMA_CHANNEL, ENABLE);
}
```

使用DMA_InitTypeDef结构体定义一个DMA初始化变量，参考：

```c
typedef struct
{
uint32_t DMA_PeripheralBaseAddr;   // 外设地址
uint32_t DMA_MemoryBaseAddr;       // 存储器地址
uint32_t DMA_DIR;                  // 传输方向
uint32_t DMA_BufferSize;           // 传输数目
uint32_t DMA_PeripheralInc;        // 外设地址增量模式
uint32_t DMA_MemoryInc;            // 存储器地址增量模式
uint32_t DMA_PeripheralDataSize;   // 外设数据宽度
uint32_t DMA_MemoryDataSize;       // 存储器数据宽度
uint32_t DMA_Mode;                 // 模式选择
uint32_t DMA_Priority;             // 通道优先级
uint32_t DMA_M2M;                  // 存储器到存储器模式
}DMA_InitTypeDef;
```

调用RCC_AHBPeriphClockCmd函数开启DMA时钟，使用DMA控制器之前必须开启对应的时钟。

源地址和目标地址使用之前定义的数组首地址，传输的数据量为宏BUFFER_SIZE决定，源和目标地址指针地址递增， 使用一次传输模式不能循环传输，因为只有一个DMA通道，优先级随便设置，最后调用DMA_Init函数完成DMA的初始化配置。

DMA_ClearFlag函数用于清除DMA标志位，代码用到传输完成标志位，使用之前先清除传输完成标志位以免产生不必要干扰。 DMA_ClearFlag函数需要1个形参，即事件标志位，可选有传输完成标志位、半传输标志位、FIFO错误标志位、传输错误标志位等等， 非常多，我们这里选择传输完成标志位，由宏DMA_FLAG_TC定义。

DMA_Cmd函数用于启动或者停止DMA数据传输，它接收两个参数，第一个是DMA通道，另外一个是开启ENABLE或者停止DISABLE。

- 存储器数据对比

```c
/*
判断指定长度的两个数据源是否完全相等，
如果完全相等返回1，只要其中一对数据不相等返回0
*/
uint8_t Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength)
{
  /* 数据长度递减 */
  while(BufferLength--)
  {
    /* 判断两个数据源是否对应相等 */
    if(*pBuffer != *pBuffer1)
    {
      /* 对应数据源不相等马上退出函数，并返回0 */
      return 0;
    }
    /* 递增两个数据源的地址指针 */
    pBuffer++;
    pBuffer1++;
  }
  /* 完成判断并且对应数据相对 */
  return 1; // 数据完全相等
}
```

判断指定长度的两个数据源是否完全相等，如果完全相等返回1；只要其中一对数据不相等返回0。 它需要三个形参，前两个是两个数据源的地址，第三个是要比较数据长度。

- 主函数

```c
int main(void)
{
  // 1.定义存放比较结果变量
  uint8_t TransferStatus;
  
	// 2.LED 端口初始化
	LED_GPIO_Config();
    
  // 3.设置RGB彩色灯为紫色
  LED_PURPLE;  
  
  // 4.简单延时函数
  Delay(0xFFFFFF);  
  
  // 5.DMA传输配置
  DMA_Config(); 
  
  // 6.等待DMA传输完成
  while(DMA_GetFlagStatus(DMA_FLAG_TC) == RESET)
  {
    
  }   
  
  // 7.比较源数据与传输后数据
  TransferStatus = Buffercmp(aSRC_Const_Buffer, aDST_Buffer, BUFFER_SIZE);
  
  // 8.判断源数据与传输后数据比较结果
  if(TransferStatus == 0)  
  {
    /* 源数据与传输后数据不相等时RGB彩色灯显示红色 */
    LED_RED;
  }
  else
  { 
    /* 源数据与传输后数据相等时RGB彩色灯显示蓝色 */
    LED_BLUE;
  }

	while (1)
	{		

	}
}
```

首先定义一个变量用来保存存储器数据比较结果。

RGB彩色灯用来指示程序进程，使用之前需要初始化它，LED_GPIO_Config定义在bsp_led.c文件中。开始设置RGB彩色灯为紫色， LED_PURPLE是定义在bsp_led.h文件的一个宏定义。

Delay函数只是一个简单的延时函数。

调用DMA_Config函数完成DMA数据流配置并启动DMA数据传输。

DMA_GetFlagStatus函数获取DMA事件标志位的当前状态，这里获取DMA数据传输完成这个标志位，使用循环持续等待直到该标志位被置位， 即DMA传输完成这个事件发生，然后退出循环，运行之后程序。

确定DMA传输完成之后就可以调用Buffercmp函数比较源数据与DMA传输后目标地址的数据是否一一对应。TransferStatus保存比较结果， 如果为1表示两个数据源一一对应相等说明DMA传输成功；相反，如果为0表示两个数据源数据存在不等情况，说明DMA传输出错。

如果DMA传输成功设置RGB彩色灯为蓝色，如果DMA传输出错设置RGB彩色灯为红色。

## 3. 小节

这一章相比于之前的串口通信就简单多了，主要就是理解一个配置的问题，然后直接调用即可，我们可以再来梳理一下：

1. 首先宏定义DAM基本参数嘛：选择通道、时钟、传输完成标志，传输数据大小，内部flash数据，目标sram（定义一个空间用来接收）

2. 各种函数声明，不必多说

3. 最重要DMA配置：初始化结构体->开启DMA时钟->设置源数据地址（我们的flash中存储的）->设置目标存储地址（sram）->设置传输方向（DMA_DIR_PeripheralSRC，意思是外设到存储器）->设置传输大小（我们已经宏定义过了）->设置外设地址递增（ DMA_PeripheralInc_Enable）->设置内存地址递增（DMA_MemoryInc_Enable）->设置外设数据单位（DMA_PeripheralDataSize_Word）->设置内存数据单位（DMA_MemoryDataSize_Word）->设置MDA模式，这里我们选择一次模式->配置通道优先级为高->使能内存到内存传输->接下来三件套（配置结构体，清除标志位，使能）

4. 判断两个数据长度是否相等：

```c
uint8_t Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength)
{
  /* 数据长度递减 */
  while(BufferLength--)
  {
    /* 判断两个数据源是否对应相等 */
    if(*pBuffer != *pBuffer1)
    {
      /* 对应数据源不相等马上退出函数，并返回0 */
      return 0;
    }
    /* 递增两个数据源的地址指针 */
    pBuffer++;
    pBuffer1++;
  }
  /* 完成判断并且对应数据相对 */
  return 1; // 数据完全相等
}
```

5. 主函数往往最简单了，自行看注释理解吧


