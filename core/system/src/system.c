/*
 * Copyright (c) 2013-2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <uvisor.h>
#include <CrashCatcher.h>

/* All system IRQs are by default weakly linked to the system default handler */
void UVISOR_ALIAS(isr_default_sys_handler) NonMaskableInt_IRQn_Handler(void);
void UVISOR_ALIAS(isr_default_sys_handler) HardFault_IRQn_Handler(void);
void UVISOR_ALIAS(isr_default_sys_handler) MemoryManagement_IRQn_Handler(void);
void UVISOR_ALIAS(isr_default_sys_handler) BusFault_IRQn_Handler(void);
void UVISOR_ALIAS(isr_default_sys_handler) UsageFault_IRQn_Handler(void);
void UVISOR_ALIAS(isr_default_sys_handler) SVCall_IRQn_Handler(void);
void UVISOR_ALIAS(isr_default_sys_handler) DebugMonitor_IRQn_Handler(void);
void UVISOR_ALIAS(isr_default_sys_handler) PendSV_IRQn_Handler(void);
void UVISOR_ALIAS(isr_default_sys_handler) SysTick_IRQn_Handler(void);

/* Default vector table (placed in Flash) */
__attribute__((section(".isr"))) const TIsrVector g_isr_vector[ISR_VECTORS] =
{
    /* Initial stack pointer */
    (TIsrVector) &__stack_top__,

    /* System IRQs */
    &main_entry,                           /* -15 */
    NonMaskableInt_IRQn_Handler,           /* -14 */
    HardFault_IRQn_Handler,                /* -13 */
    MemoryManagement_IRQn_Handler,         /* -12 */
    BusFault_IRQn_Handler,                 /* -11 */
    UsageFault_IRQn_Handler,               /* -10 */
    isr_default_sys_handler,               /* - 9 */
    isr_default_sys_handler,               /* - 8 */
    isr_default_sys_handler,               /* - 7 */
    isr_default_sys_handler,               /* - 6 */
    SVCall_IRQn_Handler,                   /* - 5 */
    DebugMonitor_IRQn_Handler,             /* - 4 */
    isr_default_sys_handler,               /* - 3 */
    PendSV_IRQn_Handler,                   /* - 2 */
    SysTick_IRQn_Handler,                  /* - 1 */

    /* NVIC IRQs */
    /* Note: This is a GCC extension. */
    [NVIC_OFFSET ... (ISR_VECTORS - 1)] = isr_default_handler
};

void UVISOR_NAKED UVISOR_NORETURN isr_default_sys_handler(void)
{
   /* CrashCatcher routine */
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
        exceptionLR */
    /*50 equals CRASH_CATCHER_STACK_WORD_COUNT*/
    asm volatile(
        "mrs     r1, xpsr\n"
        "mrs     r2, psp\n"
        "mrs     r3, msp\n"
        "ldr     sp, =(0x20000000 + 4 * 50)\n"
        "push.w  {r1-r11,lr}\n"
        );


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
    asm volatile(

        /*test for whether using FPU*/
        /*read Coprocessor Access Control Register*/
        "ldr     r1, =( 0xE000ED88 )\n"
        "ldr     r1, [r1]\n"
        /*r0 (after shift) represents for coProcessor10and11EnabledBits*/
        "mov     r0, #5\n"
        /*Decide auto-stacked size*/
        "lsl     r0, r0, #20\n"
        "teq     r1, r0\n"
        "ite     eq\n"
        "moveq   r0, #25\n"
        "movne   r0, #8\n"


        /*get lr*/
        "ldr     r1, =(0x20000000 + 4 * (50 - 1) )\n"
        "ldr     r1, [r1]\n"
        /*test for using msp or psp*/
        "ands    r1, r1, 0x4\n"                               
        "cmp     r1, 0x4\n"
        "ite     eq\n"
        /*using psp,previously */
        "ldreq   r1, =(0x20000000 + 4 * (50 - 11) )\n"
        /*using msp,previously*/
        "ldrne   r1, =(0x20000000 + 4 * (50 - 10) )\n"

        "ldr     r1, [r1]\n"

        /*get the address of auto_stack[0]*/
        //"mov     r3, (0x20000000 + (4 * (50)) )\n",due to the format,the general registers are not able to operate on such big number at once.
        "mov     r3, #2\n"
        "lsl     r3, r3, #28\n"/*  r3=0x20000000  */
        "mov     r2, #50\n"    /*  r2=50,the index of auto_stack[0] */
        "lsl     r2, r2, #2\n" /*  r2=r2*4  */
        "add     r3, r3, r2\n" /*  r3=0x20000000+50*4 */

        /*store auto-stacked registers to g_crashCatcherStack*/
        /*r0 is the stack size(loop_counter) that we need to store*/
        /*r1 is the (sp_address) that we used*/
        /*r3 is the address of auto_stack[i]*/
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
"CCstore:\n"
        /*  r2=(*r1),then r1+=4 */
        "ldr     r2, [r1], #4\n"
        /*  (*r3)=r2 */
        "str     r2, [r3]\n"
        /*  r3=r3+4  */
        "add     r3, r3, #4\n"
        /* loop_counter-=1 */
        "subs    r0, r0, #1\n"
        /* if (loop_counter > 1) */
        "it      hi\n"
        "bhi     CCstore\n"
        /*end of storing auto-stacked registers*/
        );



    asm volatile(
        "\n"
        /*get lr*/
        "ldr     r0, =(0x20000000 + 4 * (50 - 1) )\n"
        "ldr     r0, [r0]\n"
        /*get msp*/
        "ldr     r1, =(0x20000000 + 4 * (50 - 10) )\n"
        "ldr     r1, [r1]\n"
        /*restore sp*/
        "msr     msp, r1\n"
        );


    /* Handle system IRQ with an unprivileged handler. */
    /* Note: The original lr and the MSP are passed to the actual handler */
    asm volatile(
        /*
        "mov r0, lr\n"
        "mrs r1, MSP\n"
        */
        "push {lr}\n"
        "blx vmpu_sys_mux_handler\n"
        "pop {pc}\n"
    );
}

void UVISOR_NAKED UVISOR_NORETURN isr_default_handler(void)
{
    /* Handle NVIC IRQ with an unprivileged handler.
     * Serving an IRQn in unprivileged mode is achieved by mean of two SVCalls:
     * The first one de-previliges execution, the second one re-privileges it. */
    /* Note: NONBASETHRDENA (in SCB) must be set to 1 for this to work. */
    asm volatile(
        "svc  %[unvic_in]\n"
        "svc  %[unvic_out]\n"
        "bx   lr\n"
        ::[unvic_in]  "i" ((UVISOR_SVC_ID_UNVIC_IN) & 0xFF),
          [unvic_out] "i" ((UVISOR_SVC_ID_UNVIC_OUT) & 0xFF)
    );
}
