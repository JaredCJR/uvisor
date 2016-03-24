
#ifndef __uvisor_CrashCatcher_H
#define __uvisor_CrashCatcher_H



/* Definitions only required from C code. */
#if !__ASSEMBLER__

#include <uvisor.h>

UVISOR_EXTERN uint32_t __CrashCatcher_BASE__;

/*Need to be as same as the uvisor-helloworld CrashCatcher header file*/
#define CRASH_CATCHER_STACK_WORD_COUNT 50
#define CRASH_CATCHER_AUTO_STACKED_WORD_COUNT 45
#define CRASH_CATCHER_FAULT_REGISTERS_WORD_COUNT 5

#define pCC_register_stack_bottom(BASE)       (uint32_t)(BASE + CC_WORD_SIZE * (CRASH_CATCHER_STACK_WORD_COUNT - 1))
#define pCC_register_lr(BASE)                 (uint32_t)pCC_register_stack_bottom(BASE)
#define pCC_register_psp(BASE)                (uint32_t)(BASE + CC_WORD_SIZE * (CRASH_CATCHER_STACK_WORD_COUNT - 11))
#define pCC_register_msp(BASE)                (uint32_t)(BASE + CC_WORD_SIZE * (CRASH_CATCHER_STACK_WORD_COUNT - 10))
#define pCC_register_autostack_top(BASE)      (uint32_t)(pCC_Storage_Base + CC_WORD_SIZE * CRASH_CATCHER_STACK_WORD_COUNT )
#define pCC_register_FaultStatus_top(BASE)    (uint32_t)(pCC_Storage_Base + CC_WORD_SIZE * (CRASH_CATCHER_STACK_WORD_COUNT + CRASH_CATCHER_AUTO_STACKED_WORD_COUNT) )


#define  pCC_Storage_Base    0x20000000/*default base address*/
#define  CC_WORD_SIZE        4

uint32_t pCC_Base                    = (uint32_t)&__CrashCatcher_BASE__;/*This should be registered*/
uint32_t pCC_Storage_stack_bottom    = pCC_register_stack_bottom(pCC_Storage_Base);
uint32_t pCC_Storage_stack_lr        = pCC_register_lr(pCC_Storage_Base);
uint32_t pCC_Storage_stack_psp       = pCC_register_psp(pCC_Storage_Base);
uint32_t pCC_Storage_stack_msp       = pCC_register_msp(pCC_Storage_Base);
uint32_t pCC_Storage_autostack_top   = pCC_register_autostack_top(pCC_Storage_Base);
uint32_t pCC_Storage_FaultStatus_top = pCC_register_FaultStatus_top(pCC_Storage_Base);


#endif // #if !__ASSEMBLER__
#endif /* end of __uvisor_CrashCatcher_H */
