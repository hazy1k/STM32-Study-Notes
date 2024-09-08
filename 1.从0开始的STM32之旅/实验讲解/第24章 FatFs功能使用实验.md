# 第二十四章 FatFs功能使用实验

上个实验我们实现了FatFs的格式化、读文件和写文件功能，这个已经满足很多部分的运用需要。有时，我们需要更多的文件操作功能， FatFs还是提供了不少的功能的，比如设备存储空间信息获取、读写文件指针定位、创建目录、文件移动和重命名、文件或目录信息获取等等功能。 我们接下来这个实验内容就是展示FatFs众多功能，提供一个很好了范例，以后有用到相关内容，参考使用非常方便。

## 1. 硬件设计

本实验主要使用FatFs软件功能，不需要其他硬件模块，使用与FatFs移植实验相同硬件配置即可。

## 2. 软件设计

上个实验我们已经移植好了FatFs，这个例程主要是应用，所以简单起见，直接拷贝上个实验的工程文件，保持FatFs底层驱动程序， 我们只改main.c文件内容，实现应用程序。

- FatFs多项功能测试

```c
// FatFs多项功能测试 
static FRESULT miscellaneous(void)
{
    DIR dir;
    FATFS *pfs;
    DWORD fre_clust, fre_sect, tot_sect;

    printf("\n*************** 设备信息获取 ***************\r\n");
    // 获取设备信息和空簇大小 
    res_flash = f_getfree("1:", &fre_clust, &pfs);

    // 计算得到总的扇区个数和空扇区个数 
    tot_sect = (pfs->n_fatent - 2) * pfs->csize;
    fre_sect = fre_clust * pfs->csize;

    // 打印信息(4096 字节/扇区) 
    printf("》设备总空间：%10lu KB。\n》可用空间：  %10lu KB。\n",
        tot_sect *4, fre_sect *4);

    printf("\n******** 文件定位和格式化写入功能测试 ********\r\n");
    res_flash = f_open(&fnew, "1:FatFs功能测试文件.txt",
                    FA_CREATE_ALWAYS|FA_WRITE|FA_READ );
    res_flash=f_write(&fnew,"欢迎使用野火STM32开发板",50,&fnum);
    if (res_flash == FR_OK )
    {
        //  文件定位，定位到文件的末尾
        res_flash = f_lseek(&fnew,f_size(&fnew)-1);
        if (res_flash == FR_OK)
        {
            // 格式化写入，参数格式类似printf函数 
            f_printf(&fnew,"\n在原来文件新添加一行内容\n");
            f_printf(&fnew,"》设备总空间：%10lu KB。\n》可用空间;%10lu KB。\n",
                    tot_sect *4, fre_sect *4);
            // 文件定位到文件起始位置
            res_flash = f_lseek(&fnew,0);
            // 读取文件所有内容到缓存区
            res_flash = f_read(&fnew,readbuffer,f_size(&fnew),&fnum);
            if (res_flash == FR_OK)
            {
                printf("》文件内容：\n%s\n",readbuffer);
            }
        }
        f_close(&fnew);

        printf("\n********** 目录创建和重命名功能测试 **********\r\n");
        // 尝试打开目录
        res_flash = f_opendir(&dir,"1:TestDir");
        if(res_flash != FR_OK)
        {
            // 打开目录失败，就创建目录 
            res_flash = f_mkdir("1:TestDir");
        }
        else
        {
            // 如果目录已经存在，关闭它 
            res_flash=f_closedir(&dir);
            // 删除文件 
            f_unlink("1:TestDir/testdir.txt");
        }
        if(res_flash == FR_OK)
        {
            // 重命名并移动文件
            res_flash=f_rename("1:FatFs功能测试文件.txt",
                            "1:TestDir/testdir.txt");
            if(res_flash == FR_OK)
            {
                printf("》重命名并移动文件操作成功\n");
            }
            else
            {
                printf("》重命名并移动文件操作失败：%d\n",res_flash);
            }
        }
    }
    else
    {
        printf("!! 打开文件失败：%d\n",res_flash);
        printf("!! 或许需要再次运行“FatFs移植与读写测试”工程\n");
    }
    return res_flash;
}
```

首先是设备存储信息获取，目的是获取设备总容量和剩余可用空间。f_getfree函数是设备空闲簇信息获取函数，有三个形参，第一个参数为逻辑设备编号； 第二个参数为返回空闲簇数量，这里1簇等于1个扇区；第三个参数为返回指向文件系统对象的指针。通过计算可得到设备总的扇区个数以及空闲扇区个数， 对于SPI Flash芯片我们设置每个扇区为4096字节大小，即4KB。这样很容易就算出设备存储信息。

