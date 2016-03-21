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
#include "CrashCatcher.h"


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
    asm volatile(
        "mov     r0,lr\n"
        "bl      ccPush_unStackedRegisters\n"
        "bl      ccPush_autoStackedRegisters\n"
        );



    asm volatile(
        /*
          Configurable Fault Status Register, CFSR
          MemManage Status Register, MMFSR
          BusFault Status Register, BFSR
          UsageFault Status Register, UFSR
          HardFault Status Register, HFSR
        */
        "ldr     r0, =(0xE000ED28)\n"/*Start address*/
        /* r1 is the destination of storage*/
        "ldr     r1, =(0x20000000 + 4 * 95 )\n"
        /* r2 is loop counter*/
        "mov     r2, #5\n"
/* store to destination*/
"CCstore_fault:\n"
        "ldr     r3, [r0], #4\n"
        "str     r3, [r1], #4\n"
        /*forward to the next*/
        //"add     r0, r0, #4\n"
        //"add     r1, r1, #4\n"
        "subs    r2, r2, #1\n"
        "it      hi\n"
        "bhi     CCstore_fault\n"
        /*end of storing fault status registers*/
        );



/*Prepare for the arguments passed to the uVisor*/
    asm volatile(
        "\n"
        /*get lr*/
        "ldr     r0, =(0x20000000 + 4 * (50 - 1) )\n"
        "ldr     r0, [r0]\n"
        /*get msp*/
        "ldr     r1, =(0x20000000 + 4 * (50 - 10) )\n"
        "ldr     r1, [r1]\n"
        /*restore lr*/
        "mov     lr , r0\n"
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
