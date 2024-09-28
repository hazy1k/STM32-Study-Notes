# 第四十九章 液晶显示英文（字库存储内部FLASH)

## 1. 硬件设计

针对不同模式的液晶显示字符工程，需要有不同的硬件支持。字库存储在STM32芯片内部FLASH的工程跟普通液晶显示的硬件需求无异。 需要外部字库的工程，要有额外的SPI-FLASH、SD支持，使用外部FLASH时，我们的实验板上直接用板子上的SPI-FLASH芯片存储字库， 出厂前我们已给FLASH芯片烧录了前面的《GB2312_H1616.FON》字库文件，如果想把我们的程序移植到自己设计产品上， 请确保该系统包含有存储了字库的FLASH芯片，才能正常显示汉字。使用SD卡时，需要给板子接入存储有《GB2312_H1616.FON》字库文件的MicroSD卡， SD卡的文件系统格式需要是FAT格式，且字库文件所在的目录需要跟程序里使用的文件目录一致。

关于SPI-FLASH和SD卡的原理图及驱动说明可参考其他的章节。给外部SPI-FLASH和SD卡存储字库的操作我们将在另一个文档中说明。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCDdis10.png)

这些实验是在《液晶显示》工程的基础上修改的，主要添加字符显示相关的内容，本小节只讲解这部分新增的函数。关于液晶驱动的原理在此不再赘述， 不理解这部分的可阅读前面的章节。

## 2. 软件设计

### 2.1 编程目标

1. 获取字模数据；

2. 根据字模格式，编写液晶显示函数；

3. 编写测试程序，控制液晶英文。

### 2.2 代码分析

#### 2.2.1 ASCII字模数据

要显示字符首先要有字库数据， 在工程的“fonts.c”文件中我们定义了一系列大小为24x32、16x24、8x16的ASCII码表的字模数据：

```c
/*
* 常用ASCII表，偏移量32，大小:24（高度）* 16 （宽度）
*/
//@conslons字体，阴码点阵格式，逐行顺向取摸
const uint8_t ASCII16x24_Table [ ] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x03,0x80,0x01,0x80,0x01,0x80,
    0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,
    0x01,0x80,0x01,0x80,0x01,0x80,0x00,0x00,
    0x00,0x00,0x03,0xc0,0x03,0xc0,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

    /*以下部分省略.....，包含从 空格 至 波浪号 的ASCII码图形字模数据*/
```

由于ASCII中的字符并不多，所以本工程中直接以C语言数组的方式存储这些字模数据，C语言的const数组是作为常量直接存储到STM32芯片的内部FLASH中的， 所以如果不需要显示中文，可以不用外部的SPI-FLASH芯片，可省去烧录字库的麻烦。以上代码定义的ASCII16x24_Table数组是16x24大小的ASCII字库。

#### 2.2.2 管理英文字模的结构体

为了方便使用各种不同的字体，工程中定义了一个“sFont”结构体类型，并利用它定义存储了不同字体信息的结构体变量：

```c
/*字体格式*/
typedef struct _tFont {
    const uint8_t *table; // 指向字模数据的指针
    uint16_t Width;       // 字模的像素宽度
    uint16_t Height;      // 字模的像素高度
} sFONT;

sFONT Font8x16 = 
{
    ASCII8x16_Table,
    8, // 字模宽 
    16,// 字模高
};
sFONT Font16x24 = 
{
    ASCII16x24_Table,
    16, // 字模宽 
    24, // 字模高 
};
sFONT Font24x32 = 
{
    ASCII24x32_Table,
    24, // 字模宽
    32, // 字模高 
};
```

这个结构体类型定义了三个变量，第一个是指向字模数据的指针，即前面提到的C语言数组，每二、三个变量存储了该字模单个字符的像素宽度和高度。 利用这个类型定义了Font8x16、Font16x24之类的变量，方便显示时寻址。

#### 2.2.3 切换字体

在程序中若要方便切换字体，还需要定义一个存储了当前选择字体的变量LCD_Currentfonts

```c
// 用于存储当前选择的字体格式的全局变量
static sFONT *LCD_Currentfonts;
// 设置字体格式(英文)
void LCD_SetFont(sFONT *fonts)
{
    LCD_Currentfonts = fonts;
}
```

使用LCD_SetFont可以切换LCD_Currentfonts指向的字体类型，函数的可输入参数即前面的Font8x16、Font16x24之类的变量。

#### 2.2.4 ASCII字符显示函数

利用字模数据以及上述结构体变量，我们可以编写一个能显示各种字体的通用函数

