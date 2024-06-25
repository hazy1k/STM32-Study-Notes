# 第四章 GPIO介绍

    在学习STM32之前我们应该是学过51单片机的，对于GPIO这个概念我们应该有初步的理解，但是STM32和51又有很多不一样，下面开始介绍。

## 1. GPIO简介

    GPIO就是通用输入输出端口的简称，简单来说就是我们可以控制的引脚，通过这些引脚我们控制各种外部设备。STM32芯片的GPIO被分成很多组，每组有16个引脚，如型号为STM32F103VET6型号的芯片有GPIOA、GPIOB、 GPIOC至GPIOE共5组

    最基本的输出功能是由STM32控制引脚输出高、低电平，实现开关控制，如把GPIO引脚接入到LED灯，那就可以控制LED灯的亮灭， 引脚接入到继电器或三极管，那就可以通过继电器或三极管控制外部大功率电路的通断。

## 2. GPIO框图解读

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LEDreg002.png)

### 2.1 基本结构分析

    下面我们按图中的编号对GPIO端口的结构部件进行说明。

#### 2.1.1 保护二极管及上、下拉电阻

    引脚的两个保护二级管可以防止引脚外部过高或过低的电压输入，当引脚电压高于VDD时， 上方的二极管导通，当引脚电压低于VSS时，下方的二极管导通，防止不正常电压引入芯片导致芯片烧毁。 

    尽管有这样的保护，并不意味着STM32的引脚能直接外接大功率驱动器件，如：直接驱动电机，强制驱动要么电机不转，要么导致芯片烧坏，必须要加大功率及隔离电路驱动。

    详细信息可以参考：[https://blog.csdn.net/wdnmd_lirui/article/details/136122957]()

#### 2.1.2 P-MOX管和N-MOS管（重点嗷）

    GPIO引脚线路经过两个保护二极管后，向上流向输入模式结构，向下流向输出模式结构。

    让我们先看一下输出模式部分，线路经过一个由P-MOS和N-MOS管组成的单元电路。 这个结构使GPIO具有了“推挽输出”和“开漏输出”两种模式。

    所谓的推挽输出模式，是根据这两个MOS管的工作方式来命名的：

    在该结构中输入高电平时，经过反向后，上方的P-MOS导通，下方的N-MOS关闭， 对外输出高电平；而在该结构中输入低电平时，经过反向后，N-MOS管导通，P-MOS关闭，对外输出低电平。（也就是高电平P导通对外输出高电平，低电平N导通对外输出低电平）当引脚高低电平切换时，两个管子轮流导通， P管负责灌电流，N管负责拉电流，使其负载能力和开关速度都比普通的方式有很大的提高。

    推挽输出的低电平为0伏，高电平为3.3伏， 具体参考图 [推挽等效电路](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LED_register.html#id5) ，它是推挽输出模式时的等效电路。

    关于更多推挽输出可以参考：[推挽电路工作原理详解（四类互补推挽式功率放大电路分析） - 变流、电压变换、逆变电路 - 电子发烧友网 (elecfans.com)](https://www.elecfans.com/article/83/147/2017/20171102574402.html)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LEDreg003.png)

    而在开漏输出模式时，上方的P-MOS管完全不工作。如果我们控制输出为0，则P-MOS管关闭，N-MOS管导通。若控制输出为1 (它无法直接输出高电平)时，则P-MOS管和N-MOS管都关闭，所以引脚既不输出高电平，也不输出低电平，为高阻态。 

    为正常使用时必须外部接上拉电阻，参考图 [开漏电路](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LED_register.html#id6) 中等效电路。它具有“线与”特性，也就是说，若有很多个开漏模式引脚连接到一起时， 只有当所有引脚都输出高阻态，才由上拉电阻提供高电平，此高电平的电压为外部上拉电阻所接的电源的电压。若其中一个引脚为低电平， 那线路就相当于短路接地，使得整条线路都为低电平，0伏。（就好像&逻辑，全为1才为1，有一个0全为0）

    推挽输出模式一般应用在输出电平为 0 和 3.3 伏而且需要高速切换开关状态的场合。在STM32的应用中，除了必须用开漏模式的场合，我们都习惯使用推挽输出模式。

    关于更多开漏输出可以参考：[GPIO推挽输出和开漏输出模式区别详解_gpio的推挽输出和开漏输出有何区别-CSDN博客](https://blog.csdn.net/weixin_44788542/article/details/115303125)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LEDreg004.png)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LEDreg005.png)

#### 2.1.3 输出数据寄存器

    前面提到的双MOS管结构电路的输入信号，是由GPIO“输出数据寄存器GPIOx_ODR”提供的，因此我们通过修改输出数据寄存器的值就可以修改GPIO引脚的输出电平。 而“置位/复位寄存器GPIOx_BSRR”可以通过修改输出数据寄存器的值从而影响电路的输出。

    关于更多输出数据寄存器参考：[关于STM32的GPIO端口 输入/输出数据寄存器_端口输出数据寄存器给1是正吗-CSDN博客](https://blog.csdn.net/weixin_41629848/article/details/98884980)

#### 2.1.4 复用功能输出

    “复用功能输出”中的“复用”是指STM32的其它片上外设对GPIO引脚进行控制，此时GPIO引脚用作该外设功能的一部分，算是第二用途。 从其它外设引出来的“复用功能输出信号”与GPIO本身的数据据寄存器都连接到双MOS管结构的输入中，通过图中的梯形结构作为开关切换选择。

    例如我们使用USART串口通讯时，需要用到某个GPIO引脚作为通讯发送引脚，这个时候就可以把该GPIO引脚配置成USART串口复用功能，由串口外设控制该引脚，发送数据。

    关于更多复用功能输出可以参考：[复用功能输入输出的概念 一文读懂GPIO复用功能 - 控制/MCU - 电子发烧友网 (elecfans.com)](https://www.elecfans.com/emb/danpianji/20180613694709.html)

```c
// GPIOB 16个IO全部输出 0XFFFF
GPIOB->ODR = 0XFFFF;
```

#### 2.1.5 输入数据寄存器

    再观察GPIO结构框图的上半部分，GPIO引脚经过内部的上、下拉电阻，可以配置成上/下拉输入，然后再连接到施密特触发器，信号经过触发器后， 模拟信号转化为0、1的数字信号，然后存储在“输入数据寄存器GPIOx_IDR”中，通过读取该寄存器就可以了解GPIO引脚的电平状态。

```c
// 读取GPIOB端口的16位数据值
uint16_t temp;
temp = GPIOB->IDR;
```

    关于更多输入数据寄存器可以参考：[明解STM32—GPIO理论基础知识篇之寄存器原理 - Sharemaker - 博客园 (cnblogs.com)](https://www.cnblogs.com/Sharemaker/p/17281410.html)

#### 2.1.6 复用功能输入

    与“复用功能输出”模式类似，在“复用功能输入模式”时，GPIO引脚的信号传输到STM32其它片上外设，由该外设读取引脚状态。

    同样，如我们使用USART串口通讯时，需要用到某个GPIO引脚作为通讯接收引脚，这个时候就可以把该GPIO引脚配置成USART串口复用功能，使USART可以通过该通讯引脚的接收远端数据。

#### 2.1.7 模拟输入输出

    当GPIO引脚用于ADC采集电压的输入通道时，用作“模拟输入”功能，此时信号是不经过施密特触发器的，因为经过施密特触发器后信号只有0、1两种状态， 所以ADC外设要采集到原始的模拟信号，信号源输入必须在施密特触发器之前。

    类似地，当GPIO引脚用于DAC作为模拟电压输出通道时，此时作为“模拟输出”功能， DAC的模拟信号输出就不经过双MOS管结构，模拟信号直接输出到引脚。

    更多可以参考：[一文讲清模拟信号、自然信号、数字信号、模拟输入输出_输入的信号用什么模拟-CSDN博客](https://blog.csdn.net/bjbz_cxy/article/details/122078379)

### 2.2 GPIO工作模式

    总结一下，由GPIO的结构决定了GPIO可以配置成以下模式：

```c
// GPIO 8种工作模式

typedef enum
{
    GPIO_Mode_AIN = 0x0,           // 模拟输入
    GPIO_Mode_IN_FLOATING = 0x04,  // 浮空输入
    GPIO_Mode_IPD = 0x28,          // 下拉输入
    GPIO_Mode_IPU = 0x48,          // 上拉输入
    GPIO_Mode_Out_OD = 0x14,       // 开漏输出
    GPIO_Mode_Out_PP = 0x10,       // 推挽输出
    GPIO_Mode_AF_OD = 0x1C,        // 复用开漏输出
    GPIO_Mode_AF_PP = 0x18         // 复用推挽输出
} GPIOMode_TypeDef;
```

    在固件库中，GPIO总共有8种细分的工作模式，大致归类为以下三类：输入/输出模式，复用功能

#### 2.2.1 输入模式（模拟/浮空/上拉/下拉）

    在输入模式时，施密特触发器打开，输出被禁止，可通过输入数据寄存器GPIOx_IDR读取I/O状态。其中输入模式，可设置为上拉、 下拉、浮空和模拟输入四种。上拉和下拉输入很好理解，默认的电平由上拉或者下拉决定。浮空输入的电平是不确定的，完全由外部的输入决定， 一般接按键的时候用的是这个模式。模拟输入则用于ADC采集。

    参考：[【硬件】关于上拉输入、下拉输入、模拟输入、浮空输入、推挽输出、开漏输出、复用输出的区别_上拉输入和下拉输入的区别-CSDN博客](https://blog.csdn.net/weixin_42425594/article/details/86742339#:~:text=%E3%80%90%E7%A1%AC%E4%BB%B6%E3%80%91%E5%85%B3%E4%BA%8E%E4%B8%8A%E6%8B%89%E8%BE%93%E5%85%A5%E3%80%81%E4%B8%8B%E6%8B%89%E8%BE%93%E5%85%A5%E3%80%81%E6%A8%A1%E6%8B%9F%E8%BE%93%E5%85%A5%E3%80%81%E6%B5%AE%E7%A9%BA%E8%BE%93%E5%85%A5%E3%80%81%E6%8E%A8%E6%8C%BD%E8%BE%93%E5%87%BA%E3%80%81%E5%BC%80%E6%BC%8F%E8%BE%93%E5%87%BA%E3%80%81%E5%A4%8D%E7%94%A8%E8%BE%93%E5%87%BA%E7%9A%84%E5%8C%BA%E5%88%AB%201%20%281%29GPIO_Mode_AIN%20%E6%A8%A1%E6%8B%9F%E8%BE%93%E5%85%A5---%E5%BA%94%E7%94%A8ADC%E6%A8%A1%E6%8B%9F%E8%BE%93%E5%85%A5%EF%BC%8C%E6%88%96%E8%80%85%E4%BD%8E%E5%8A%9F%E8%80%97%E4%B8%8B%E7%9C%81%E7%94%B5%202%20%282%29GPIO_Mode_IN_FLOATING%20%E6%B5%AE%E7%A9%BA%E8%BE%93%E5%85%A5---%E5%8F%AF%E4%BB%A5%E5%81%9AKEY%E8%AF%86%E5%88%AB%203,IO%E8%BE%93%E5%87%BA1%20-%E6%8E%A5VCC%EF%BC%8C%E8%AF%BB%E8%BE%93%E5%85%A5%E5%80%BC%E6%98%AF%E6%9C%AA%E7%9F%A5%E7%9A%84%207%20%287%29GPIO_Mode_AF_OD%20%E5%A4%8D%E7%94%A8%E5%BC%80%E6%BC%8F%E8%BE%93%E5%87%BA---%E7%89%87%E5%86%85%E5%A4%96%E8%AE%BE%E5%8A%9F%E8%83%BD%EF%BC%88TX1%2CMOSI%2CMISO.SCK.SS%EF%BC%89%208%20%288%29GPIO_Mode_AF_PP%20%E5%A4%8D%E7%94%A8%E6%8E%A8%E6%8C%BD%E8%BE%93%E5%87%BA---%E7%89%87%E5%86%85%E5%A4%96%E8%AE%BE%E5%8A%9F%E8%83%BD%EF%BC%88I2C%E7%9A%84SCL%2CSDA%EF%BC%89)

#### 2.2.2 输出模式（推挽/开漏）

    在输出模式中，推挽模式时双MOS管以轮流方式工作，输出数据寄存器GPIOx_ODR可控制I/O输出高低电平。开漏模式时，只有N-MOS管工作， 输出数据寄存器可控制I/O输出高阻态或低电平。

    输出速度可配置，有2，10，50MHz的选项。此处的输出速度即I/O支持的高低电平状态最高切换频率， 支持的频率越高，功耗越大。（平时我们默认开最大，因为对功耗要求不高）

    在输出模式时施密特触发器是打开的，即输入可用，通过输入数据寄存器GPIOx_IDR可读取I/O的实际状态。

#### 2.2.3 复用功能（推挽/开漏）

    复用功能模式中，输出使能，输出速度可配置，可工作在推挽及开漏模式，但是输出信号源于其它外设，输出数据寄存器GPIOx_ODR无效； 输入可用，通过输入数据寄存器可获取I/O实际状态，但一般直接用外设的寄存器来获取该数据信号。

    在GPIO外设中， 控制端口高低控制寄存器CRH和CRL可以配置每个GPIO的工作模式和工作的速度，每4个位控制一个IO， CRH控制端口的高八位，CRL控制端口的低8位，具体的看CRH和CRL的寄存器描述。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LEDreg006.png)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LEDreg007.png)

    关于复用功能更多信息：[STM32-IO引脚复用-原理和使用 - jym蒟蒻 - 博客园 (cnblogs.com)](https://www.cnblogs.com/jiangyiming/p/15790845.html)

    关于CRL和CRH的更多信息：[STM32 GPIO的配置寄存器（CRL、CRH）快速学习_gpioa->crh-CSDN博客](https://blog.csdn.net/weixin_45915259/article/details/123878323)

    
