【*】程序简介
-工程名称：SPI-FatFs文件系统
-实验平台: 野火STM32 F103-指南者 开发板 
-ST固件库版本：3.5
【 ！】功能简介：
把FLASH芯片格式化成文件系统，使可以文件系统的方式读写文件数据
学习目的：学习文件系统在flash上的移植和应用。
【 ！】实验操作：
电脑端使用串口调试助手，选择电脑与STM32相连的COM口，设置为115200-N-8-1，
复位开发板，即可接收STM32串口发送给电脑的调试信息。													
【*】程序描述：
< diskio.c >	
1.宏定义设备物理编号
2.设备状态获取
	1.状态设置为未初始化				
	2.获取物理编号，使用switch函数切换到不同的物理设备
	3.对于 SPI Flash 芯片，我们直接调用在 SPI_FLASH_ReadID获取设备 ID确认芯片是否正常初始化，
	  如果正确，函数清除未初始化标志；如果错误，函数返回未初始化状态
	PS：SPI_FLASH相关函数定义在 flash.c 文件中
3.设备初始化
	1.状态设置为未初始化	
	2.指定设备物理编号
	3.延时一小段时间，此期间唤醒SPI FLASH，获取SPI FLASH芯片状态为当前状态
	4.超时还未完成前面的操作，状态回到未初始化状态
4.扇区读取
	1.状态设置为无效参数
	2.通过设备物理编号切换到对应的设备，读取对应的空间，读取完则把状态设置为成功，
	   出现别的情况则状态回到无效参数
	 * 开发板使用的SPI Flash芯片型号为W25Q64，每个扇区大小为4096个字节(4KB)，总共有8M 字节空间，
  	   为兼容后面实验程序，见上外部Flash使用情况说明表，我们只将部分空间分配给FatFs使用，
	   例如如果偏移512个扇区，即偏移了2MB 
	如果不想偏移，则把读扇区函数和写扇区函数的 sector+=那行注释掉就行 以及 返回总扇区个数的函数也要对应改
	 * SPI_FLASH_BufferRead中，第二个参数为指定数据读取地址，左移12位实际是乘以4096，
	 第三个参数为读取数据个数，也是需要使用左移12位，左移位数与扇区的大小有关
5.扇区写入
	（原理大致与扇区读取相同）
	1.用到扇区擦除函数，再调用数据写入函数把数据写入到指定位置内
	2.当扇区个数为0时，返回“无效参数”
	3.使用设备物理编号切换到对应设备，在使用SPI时在写入数据之前需要先擦除再写入，出现其他情况返回“无效参数”
6.其他控制
	1.对于 SPI Flash 芯片，为支持 FatFs 格式化功能，需要用到获取扇区数量 (GET_SECTOR_COUNT)、获取扇区大小（GET_SECTOR_SIZE）和获取擦除块数量 (GET_BLOCK_SIZE)指令
	 * disk_ioctl是一个负责杂项的函数，它的功能由提供的指令变化会有所不同，在本函数中，由指令的不同切换到不同的返回值
7.时间戳获取
	 * get_fattime 在 ff.c 文件中被调用。
	  FatFs 在文件创建、被修改时会记录时间，这里直接使用赋值方法（2015年1月1日0点0分0秒）设定时间戳
< ffconf.h >
	1.置一格式化功能以使用格式化
	2.为支持简体中文文件名需要语言功能选择为936
	3.把长文件名支持配置为2，使其支持长文件名，指定使用栈空间为缓冲区
	4.把指定物理设备数量设置为2
	5.扇区的最大值指定为4096（SPI FLASH芯片），最小值指定为512（SD卡扇区大小）
< main.c >
	1.初始化LED，初始化串口，输出提示信息
测试格式化
	1.使用f_mount 函数挂载工作区
	2.当f_mount 函数返回值为 FR_NO_FILESYSTEM，说明还没有文件系统则使用 f_mkfs格式化，创建文件系统
		检测到格式化成功后需要先取消挂载原来设备，再重新挂载设备；没成功则返回提示信息
	  当f_mount 函数返回值为不成功，输出提示信息
	  当f_mount 函数返回值为成功，输出提示信息，表明文件系统挂载成功
写入测试
	1.使用 f_open 函数打开文件，访问文件模式选择为总是新建文件与写模式
	2.f_open 函数返回值为成功，输出提示信息
		使用f_write函数向文件写入数据，指针地址为fnum变量，它指示成功写入的字节个数。
			f_write 函数返回值为成功，输出提示信息
			f_write 函数返回值为失败，输出提示信息
	   f_open 函数返回值为不成功，输出提示信息
读取测试
	1.使用 f_open 函数打开文件，访问文件模式选择为打开已存在的文件与读模式
	2.f_open 函数返回值为成功，输出提示信息
		使用f_read函数向文件写入数据，指针地址为fnum变量，它指示成功读取的字节个数。
			f_read 函数返回值为成功，输出提示信息
			f_read 函数返回值为不成功，输出提示信息
	   f_open 函数返回值为不成功，输出提示信息
关闭
	1.使用f_close 函数用于不再对文件进行读写操作，关闭文件
	2.取消挂载文件系统
【*】 函数介绍：
f_mount：挂载函数，相当于在代码里面初始化了文件系统这整一个代码
形参：
	1.指向 FATFS 变量指针，为null时则取消物理设备挂载
	2.逻辑设备编号
	3.是否立即挂载
f_mkfs ：格式化操作函数
形参：
	1.逻辑设备编号
	2.硬盘/软盘
	3.指定扇区大小，为0时则去disk_ioctl 函数获取大小
f_open ：打开文件
形参：
	1.文件对象指针
	2.目标文件
	3.访问文件模式选择
f_write ：数据写入
形参：
	1.文件对象指针，使用与 f_open 函数一致即可
	2.待写入数据的首地址
	3.写入数据的字节数
	4.32 位无符号整形指针
f_read ：数据读取
形参：
	1.文件对象指针，使用与 f_open 函数一致即可
	2.用来存放读出数据的首地址
	3.欲读取数据的字节数
	4.32 位无符号整形指针