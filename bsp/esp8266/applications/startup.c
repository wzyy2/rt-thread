#include <rtthread.h>
#include <rthw.h>

#include "board.h"


/**
 * This function will startup RT-Thread RTOS
 */
void IRAM rtthread_startup(void)
{
	/* disable interrupt first */
    rt_hw_interrupt_disable();

    /* initialize board */
    rt_hw_board_init();

    /* show RT-Thread version */
    rt_show_version();

    /* initialize memory system */
#ifdef RT_USING_HEAP
    rt_system_heap_init((void *)HEAP_BEGIN, (void *)HEAP_END);
#endif
    
    /* initialize scheduler system */
    rt_system_scheduler_init();

    /* initialize timer */
    rt_system_timer_init();

    /* initialize soft timer thread */
    rt_system_timer_thread_init();

    /* initialize application */
    rt_application_init();

    /* initialize idle thread */
    rt_thread_idle_init();

    /* start scheduler */
    rt_system_scheduler_start();
    while(1);
    /* never reach here */
    return ;
}

/*@}*/