接下来是文件读写指针定位和格式化输入功能测试。文件定位在一些场合非常有用，比如我们需要记录多项数据，但每项数据长度不确定， 但有个最长长度，我们就可以使用文件定位lseek函数功能把数据存放在规定好的地址空间上。当我们需要读取文件内容时就使用文件定位函数定位到对应地址读取。

使用文件读写操作之前都必须使用f_open函数打开文件，开始的时候读写指针是在文件起始位置的，马上写入数据的话会覆盖原来文件内容。 这里，我们使用f_lseek函数定位到文件末尾位置，再写入内容。f_lseek函数有两个形参，第一个参数为文件对象指针，第二个参数为需要定位的字节数， 这个字节数是相对文件起始位置的，比如设置为0，则将文件读写指针定位到文件起始位置。

f_printf函数是格式化写入函数，需要把ffconf.h文件中的_USE_STRFUNC配置为1才支持。f_printf函数用法类似C库函数printf函数， 只是它将数据直接写入到文件中。

最后是目录创建和文件移动和重命名功能。使用f_opendir函数可以打开路径(这里不区分目录和路径概念，下同)，如果路径不存在则创建， 如果存在则使用f_closedir函数关闭已经打开的路径，并把以前创建的文件删除。新版的FatFs支持相对路径功能，使路径操作更加灵活。 f_opendir函数有两个形参，第一个参数为指向路径对象的指针，第二个参数为路径。f_closedir函数只需要指向路径对象的指针一个形参。

f_mkdir函数用于创建路径，如果指定的路径不存在就创建它，创建的路径存在形式就是文件夹。f_mkdir函数只要一个形参，就是指定路径。

f_rename函数是带有移动功能的重命名函数，它有两个形参，第一个参数为源文件名称，第二个参数为目标名称。目标名称可附带路径， 如果路径与源文件路径不同则移动文件到目标路径下。

- 文件信息获取

```c
static FRESULT file_check(void)
{
    static FILINFO fno;

    // 获取文件信息，必须确保文件存在
    res_flash = f_stat("1:TestDir/testdir.txt",&fno);
    if (res_flash==FR_OK) {
        printf("“testdir.txt”文件信息：\n");
        printf("》文件大小: %ld(字节)\n", fno.fsize);
        printf("》时间戳: %u/%02u/%02u, %02u:%02u\n",
            (fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,
            fno.ftime >> 11, fno.ftime >> 5 & 63);
        printf("》属性: %c%c%c%c%c\n\n",
            (fno.fattrib & AM_DIR) ? 'D' : '-',      // 是一个目录
            (fno.fattrib & AM_RDO) ? 'R' : '-',      // 只读文件
            (fno.fattrib & AM_HID) ? 'H' : '-',      // 隐藏文件
            (fno.fattrib & AM_SYS) ? 'S' : '-',      // 系统文件
            (fno.fattrib & AM_ARC) ? 'A' : '-');     // 档案文件
    }
    return res_flash;
}
```

f_stat函数用于获取文件的属性，有两个形参，第一个参数为文件路径，第二个参数为返回指向文件信息结构体变量的指针。 文件信息结构体变量包含文件的大小、最后修改时间和日期、文件属性、短文件名以及长文件名等信息。

- 路径扫描

```c
static FRESULT scan_files (char* path)
{
    FRESULT res;    //部分在递归过程被修改的变量，不用全局变量
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;        // 文件名

#if _USE_LFN
    // 长文件名支持 
    // 简体中文需要2个字节保存一个“字”
    static char lfn[_MAX_LFN*2 + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif
    //打开目录
    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        i = strlen(path);
        for (;;) {
            //读取目录下的内容，再读会自动读下一个文件
            res = f_readdir(&dir, &fno);
            //为空时表示所有项目读取完毕，跳出
            if (res != FR_OK || fno.fname[0] == 0) break;
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            //点表示当前目录，跳过
            if (*fn == '.') continue;
            //目录，递归读取
            if (fno.fattrib & AM_DIR) {
                //合成完整目录名
                sprintf(&path[i], "/%s", fn);
                //递归遍历
                res = scan_files(path);
                path[i] = 0;
                //打开失败，跳出循环
                if (res != FR_OK)
                    break;
            } else {
                printf("%s/%s\r\n", path, fn);              //输出文件名
                /* 可以在这里提取特定格式的文件路径 */
            }//else
        } //for
    }
    return res;
}
```

