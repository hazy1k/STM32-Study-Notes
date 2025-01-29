# 第三十九章 WWDG喂狗实验

## 1. 硬件设计

1. WWDG一个

2. LED两个

WWDG属于单片机内部资源，不需要外部电路，需要两个LED来指示程序的运行状态。

## 2. 软件设计

- WWDG配置函数

```c
void WWDG_Config(uint8_t tr, uint8_t wr, uint32_t prv)
{
    // 开启 WWDG 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
    // 设置递减计数器的值
    WWDG_SetCounter(tr);
    // 设置预分频器的值
    WWDG_SetPrescaler(prv);
    // 设置上窗口值
    WWDG_SetWindowValue(wr);
    // 设置计数器的值，使能WWDG
    WWDG_Enable(WWDG_CNT);
    // 清除提前唤醒中断标志位
    WWDG_ClearFlag();
    // 配置WWDG中断优先级
    WWDG_NVIC_Config();
    // 开WWDG 中断
    WWDG_EnableIT();
}
```

照例我们还得分析一下新出现的库函数：

### 1. **WWDG_SetCounter(uint8_t Counter)**

设置递减计数器的值。这个函数用于初始化WWDG的计数器值，从而控制复位时间。

- **参数**：
  
  - `Counter`：设置的计数器值，范围通常是0到127（具体取决于具体微控制器的实现）。

- **功能**：
  
  - 该函数会设置WWDG的计数器，WWDG会在这个值递减到0时产生复位。

### 2. **WWDG_SetPrescaler(uint8_t Prescaler)**

设置预分频器的值，用于控制WWDG计数器的时钟频率。

- **参数**：
  
  - `Prescaler`：预分频器的设置值，通常有多个选项（如1、2、4、8等）。

- **功能**：
  
  - 通过设置预分频器，控制WWDG计数器的时钟，使得计数器以不同的速度递减。

### 3. **WWDG_SetWindowValue(uint8_t WindowValue)**

设置上窗口值，用于控制重装载的条件。

- **参数**：
  
  - `WindowValue`：窗口的大小，范围通常是0到127。

- **功能**：
  
  - 该值决定了计数器在重装载前必须保持的最小值。如果在窗口内（即计数器值大于该值）重装载，会被接受；否则，WWDG会复位。

### 4. **WWDG_Enable(uint8_t Counter)**

使能WWDG，并设置计数器的初始值。

- **参数**：
  
  - `Counter`：初始化计数器值，通常需要在设置之后调用。

- **功能**：
  
  - 启动WWDG，开始计数。在此之后，WWDG将根据设置的计数器值进行递减。

### 5. **WWDG_ClearFlag(void)**

清除提前唤醒中断标志位。

- **功能**：
  - 如果在特定条件下（如进入低功耗模式），需要清除中断标志以避免错误复位。

### 6. **WWDG_NVIC_Config(void)**

配置WWDG中断的优先级。

- **功能**：
  - 通过设置中断优先级，确保WWDG相关的中断能按照预期顺序执行。

### 7. **WWDG_EnableIT(void)**

使能WWDG中断。

- **功能**：
  - 启动WWDG的中断功能，以便在特定条件下产生中断。

---

WWDG配置函数有三个形参，tr是计数器的值，一般我们设置成最大0X7F，wr是上窗口的值，这个我们要根据监控的程序的运行时间来设置， 但是值必须在0X40和计数器的值之间，prv用来设置预分频的值，取值可以是：

```c
/*
*     @arg WWDG_Prescaler_1: WWDG counter clock = (PCLK1/4096)/1
*     @arg WWDG_Prescaler_2: WWDG counter clock = (PCLK1/4096)/2
*     @arg WWDG_Prescaler_4: WWDG counter clock = (PCLK1/4096)/4
*     @arg WWDG_Prescaler_8: WWDG counter clock = (PCLK1/4096)/8
*/
```

这些宏在stm32f10x_wwdg.h中定义，宏展开是32位的16进制数，具体作用是设置配置寄存器CFR的位8:7  WDGTB[1:0]，获得各种分频系数。

- WWDG中断优先级函数

```c
// WWDG 中断优先级初始化
static void WWDG_NVIC_Config(void)
{
    NVIC_InitTypeDef   NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
```

在递减计数器减到0X40的时候，我们开启了提前唤醒中断，这个中断我们称它为死前中断或者叫遗嘱中断， 在中断函数里面我们应该出来最重要的事情，而且必须得快，因为递减计数器再减一次，就会产生系统复位。

- 中断服务函数

