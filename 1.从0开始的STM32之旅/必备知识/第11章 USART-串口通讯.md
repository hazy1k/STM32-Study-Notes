# 第十一章 USART-串口通讯

## 1. 串口通讯协议简介

    串口通讯(Serial Communication)是一种设备间非常常用的串行通讯方式，因为它简单便捷，因此大部分电子设备都支持该通讯方式， 电子工程师在调试设备时也经常使用该通讯方式输出调试信息。

### 1.1 物理层

    串口通讯的物理层有很多标准及变种，我们主要讲解RS-232标准 ，RS-232标准主要规定了信号的用途、通讯接口以及信号的电平标准。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/USART002.jpg)

    在上面的通讯方式中，两个通讯设备的“DB9接口”之间通过串口信号线建立起连接，串口信号线中使用“RS-232标准”传输数据信号。 由于RS-232电平标准的信号不能直接被控制器直接识别，所以这些信号会经过一个“电平转换芯片”转换成控制器能识别的“TTL标准”的电平信号，才能实现通讯。

#### 1.1.1 电平标准

    根据通讯使用的电平标准不同，串口通讯可分为TTL标准及RS-232标准，见表

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/USART01.png)

    我们知道常见的电子电路中常使用TTL的电平标准，理想状态下，使用5V表示二进制逻辑1，使用0V表示逻辑0； 而为了增加串口通讯的远距离传输及抗干扰能力，它使用-15V表示逻辑1，+15V表示逻辑0。 使用RS232与TTL电平校准表示同一个信号时的对比见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/USART003.jpg)

    因为控制器一般使用TTL电平标准，所以常常会使用MAX3232芯片对TTL及RS-232电平的信号进行互相转换。

#### 1.1.2 RS-232信号线

    在最初的应用中，RS-232串口标准常用于计算机、路由与调制调解器(MODEN，俗称“猫”)之间的通讯 ，在这种通讯系统中， 设备被分为数据终端设备DTE(计算机、路由)和数据通讯设备DCE(调制调解器)。我们以这种通讯模型讲解它们的信号线连接方式及各个信号线的作用。

    在旧式的台式计算机中一般会有RS-232标准的COM口(也称DB9接口)，见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/USART004.jpg)

    其中接线口以针式引出信号线的称为公头，以孔式引出信号线的称为母头。在计算机中一般引出公头接口，而在调制调解器设备中引出的一般为母头，使用上图中的串口线即可把它与计算机连接起来。通讯时，串口线中传输的信号就是使用前面讲解的RS-232标准调制的。

    在这种应用场合下，DB9接口中的公头及母头的各个引脚的标准信号线接法见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/USART005.jpg)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/USART02.png)

    上表中的是计算机端的DB9公头标准接法，由于两个通讯设备之间的收发信号(RXD与TXD)应交叉相连， 所以调制调解器端的DB9母头的收发信号接法一般与公头的相反，两个设备之间连接时，只要使用“直通型”的串口线连接起来即可， 见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/USART006.jpg)

    串口线中的RTS、CTS、DSR、DTR及DCD信号，使用逻辑 1表示信号有效，逻辑0表示信号无效。 例如，当计算机端控制DTR信号线表示为逻辑1时，它是为了告知远端的调制调解器，本机已准备好接收数据，0则表示还没准备就绪。

    在目前的其它工业控制使用的串口通讯中，一般只使用RXD、TXD以及GND三条信号线， 直接传输数据信号，而RTS、CTS、DSR、DTR及DCD信号都被裁剪掉了。

### 1.2 协议层

    串口通讯的数据包由发送设备通过自身的TXD接口传输到接收设备的RXD接口。在串口通讯的协议层中， 规定了数据包的内容，它由启始位、主体数据、校验位以及停止位组成，通讯双方的数据包格式要约定一致才能正常收发数据， 其组成见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/USART007.jpg)

