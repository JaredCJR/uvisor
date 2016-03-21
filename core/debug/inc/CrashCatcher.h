
#ifndef __uvisor_CrashCatcher_H
#define __uvisor_CrashCatcher_H

#define ccADDR  (0x20000000 + 4 * 50);


/* Definitions only required from C code. */
#if !__ASSEMBLER__

#include <uvisor.h>


void ccPush_unStackedRegisters(uint32_t para);
uint32_t pCC_Storage_stack_top = (uint32_t)(0x20000000 + 4 * 50);


#endif // #if !__ASSEMBLER__
#endif /* end of __uvisor_CrashCatcher_H */