scan_files函数用来扫描指定路径下的文件。比如我们设计一个mp3播放器，我们需要提取mp3格式文件，诸如*.txt、*.c文件我们统统不可要的， 这时我们就必须扫描路径下所有文件并把*.mp3或*.MP3格式文件提取出来。这里我们不提取特定格式文件，而是把所有文件名称都通过串口打印出来。

我们在ffconf.h文件中定义了长文件名称支持(_USE_LFN=2)，一般有用到简体中文文件名称的都要长文件名支持。短文件名称是8.3格式，即名称是8个字节， 后缀名是3个字节，对于使用英文名称还可以，使用中文名称就很容易长度不够了。使能了长文件名支持后，使用之前需要指定文件名的存储区还有存储区的大小。

接下来就是使用f_opendir函数打开指定的路径。如果路径存在就使用f_readdir函数读取路径下内容，f_readdir函数可以读取路径下的文件或者文件夹， 并保存信息到文件信息对象变量内。f_readdir函数有两个形参，第一个参数为指向路径对象变量的指针，第二个参数为指向文件信息对象的指针。 f_readdir函数另外一个特性是自动读取下一个文件对象，即循序运行该函数可以读取该路径下的所有文件。所以，在程序中， 我们使用for循环让f_readdir函数读取所有文件，并在读取所有文件之后退出循环。

在f_readdir函数成功读取到一个对象时，我们还不清楚它是一个文件还是一个文件夹，此时我们就可以使用文件信息对象变量的文件属性来判断了， 如果判断得出是个文件那我们就直接通过串口打印出来就好了。如果是个文件夹，我们就要进入该文件夹扫描，这时就重新调用扫描函数scan_files就可以了， 形成一个递归调用结构，只是我们这次用的参数与最开始时候是不同的，现在是使用子文件夹名称。

- 主函数

```c
int main(void)
{
    /* 初始化调试串口，一般为串口1 */
    USART_Config();
    printf("******** 这是一个SPI FLASH 文件系统实验 *******\r\n");

    // 在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
    res_flash = f_mount(&fs,"1:",1);
    if (res_flash!=FR_OK) {
        printf("！！外部Flash挂载文件系统失败。(%d)\r\n",res_flash);
        printf("！！可能原因：SPI Flash初始化不成功。\r\n");
        while (1);
    } else {
        printf("》文件系统挂载成功，可以进行测试\r\n");
    }

    // FatFs多项功能测试
    res_flash = miscellaneous();

    printf("\n*************** 文件信息获取测试 **************\r\n");
    res_flash = file_check();

    printf("***************** 文件扫描测试 ****************\r\n");
    strcpy(fpath,"1:");
    scan_files(fpath);

    // 不再使用文件系统，取消挂载文件系统 
    f_mount(NULL,"1:",1);

    // 操作完成，停机 
    while (1) {
    }
}
```

串口在程序调试中经常使用，可以把变量值直观打印到串口调试助手，这个信息非常重要，同样在使用之前需要调用Debug_USART_Config函数完成调试串口初始化。

使用FatFs进行文件操作之前都使用f_mount函数挂载物理设备，这里我们使用SPI Flash芯片上的FAT文件系统。

接下来我们直接调用miscellaneous函数进行FatFs设备信息获取、文件定位和格式化写入功能以及目录创建和重命名功能测试。 调用file_check函数进行文件信息获取测试。

scan_files函数用来扫描路径下的所有文件，fpath是我们定义的一个包含100个元素的字符型数组， 并将其赋值为SPI Flash芯片物理编号对于的根目录。 这样允许scan_files函数见打印SPIFlash芯片内FatFs所有文件到串口调试助手。注意，这里的定义fpaht数组是必不可少的， 因为scan_files函数本身是个递归函数，要求实际参数有较大空间的缓存区。

保证开发板相关硬件连接正确，用USB线连接开发板“USB TO UART”接口跟电脑，在电脑端打开串口调试助手， 把编译好的程序下载到开发板。程序开始运行，在串口调试助手可看到每个阶段测试相关信息情况。

## 3. 小结

总的来说，这节就是多了一些功能，包括设备信息获取、文件操作、以及目录操作。

1. **获取设备信息：**

```c
res_flash = f_getfree("1:", &fre_clust, &pfs);
```

使用`f_getfree`获取“1:”驱动器的空闲簇数和设备信息。`fre_clust`存储空闲簇数，`pfs`指向文件系统对象。

```c
tot_sect = (pfs->n_fatent - 2) * pfs->csize;
fre_sect = fre_clust * pfs->csize;
```

计算总扇区数和空闲扇区数。`pfs->n_fatent`是FAT表的条目数，减去2是因为保留了根目录的两个条目。`pfs->csize`是每簇的扇区数。