```c
/*
在 ILI9341 显示器上显示一个英文字符
usX ：在特定扫描方向下字符的起始X坐标
usY ：在特定扫描方向下该点的起始Y坐标
cChar ：要显示的英文字符
可使用LCD_SetBackColor、LCD_SetTextColor、LCD_SetColors函数设置颜色
*/
void ILI9341_DispChar_EN(uint16_t usX, uint16_t usY, const char cChar)
{
    uint8_t  byteCount, bitCount,fontLength;
    uint16_t ucRelativePositon;
    uint8_t *Pfont;
    // 对ascii码表偏移（字模表不包含ASCII表的前32个非图形符号）
    ucRelativePositon = cChar - ' ';
    //每个字模的字节数
    fontLength = (LCD_Currentfonts->Width*LCD_Currentfonts->Height)/8;
    //字模首地址
    // ascii码表偏移值乘以每个字模的字节数，求出字模的偏移位置
    Pfont = (uint8_t *)&LCD_Currentfonts->table[ucRelativePositon * fontLength];
    //设置显示窗口
    ILI9341_OpenWindow(usX, usY, LCD_Currentfonts->Width, LCD_Currentfonts->Height);
    ILI9341_Write_Cmd(CMD_SetPixel );
    // 按字节读取字模数据
    // 由于前面直接设置了显示窗口，显示数据会自动换行
    for(byteCount = 0; byteCount < fontLength; byteCount++)
    {
        // 一位一位处理要显示的颜色
        for(bitCount = 0; bitCount < 8; bitCount++) 
        {
            if(Pfont[byteCount] & (0x80>>bitCount))
                ILI9341_Write_Data(CurrentTextColor);
            else
                ILI9341_Write_Data(CurrentBackColor);
        }
    }
}
```

这个函数与前文中的串口打印字模到上位机的那个函数原理是一样的，只是这个函数要使用液晶显示， 且字模数据并不是一个个独立的数组，而是所有字符的字模都放到同一个数组里，所以显示时，要根据字符编码计算字模数据的偏移， 并把串口打印的处理改成像素点显示。

#### 2.2.5 显示字符串

对ILI9341_DispChar_EN函数进行封装，我们可以得到ASCII字符的字符串显示函数：

```c
/*
在 ILI9341 显示器上显示英文字符串
usX ：在特定扫描方向下字符的起始X坐标
usY ：在特定扫描方向下字符的起始Y坐标
pStr ：要显示的英文字符串的首地址
*/
void ILI9341_DispString_EN(uint16_t usX, uint16_t usY,  char *pStr)
{
    while(* pStr != '\0') 
    {
        if((usX-ILI9341_DispWindow_X_Star+LCD_Currentfonts->Width)>LCD_X_LENGTH) 
        {
            usX = ILI9341_DispWindow_X_Star;
            usY += LCD_Currentfonts->Height;
        }
        if((usY-ILI9341_DispWindow_Y_Star+LCD_Currentfonts->Height)>LCD_Y_LENGTH)
        {
            usX = ILI9341_DispWindow_X_Star;
            usY = ILI9341_DispWindow_Y_Star;
        }
        ILI9341_DispChar_EN(usX, usY, *pStr);
        pStr ++;
        usX += LCD_Currentfonts->Width;
    }
}
```

本函数中的输入参数pStr为指向要显示的字符串的指针，在函数的内部利用while循环把字符串中的字符一个个地利用ILI9341_DispChar_EN函数显示到液晶屏上， 当遇到字符串结束符’0’时完成显示，退出while循环，结束函数。在while循环的开头，有两个if判断操作， 它们分别用于判断显示字符时的X及Y坐标是否超出屏幕的边沿，若超出了则换到下一行，使用这个函数， 可以很方便地利用“ILI9341_DispString_EN (10，20,”test”)”这样的格式在液晶屏上直接显示一串字符。

#### 2.2.6 使用宏计算Y坐标

使用ILI9341_DispString_EN函数显示时，需要注意字Y方向字符覆盖的问题，例如在(10,20)坐标处显示了一行字体高度为16像素的字符串， 如果再显示另一串字符时指定的坐标为(10,25)，那么由于高度预留不足，会出现字符覆盖的现象，因此调用时需要小心计算Y方向的坐标。 为了简化操作，代码中定义了一个宏LINE及函数ILI9341_DispStringLine_EN来显示字符串：

```c
#define LINE(x) ((x) * (((sFONT *)LCD_GetFont())->Height))
/*
在 ILI9341 显示器上显示英文字符串
line ：在特定扫描方向下字符串的起始Y坐标
本参数可使用宏LINE(0)、LINE(1)等方式指定文字坐标，
宏LINE(x)会根据当前选择的字体来计算Y坐标值。
显示中文且使用LINE宏时，需要把英文字体设置成Font8x16
pStr ：要显示的英文字符串的首地址
*/
void ILI9341_DispStringLine_EN(uint16_t line,  char *pStr)
{
    uint16_t usX = 0;
    while(*pStr != '\0') 
    {
        if((usX-ILI9341_DispWindow_X_Star+LCD_Currentfonts->Width)>LCD_X_LENGTH)
        {
            usX = ILI9341_DispWindow_X_Star;
            line += LCD_Currentfonts->Height;
        }
        if((line-ILI9341_DispWindow_Y_Star+LCD_Currentfonts->Height)>LCD_Y_LENGTH)
        {
            usX = ILI9341_DispWindow_X_Star;
            line = ILI9341_DispWindow_Y_Star;
        }
        ILI9341_DispChar_EN(usX, line, *pStr);
        pStr ++;
        usX += LCD_Currentfonts->Width;
    }
}
```

