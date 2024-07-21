# 第五章 蜂鸣器实验

## 1. 硬件设计

我们开发板上面使用的有源蜂鸣器，图中BEEP1：

![屏幕截图 2024 07 21 180628](https://img.picgo.net/2024/07/21/-2024-07-21-18062893891a234e02235d.png)

蜂鸣器分无源和有源两种，从电路上来看，这两种蜂鸣器在这都可以工作，区别是单片机程序不一样。

当节点Beep为高电平时，三极管Q4截止，蜂鸣器无电流，不响。

当节点Beep为低电平时，三极管Q4导通，蜂鸣器有电流，会响。

所以在Beep节点输入PWM脉冲时，蜂鸣器就会发出响声。

参考：[蜂鸣器原理与实现方法 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/513107262)

## 2. 软件设计

- 蜂鸣器初始化函数

```c
#ifndef __BEEP_H
#define	__BEEP_H

#include "stm32f10x.h"

/* 定义蜂鸣器连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的蜂鸣器引脚 */
#define BEEP_GPIO_PORT    	GPIOA			     // 蜂鸣器使用的GPIO端口
#define BEEP_GPIO_CLK 	    RCC_APB2Periph_GPIOA // 开启GPIOA端口时钟
#define BEEP_GPIO_PIN		GPIO_Pin_8			 // 连接到蜂鸣器的GPIO的引脚

/* 高电平时，蜂鸣器响 */
#define ON  1
#define OFF 0

/* 带参宏，可以像内联函数一样使用 */
#define BEEP(a)	if (a)	\
					GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);\
					else		\
					GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN)

void BEEP_GPIO_Config(void);
					
#endif /* __BEEP_H */


```

```c
#include "bsp_beep.h"   

// 蜂鸣器初始化函数
void BEEP_GPIO_Config(void)
{        
        /*定义一个GPIO_InitTypeDef类型的结构体*/
        GPIO_InitTypeDef GPIO_InitStructure;

        /*开启控制蜂鸣器的GPIO的端口时钟*/
        RCC_APB2PeriphClockCmd(BEEP_GPIO_CLK, ENABLE); 

        /*选择要控制蜂鸣器的GPIO*/                                                               
        GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;    

        /*设置GPIO模式为通用推挽输出*/
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

        /*设置GPIO速率为50MHz */   
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

        /*调用库函数，初始化控制蜂鸣器的GPIO*/
        GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);             

        /* 关闭蜂鸣器*/
        GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);     
}
```

- 主函数

```c
#include "stm32f10x.h"
#include "./beep/bsp_beep.h"

// 函数声明
void Delay(__IO u32 nCount); 

int main(void)
{    
    // 蜂鸣器初始化
    BEEP_GPIO_Config();    

    while(1)
    {        
        BEEP( ON ); // 打开蜂鸣器     
        Delay(0x0FFFFF);    

        BEEP( OFF );        
        Delay(0x0FFFFF);
    }
}

// 延时函数
void Delay(__IO uint32_t nCount) 
{
    for(; nCount != 0; nCount--);
}
```

## 3. 小结

这个实验还是比较简单，主要就是配置蜂鸣器初始化函数了，但是作为初学者我们还是得好好分析一下子，从beep头文件开始

- 首先就是要定义端口、时钟、引脚了

```c
#define BEEP_GPIO_PORT    	GPIOA			     // 蜂鸣器使用的GPIO端口
#define BEEP_GPIO_CLK 	    RCC_APB2Periph_GPIOA // 开启GPIOA端口时钟
#define BEEP_GPIO_PIN		GPIO_Pin_8			 // 连接到蜂鸣器的GPIO的引脚
```

这个我们了解过工程模板和参考以前的知识就知道了，我们要利用单片机驱动某个对象，首先确立我们要使用的端口，确立了端口就打开对应的时钟呗，之后规定我们要连接的引脚

- 我们知道，有源蜂鸣器，高电平1响，0就关，为了代码可读性，我们使用宏定义

```c
#define ON  1
#define OFF 0
```

随后我们分析一下beep.c，其实这没什么可分析的，都是使用的库函数，我们直接填几个参数就行了

```c
void BEEP_GPIO_Config(void)
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启控制蜂鸣器的GPIO的端口时钟*/
		RCC_APB2PeriphClockCmd(BEEP_GPIO_CLK, ENABLE); 

		/*选择要控制蜂鸣器的GPIO*/															   
		GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;	

		/*设置GPIO模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*设置GPIO速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*调用库函数，初始化控制蜂鸣器的GPIO*/
		GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);			 
    
		/* 关闭蜂鸣器yi
		GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);	 
}

```

以后的整体结构差不多都是这样

1. 定义一个结构体(通常为GPIO_InitTypeDef GPIO_InitStructure;)

2. 开启GPIO的时钟(参数：XXX_GPIO_PIN，ENABLE)

3. 选择要控制引脚（选择XXX_GPIO_PIN)

4. 选择输出模式（可选OD\PP)

5. 设置速率（可选5，10，50）MHz

6. 调用库函数，进行初始化（参数：端口，引用结构体）

7. 调用函数关闭对象（参数：端口，引脚）

那主函数更不必说了

```c
#include "stm32f10x.h"
#include "./beep/bsp_beep.h"

void Delay(__IO u32 nCount); 

int main(void)
{	
	// 初始化蜂鸣器GPIO
	BEEP_GPIO_Config();	
	
	while(1)
	{		
		BEEP( ON ); // 开启蜂鸣器	
		Delay(0x0FFFFF);	

		BEEP( OFF );		
		Delay(0x0FFFFF);
	}
}

// 延时函数
void Delay(__IO uint32_t nCount) 
{
	for(; nCount != 0; nCount--);
}
```

首先就是初始化beep了，之后想让蜂鸣器响，就是不断发送脉冲咯，就是不断的开->延时->关->延时

---

2024.7.21第一次修订
