
#include <rtthread.h>
#include <rthw.h>

#include "xtensa.h"

void IRAM sdk_user_fatal_exception_handler() {
    rt_uint32_t exccause, epc1, epc2, epc3, excvaddr, depc, excsave1;

    rt_hw_interrupt_disable();

    RSR(exccause, exccause);
    rt_kprintf("Fatal exception (%d): \n", (int)exccause);
    RSR(epc1, epc1);
    RSR(epc2, epc2);
    RSR(epc3, epc3);
    RSR(excvaddr, excvaddr);
    RSR(depc, depc);
    RSR(excsave1, excsave1);
    rt_kprintf("%s=0x%08x\n", "epc1", epc1);
    rt_kprintf("%s=0x%08x\n", "epc2", epc2);
    rt_kprintf("%s=0x%08x\n", "epc3", epc3);
    rt_kprintf("%s=0x%08x\n", "excvaddr", excvaddr);
    rt_kprintf("%s=0x%08x\n", "depc", depc);
    rt_kprintf("%s=0x%08x\n", "excsave1", excsave1);

    while(1);
    
#ifdef RT_USING_FINSH
    list_thread();
#endif


        
}