```c
// WWDG 中断复服务程序，如果发生了此中断，表示程序已经出现了故障，
// 这是一个死前中断。在此中断服务程序中应该干最重要的事，
// 比如保存重要的数据等，这个时间具体有多长，要
// 由WDGTB的值决定：
// WDGTB:0   113us
// WDGTB:1   227us
// WDGTB:2   455us
// WDGTB:3   910us
void WWDG_IRQHandler(void)
{
    // 清除中断标志位
    WWDG_ClearFlag();
    // LED2亮，点亮LED只是示意性的操作，
    // 真正使用的时候，这里应该是做最重要的事情
    LED2(ON);
}
```

- 喂狗函数

```c
// 喂狗
void WWDG_Feed(void)
{
    // 喂狗，刷新递减计数器的值，设置成最大WDG_CNT=0X7F
    WWDG_SetCounter(WWDG_CNT);
}
```

喂狗就是重新刷新递减计数器的值防止系统复位，喂狗一般是在主函数中喂。

- 主函数

```c
int main(void)
{    
    uint8_t wwdg_tr, wwdg_wr; 

    // 配置LED GPIO，并关闭LED
    LED_GPIO_Config();
    LED1(ON) ;
    SOFT_Delay(0X00FFFFFF);    
    // 初始化WWDG：配置计数器初始值，配置上窗口值，启动WWDG，使能提前唤醒中断
    WWDG_Config(0X7F, 0X5F, WWDG_Prescaler_8);// 函数参数：计数器值，窗口值，预分频系数
    // 窗口值我们在初始化的时候设置成0X5F，这个值不会改变
    wwdg_wr = WWDG->CFR & 0X7F;
    while(1)
    {    
        LED1(OFF);
        //-----------------------------------------------------
        // 这部分应该写需要被WWDG监控的程序，这段程序运行的时间
        // 决定了窗口值应该设置成多大。
        //-----------------------------------------------------

        // 计时器值，初始化成最大0X7F，当开启WWDG时候，这个值会不断减小
        // 当计数器的值大于窗口值时喂狗的话，会复位，当计数器减少到0X40
        // 还没有喂狗的话就非常非常危险了，计数器再减一次到了0X3F时就复位
        // 所以要当计数器的值在窗口值和0X40之间的时候喂狗，其中0X40是固定的。
        wwdg_tr = WWDG->CR & 0X7F;

        if(wwdg_tr < wwdg_wr)
        {
            // 喂狗，重新设置计数器的值为最大0X7F
            WWDG_Feed();
        }
    }
}
```

主函数中我们把WWDG的计数器的值设置 为0X7F，上窗口值设置为0X5F，分频系数为8分频。在while死循环中，我们不断读取计数器的值， 当计数器的值减小到小于上窗口值的时候，我们喂狗，让计数器重新计数。

在while死循环中，一般是我们需要监控的程序，这部分代码的运行时间，决定了上窗口值应该设置为多少，当监控的程序运行完毕之后， 我们需要执行喂狗程序，比起独立看门狗，这个喂狗的窗口时间是非常短的，对时间要求很精确。如果没有在这个窗口时间内喂狗的话， 那就说明程序出故障了，会产生提前唤醒中断，最后系统复位。

## 3. 小结

### 实验目的

了解和实践如何使用看门狗定时器（WWDG）来防止系统崩溃。

### 硬件需求

1. STM32开发板（如STM32F4系列）
2. LED灯（用于指示状态）
3. 按键（用于手动触发故障）

### 实验步骤

**初始化WWDG**：

- 配置WWDG时钟、计数器值、窗口值和预分频器。

```c
WWDG_SetPrescaler(WWDG_PRESCALER_8);
WWDG_SetWindowValue(80); // 窗口值
WWDG_SetCounter(127); // 初始计数器值
WWDG_Enable(127); // 启用WWDG
```

**LED指示状态**：

- 配置GPIO，控制LED灯闪烁以指示系统正常运行。

```c
GPIO_Init(); // 初始化LED
```

**喂狗函数**：

- 定义一个定期调用的喂狗函数，重装载WWDG计数器。

```c
void FeedWatchdog() {
    WWDG_SetCounter(127); // 重装载
}
```

**主循环**：

- 在主循环中定期调用喂狗函数，例如每500ms。
- 使用按键模拟故障（故意不喂狗）。

```c
while (1) {
    FeedWatchdog();
    HAL_Delay(500); // 等待500ms
    // 按键检测
    if (ButtonPressed()) {
        while (1); // 不喂狗，触发复位
    }
}**复位处理**：
```

- 当WWDG计数器降到0时，系统自动复位。复位后，LED灯重新启动，指示系统已恢复。

---

2024.9.19 第一次修订，后期不再维护