```c
printf("》设备总空间：%10lu KB。\n》可用空间：  %10lu KB。\n",
     tot_sect *4, fre_sect *4);
```

打印设备的总空间和可用空间，以KB为单位（假设每扇区4KB）。

2. 文件定位和格式化写入：

```c
res_flash = f_open(&fnew, "1:FatFs功能测试文件.txt",FA_CREATE_ALWAYS|FA_WRITE|FA_READ );

```

创建一个新文件“FatFs功能测试文件.txt”，若已存在则覆盖。以读写模式打开文件。

```c
res_flash=f_write(&fnew,"欢迎使用野火STM32开发板",50,&fnum);
```

向文件写入一段文本，`50`是要写入的字节数，`fnum`是实际写入的字节数。

```c
res_flash = f_lseek(&fnew,f_size(&fnew)-1);
```

定位到文件的末尾，为添加新内容做准备。

```c
f_printf(&fnew,"\n在原来文件新添加一行内容\n");
f_printf(&fnew,"》设备总空间：%10lu KB。\n》可用空间;%10lu KB。\n",
         tot_sect *4, fre_sect *4);
```

使用`f_printf`在文件末尾添加新内容，类似于`printf`。

```c
res_flash = f_lseek(&fnew,0);
res_flash = f_read(&fnew,readbuffer,f_size(&fnew),&fnum);
```

定位到文件起始位置，读取整个文件的内容到`readbuffer`中。

```c
printf("》文件内容：\n%s\n",readbuffer);
```

打印文件的内容。

3. 目录创建和重命名

```c
res_flash = f_opendir(&dir,"1:TestDir");
if(res_flash != FR_OK)
{
    res_flash = f_mkdir("1:TestDir");
}

```

尝试打开目录“TestDir”。若打开失败，则创建该目录。

```c
f_unlink("1:TestDir/testdir.txt");
```

如果目录已经存在，则关闭它并删除其中的文件“testdir.txt”。

```c
res_flash=f_rename("1:FatFs功能测试文件.txt",
                    "1:TestDir/testdir.txt");

```

将之前创建的文件重命名并移动到“TestDir”目录下。

```c
if(res_flash == FR_OK)
{
    printf("》重命名并移动文件操作成功\n");
}
else
{
    printf("》重命名并移动文件操作失败：%d\n",res_flash);
}
```

打印重命名和移动操作的结果。

---

我们再来简单回顾一下在stm32中使用FatFs的基本流程

1. **初始化**：
   初始化STM32的硬件接口（如SPI、SDIO等），以确保与存储设备（如SD卡）通信正常。

2. **挂载文件系统**：
   使用`f_mount`函数挂载文件系统到指定驱动器。

3. **文件操作**：
   使用`f_open`、`f_read`、`f_write`等函数进行文件的读写操作。

4. **目录操作**：
   使用`f_opendir`、`f_mkdir`、`f_rename`等函数进行目录的创建、删除和重命名操作。

5. **卸载文件系统**：
   使用`f_mount`函数卸载文件系统，通常在操作结束后进行。

**示例代码**：

```c
#include "ff.h" // FatFs头文件
#include "diskio.h" // 硬件接口头文件

FATFS fs; // 文件系统对象
FIL fil;  // 文件对象
FRESULT res; // FatFs返回值
BYTE buffer[128]; // 读写缓存区

void fatfs_example(void)
{
    // 1. 初始化硬件接口（例如SD卡）
    // (初始化代码视具体硬件而定)

    // 2. 挂载文件系统
    res = f_mount(&fs, "", 1);
    if (res != FR_OK) {
        printf("挂载文件系统失败: %d\n", res);
        return;
    }

    // 3. 文件写入操作
    res = f_open(&fil, "example.txt", FA_WRITE | FA_CREATE_ALWAYS);
    if (res == FR_OK) {
        f_write(&fil, "Hello, FatFs!", 13, NULL);
        f_close(&fil);
    } else {
        printf("打开文件失败: %d\n", res);
    }

    // 4. 文件读取操作
    res = f_open(&fil, "example.txt", FA_READ);
    if (res == FR_OK) {
        f_read(&fil, buffer, sizeof(buffer) - 1, NULL);
        buffer[sizeof(buffer) - 1] = '\0'; // 确保字符串终止
        printf("文件内容: %s\n", buffer);
        f_close(&fil);
    } else {
        printf("打开文件失败: %d\n", res);
    }

    // 5. 卸载文件系统
    f_mount(NULL, "", 1);
}

```

---

2024.9.8 第一次修订，后期不再维护
