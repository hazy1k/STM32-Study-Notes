# 第五十一章 LCD显示-字库SD卡

## 1. 导入

显示ASCII编码比较简单，由于字库文件小，甚至都不需要使用外部的存储器，而显示汉字时，由于我们的字库是存储到外部存储器上的， 这涉及到额外的获取字模数据的操作。

我们分别制作了两个工程来演示如何显示汉字，以下部分的内容请打开“液晶显示中英文（字库在外部FLASH）”和“液晶显示中英文（字库在SD卡）”工程阅读理解。 这两个工程使用的字库文件内容相同，只是字库存储的位置不一样，工程中我们把获取字库数据相关的函数代码写在“fonts.c”及“fonts.h”文件中， 字符显示的函数仍存储在LCD驱动文件“bsp_ili9341_lcd.c”及“bsp_ili9341_lcd.h”中。

## 2. 软件设计

### 2.1 编程目标

- 获取字模数据

- 根据字模格式，编写液晶显示函数

- 编写测速程序，控制液晶汉字

### 2.2 代码分析

#### 2.2.1 显示汉字字符

由于我们的GB2312字库文件与ASCII字库文件是使用同一种方式生成的，但字符的编码不同导致字模偏移地址计算有区别， 且字模数据存储的位置不同，所以为了显示汉字，需要另外编写一个字符显示函数，它利用前文生成的《GB2312_H1616.FON》字库显示GB2312编码里的字符

```c
#define WIDTH_CH_CHAR   16 // 中文字符宽度
#define HEIGHT_CH_CHAR  16 // 中文字符高度
/*
在 ILI9341 显示器上显示一个中文字符
usX ：在特定扫描方向下字符的起始X坐标
usY ：在特定扫描方向下字符的起始Y坐标
usChar ：要显示的中文字符（国标码）
*/
void ILI9341_DispChar_CH(uint16_t usX, uint16_t usY, uint16_t usChar)
{
    uint8_t rowCount, bitCount;
    uint8_t ucBuffer [WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8];
    uint16_t usTemp;
    // 设置显示窗口
    ILI9341_OpenWindow(usX, usY, WIDTH_CH_CHAR, HEIGHT_CH_CHAR);
    ILI9341_Write_Cmd(CMD_SetPixel);
    // 取字模数据
    GetGBKCode(ucBuffer, usChar);
    for(rowCount = 0; rowCount < HEIGHT_CH_CHAR; rowCount++) 
    {
        // 取出两个字节的数据，在lcd上即是一个汉字的一行 
        usTemp = ucBuffer [rowCount * 2];
        usTemp = (usTemp << 8);
        usTemp |= ucBuffer [rowCount * 2 + 1];
        for(bitCount = 0; bitCount < WIDTH_CH_CHAR; bitCount ++) 
        {
            if(usTemp & (0x8000 >> bitCount)) // 高位在前
                ILI9341_Write_Data(CurrentTextColor);
            else
                ILI9341_Write_Data(CurrentBackColor);
        }
    }
}
```

这个GB2312码的显示函数与ASCII码的显示函数是很类似的，它的输入参数有usX，usY及usChar。其中usX和usY用于设定字符的显示位置， usChar是字符的编码，这是一个16位的变量，因为GB2312编码中每个字符是2个字节的。函数的执行流程介绍如下：

1. 使用ILI9341_OpenWindow和 ILI9341_Write_Cmd来设置显示窗口并发送显示像素命令。

2. 使用GetGBKCode函数获取字模数据，向该函数输入usChar参数(字符的编码)， 它会从外部SPI-FLASH芯片或SD卡中读取该字符的字模数据，读取得的数据被存储到数组ucBuffer中。关于GetGBKCode函数我们在后面详细讲解。

3. 遍历像素点。这个代码在遍历时还使用了usTemp变量用来缓存一行的字模数据(本字模一行有2个字节)， 然后一位一位地判断这些数据，数据位为1的时候，像素点就显示字体颜色，否则显示背景颜色。原理是跟ASCII字符显示一样的。

#### 2.2.2 显示中英文字符串

类似地，我们希望希望汉字也能直接以字符串的形式来调用函数显示，而且最好是中英文字符可以混在一个字符串里。 为此，我们编写了LCD_DisplayStringLine_EN_CH 函数：