#### 1.2.1 波特率

    本章中主要讲解的是串口异步通讯，异步通讯中由于没有时钟信号(如前面讲解的DB9接口中是没有时钟信号的)， 所以两个通讯设备之间需要约定好波特率，即每个码元的长度，以便对信号进行解码， 图 [中用虚线分开的每一格就是代表一个码元。常见的波特率为4800、9600、115200等。

#### 1.2.2 通讯的起始和停止信号

    串口通讯的一个数据包从起始信号开始，直到停止信号结束。数据包的起始信号由一个逻辑0的数据位表示， 而数据包的停止信号可由0.5、1、1.5或2个逻辑1的数据位表示，只要双方约定一致即可。

#### 1.2.3 有效数据

    在数据包的起始位之后紧接着的就是要传输的主体数据内容，也称为有效数据，有效数据的长度常被约定为5、6、7或8位长。

#### 1.2.4 数据校验

    在有效数据之后，有一个可选的数据校验位。由于数据通信相对更容易受到外部干扰导致传输数据出现偏差， 可以在传输过程加上校验位来解决这个问题。校验方法有奇校验(odd)、偶校验(even)、0校验(space)、1校验(mark)以及无校验(noparity)。

    奇校验要求有效数据和校验位中“1”的个数为奇数，比如一个8位长的有效数据为：01101001，此时总共有4个“1”， 为达到奇校验效果，校验位为“1”，最后传输的数据将是8位的有效数据加上1位的校验位总共9位。

    偶校验与奇校验要求刚好相反，要求帧数据和校验位中“1”的个数为偶数， 比如数据帧：11001010，此时数据帧“1”的个数为4个，所以偶校验位为“0”。

    0校验是不管有效数据中的内容是什么，校验位总为“0”，1校验是校验位总为“1”。

## 2. STM32的USART简介

    通用同步异步收发器(Universal Synchronous Asynchronous Receiver and Transmitter)是一个串行通信设备， 可以灵活地与外部设备进行全双工数据交换。 有别于USART还有一个UART(Universal Asynchronous Receiver and Transmitter)， 它是在USART基础上裁剪掉了同步通信功能，只有异步通信。 简单区分同步和异步就是看通信时需不需要对外提供时钟输出，我们平时用的串口通信基本都是UART。

    串行通信一般是以帧格式传输数据，即是一帧一帧的传输，每帧包含有起始信号、数据信息、停止信息， 可能还有校验信息。USART就是对这些传输参数有具体规定，当然也不是只有唯一一个参数值，很多参数值都可以自定义设置，只是增强它的兼容性。

    USART满足外部设备对工业标准NRZ异步串行数据格式的要求，并且使用了小数波特率发生器， 可以提供多种波特率，使得它的应用更加广泛。USART支持同步单向通信和半双工单线通信；还支持局域互连网络LIN、 智能卡(SmartCard)协议与lrDA(红外线数据协会) SIR ENDEC规范。

    USART支持使用DMA，可实现高速数据通信，有关DMA具体应用将在DMA章节作具体讲解。

    USART在STM32应用最多莫过于“打印”程序信息，一般在硬件设计时都会预留一个USART通信接口连接电脑， 用于在调试程序是可以把一些调试信息“打印”在电脑端的串口调试助手工具上，从而了解程序运行是否正确、如果出错哪具体哪里出错等等。

## 3. USART功能框图

    USART的功能框图包含了USART最核心内容，掌握了功能框图，对USART就有一个整体的把握， 在编程时就思路就非常清晰。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/USART008.png)

- 功能引脚

    TX： 发送数据输出引脚。

    RX： 接收数据输入引脚。

    SW_RX： 数据接收引脚，只用于单线和智能卡模式，属于内部引脚，没有具体外部引脚。

    nRTS： 请求以发送(Request To Send)，n表示低电平有效。如果使能RTS流控制，当USART接收器准备好接收新数据时就会将nRTS变成低电平； 当接收寄存器已满时，nRTS将被设置为高电平。该引脚只适用于硬件流控制。

    nCTS： 清除以发送(Clear To Send)，n表示低电平有效。如果使能CTS流控制，发送器在发送下一帧数据之前会检测nCTS引脚， 如果为低电平，表示可以发送数据，如果为高电平则在发送完当前数据帧之后停止发送。该引脚只适用于硬件流控制。

    SCLK： 发送器时钟输出引脚。这个引脚仅适用于同步模式。

    USART引脚在STM32F103VET6芯片具体分布见表

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/USART03.png)

    STM32F103VET6系统控制器有三个USART和两个UART，其中USART1和时钟来源于APB2总线时钟，其最大频率为72MHz， 其他四个的时钟来源于APB1总线时钟，其最大频率为36MHz。UART只是异步传输功能，所以没有SCLK、nCTS和nRTS功能引脚。

- 数据寄存器

    USART数据寄存器(USART_DR)只有低9位有效，并且第9位数据是否有效要取决于USART控制寄存器1(USART_CR1)的M位设置， 当M位为0时表示8位数据字长，当M位为1表示9位数据字长，我们一般使用8位数据字长。

    USART_DR包含了已发送的数据或者接收到的数据。USART_DR实际是包含了两个寄存器，一个专门用于发送的可写TDR， 一个专门用于接收的可读RDR。当进行发送操作时，往USART_DR写入数据会自动存储在TDR内；当进行读取操作时，向USART_DR读取数据会自动提取RDR数据。

    TDR和RDR都是介于系统总线和移位寄存器之间。串行通信是一个位一个位传输的，发送时把TDR内容转移到发送移位寄存器， 然后把移位寄存器数据每一位发送出去，接收时把接收到的每一位顺序保存在接收移位寄存器内然后才转移到RDR。

    USART支持DMA传输，可以实现高速数据传输，具体DMA使用将在DMA章节讲解。

