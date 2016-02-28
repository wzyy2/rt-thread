#include <stdlib.h>
#include <rtthread.h>
#include <components.h>

#include "esp8266.h"

volatile rt_uint32_t count = 0;
volatile rt_uint32_t count2 = 0;
void IRAM rt_test()
{
        rt_uint32_t ccount;

        asm volatile ("rsr %0, ps" : "=a" (ccount));
        rt_kprintf("pc : %x \r\n", ccount);
}

void rt_init_thread_entry(void* parameter)
{
    rt_test();
    /* do component initialization */
    rt_components_init();

    while(1) {
            rt_thread_delay(RT_TICK_PER_SECOND);
    }
        

    
}
void rt_init_thread_entry2(void* parameter)
{

    while(1) {
            rt_kprintf("test2 \r\n");
            rt_thread_delay(RT_TICK_PER_SECOND/2);
    }
        

    /* do component initialization */
    //rt_components_init();
    
}

int rt_application_init()
{
    rt_thread_t tid;

    tid = rt_thread_create("init",
        rt_init_thread_entry, "hello world",
        1024, RT_THREAD_PRIORITY_MAX/3, 200);

    if (tid != RT_NULL)
        rt_thread_startup(tid);

    // tid = rt_thread_create("init2",
    //     rt_init_thread_entry2, "hello world",
    //     2048, RT_THREAD_PRIORITY_MAX/3 + 1, 200);

    // if (tid != RT_NULL);
    //     rt_thread_startup(tid);

    return 0;
}
