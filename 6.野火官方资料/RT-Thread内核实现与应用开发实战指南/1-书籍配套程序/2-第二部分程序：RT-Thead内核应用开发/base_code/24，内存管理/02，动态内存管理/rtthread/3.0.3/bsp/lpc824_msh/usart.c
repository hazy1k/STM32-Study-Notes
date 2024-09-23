/*
 * File      : usart.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006-2013, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-28     Tanek        the first version
 */
#include <rtthread.h>
#include "usart.h"

#include "peri_driver.h"

#ifdef RT_USING_UART

#ifndef RT_USING_DEVICE
#error "you must define RT_USING_DEVICE with uart device"
#endif

/* LPC8XX uart driver */
struct lpc8xx_uart
{
    struct rt_device parent;
    LPC_USART_T * uart_base;
    IRQn_Type uart_irq;
    
    rt_uint8_t data;
    struct rt_semaphore sem;
    
};
#ifdef RT_USING_UART0
struct lpc8xx_uart uart0_device;
#endif

#ifdef RT_USING_UART1
struct lpc8xx_uart uart1_device;
#endif

#ifdef RT_USING_UART2
struct lpc8xx_uart uart2_device;
#endif

#if !defined(RT_USING_UART0) && !defined(RT_USING_UART1) && !defined(RT_USING_UART2)
#error "you must define RT_USING_UARTx(x=0, 1, 2) at least one"
#endif

void uart_irq_handler(struct lpc8xx_uart* uart)
{
    uint32_t status;
    
    /* enter interrupt */
    rt_interrupt_enter();
    
    status = Chip_UART_GetStatus(uart->uart_base);
    if(status & UART_STAT_RXRDY)    // RXIRQ
    {
        uart->data = (rt_uint8_t)Chip_UART_ReadByte(uart->uart_base);
        rt_sem_release(&(uart->sem));
    }
    
    /* leave interrupt */
    rt_interrupt_leave();
}

#ifdef RT_USING_UART0
void UART0_IRQHandler(void)
{
    uart_irq_handler(&uart0_device);
}
#endif

#ifdef RT_USING_UART1
void UART1_IRQHandler(void)
{
    uart_irq_handler(&uart1_device);
}
#endif

#ifdef RT_USING_UART2
void UART2_IRQHandler(void)
{
    uart_irq_handler(&uart2_device);
}
#endif

static void uart1_io_init(LPC_USART_T * uart_base)
{
    /* Enable the clock to the Switch Matrix */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
    
    Chip_Clock_SetUARTClockDiv(1);
    
#ifdef RT_USING_UART0
    if (uart_base == LPC_USART0)
    {
		Chip_SWM_MovablePinAssign(SWM_U0_TXD_O, 4);
		Chip_SWM_MovablePinAssign(SWM_U0_RXD_I, 0);
    }
    else
#endif

#ifdef RT_USING_UART1
    if (uart_base == LPC_USART1)
    {
		Chip_SWM_MovablePinAssign(SWM_U1_TXD_O, 4);
		Chip_SWM_MovablePinAssign(SWM_U1_RXD_I, 0);
    }
    else
#endif

#ifdef RT_USING_UART2
    if (uart_base == LPC_USART2)
    {
		Chip_SWM_MovablePinAssign(SWM_U2_TXD_O, 4);
		Chip_SWM_MovablePinAssign(SWM_U2_RXD_I, 0);
    }
    else
#endif
    {
        RT_ASSERT((uart_base == LPC_USART0) || (uart_base == LPC_USART1) || (uart_base == LPC_USART2));
    }
    
	/* Disable the clock to the Switch Matrix to save power */
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);
}

static void uart_ll_init(LPC_USART_T * uart_base)
{
    Chip_UART_Init(uart_base);
    Chip_UART_ConfigData(uart_base, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1);
	Chip_Clock_SetUSARTNBaseClockRate((115200 * 6 * 16), true);
	Chip_UART_SetBaud(uart_base, 115200);
	Chip_UART_Enable(uart_base);
	Chip_UART_TXEnable(uart_base);
    
	// we must NOT enable TX ready/idle IRQ before we want to write data
	// otherwise the IRQs will happen as soon as Uart IRQ is enabled in NVIC
	Chip_UART_IntDisable(uart_base, UART_INTEN_TXRDY | UART_INTEN_TXIDLE);
	Chip_UART_IntEnable(uart_base, UART_INTEN_RXRDY);
}

static rt_err_t rt_uart_init (rt_device_t dev)
{
    struct lpc8xx_uart* uart;
    RT_ASSERT(dev != RT_NULL);
    uart = (struct lpc8xx_uart *)dev;
    
    uart1_io_init(uart->uart_base);
    uart_ll_init(uart->uart_base);

    return RT_EOK;
}

