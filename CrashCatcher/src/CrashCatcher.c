/*
 * Copyright (c) 2016 Jia-Rung Chang (https://github.com/JaredCJR)
 */

#include "../inc/CrashCatcherPriv.h"

/* Fault handler will switch MSP to use this area as the stack while CrashCatcher code is running.
   NOTE: If you change the size of this buffer, it also needs to be changed in the Fault_Handler
         when initializing the stack pointer. */

uint32_t g_crashCatcherStack[CRASH_CATCHER_STACK_WORD_COUNT];
