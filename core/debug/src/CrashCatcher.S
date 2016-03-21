#include "../inc/CrashCatcher.h"

.global pCC_Storage_stack_lr
.extern pCC_Storage_stack_lr
.global pCC_Storage_stack_psp
.extern pCC_Storage_stack_psp
.global pCC_Storage_stack_msp
.extern pCC_Storage_stack_msp
.global pCC_Storage_autostack_top
.extern pCC_Storage_autostack_top
.global pCC_Storage_FaultStatus_top
.extern pCC_Storage_FaultStatus_top
.global pCC_Storage_stack_bottom
.extern pCC_Storage_stack_bottom



.text
.syntax unified
.arch armv7-m



/* Push the following onto the stack (see CrashCatcherExceptionRegisters structure). The g_crashCatcherStack buffer
   is reserved for use as the stack while CrashCatcher is running.
        exceptionPSR
        psp
        msp
        r4
        r5
        r6
        r7
        r8
        r9
        r10
        r11
        exceptionLR 
*/
/* 50 equals CRASH_CATCHER_STACK_WORD_COUNT
   exceptionLR is stored at stack[50]
   r11         is stored at stack[49]
   ...
*/
.global ccPush_unStackedRegisters
.type ccPush_unStackedRegisters, %function
.thumb_func
ccPush_unStackedRegisters:
    mrs     r1, xpsr
    mrs     r3, msp
    ldr     r2, =pCC_Storage_stack_bottom /*Due to stmdb will cross the current address,so we use "the next address of unstack storage"*/
    ldr     sp, [r2]
    mrs     r2, psp
    str.w   r0, [sp]
    push.w  {r1-r11}

    bx       lr

/* Let assembler know that we have hit the end of the function.*/
.pool
.size   ccPush_unStackedRegisters, .-ccPush_unStackedRegisters






/*Save the auto-stacked registers to be accessed by CrashCatcher
        r0
        r1                                                                                                                              
        r2
        r3
        r12
        lr
        pc
        psr
The following floating point registers are only stacked when the LR_FLOAT bit is set in exceptionLR.
        floats[16]
        fpscr
        reserved //for 8-bytes alignments
*/
.global ccPush_autoStackedRegisters
.type ccPush_autoStackedRegisters, %function
.thumb_func
ccPush_autoStackedRegisters:

    /*test for whether using FPU*/                                                                                                  
    ldr     r1, =( 0xE000ED88 ) /*read Coprocessor Access Control Register*/
    ldr     r1, [r1]
    /*r0 (after shift) represents for coProcessor10and11EnabledBits*/
    mov     r0, #5
    /*Decide auto-stacked size*/
    lsl     r0, r0, #20
    teq     r1, r0
    ite     eq
    moveq   r0, #25
    movne   r0, #8


    /*get lr*/
    ldr     r1, =pCC_Storage_stack_lr
    ldr     r1, [r1]
    ldr     r1, [r1]
    /*test for using msp or psp*/
    ands    r1, r1, 0x4                         
    cmp     r1, 0x4
    ite     eq
    /*using psp,previously */
    ldreq   r1, =pCC_Storage_stack_psp
    /*using msp,previously*/
    ldrne   r1, =pCC_Storage_stack_msp

    ldr     r1, [r1]
    ldr     r1, [r1]

    /*get the address of auto_stack[0]*/
    ldr     r3, =pCC_Storage_autostack_top
    ldr     r3, [r3]

/*
    r0 is the stack size(loop_counter) that we need to store
    r1 is the (sp_address) that we used
    r3 is the address of auto_stack[i]
*/
/* 
    auto_stack[0] = r0
    auto_stack[1] = r1
    auto_stack[2] = r2
    auto_stack[3] = r3
    auto_stack[4] = r12
    auto_stack[5] = lr
    auto_stack[6] = pc
    auto_stack[7] = psr
    floats[0]
    ...
    floats[15]
    fpscr
*/
CCstore_auto:
    ldr     r2, [r1], #4        /*  r2=(*r1),then r1+=4 */   
    str     r2, [r3]            /*  (*r3)=r2 */
    add     r3, r3, #4          /*  r3=r3+4  */
    subs    r0, r0, #1          /* loop_counter-=1 */
    it      hi                  /* if (loop_counter > 1) */
    bhi     CCstore_auto

/*end of storing auto-stacked registers*/
    bx      lr
/* Let assembler know that we have hit the end of the function.*/
.pool
.size   ccPush_autoStackedRegisters, .-ccPush_autoStackedRegisters





.global ccPush_FaultStatusRegisters
.type ccPush_FaultStatusRegisters, %function
.thumb_func
ccPush_FaultStatusRegisters:

/*
    Configurable Fault Status Register, CFSR
    MemManage Status Register, MMFSR
    BusFault Status Register, BFSR
    UsageFault Status Register, UFSR
    HardFault Status Register, HFSR
*/
    ldr     r0, =(0xE000ED28)                  /*FaultStatus registers Start address*/
    ldr     r1, =pCC_Storage_FaultStatus_top   /* r1 is the destination of storage*/
    ldr     r1, [r1]
    mov     r2, #5                             /* r2 is loop counter*/

/* store to destination*/
CCstore_fault:
    /*store and forward to the next*/                                                                                             
    ldr     r3, [r0], #4
    str     r3, [r1], #4

    subs    r2, r2, #1
    it      hi
    bhi     CCstore_fault

/*end of storing fault status registers*/
    bx      lr
/* Let assembler know that we have hit the end of the function.*/
.pool
.size   ccPush_FaultStatusRegisters, .-ccPush_FaultStatusRegisters





.global ccPush_ProcedureCleanUp
.type ccPush_ProcedureCleanUp , %function
.thumb_func
ccPush_ProcedureCleanUp:
    /*get lr*/
    ldr     r0, =pCC_Storage_stack_lr
    ldr     r0, [r0]
    ldr     r0, [r0]

    /*get msp*/                                                                                                                              
    ldr     r1, =pCC_Storage_stack_msp
    ldr     r1, [r1]
    ldr     r1, [r1]

    bx      lr
/* Let assembler know that we have hit the end of the function.*/
.pool
.size   ccPush_ProcedureCleanUp , .-ccPush_ProcedureCleanUp 



.end
