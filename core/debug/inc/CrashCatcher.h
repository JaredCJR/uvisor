
#ifndef __uvisor_CrashCatcher_H
#define __uvisor_CrashCatcher_H

#define ccADDR  (0x20000000 + 4 * 50);


/* Definitions only required from C code. */
#if !__ASSEMBLER__

#include <uvisor.h>


uint32_t pCC_Storage_stack_lr  = (uint32_t)(0x20000000 + 4 * (50 - 1) );
uint32_t pCC_Storage_stack_psp = (uint32_t)(0x20000000 + 4 * (50 - 11) );
uint32_t pCC_Storage_stack_msp = (uint32_t)(0x20000000 + 4 * (50 - 10) );
uint32_t pCC_Storage_autostack_top = (uint32_t)(0x20000000+ 4 * 50);
uint32_t pCC_Storage_FaultStatus_top = (uint32_t)(0x20000000 + 4 * 95 );


#endif // #if !__ASSEMBLER__
#endif /* end of __uvisor_CrashCatcher_H */
