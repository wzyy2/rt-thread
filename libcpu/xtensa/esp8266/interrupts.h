/* ESP8266 Xtensa interrupt management functions
 *
 * Some (w/ sdk_ prefix) are implemented in binary libs, rest are
 * inlines replacing functions in the binary libraries.
 *
 * Part of esp-open-rtos
 * Copyright (C) 2015 Superhouse Automation Pty Ltd
 * BSD Licensed as described in the file LICENSE
 */
#ifndef _XTENSA_INTERRUPTS_H
#define _XTENSA_INTERRUPTS_H

#include <rtthread.h>
 
enum SVC_ReqType {
  SVC_Software = 1,
  SVC_MACLayer = 2,
};

#ifndef XTSTR
#define _XTSTR(x)   # x
#define XTSTR(x)    _XTSTR(x)
#endif

/* Interrupt numbers for level 1 exception handler. */
typedef enum {
    INUM_SLC = 1,
    INUM_SPI = 2,
    INUM_GPIO = 4,
    INUM_UART = 5,
    INUM_TICK = 6, /* RTOS timer tick, possibly xtensa CPU CCOMPARE0(?) */
    INUM_SOFT = 7,
    INUM_WDT = 8,
    INUM_TIMER_FRC1 = 9,

    /* FRC2 default handler. Configured by sdk_ets_timer_init, which
       runs as part of default libmain.a startup code, assigns
       interrupt handler to sdk_vApplicationTickHook+0x68
     */
    INUM_TIMER_FRC2 = 10,
} xt_isr_num_t;

static inline void rt_clear_ints(rt_uint32_t mask)
{
    asm volatile ("wsr %0, intclear; esync" :: "a" (mask));
}


#endif