- 控制器

    USART有专门控制发送的发送器、控制接收的接收器，还有唤醒单元、中断控制等等。 使用USART之前需要向USART_CR1寄存器的UE位置1使能USART，UE位用来开启供给给串口的时钟。

    发送或者接收数据字长可选8位或9位，由USART_CR1的M位控制。

- 发送器

    当USART_CR1寄存器的发送使能位TE置1时，启动数据发送，发送移位寄存器的数据会在TX引脚输出， 低位在前，高位在后。如果是同步模式SCLK也输出时钟信号。

    一个字符帧发送需要三个部分：起始位+数据帧+停止位。起始位是一个位周期的低电平，位周期就是每一位占用的时间； 数据帧就是我们要发送的8位或9位数据，数据是从最低位开始传输的；停止位是一定时间周期的高电平。

    停止位时间长短是可以通过USART控制寄存器2(USART_CR2)的STOP[1:0]位控制，可选0.5个、1个、1.5个和2个停止位。 默认使用1个停止位。2个停止位适用于正常USART模式、单线模式和调制解调器模式。0.5个和1.5个停止位用于智能卡模式。

    当选择8位字长，使用1个停止位时，具体发送字符时序图见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/USART009.png)

    当发送使能位TE置1之后，发送器开始会先发送一个空闲帧(一个数据帧长度的高电平)，接下来就可以往USART_DR寄存器写入要发送的数据。 在写入最后一个数据后，需要等待USART状态寄存器(USART_SR)的TC位为1，表示数据传输完成，如果USART_CR1寄存器的TCIE位置1，将产生中断。

    在发送数据时，编程的时候有几个比较重要的标志位我们来总结下。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/USART04.png)

- 接收器

    如果将USART_CR1寄存器的RE位置1，使能USART接收，使得接收器在RX线开始搜索起始位。 在确定到起始位后就根据RX线电平状态把数据存放在接收移位寄存器内。接收完成后就把接收移位寄存器数据移到RDR内， 并把USART_SR寄存器的RXNE位置1，同时如果USART_CR2寄存器的RXNEIE置1的话可以产生中断。

    在接收数据时，编程的时候有几个比较重要的标志位我们来总结下：

![屏幕截图 2024 06 26 144820](https://img.picgo.net/2024/06/26/-2024-06-26-1448206232babbcc274316.png)

- 小数波特率生成

    波特率指数据信号对载波的调制速率，它用单位时间内载波调制状态改变次数来表示，单位为波特。 比特率指单位时间内传输的比特数，单位bit/s(bps)。对于USART波特率与比特率相等，以后不区分这两个概念。波特率越大，传输速率越快。

    USART的发送器和接收器使用相同的波特率。计算公式如下：

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/USART010.png)

    其中，fPLCK为USART时钟， USARTDIV是一个存放在波特率寄存器(USART_BRR)的一个无符号定点数。 其中DIV_Mantissa[11:0]位定义USARTDIV的整数部分，DIV_Fraction[3:0]位定义USARTDIV的小数部分。

    例如：DIV_Mantissa=24(0x18)，DIV_Fraction=10(0x0A)，此时USART_BRR值为0x18A； 那么USARTDIV的小数位10/16=0.625；整数位24，最终USARTDIV的值为24.625。

    如果知道USARTDIV值为27.68，那么DIV_Fraction=16*0.68=10.88，最接近的正整数为11， 所以DIV_Fraction[3:0]为0xB；DIV_Mantissa=整数(27.68)=27，即为0x1B。

    波特率的常用值有2400、9600、19200、115200。下面以实例讲解如何设定寄存器值得到波特率的值。

    我们知道USART1使用APB2总线时钟，最高可达72MHz，其他USART的最高频率为36MHz。 我们选取USART1作为实例讲解，即fPLCK=72MHz。为得到115200bps的波特率，此时：

