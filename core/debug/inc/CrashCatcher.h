
#ifndef __uvisor_CrashCatcher_H
#define __uvisor_CrashCatcher_H



/* Definitions only required from C code. */
#if !__ASSEMBLER__

#include <uvisor.h>

#define  pCC_Storage_Base    0x20000000
#define  CC_WORD_SIZE        4
uint32_t pCC_Storage_stack_bottom    = (uint32_t)(pCC_Storage_Base + CC_WORD_SIZE * (50 - 1) );
uint32_t pCC_Storage_stack_lr        = (uint32_t)(pCC_Storage_Base + CC_WORD_SIZE * (50 - 1) );
uint32_t pCC_Storage_stack_psp       = (uint32_t)(pCC_Storage_Base + CC_WORD_SIZE * (50 - 11) );
uint32_t pCC_Storage_stack_msp       = (uint32_t)(pCC_Storage_Base + CC_WORD_SIZE * (50 - 10) );
uint32_t pCC_Storage_autostack_top   = (uint32_t)(pCC_Storage_Base + CC_WORD_SIZE * 50 );
uint32_t pCC_Storage_FaultStatus_top = (uint32_t)(pCC_Storage_Base + CC_WORD_SIZE * 95 );


#endif // #if !__ASSEMBLER__
#endif /* end of __uvisor_CrashCatcher_H */
