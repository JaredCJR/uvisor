
#ifndef __CrashCatcher_H
#define __CrashCatcher_H


#define CRASH_CATCHER_STACK_WORD_COUNT 100

/* Fault handler will switch MSP to use this area as the stack while CrashCatcher code is running.
   NOTE: If you change the size of this buffer, it also needs to be changed in the HardFault_Handler (in
         FaultHandler_arm*.S) when initializing the stack pointer. */
typedef struct
{
    volatile uint32_t stack[CRASH_CATCHER_STACK_WORD_COUNT];
}CatcherStack_TypeDef;

#define CatcherStack_Base     ((uint32_t)0x20000000)
#define g_crashCatcherStack   ((CatcherStack_TypeDef *)CatcherStack_Base)

#endif /* end of __CrashCatcher_H */
