# 第十三章 EXTI外部中断

## 1. EXTI简介

    EXTI（External interrupt/event controller）—外部中断/事件控制器，管理了控制器的20个中断/事件线。 每个中断/事件线都对应有一个边沿检测器，可以实现输入信号的上升沿检测和下降沿的检测。EXTI可以实现对每个中断/事件线进行单独配置， 可以单独配置为中断或者事件，以及触发事件的属性。

## 2. EXTI功能框图

    在图 [EXTI功能框图](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/EXTI.html#id3) 可以看到很多在信号线上打一个斜杠并标注“20”字样，这个表示在控制器内部类似的信号线路有20个， 这与EXTI总共有20个中断/事件线是吻合的。所以我们只要明白其中一个的原理，那其他19个线路原理也就知道了。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/EXTI002.png)

    EXTI可分为两大部分功能，一个是产生中断，另一个是产生事件，这两个功能从硬件上就有所不同。

    首先我们来看图 [EXTI功能框图](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/EXTI.html#id3) 中红色虚线指示的电路流程。它是一个产生中断的线路，最终信号流入到NVIC控制器内。

    编号1是输入线，EXTI控制器有19个中断/事件输入线，这些输入线可以通过寄存器设置为任意一个GPIO，也可以是一些外设的事件， 这部分内容我们将在后面专门讲解。输入线一般是存在电平变化的信号。

    编号2是一个边沿检测电路，它会根据上升沿触发选择寄存器(EXTI_RTSR)和下降沿触发选择寄存器(EXTI_FTSR)对应位的设置来控制信号触发。 边沿检测电路以输入线作为信号输入端，如果检测到有边沿跳变就输出有效信号1给编号3电路，否则输出无效信号0。 而EXTI_RTSR和EXTI_FTSR两个寄存器可以控制需要检测哪些类型的电平跳变过程，可以是只有上升沿触发、只有下降沿触发或者上升沿和下降沿都触发。

    编号3电路实际就是一个或门电路，它的一个输入来自编号2电路，另外一个输入来自软件中断事件寄存器(EXTI_SWIER)。EXTI_SWIER允许我们通过程序控制就可以启动中断/事件线， 这在某些地方非常有用。我们知道或门的作用就是有1就为1，所以这两个输入随便一个有有效信号1就可以输出1给编号4和编号6电路。

    编号4电路是一个与门电路，它的一个输入是编号3电路，另外一个输入来自中断屏蔽寄存器(EXTI_IMR)。与门电路要求输入都为1才输出1， 导致的结果是如果EXTI_IMR设置为0时，那不管编号3电路的输出信号是1还是0，最终编号4电路输出的信号都为0；如果EXTI_IMR设置为1时， 最终编号4电路输出的信号才由编号3电路的输出信号决定，这样我们可以简单的控制EXTI_IMR来实现是否产生中断的目的。 编号4电路输出的信号会被保存到挂起寄存器(EXTI_PR)内，如果确定编号4电路输出为1就会把EXTI_PR对应位置1。

    编号5是将EXTI_PR寄存器内容输出到NVIC内，从而实现系统中断事件控制。

    接下来我们来看看绿色虚线指示的电路流程。它是一个产生事件的线路，最终输出一个脉冲信号。

    产生事件线路是在编号3电路之后与中断线路有所不同，之前电路都是共用的。编号6电路是一个与门，它的一个输入来自编号3电路， 另外一个输入来自事件屏蔽寄存器(EXTI_EMR)。如果EXTI_EMR设置为0时，那不管编号3电路的输出信号是1还是0，最终编号6电路输出的信号都为0； 如果EXTI_EMR设置为1时，最终编号6电路输出的信号才由编号3电路的输出信号决定，这样我们可以简单的控制EXTI_EMR来实现是否产生事件的目的。

    编号7是一个脉冲发生器电路，当它的输入端，即编号6电路的输出端，是一个有效信号1时就会产生一个脉冲；如果输入端是无效信号就不会输出脉冲。

    编号8是一个脉冲信号，就是产生事件的线路最终的产物，这个脉冲信号可以给其他外设电路使用，比如定时器TIM、模拟数字转换器ADC等等，这样的脉冲信号一般用来触发TIM或者ADC开始转换。

    产生中断线路目的是把输入信号输入到NVIC，进一步会运行中断服务函数，实现功能，这样是软件级的。而产生事件线路目的就是传输一个脉冲信号给其他外设使用， 并且是电路级别的信号传输，属于硬件级的。

    另外，EXTI是在APB2总线上的，在编程时候需要注意到这点。

## 3. 中断/事件线

    EXTI有20个中断/事件线，每个GPIO都可以被设置为输入线，占用EXTI0至EXTI15， 还有另外七根用于特定的外设事件，见表 [EXTI中断_事件线](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/EXTI.html#id5) 。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/EXTI01.png)

    EXTI0至EXTI15用于GPIO，通过编程控制可以实现任意一个GPIO作为EXTI的输入源。由表 [EXTI中断_事件线](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/EXTI.html#id5) 可知， EXTI0可以通过AFIO的外部中断配置寄存器1(AFIO_EXTICR1)的EXTI0[3:0]位选择配置为PA0、 PB0、PC0、PD0、PE0、PF0、PG0、PH0或者PI0，见图 [EXTI0输入源选择](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/EXTI.html#exti0) 。其他EXTI线(EXTI中断/事件线)使用配置都是类似的。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/EXTI003.png)

## 4. EXTI初始化结构体详解

    标准库函数对每个外设都建立了一个初始化结构体，比如EXTI_InitTypeDef，结构体成员用于设置外设工作参数，并由外设初始化配置函数， 比如EXTI_Init()调用，这些设定参数将会设置外设相应的寄存器，达到配置外设工作环境的目的。

    初始化结构体和初始化库函数配合使用是标准库精髓所在，理解了初始化结构体每个成员意义基本上就可以对该外设运用自如了。 初始化结构体定义在stm32f10x_exti.h文件中，初始化库函数定义在stm32f10x_exti.c文件中，编程时我们可以结合这两个文件内注释使用。

```c
// EXTI-1 EXTI初始化结构体

typedef struct {
    uint32_t EXTI_Line;                 // 中断/事件线
    EXTIMode_TypeDef EXTI_Mode;         // EXTI模式
    EXTITrigger_TypeDef EXTI_Trigger;   // 触发类型
    FunctionalState EXTI_LineCmd;       // EXTI使能
} EXTI_InitTypeDef;
```

1. EXTI_Line：EXTI中断/事件线选择，可选EXTI0至EXTI19，可参考表 [EXTI中断_事件线](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/EXTI.html#id5) 选择。

2. EXTI_Mode：EXTI模式选择，可选为产生中断(EXTI_Mode_Interrupt)或者产生事件(EXTI_Mode_Event)。

3. EXTI_Trigger：EXTI边沿触发事件，可选上升沿触发(EXTI_Trigger_Rising)、 下降沿触发( EXTI_Trigger_Falling)或者上升沿和下降沿都触发( EXTI_Trigger_Rising_Falling)。

4. EXTI_LineCmd：控制是否使能EXTI线，可选使能EXTI线(ENABLE)或禁用(DISABLE)。

## 5. 外部中断控制实验

    中断在嵌入式应用中占有非常重要的地位，几乎每个控制器都有中断功能。中断对保证紧急事件得到第一时间处理是非常重要的。

    我们设计使用外接的按键来作为触发源，使得控制器产生中断，并在中断服务函数中实现控制RGB彩灯的任务。

### 5.1 硬件设计

    轻触按键在按下时会使得引脚接通，通过电路设计可以使得按下时产生电平变化，见图 [按键电路设计](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/EXTI.html#id9) 。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/EXTI004.png)

### 5.2 软件设计

    这里只讲解核心的部分代码，有些变量的设置，头文件的包含等并没有涉及到，完整的代码请参考本章配套的工程。 我们创建了两个文件：bsp_exti.c和bsp_exti.h文件用来存放EXTI驱动程序及相关宏定义，中断服务函数放在stm32f10x_it.h文件中。

#### 5.2.1 编程要点

1. 初始化用来产生中断的GPIO；

2. 初始化EXTI；

3. 配置NVIC；

4. 编写中断服务函数；

#### 5.2.2 软件分析

- 按键和EXTI宏定义

```c
// 引脚定义
#define KEY1_INT_GPIO_PORT         GPIOA
#define KEY1_INT_GPIO_CLK          (RCC_APB2Periph_GPIOA\
                                    |RCC_APB2Periph_AFIO)
#define KEY1_INT_GPIO_PIN          GPIO_Pin_0
#define KEY1_INT_EXTI_PORTSOURCE   GPIO_PortSourceGPIOA
#define KEY1_INT_EXTI_PINSOURCE    GPIO_PinSource0
#define KEY1_INT_EXTI_LINE         EXTI_Line0
#define KEY1_INT_EXTI_IRQ          EXTI0_IRQn

#define KEY1_IRQHandler            EXTI0_IRQHandler


#define KEY2_INT_GPIO_PORT         GPIOC
#define KEY2_INT_GPIO_CLK          (RCC_APB2Periph_GPIOC\
                                    |RCC_APB2Periph_AFIO)
#define KEY2_INT_GPIO_PIN          GPIO_Pin_13
#define KEY2_INT_EXTI_PORTSOURCE   GPIO_PortSourceGPIOC
#define KEY2_INT_EXTI_PINSOURCE    GPIO_PinSource13
#define KEY2_INT_EXTI_LINE         EXTI_Line13
#define KEY2_INT_EXTI_IRQ          EXTI15_10_IRQn
```

    使用宏定义方法指定与硬件电路设计相关配置，这对于程序移植或升级非常有用的。

    在上面的宏定义中，我们除了开GPIO的端口时钟外，我们还打开了AFIO的时钟， 这是因为等下配置EXTI信号源的时候需要用到AFIO的外部中断控制寄存器AFIO_EXTICRx

- 嵌套向量中断控制器NVIC配置

```c
/static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 配置NVIC为优先级组1 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    /* 配置中断源：按键1 */
    NVIC_InitStructure.NVIC_IRQChannel = KEY1_INT_EXTI_IRQ;
    /* 配置抢占优先级：1 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    /* 配置子优先级：1 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    /* 使能中断通道 */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 配置中断源：按键2，其他使用上面相关配置 */
    NVIC_InitStructure.NVIC_IRQChannel = KEY2_INT_EXTI_IRQ;
    NVIC_Init(&NVIC_InitStructure);
}/ NVIC配置NVIC
```

    这里我们配置两个的中断软件优先级一样，如果出现了两个按键同时按下的情况，那怎么办，到底该执行哪一个中断？当两个中断的软件优先级一样的时候， 中断来临时，具体先执行哪个中断服务函数由硬件的中断编号决定，编号越小，优先级越高。

    当然，我们也可以把抢占优先级设置成一样，子优先级设置成不一样，这样就可以区别两个按键同时按下的情况，而不用硬件去对比硬件编号。

- EXTI中断配置

```c
void EXTI_Key_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    /*开启按键GPIO口的时钟*/
    RCC_APB2PeriphClockCmd(KEY1_INT_GPIO_CLK,ENABLE);

    /* 配置 NVIC 中断*/
    NVIC_Configuration();

    /*--------------------------KEY1配置---------------------*/
    /* 选择按键用到的GPIO */
    GPIO_InitStructure.GPIO_Pin = KEY1_INT_GPIO_PIN;
    /* 配置为浮空输入 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStructure);

    /* 选择EXTI的信号源 */
    GPIO_EXTILineConfig(KEY1_INT_EXTI_PORTSOURCE, \
                        KEY1_INT_EXTI_PINSOURCE);
    EXTI_InitStructure.EXTI_Line = KEY1_INT_EXTI_LINE;

    /* EXTI为中断模式 */
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    /* 上升沿中断 */
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    /* 使能中断 */
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /*--------------------------KEY2配置------------------*/
    /* 选择按键用到的GPIO */
    GPIO_InitStructure.GPIO_Pin = KEY2_INT_GPIO_PIN;
    /* 配置为浮空输入 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(KEY2_INT_GPIO_PORT, &GPIO_InitStructure);

    /* 选择EXTI的信号源 */
    GPIO_EXTILineConfig(KEY2_INT_EXTI_PORTSOURCE, \
                        KEY2_INT_EXTI_PINSOURCE);
    EXTI_InitStructure.EXTI_Line = KEY2_INT_EXTI_LINE;

    /* EXTI为中断模式 */
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    /* 下降沿中断 */
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    /* 使能中断 */
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}
```

    首先，使用GPIO_InitTypeDef和EXTI_InitTypeDef结构体定义两个用于GPIO和EXTI初始化配置的变量，关于这两个结构体前面都已经做了详细的讲解。

    使用GPIO之前必须开启GPIO端口的时钟；用到EXTI必须开启AFIO时钟。

    调用NVIC_Configuration函数完成对按键1、按键2优先级配置并使能中断通道。

    作为中断/事件输入线时需把GPIO配置为输入模式，具体为浮空输入，由外部电路完全决定引脚的状态。

    GPIO_EXTILineConfig函数用来指定中断/事件线的输入源，它实际是设定外部中断配置寄存器的AFIO_EXTICRx值，该函数接收两个参数， 第一个参数指定GPIO端口源，第二个参数为选择对应GPIO引脚源编号。

    我们的目的是产生中断，执行中断服务函数，EXTI选择中断模式，按键1使用上升沿触发方式，并使能EXTI线。

    按键2基本上采用与按键1相关参数配置，只是改为下降沿触发方式。

    两个按键的电路是一样的，可代码中我们设置按键1是上升沿中断，按键2是下降沿中断，有人就会问这是不是设置错了？实际上可以这么理解， 按键1检测的是按键按下的状态，按键2检测的是按键弹开的状态，那这样就解释的通了。

- EXTI中断服务函数

```c
void KEY1_IRQHandler(void)
{
    //确保是否产生了EXTI Line中断
    if (EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) {
        // LED1 取反
        LED1_TOGGLE;
        //清除中断标志位
        EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);
    }
}

void KEY2_IRQHandler(void)
{
    //确保是否产生了EXTI Line中断
    if (EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) {
        // LED2 取反
        LED2_TOGGLE;
        //清除中断标志位
        EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);
    }
} 
```

    当中断发生时，对应的中断服务函数就会被执行，我们可以在中断服务函数实现一些控制。

    一般为确保中断确实发生，我们会在中断服务函数中调用中断标志位状态读取函数读取外设中断标志位并判断标志位状态。

    EXTI_GetITStatus函数用来获取EXTI的中断标志位状态，如果EXTI线有中断发生函数返回“SET”否则返回“RESET”。 实际上，EXTI_GetITStatus函数是通过读取EXTI_PR寄存器值来判断EXTI线状态的。

    按键1的中断服务函数我们让LED1翻转其状态，按键2的中断服务函数我们让LED2翻转其状态。执行任务后需要调用EXTI_ClearITPendingBit函数清除EXTI线的中断标志位。

- 主函数

```c
int main(void)
{
    /* LED 端口初始化 */
    LED_GPIO_Config();

    /* 初始化EXTI中断，按下按键会触发中断，
    *  触发中断会进入stm32f10x_it.c文件中的函数
    *  KEY1_IRQHandler和KEY2_IRQHandler，处理中断，反转LED灯。
    */
    EXTI_Key_Config();

    /* 等待中断，由于使用中断方式，CPU不用轮询按键 */
    while (1) {
    }
}
```

    主函数非常简单，只有两个任务函数。LED_GPIO_Config函数定义在bsp_led.c文件内，完成RGB彩灯的GPIO初始化配置。EXTI_Key_Config函数完成两个按键的GPIO和EXTI配置。

## 6. 小结

    相比于51，stm32的中断应用难理解了不少，但是本质还是一样即优先处理中断函数内的功能，只不过配置稍微麻烦一些