![](https://img.picgo.net/2024/06/26/-2024-06-26-14504892a2a1fba175b6fb.png)

    解得USARTDIV=39.0625，可算得DIV_Fraction=0.0625*16=1=0x01，DIV_Mantissa=39=0x27，即应该设置USART_BRR的值为0x271。

### 3.1 校验控制

    STM32F103系列控制器USART支持奇偶校验。当使用校验位时，串口传输的长度将是8位的数据帧加上1位的校验位总共9位， 此时USART_CR1寄存器的M位需要设置为1，即9数据位。将USART_CR1寄存器的PCE位置1就可以启动奇偶校验控制， 奇偶校验由硬件自动完成。启动了奇偶校验控制之后，在发送数据帧时会自动添加校验位，接收数据时自动验证校验位。 接收数据时如果出现奇偶校验位验证失败，会见USART_SR寄存器的PE位置1，并可以产生奇偶校验中断。

    使能了奇偶校验控制后，每个字符帧的格式将变成：起始位+数据帧+校验位+停止位。

### 3.2 中断控制

    USART有多个中断请求事件，具体见表

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/USART06.png)

## 4. USART初始化结构体详解

    标准库函数对每个外设都建立了一个初始化结构体，比如USART_InitTypeDef，结构体成员用于设置外设工作参数， 并由外设初始化配置函数，比如USART_Init()调用，这些设定参数将会设置外设相应的寄存器，达到配置外设工作环境的目的。

    初始化结构体和初始化库函数配合使用是标准库精髓所在，理解了初始化结构体每个成员意义基本上就可以对该外设运用自如了。 初始化结构体定义在stm32f10x_usart.h文件中，初始化库函数定义在stm32f10x_usart.c文件中，编程时我们可以结合这两个文件内注释使用。

- USART初始化结构体

```c
typedef struct {
    uint32_t USART_BaudRate;            // 波特率
    uint16_t USART_WordLength;          // 字长
    uint16_t USART_StopBits;            // 停止位
    uint16_t USART_Parity;              // 校验位
    uint16_t USART_Mode;                // USART模式
    uint16_t USART_HardwareFlowControl; // 硬件流控制
} USART_InitTypeDef;
```

1) USART_BaudRate： 波特率设置。一般设置为2400、9600、19200、115200。标准库函数会根据设定值计算得到USARTDIV值，从而设置USART_BRR寄存器值。

2) USART_WordLength： 数据帧字长，可选8位或9位。它设定USART_CR1寄存器的M位的值。如果没有使能奇偶校验控制，一般使用8数据位；如果使能了奇偶校验则一般设置为9数据位。

3) USART_StopBits： 停止位设置，可选0.5个、1个、1.5个和2个停止位，它设定USART_CR2寄存器的STOP[1:0]位的值，一般我们选择1个停止位。

4) USART_Parity： 奇偶校验控制选择，可选USART_Parity_No(无校验)、USART_Parity_Even(偶校验)以及USART_Parity_Odd(奇校验)，它设定USART_CR1寄存器的PCE位和PS位的值。

5) USART_Mode： USART模式选择，有USART_Mode_Rx和USART_Mode_Tx，允许使用逻辑或运算选择两个，它设定USART_CR1寄存器的RE位和TE位。

6) USART_HardwareFlowControl： 硬件流控制选择，只有在硬件流控制模式才有效，可选有使能RTS、使能CTS、同时使能RTS和CTS、不使能硬件流。

    当使用同步模式时需要配置SCLK引脚输出脉冲的属性，标准库使用一个时钟初始化结构体USART_ClockInitTypeDef来设置，该结构体内容也只有在同步模式才需要设置。

- USART时钟初始化结构体

```c
typedef struct {
    uint16_t USART_Clock;    // 时钟使能控制
    uint16_t USART_CPOL;     // 时钟极性
    uint16_t USART_CPHA;     // 时钟相位
    uint16_t USART_LastBit;  // 最尾位时钟脉冲
} USART_ClockInitTypeDef;
```

1) USART_Clock： 同步模式下SCLK引脚上时钟输出使能控制，可选禁止时钟输出(USART_Clock_Disable)或开启时钟输出(USART_Clock_Enable)；如果使用同步模式发送，一般都需要开启时钟。它设定USART_CR2寄存器的CLKEN位的值。

2) USART_CPOL： 同步模式下SCLK引脚上输出时钟极性设置，可设置在空闲时SCLK引脚为低电平(USART_CPOL_Low)或高电平(USART_CPOL_High)。它设定USART_CR2寄存器的CPOL位的值。

3) USART_CPHA： 同步模式下SCLK引脚上输出时钟相位设置，可设置在时钟第一个变化沿捕获数据(USART_CPHA_1Edge)或在时钟第二个变化沿捕获数据。它设定USART_CR2寄存器的CPHA位的值。USART_CPHA与USART_CPOL配合使用可以获得多种模式时钟关系。

4) USART_LastBit： 选择在发送最后一个数据位的时候时钟脉冲是否在SCLK引脚输出，可以是不输出脉冲(USART_LastBit_Disable)、输出脉冲(USART_LastBit_Enable)。它设定USART_CR2寄存器的LBCL位的值。

---

2024.8.23 第一次修订，后期不再维护
