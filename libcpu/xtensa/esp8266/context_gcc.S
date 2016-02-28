#include "xtensa_context.h"

    .section .iram1.text
    .literal_position
    .balign 4

_xt_context_save:
	.global _xt_context_save
	.type _xt_context_save, @function
	s32i    a2,  sp, 20
	s32i    a3,  sp, 24
	s32i    a4,  sp, 28
	s32i    a5,  sp, 32
	s32i    a6,  sp, 36
	s32i    a7,  sp, 40
	s32i    a8,  sp, 44
	s32i    a9,  sp, 48
	s32i    a10, sp, 52
	s32i    a11, sp, 56
	s32i    a12, sp, 60
	s32i    a13, sp, 64
	s32i    a14, sp, 68
	s32i    a15, sp, 72

	rsr     a3,  sar
	s32i    a3,  sp, 76

	ret

_xt_context_restore:
	.global _xt_context_restore
	.type _xt_context_restore, @function
	l32i    a3,  sp, 76

	l32i    a2,  sp, 20

	wsr     a3,  SAR
	l32i    a3,  sp, 24
	l32i    a4,  sp, 28
	l32i    a5,  sp, 32
	l32i    a6,  sp, 36
	l32i    a7,  sp, 40
	l32i    a8,  sp, 44
	l32i    a9,  sp, 48
	l32i    a10, sp, 52
	l32i    a11, sp, 56
	l32i    a12, sp, 60
	l32i    a13, sp, 64
	l32i    a14, sp, 68
	l32i    a15, sp, 72

	ret

/*
 * void rt_hw_context_switch_to(rt_uint32 to);
 */
.global rt_hw_context_switch_to
rt_hw_context_switch_to:
	l32i    a3, a2, 0
	mov     sp, a3
	#call0   _xt_context_restore

	l32i    a0, sp, 0x08  /* get ps */

	wsr     a0, ps
	l32i    a2, sp, 0x14	/* get param */

	l32i    a0, sp, 0x04  /* get entry */
	wsr     a0, epc1
	l32i    a0, sp, 0x0c
	l32i    sp, sp, 0x10

	rfe         

/*
 * void rt_hw_context_switch_interrupt(rt_uint32 from, rt_uint32 to);
 */
.globl rt_thread_switch_interrupt_flag
.globl rt_interrupt_from_thread
.globl rt_interrupt_to_thread
.globl rt_hw_context_switch_interrupt
rt_hw_context_switch_interrupt:
	movi	a4, rt_interrupt_from_thread
	s32i 	a2, a4, 0
	movi	a4, rt_interrupt_to_thread
	s32i    a3, a4, 0
	movi 	a4, rt_thread_switch_interrupt_flag
	movi 	a5, 0x01
	s32i    a5, a4, 0
	ret

/*
 * void rt_hw_context_switch(rt_uint32 from, rt_uint32 to);
 * r0 --> from
 * r1 --> to
 */
.globl rt_hw_context_switch
rt_hw_context_switch:

	addi    sp, sp, -0x50

	/* update current thread stack pointer */	
	s32i    sp, a2, 0 

	/* pc */
	s32i    a0, sp, 0x04
	s32i 	a0, sp, 0x0c
	/* sp */
	addi 	a0, sp, 0x50
	s32i    a0, sp, 0x10
	/* ps */
	rsr     a0, ps
	s32i    a0, sp, 0x08

	mov 	a0, a3

	s32i    a2,  sp, 20
	s32i    a3,  sp, 24
	s32i    a4,  sp, 28
	s32i    a5,  sp, 32
	s32i    a6,  sp, 36
	s32i    a7,  sp, 40
	s32i    a8,  sp, 44
	s32i    a9,  sp, 48
	s32i    a10, sp, 52
	s32i    a11, sp, 56
	s32i    a12, sp, 60
	s32i    a13, sp, 64
	s32i    a14, sp, 68
	s32i    a15, sp, 72
	rsr     a3,  sar
	s32i    a3,  sp, 76
	
	l32i 	sp, a0, 0

	call0   _xt_context_restore

	l32i    a0, sp, 0x08
	wsr     a0, ps
	
	l32i    a0, sp, 0x04
	wsr     a0, epc1
	l32i    a0, sp, 0x0c
	l32i    sp, sp, 0x10
	rsync

	rfe