```c
/*
在 ILI9341 显示器上显示中英文字符串
usX ：在特定扫描方向下字符的起始X坐标
usY ：在特定扫描方向下字符的起始Y坐标
pStr ：要显示的字符串的首地址
*/
void ILI9341_DispString_EN_CH(uint16_t usX, uint16_t usY, char * pStr)
{
    uint16_t usCh;
    while(* pStr != '\0') 
    {
        if(*pStr <= 126) 
        {   // 英文字符
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
            ILI9341_DispChar_EN ( usX, usY, * pStr );
            usX +=  LCD_Currentfonts->Width;
            pStr ++;
        }
        else 
        {   // 汉字字符
            if((usX-ILI9341_DispWindow_X_Star+WIDTH_CH_CHAR)>LCD_X_LENGTH) 
            {
                usX = ILI9341_DispWindow_X_Star;
                usY += HEIGHT_CH_CHAR;
            }
            if((usY-ILI9341_DispWindow_Y_Star+HEIGHT_CH_CHAR)>LCD_Y_LENGTH) 
            {
                usX = ILI9341_DispWindow_X_Star;
                usY = ILI9341_DispWindow_Y_Star;
            }
            usCh = *(uint16_t*)pStr;
            usCh = (usCh << 8) + (usCh >> 8);
            ILI9341_DispChar_CH(usX, usY, usCh);
            usX += WIDTH_CH_CHAR;
            pStr += 2; // 一个汉字两个字节
        }
    }
}
```

这个函数根据字符串中的编码值，判断它是ASCII码还是国标码中的字符，然后作不同处理。英文部分与前方中的英文字符串显示函数是一样的， 中文部分也很类似，需要注意的是中文字符每个占2个字节，而且由于STM32芯片的数据是小端格式存储的，国标码是大端格式存储的， 所以函数中对输入参数pStr指针获取的编码usCh交换了字节顺序，再输入到单个字符的显示函数LCD_DispChar_CH中。

#### 2.2.3 获取SPI-FLASH中的字模数据

前面提到的GetGBKCode函数用于获取汉字字模数据，它根据字库文件的存储位置，有SPI-FLASH和SD卡两个版本， 我们先来分析比较简单的SPI-FLASH版本。

 该函数定义在“液晶显示中英文（字库在外部FLASH）”工程的“fonts.c”和“fonts.h”文件中。

```c
/*************fonts.h文件中的定义 **********************************/
// 使用FLASH字模
//中文字库存储在FLASH的起始地址
// FLASH
#define GBKCODE_START_ADDRESS   387*4096
// 获取字库的函数
// 定义获取中文字符字模数组的函数名，ucBuffer为存放字模数组名，usChar为中文字符（国标码）
#define GetGBKCode(ucBuffer, usChar)  GetGBKCode_from_EXFlash(ucBuffer, usChar)
int GetGBKCode_from_EXFlash(uint8_t * pBuffer, uint16_t c);
/*********************************************************************/

/************fonts.c文件中的字义**************************************/
// 使用FLASH字模
// 字模GB2312_H1616配套的函数

/*
获取FLASH中文显示字库数据
pBuffer:存储字库矩阵的缓冲区
c ： 要获取的文字
*/
int GetGBKCode_from_EXFlash( uint8_t * pBuffer, uint16_t c)
{
    unsigned char High8bit,Low8bit;
    unsigned int pos;
    static uint8_t everRead=0;

    // 第一次使用，初始化FLASH
    if(everRead == 0) 
    {
        SPI_FLASH_Init();
        everRead = 1;
    }
    High8bit= c >> 8;     // 取高8位数据 
    Low8bit= c & 0x00FF;  // 取低8位数据
    // GB2312 公式
    pos = ((High8bit-0xa1)*94+Low8bit-0xa1)*WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8;
    // 读取字库数据
    SPI_FLASH_BufferRead(pBuffer,GBKCODE_START_ADDRESS+pos,
    WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8);
    return 0;
}
```

这个GetGBKCode实质上是一个宏，当使用SPI-FLASH作为字库数据源时，它等效于GetGBKCode_from_EXFlash函数，它的执行过程如下：

1. 初始化SPI外设，以确保后面使用SPI读取FLASH内容时SPI已正常工作， 并且初始化后做一个标记，以后再读取字模数据的时候就不需要再次初始化SPI了；

2. 取出要显示字符的GB2312编码的高位字节和低位字节， 以便后面用于计算字符的字模地址偏移；

3. 根据字符的编码及字模的大小导出的寻址公式， 计算当前要显示字模数据在字库中的地址偏移；

4. 利用SPI_FLASH_BufferRead函数，从SPI-FLASH中读取该字模的数据， 输入参数中的GBKCODE_START_ADDRESS是在代码头部定义的一个宏， 它是字库文件存储在SPI-FLASH芯片的基地址，该基地址加上字模在字库中的地址偏移，即可求出字模在SPI-FLASH中存储的实际位置。 这个基地址具体数值是在我们烧录FLASH字库时决定的，程序中定义的是实验板出厂时默认烧录的位置。

5. 获取到的字模数据存储到pBuffer指针指向的存储空间， 显示汉字的函数直接利用它来显示字符。

#### 2.2.4 获取SD卡中的字模数据

类似地，从SD卡中获取字模数据时，使用GetGBKCode_from_sd函数，该函数定义在“液晶显示中英文（字库在SD卡）”工程的“fonts.c”和“fonts.h”文件中。

