#include "../inc/CrashCatcher.h"

.global pCC_Storage_stack_top
.extern pCC_Storage_stack_top


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
    ldr     r2, =pCC_Storage_stack_top
    ldr     sp, [r2]
    mrs     r2, psp
    str.w   r0, [sp, #-4]!
    push.w  {r1-r11}

    bx       lr

/* Let assembler know that we have hit the end of the function.*/
.pool
.size   ccPush_unStackedRegisters, .-ccPush_unStackedRegisters













.end
