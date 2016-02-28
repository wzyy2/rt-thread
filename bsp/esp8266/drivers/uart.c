#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "board.h"

struct esp8266_uart
{
    unsigned long num;
    int irq;
};

static rt_err_t esp8266_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct esp8266_uart* uart;
    unsigned long base;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct esp8266_uart *)serial->parent.user_data;
    RT_ASSERT(uart);
    base = uart->num;
    
    //uart_set_baud(base, cfg->baud_rate);

    return RT_EOK;
}

static rt_err_t esp8266_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct esp8266_uart* uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct esp8266_uart *)serial->parent.user_data;

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        /* disable rx irq */
        rt_hw_interrupt_mask(1 << uart->irq);
        break;
    case RT_DEVICE_CTRL_SET_INT:
        /* enable rx irq */
        rt_hw_interrupt_umask(1 << uart->irq);
        break;
    }

    return RT_EOK;
}

static int esp8266_putc(struct rt_serial_device *serial, char c)
{
    struct esp8266_uart* uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct esp8266_uart *)serial->parent.user_data;

    uart_putc(uart->num, c);

    return 1;
}

static int esp8266_getc(struct rt_serial_device *serial)
{
    int ch;
    struct esp8266_uart* uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct esp8266_uart *)serial->parent.user_data;

    ch = uart_getc_nowait(uart->num);

    return ch;
}

static const struct rt_uart_ops esp8266_uart_ops =
{
    esp8266_configure,
    esp8266_control,
    esp8266_putc,
    esp8266_getc,
};

struct esp8266_uart uart0 =
{
    0,
    INUM_UART,
};

static void IRAM uart_rx_handler(int irqno, void* param)
{
    struct esp8266_uart* uart;
    struct rt_serial_device *serial;

    serial = (struct rt_serial_device*)param;
    uart = (struct esp8266_uart *)serial->parent.user_data;

    if (UART(uart->num).INT_STATUS & UART_INT_STATUS_RXFIFO_FULL) {
        UART(uart->num).INT_CLEAR = UART_INT_CLEAR_RXFIFO_FULL;
        if (UART(uart->num).STATUS & (UART_STATUS_RXFIFO_COUNT_M << UART_STATUS_RXFIFO_COUNT_S)) {
            //rt_hw_interrupt_mask(1 << INUM_UART);
            //while(serial->ops->getc(serial) != -1);
            //rt_kprintf("error \r\n");
        	rt_hw_serial_isr(serial, RT_SERIAL_EVENT_RX_IND);
        }
    } else {
        rt_kprintf("Error: unexpected uart irq, INT_STATUS 0x%02x\n", UART(uart->num).INT_STATUS);
    }    
}

struct rt_serial_device serial0;

int rt_hw_serial_init() 
{
	struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    config.baud_rate = 115200;
    serial0.ops    = &esp8266_uart_ops;
    serial0.config = config;

    rt_hw_interrupt_install(INUM_UART, uart_rx_handler, &serial0, "uart0");
    
    // reset the rx fifo
    uint32_t conf = UART(0).CONF0;
    UART(0).CONF0 = conf | UART_CONF0_RXFIFO_RESET;
    UART(0).CONF0 = conf & ~UART_CONF0_RXFIFO_RESET;

    // set rx fifo trigger
    UART(0).CONF1 |= (1 & UART_CONF1_RXFIFO_FULL_THRESHOLD_M) << UART_CONF1_RXFIFO_FULL_THRESHOLD_S;

    // clear all interrupts
    UART(0).INT_CLEAR = 0x1ff;

    // enable rx_interrupt
    UART(0).INT_ENABLE = UART_INT_ENABLE_RXFIFO_FULL;

    rt_hw_serial_register(&serial0, "uart0",
            RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
            &uart0);

	rt_hw_interrupt_umask(1 << INUM_UART);

	return 0;
}


INIT_BOARD_EXPORT(rt_hw_serial_init);
