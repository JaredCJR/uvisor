/*
 * Copyright 2016 Chang,Jia-Rung (https://github.com/JaredCJR)
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
#ifndef __MRI_H__
#define __MRI_H__

#include <uvisor.h>

#define CoreDebug_DEMCR_MON_END     (1 << 16)

static void defaultSvcAndSysTickInterruptsToPriority1(void)                                                                                  
{
    //NVIC_SetPriority(SVCall_IRQn, 1);
    NVIC_SetPriority(PendSV_IRQn, 1);
    NVIC_SetPriority(SysTick_IRQn, 1);
}

static void enableDebugMonitorAtPriority0(void)
{
    NVIC_SetPriority(DebugMonitor_IRQn, 0);                                                                                                      
    CoreDebug->DEMCR |=  CoreDebug_DEMCR_MON_END;
}

void __mriCortexMInit(void)
{
    defaultSvcAndSysTickInterruptsToPriority1();
    enableDebugMonitorAtPriority0();
}

#endif/*__MRI_H__*/