```c
/*************fonts.h文件中的定义 **********************************/
// 使用SD字模
// SD卡字模路径
#define GBKCODE_FILE_NAME     "0:/srcdata/GB2312_H1616.FON"
// 获取字库的函数
// 定义中文字符字模数组的函数名，ucBuffer为存放字模数组名，usChar为中文字符（国标码）
#define GetGBKCode(ucBuffer, usChar)  GetGBKCode_from_sd(ucBuffer, usChar)
int GetGBKCode_from_sd(uint8_t *pBuffer, uint16_t c);
/*********************************************************************/

/************fonts.c文件中的字义**************************************/
// 使用SD字模
static FIL fnew;       //  文件句柄 
static FATFS fs;       // 文件系统句柄 
static FRESULT res_sd;
static UINT br;        // 文件 R/W 计数 
/*
获取SD卡中文显示字库数据
pBuffer:存储字库矩阵的缓冲区
c ： 要获取的文字
*/
int GetGBKCode_from_sd ( uint8_t * pBuffer, uint16_t c)
{
    unsigned char High8bit,Low8bit;
    unsigned int pos;
    static uint8_t everRead = 0;

    High8bit= c >> 8;    // 取高8位数据 
    Low8bit= c & 0x00FF; // 取低8位数据 
    pos = ((High8bit-0xa1)*94+Low8bit-0xa1)*WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8;
    // 第一次使用，挂载文件系统，初始化sd
    if (everRead == 0) 
    {
        res_sd = f_mount(&fs,"0:",1);
        everRead = 1;
    }
    res_sd = f_open(&fnew , GBKCODE_FILE_NAME, FA_OPEN_EXISTING | FA_READ);
    if(res_sd == FR_OK) 
    {
        f_lseek (&fnew, pos);   //指针偏移
        // 16*16大小的汉字 其字模 占用16*16/8个字节
        res_sd = f_read( &fnew, pBuffer, WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8, &br );
        f_close(&fnew);
        return 0;
    } 
    else
        return -1;
}
```

当字库的数据源在SD卡时，GetGBKCode宏指向的是这个GetGBKCode_from_sd函数。由于字库是使用SD卡的文件系统存储的， 从SD卡中获取字模数据实质上是直接读取字库文件，利用f_lseek函数偏移文件的读取指针，使它能够读取特定字符的字模数据。

由于使用文件系统的方式读取数据比较慢，而SD卡大多数都会使用文件系统，所以我们一般使用SPI-FLASH直接存储字库(不带文件系统地使用)， 市场上有一些厂商直接生产专用的字库芯片，可以直接使用，省去自己制作字库的麻烦

#### 2.2.5 显示GB2312字符示例

下面我们再来看main文件是如何利用这些函数显示GB2312的字符，由于我们用GetGBKCode宏屏蔽了差异，所以在上层使用字符串函数时， 不需要针对不同的字库来源写不同的代码

```c
int main(void)
{
    ILI9341_Init (); // LCD 初始化
    USART_Config();
    printf("\r\n ****** 液晶屏中文显示程序（字模文件在SD卡）**** \r\n");
    printf("\r\n 实验前请阅读工程中的readme.txt文件说明，存储字模数据到SPI-FLASH或SD卡\r\n");
//其中0、3、5、6 模式适合从左至右显示文字，
//不推荐使用其它模式显示文字 其它模式显示文字会有镜像效果
//其中 6 模式为大部分液晶例程的默认显示方向
    ILI9341_GramScan(6);
    while(1) 
    {
        LCD_Test();
    }
}
```

main文件中的初始化流程与普通的液晶初始化没有区别，这里也不需要初始化SPI或SDIO， 因为我们在获取字库的函数中包含了相应的初始化流程。在while循环里调用的LCD_Test包含了显示GB2312字符串的示例：

```c
// 用于测试各种液晶的函数
void LCD_Test(void)
{
    // 演示显示变量
    static uint8_t testCNT = 0;
    char dispBuff[100]; // 缓冲区
    testCNT++;

    LCD_SetFont(&Font8x16);
    LCD_SetColors(RED,BLACK);
    ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH); // 清屏，显示全黑
    /********显示字符串示例*******/
    ILI9341_DispStringLine_EN_CH(LINE(0),"野火3.2寸LCD参数：");
    ILI9341_DispStringLine_EN_CH(LINE(1),"分辨率：240x320 px");
    ILI9341_DispStringLine_EN_CH(LINE(2),"ILI9341液晶驱动");
    ILI9341_DispStringLine_EN_CH(LINE(3),"XPT2046触摸屏驱动");
    /********显示变量示例*******/
    LCD_SetTextColor(GREEN);
    // 使用c标准库把变量转化成字符串
    sprintf(dispBuff,"显示变量： %d ",testCNT);
    LCD_ClearLine(LINE(5)); // 清除单行文字 
    // 然后显示该字符串即可，其它变量也是这样处理
    ILI9341_DispStringLine_EN_CH(LINE(5), dispBuff);
    /******其它液晶演示示例省略******/
```

---

2024.9.28 第一次修订，后期不再维护
