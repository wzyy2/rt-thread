#include <rtthread.h>
#include <rthw.h>
#include <string.h>
#include "board.h"

#define portBASE_TYPE           long
typedef uint32_t portTickType;
/* Defines the prototype to which task functions must conform. */
typedef void (*pdTASK_CODE)( void * );

#define pdFALSE     ( ( portBASE_TYPE ) 0 )
#define pdTRUE      ( ( portBASE_TYPE ) 1 )


void inline *zalloc(size_t nbytes)
{
    return rt_calloc(1, nbytes);
}

void inline _xt_isr_attach(uint8_t i, rt_isr_handler_t func)
{
    rt_hw_interrupt_install(i, func,
        NULL, NULL);
}

/* Each task maintains its own interrupt status in the critical nesting
variable. */
static unsigned portBASE_TYPE uxCriticalNesting = 0;
unsigned cpu_sr;
extern char sdk_NMIIrqIsOn;
char level1_int_disabled;

/* These nested vPortEnter/ExitCritical macros are called by SDK
 * libraries in libmain, libnet80211, libpp
 *
 * It may be possible to replace the global nesting count variable
 * with a save/restore of interrupt level, although it's difficult as
 * the functions have no return value.
 */
void IRAM vPortEnterCritical( void )
{
    if(!sdk_NMIIrqIsOn && !level1_int_disabled) {
    cpu_sr = rt_hw_interrupt_disable();
    level1_int_disabled = 1;
    }
    uxCriticalNesting++;
}

void IRAM vPortExitCritical( void )
{
    uxCriticalNesting--;
    if( uxCriticalNesting == 0 ) {
        if(!sdk_NMIIrqIsOn && level1_int_disabled) {
            level1_int_disabled = 0;
            rt_hw_interrupt_enable(cpu_sr);
        }
    }
}

rt_size_t inline xPortGetFreeHeapSize()
{
    rt_uint32_t total,used;
    rt_memory_info(&total, &used, NULL);

    return total - used;
}

void inline vTaskSuspendAll( void )
{
    rt_enter_critical();
}

signed portBASE_TYPE inline xTaskResumeAll()
{
    rt_exit_critical();   

    return pdTRUE; 
}

signed portBASE_TYPE xTaskGenericCreate()
{
    // rt_thread_create(pcName,
    //                          pxTaskCode,
    //                          pvParameters,
    //                          usStackDepth,
    //                          rt_uint8_t  priority,
    //                          rt_uint32_t tick)
}

void inline vTaskDelay(portTickType xTicksToDelay)
{
    rt_thread_delay(xTicksToDelay);
}

void inline vTaskSwitchContext()
{
   rt_schedule(); 
}

portTickType inline xTaskGetTickCount( void )
{
    portTickType xTicks;

    /* Critical section required if running on a 16 bit processor. */
    vPortEnterCritical();
    {
        xTicks = rt_tick_get();
    }
    vPortExitCritical();

    return xTicks;
}

void xTimerCreate()
{
    
}

void xTimerGenericCommand()
{
    
}


void xQueueGenericReceive()
{
    
}

void xQueueGenericCreate()
{
    
}



void xQueueGenericSend()
{
    
}



void xQueueGenericSendFromISR()
{
    
}
void uxQueueMessagesWaitingFromISR()
{
    
}