本函数主体与ILI9341_DispString_EN差异不大，主要是删减了X方向的坐标，另外使用输入参数line来指定Y方向的坐标，调用本函数时， 一般配合上面的LINE宏来使用，该宏会根据当前选择的字体高度来计算字符Y方向的间隔，如当前字体为8x16时，字体高度为16像素， 所以LINE(1)会返回数值16，LINE(3)会返回数值48。 利用它配合本函数可以使用“ ILI9341_DispStringLine_EN (LINE(1),”test”);” 的形式来使字符串显示在第1行，即Y方向为16像素处， 当然，使用前面的函数“ ILI9341_DispString_EN(10,LINE(1),”test”); ”同样也是可以的，并且可指定X坐标。

#### 2.2.7 清除屏幕字符

在实际应用中，还经常需要把当前屏幕显示的内容清除掉，这可以使用

```c
/*
对ILI9341显示器的某一窗口以某种颜色进行清屏
usX ：在特定扫描方向下窗口的起点X坐标
usY ：在特定扫描方向下窗口的起点Y坐标
usWidth ：窗口的宽度
usHeight ：窗口的高度
*/
void ILI9341_Clear(uint16_t usX, uint16_t usY,uint16_t usWidth, uint16_t usHeight)
{
    ILI9341_OpenWindow(usX, usY, usWidth, usHeight);
    ILI9341_FillColor(usWidth * usHeight, CurrentBackColor);

}
/*
清除某行文字
Line: 指定要删除的行
本参数可使用宏LINE(0)、LINE(1)等方式指定要删除的行，
宏LINE(x)会根据当前选择的字体来计算Y坐标值，并删除当前字体高度的第x行。
*/
void LCD_ClearLine(uint16_t Line)
{
    ILI9341_Clear(0, Line, LCD_X_LENGTH, ((sFONT *)LCD_GetFont())->Height);
}
```

代码中的ILI9341_Clear函数可以直接清除一个指定的矩形，它会把该矩形显示成当前设置的背景颜色CurrentBackColor，实现清除图像的效果， 而LCD_ClearLine函数对它进行了封装，使用LINE宏配合，可以比较方便地清除单行字符串，如调用LCD_ClearLine(LINE(1))可以清除第1行的字符串。

#### 2.2.8 显示ASCII码示例

下面我们再来看main文件是如何利用这些函数显示ASCII码字符的

```c
int main ( void )
{
    ILI9341_Init ();         //LCD 初始化
    USART_Config();
    printf("\r\n ********** 液晶屏英文显示程序*********** \r\n");
    printf("\r\n 本程序不支持中文，显示中文的程序请学习下一章 \r\n");
//其中0、3、5、6 模式适合从左至右显示文字，
//不推荐使用其它模式显示文字 其它模式显示文字会有镜像效果
//其中 6 模式为大部分液晶例程的默认显示方向
    ILI9341_GramScan(6);
    while (1) 
    {
        LCD_Test();
    }
}
```

main函数中主要是对液晶屏初始化，初始化完成后就能够显示ASCII码字符了，无需利用SPI-FLASH及SD卡。 在while循环中调用的LCD_Test函数包含了显示字符的函数调用示例，见

```c
// 用于测试各种液晶的函数
void LCD_Test(void)
{
    // 演示显示变量
    static uint8_t testCNT = 0;
    char dispBuff[100];
    testCNT++;
    LCD_SetFont(&Font8x16);
    LCD_SetColors(RED,BLACK);
    ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH); // 清屏，显示全黑
    // 显示字符串示例
    ILI9341_DispStringLine_EN(LINE(0),"BH 3.2 inch LCD para:");
    ILI9341_DispStringLine_EN(LINE(1),"Image resolution:240x320 px");
    ILI9341_DispStringLine_EN(LINE(2),"ILI9341 LCD driver");
    ILI9341_DispStringLine_EN(LINE(3),"XPT2046 Touch Pad driver");
    // 显示变量示例
    LCD_SetFont(&Font16x24);
    LCD_SetTextColor(GREEN);
    // 使用c标准库把变量转化成字符串
    sprintf(dispBuff,"Count : %d ",testCNT);
    LCD_ClearLine(LINE(4)); // 清除单行文字 
    // 然后显示该字符串即可，其它变量也是这样处理
    ILI9341_DispStringLine_EN(LINE(4), dispBuff);
    /*以下省略其它液晶演示示例*/
}
```

这段代码包含了使用字符串显示函数显示常量字符和变量的示例。显示常量字符串时，直接使用双引号括起要显示的字符串即可， 根据C语言的语法，这些字符串会被转化成常量数组，数组内存储对应字符的ASCII码，然后存储到STM32的FLASH空间， 函数调用时通过指针来找到对应的ASCII码，液晶显示函数使用前面分析过的流程，转换成液晶显示输出。

---

2024.9.28 第一次修订，后期不再维护
