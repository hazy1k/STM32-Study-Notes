# DMA-内存到外设

## 功能简介

我们先定义一个数据变量，存于 SRAM 中，然后通过 DMA 的方式传输到串口的数据寄存器，然后通过串口把这些数据发送到电脑的上位机显示出来。

## 引脚分配

存储器到外设模式使用到 USART1 功能，具体电路设置参考 USART 章节，无需其他硬件设计。