static rt_err_t rt_uart_open(rt_device_t dev, rt_uint16_t oflag)
{
    struct lpc8xx_uart* uart;
    RT_ASSERT(dev != RT_NULL);
    uart = (struct lpc8xx_uart *)dev;

    if (dev->flag & RT_DEVICE_FLAG_INT_RX)
    {
        /* Enable the UART Interrupt */
        NVIC_EnableIRQ(uart->uart_irq);
    }

    return RT_EOK;
}

static rt_err_t rt_uart_close(rt_device_t dev)
{
    struct lpc8xx_uart* uart;
    RT_ASSERT(dev != RT_NULL);
    uart = (struct lpc8xx_uart *)dev;

    if (dev->flag & RT_DEVICE_FLAG_INT_RX)
    {
        /* Disable the UART Interrupt */
        NVIC_DisableIRQ(uart->uart_irq);
    }

    return RT_EOK;
}

static rt_size_t rt_uart_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    struct lpc8xx_uart* uart;
    RT_ASSERT(dev != RT_NULL);
    uart = (struct lpc8xx_uart *)dev;

    RT_ASSERT(uart != RT_NULL);

    rt_sem_take(&(uart->sem), RT_WAITING_FOREVER);
    *((rt_uint8_t *)buffer) = uart->data;

    return 1;
}

static rt_size_t rt_uart_write(rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    char *ptr = (char*) buffer;
    struct lpc8xx_uart* uart;
    RT_ASSERT(dev != RT_NULL);
    uart = (struct lpc8xx_uart *)dev;

    if (dev->open_flag & RT_DEVICE_FLAG_STREAM)
    {
        /* stream mode */
        while (size)
        {
            if (*ptr == '\n')
            {
                while (!(Chip_UART_GetStatus(uart->uart_base) & UART_STAT_TXRDY));
                Chip_UART_SendByte(uart->uart_base, '\r');   
            }

                while (!(Chip_UART_GetStatus(uart->uart_base) & UART_STAT_TXRDY));
            Chip_UART_SendByte(uart->uart_base, *ptr); 

            ptr ++;
            size --;
        }
    }
    else
    {
        while (size)
        {
            while (!(Chip_UART_GetStatus(uart->uart_base) & UART_STAT_TXRDY));
            Chip_UART_SendByte(uart->uart_base, *ptr); 

            ptr++;
            size--;
        }
    }

    return (rt_size_t) ptr - (rt_size_t) buffer;
}

int rt_hw_usart_init(void)
{
#ifdef RT_USING_UART0
    {
        struct lpc8xx_uart* uart;

        /* get uart device */
        uart = &uart0_device;

        /* device initialization */
        uart->uart_base = LPC_USART0;
        uart->uart_irq  = UART0_IRQn;
        
        /* device interface */
        uart->parent.type = RT_Device_Class_Char;
        uart->parent.init 	    = rt_uart_init;
        uart->parent.open 	    = rt_uart_open;
        uart->parent.close      = rt_uart_close;
        uart->parent.read 	    = rt_uart_read;
        uart->parent.write      = rt_uart_write;
        uart->parent.control    = RT_NULL;
        uart->parent.user_data  = RT_NULL;
        
        rt_sem_init(&(uart->sem), "suart0", 0, 0);
        rt_device_register(&uart->parent, "uart0", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    }
#endif
    
#ifdef RT_USING_UART1
    {
        struct lpc8xx_uart* uart;

        /* get uart device */
        uart = &uart1_device;

        /* device initialization */
        uart->uart_base = LPC_USART1;
        uart->uart_irq  = UART1_IRQn;

        /* device interface */
        uart->parent.type       = RT_Device_Class_Char;
        uart->parent.init 	    = rt_uart_init;
        uart->parent.open 	    = rt_uart_open;
        uart->parent.close      = rt_uart_close;
        uart->parent.read 	    = rt_uart_read;
        uart->parent.write      = rt_uart_write;
        uart->parent.control    = RT_NULL;
        uart->parent.user_data  = RT_NULL;

        rt_sem_init(&(uart->sem), "suart1", 0, 0);
        rt_device_register(&uart->parent, "uart1", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    }
#endif

#ifdef RT_USING_UART2
    {
        struct lpc8xx_uart* uart;

        /* get uart device */
        uart = &uart2_device;

        /* device initialization */
        uart->uart_base = LPC_USART2;
        uart->uart_irq  = UART2_IRQn;

        /* device interface */
        uart->parent.type       = RT_Device_Class_Char;
        uart->parent.init 	    = rt_uart_init;
        uart->parent.open 	    = rt_uart_open;
        uart->parent.close      = rt_uart_close;
        uart->parent.read 	    = rt_uart_read;
        uart->parent.write      = rt_uart_write;
        uart->parent.control    = RT_NULL;
        uart->parent.user_data  = RT_NULL;

        rt_sem_init(&(uart->sem), "suart2", 0, 0);
        rt_device_register(&uart->parent, "uart2", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    }
#endif /* RT_USING_UART2 */
    return 0;
}
INIT_BOARD_EXPORT(rt_hw_usart_init);

#endif /*RT_USING_UART*/
