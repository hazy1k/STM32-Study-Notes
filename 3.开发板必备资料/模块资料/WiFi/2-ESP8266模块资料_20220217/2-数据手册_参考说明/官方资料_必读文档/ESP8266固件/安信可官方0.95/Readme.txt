下载地址：http://www.ai-thinker.com/forum.php?mod=viewthread&tid=512&extra=page%3D1




更新日志

1  修正了串口频繁执行AT指令时重启的问题与大数据量停止响应问题
2  不再挑剔串口调试软件，支持PuTTY，SecureCRT和其他串口调试工具，对AT命令结尾，不强制\r\n，CR或LF都可以，CRLF也可以
3  AT+CIPAP?  返回本模块当前AP模式下的IP地址AT+CIPAP="192.168.4.2"设置AP模式下的IP地址
4  AT+CIPSTA?  查看STA模式下的IP地址AT+CIPSTA="192.168.1.2"设置STA模式下的固定IP 以上操作都是立即生效不需要重启
5  AT+CIPAPMAC? 查询当前AP模式的MAC地址 设置当前AP模式的MAC地址AT+CIPAPMAC="1a:fe:34:9e:34:a9"
6  AT+CIPSTAMAC?询STA模式下的MAC地址AT+CIPSTAMAC="18:fe:34:9e:34:a9"设置当前AP模式的MAC地址
7  AT+CIOADC命令读取ADC值注意：ADC输入电压范围是0V到1V之间！0到1024,10位精度
8  AT+CIOREAD命令读取GPIO电平AT+CIOREAD=0读取GPIO0的电平假设GPIO0是高电平，会返回1:HIGH,否则返回0 : LOW GPIO的范围是数字，GPIO0,GPIO16都可以
9  AT+CIOWRITE命令设置GPIO电平AT+CIOWRITE=0,0   设置GPIO0的电平为低AT+CIOWRITE=16,1 设置GPIO16的电平为高
10 波特率支持更广110,300,600,1200,2400,4800,9600,19200,38400,57600,74880,115200,230400,256000,460800,921600
11 bugfix

下一版本支持6路PWM AT设置 云服务端 以及Smartconfig的AT指令




烧录方式

在正常启动状态下只需把GPIO 0脚位拉低即可进入FLASH烧写模式


在配置Tag里按照图示只选中地址为0x00000的选项（在相关选项打X），点击选择新版本固件
然后开始选择目标串口即可一键烧写(无反应的话需要反复重新上电）。完成后记得把GOIO 0 拉高。（内部已默认拉高）