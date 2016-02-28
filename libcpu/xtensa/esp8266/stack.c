#include <rtthread.h>

#include "xtensa.h"

/**
 * This function will initialize thread stack
 *
 * @param tentry the entry of thread
 * @param parameter the parameter of entry 
 * @param stack_addr the beginning stack address
 * @param texit the function will be called when thread exit
 *
 * @return stack address
 */
rt_uint8_t *rt_hw_stack_init(void *tentry, void *parameter,
	rt_uint8_t *stack_addr, void *texit)
{
    #define SET_STKREG(r,v) sp[(r) >> 2] = (rt_uint32_t)(v)
    rt_uint32_t *sp, *tp;

    /* Create interrupt stack frame aligned to 16 byte boundary */
    sp = (rt_uint32_t*) (((rt_uint32_t)(stack_addr+1) - XT_CP_SIZE - XT_STK_FRMSZ) & ~0xf);

    /* Clear the entire frame (do not use memset() because we don't depend on C library) */
    for (tp = sp; tp <= (rt_uint32_t*) stack_addr; ++tp)
        *tp = 0;

    /* Explicitly initialize certain saved registers */
    SET_STKREG( XT_STK_PC,      tentry                        );  /* task entrypoint                  */
    SET_STKREG( XT_STK_A1,      (rt_uint32_t)sp + XT_STK_FRMSZ   );  /* physical top of stack frame      */
    SET_STKREG( XT_STK_A2,      (rt_uint32_t)parameter   );           /* parameters      */
    SET_STKREG( XT_STK_A0,    texit               );  /* exit */

    /* Set initial PS to int level 0, EXCM disabled ('rfe' will enable), user mode. */
    SET_STKREG( XT_STK_PS,      PS_UM  | PS_EXCM );

    return (rt_uint8_t *)sp;
}

/*@}*/
