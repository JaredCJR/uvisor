/*
 * Copyright (c) 2016 Jia-Rung Chang (https://github.com/JaredCJR)
 */

#include <uvisor.h>
#include "../inc/CrashCatcherPriv.h"

/* Fault handler will switch MSP to use this area as the stack while CrashCatcher code is running.
   NOTE: If you change the size of this buffer, it also needs to be changed in the Fault_Handler
         when initializing the stack pointer. */
uint32_t g_crashCatcherStack[CRASH_CATCHER_STACK_WORD_COUNT];


/* Test harness will define this value on 64-bit machine to provide upper 32-bits of pointer addresses. */
uint64_t g_crashCatcherTestBaseAddress;

typedef struct
{
    const CrashCatcherExceptionRegisters* pExceptionRegisters;
    const CrashCatcherStackedRegisters*   pSP;
    uint32_t                              sp;
    uint32_t                              flags;
} Object;


static uint32_t getAddressOfExceptionStack(const CrashCatcherExceptionRegisters* pExceptionRegisters)
{
    if (pExceptionRegisters->exceptionLR & LR_PSP)                                                                                               
        return pExceptionRegisters->psp;
    else
        return pExceptionRegisters->msp;
}

static const void* uint32AddressToPointer(uint32_t address)
{
    if (sizeof(uint32_t*) == 8)
        return (const void*)(unsigned long)((uint64_t)address | g_crashCatcherTestBaseAddress);                                                  
    else
        return (const void*)(unsigned long)address;
}

static Object initStackPointers(const CrashCatcherExceptionRegisters* pExceptionRegisters) 
{
    Object object;
    object.pExceptionRegisters = pExceptionRegisters;
    object.sp = getAddressOfExceptionStack(pExceptionRegisters);
    object.pSP = uint32AddressToPointer(object.sp);
    object.flags = 0;
    return object;
}


void CrashCatcher_Entry(const CrashCatcherExceptionRegisters* pExceptionRegisters)
{
    DPRINTF("\n-----*****CrashCatcher_Entry_start*****-----\n");
    
    Object object = initStackPointers(pExceptionRegisters);
    //advanceStackPointerToValueBeforeException(&object);
    //initFloatingPointFlag(&object);
/*
    do
    {
        setStackSentinel();
        CrashCatcher_DumpStart();
        dumpSignature(&object);
        dumpFlags(&object);
        dumpR0toR3(&object);
        dumpR4toR11(&object);
        dumpR12(&object);
        dumpSP(&object);
        dumpLR_PC_PSR(&object);
        dumpExceptionPSR(&object);
        if (object.flags & CRASH_CATCHER_FLAGS_FLOATING_POINT)
            dumpFloatingPointRegisters(&object);
        dumpMemoryRegions(CrashCatcher_GetMemoryRegions());
        if (!isARMv6MDevice())
            dumpFaultStatusRegisters();
        checkStackSentinelForStackOverflow();
    }
    while (CrashCatcher_DumpEnd() == CRASH_CATCHER_TRY_AGAIN);
    */
}


/* Call CrashCatcher_Entry with first argument pointing to registers that were stacked.
   The main entry point into CrashCatcher.
 */
void _CrashCatcher_Entry(void)
{
    asm volatile(
            "mov     r0, (g_crashCatcherStack + 4 * (100-11))\n"/*"100" must equal to CRASH_CATCHER_STACK_WORD_COUNT*/
            "bl      CrashCatcher_Entry\n"
    );

    DPRINTF("\n-----*****CrashCatcher_Entry_end*****-----\n");
}


