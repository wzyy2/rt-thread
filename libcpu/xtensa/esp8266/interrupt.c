
#include <rtthread.h>
#include <rthw.h>

#include "xtensa.h"
#include "interrupts.h"

#define MAX_HANDLERS    16

extern volatile rt_uint8_t rt_interrupt_nest;

/* exception and interrupt handler table */
struct rt_irq_desc isr_table[MAX_HANDLERS];
rt_uint32_t rt_interrupt_from_thread, rt_interrupt_to_thread;
rt_uint32_t rt_thread_switch_interrupt_flag;

void IRAM rt_hw_interrupt_handle(int intset)
{
    rt_interrupt_enter();

    while(intset) {
        rt_uint8_t index = __builtin_ffs(intset) - 1;

        rt_uint16_t mask = BIT(index);

        rt_clear_ints(mask);

        rt_isr_handler_t handler = isr_table[index].handler;
        void *param = isr_table[index].param;

        handler(index, param);

        intset -= mask;
    }

    rt_interrupt_leave();
}

void rt_hw_interrupt_init(void)
{
    /* init exceptions table */
    rt_memset(isr_table, 0x00, sizeof(isr_table));

    /* init interrupt nest, and context in thread sp */
    rt_interrupt_nest = 0;
    rt_interrupt_from_thread = 0;
    rt_interrupt_to_thread = 0;
    rt_thread_switch_interrupt_flag = 0;


}


rt_isr_handler_t rt_hw_interrupt_install(int vector, rt_isr_handler_t handler,
        void *param, char *name)
{
    rt_isr_handler_t old_handler = RT_NULL;

    if(vector < MAX_HANDLERS)
    {
        old_handler = isr_table[vector].handler;

        if (handler != RT_NULL)
        {
#ifdef RT_USING_INTERRUPT_INFO
            rt_strncpy(isr_table[vector].name, name, RT_NAME_MAX);
#endif /* RT_USING_INTERRUPT_INFO */
            isr_table[vector].handler = handler;
            isr_table[vector].param = param;
        }
    }

    return old_handler;
}

void IRAM rt_hw_interrupt_umask(int vector)
{
    rt_uint32_t intenable;
    asm volatile ("rsr %0, intenable" : "=a" (intenable));
    intenable |= vector;
    asm volatile ("wsr %0, intenable; esync" :: "a" (intenable));
}

void IRAM rt_hw_interrupt_mask(int vector)
{
    rt_uint32_t intenable;
    asm volatile ("rsr %0, intenable" : "=a" (intenable));
    intenable &= ~vector;
    asm volatile ("wsr %0, intenable; esync" :: "a" (intenable));
}


rt_base_t IRAM rt_hw_interrupt_disable(void)
{
    rt_base_t old_level;
    __asm__ volatile ("rsil %0, " XTSTR(XCHAL_EXCM_LEVEL) : "=a" (old_level));
    return old_level;
}

void IRAM rt_hw_interrupt_enable(rt_base_t level)
{
    __asm__ volatile ("wsr %0, ps; rsync" :: "a" (level));
}



/*@}*/
