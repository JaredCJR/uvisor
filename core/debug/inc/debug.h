/*
 * Copyright (c) 2013-2015, ARM Limited, All Rights Reserved
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
#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <uvisor.h>
#include "debug_exports.h"
#include "halt.h"

/* Debug box handle.
 * This is used internally by uVisor to keep track of the registered debug box
 * and to access its driver when needed. */
typedef struct TDebugBox {
    const TUvisorDebugDriver *driver;
    int initialized;
    uint8_t box_id;
} TDebugBox;
TDebugBox g_debug_box;

void debug_init(void);
void debug_cx_switch_in(void);
void debug_cx_switch_out(void);
void debug_exception_frame(uint32_t lr, uint32_t sp);
void debug_fault_memmanage(void);
void debug_fault_bus(void);
void debug_fault_usage(void);
void debug_fault_hard(void);
void debug_fault_debug(void);
void debug_fault_mpu(void);
void debug_fault(THaltError reason, uint32_t lr, uint32_t sp);
void debug_mpu_config(void);
void debug_map_addr_to_periph(uint32_t address);
void debug_register_driver(const TUvisorDebugDriver * const driver);
uint32_t debug_get_version(void);
void debug_halt_error(THaltError reason);
void debug_reboot(void);
void CrashCatcher_INIT(uint32_t base);

/*CrashCatcher storage global variable*/
UVISOR_EXTERN uint32_t __CrashCatcher_BASE__;/*Linker Script symbol*/
UVISOR_EXTERN uint32_t pCC_Base;
#define  pCC_Storage_Base    (uint32_t)0x20000000/*default base address,avoid cause fault*/
UVISOR_EXTERN uint32_t pCC_Storage_stack_bottom;                                                                                                                      
UVISOR_EXTERN uint32_t pCC_Storage_stack_lr;
UVISOR_EXTERN uint32_t pCC_Storage_stack_psp;
UVISOR_EXTERN uint32_t pCC_Storage_stack_msp;
UVISOR_EXTERN uint32_t pCC_Storage_autostack_top;
UVISOR_EXTERN uint32_t pCC_Storage_FaultStatus_top;


/*Need to be as same as the uvisor-helloworld CrashCatcher header file*/
#define CRASH_CATCHER_STACK_WORD_COUNT 50
#define CRASH_CATCHER_AUTO_STACKED_WORD_COUNT 45
#define CRASH_CATCHER_FAULT_REGISTERS_WORD_COUNT 5
#define  CC_WORD_SIZE        4
                                                                                                                                                                      
#define pCC_register_stack_bottom(BASE)       (uint32_t)(BASE + CC_WORD_SIZE * (CRASH_CATCHER_STACK_WORD_COUNT - 1))
#define pCC_register_lr(BASE)                 (uint32_t)pCC_register_stack_bottom(BASE)
#define pCC_register_psp(BASE)                (uint32_t)(BASE + CC_WORD_SIZE * (CRASH_CATCHER_STACK_WORD_COUNT - 11))
#define pCC_register_msp(BASE)                (uint32_t)(BASE + CC_WORD_SIZE * (CRASH_CATCHER_STACK_WORD_COUNT - 10))
#define pCC_register_autostack_top(BASE)      (uint32_t)(BASE + CC_WORD_SIZE * CRASH_CATCHER_STACK_WORD_COUNT )
#define pCC_register_FaultStatus_top(BASE)    (uint32_t)(BASE + CC_WORD_SIZE * (CRASH_CATCHER_STACK_WORD_COUNT + CRASH_CATCHER_AUTO_STACKED_WORD_COUNT) )



#define DEBUG_PRINT_HEAD(x) {\
    DPRINTF("\n***********************************************************\n");\
    DPRINTF("                    "x"\n");\
    DPRINTF("***********************************************************\n\n");\
}
#define DEBUG_PRINT_END()   {\
    DPRINTF("***********************************************************\n\n");\
}

#ifdef  NDEBUG

#define DEBUG_INIT(...)          {}
#define DEBUG_FAULT(...)         {}
#define DEBUG_CX_SWITCH_IN(...)  {}
#define DEBUG_CX_SWITCH_OUT(...) {}

#else /*NDEBUG*/

#define DEBUG_INIT          debug_init
#define DEBUG_FAULT         debug_fault
#define DEBUG_CX_SWITCH_IN  debug_cx_switch_in
#define DEBUG_CX_SWITCH_OUT debug_cx_switch_out

#endif/*NDEBUG*/

#endif/*__DEBUG_H__*/
