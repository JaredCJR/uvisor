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
#include <uvisor.h>
#include "debug.h"
#include "halt.h"
#include "svc.h"
#include "memory_map.h"

#ifndef DEBUG_MAX_BUFFER
#define DEBUG_MAX_BUFFER 128
#endif/*DEBUG_MAX_BUFFER*/

uint8_t g_buffer[DEBUG_MAX_BUFFER];
int g_buffer_pos;

#ifndef CHANNEL_DEBUG
void default_putc(uint8_t data)
{
    if(g_buffer_pos<(DEBUG_MAX_BUFFER-1))
        g_buffer[g_buffer_pos++] = data;
    else
        data = '\n';

    if(data == '\n')
    {
        g_buffer[g_buffer_pos] = 0;
        asm(
            "mov r0, #4\n"
            "mov r1, %[data]\n"
            "bkpt #0xAB\n"
            :
            : [data] "r" (&g_buffer)
            : "r0", "r1"
        );
        g_buffer_pos = 0;
    }
}
#endif/*CHANNEL_DEBUG*/

void debug_init(void)
{
    /* debugging bus faults requires them to be precise, so write buffering is
     * disabled; note: it slows down execution */
    SCnSCB->ACTLR |= 0x2;
}

static void debug_cx_state(int _indent)
{
    int i;

    /* generate indentation depending on nesting depth */
    char _sp[UVISOR_MAX_BOXES + 4];

    for (i = 0; i < _indent; i++)
    {
        _sp[i]     = ' ';
    }
    _sp[i] = '\0';

    /* print state stack */
    if (!g_svc_cx_state_ptr)
    {
        dprintf("%sNo saved state\n\r", _sp);
    }
    else
    {
        for (i = 0; i < g_svc_cx_state_ptr; i++)
        {
            dprintf("%sState %d\n\r",        _sp, i);
            dprintf("%s  src_id %d\n\r",     _sp, g_svc_cx_state[i].src_id);
            dprintf("%s  src_sp 0x%08X\n\r", _sp, g_svc_cx_state[i].src_sp);
        }
    }

    /* print current stack pointers for all boxes */
    dprintf("%s     ", _sp);
    for (i = 0; i < g_vmpu_box_count; i++)
    {
        dprintf("------------ ");
    }
    dprintf("\n%sSP: |", _sp);
    for (i = 0; i < g_vmpu_box_count; i++)
    {
        dprintf(" 0x%08X |", g_svc_cx_curr_sp[i]);
    }
    dprintf("\n%s     ", _sp);
    for (i = 0; i < g_vmpu_box_count; i++)
    {
        dprintf("------------ ");
    }
    dprintf("\n");
}

void debug_cx_switch_in(void)
{
    int i;

    /* indent debug messages linearly with the nesting depth */
    dprintf("\n\r");
    for(i = 0; i < g_svc_cx_state_ptr; i++)
    {
        dprintf("--");
    }
    dprintf("> Context switch in\n");
    debug_cx_state(2 + (g_svc_cx_state_ptr << 1));
    dprintf("\n\r");
}

void debug_cx_switch_out(void)
{
    int i;

    /* indent debug messages linearly with the nesting depth */
    dprintf("\n\r<--");
    for(i = 0; i < g_svc_cx_state_ptr; i++)
    {
        dprintf("--");
    }
    dprintf(" Context switch out\n");
    debug_cx_state(4 + (g_svc_cx_state_ptr << 1));
    dprintf("\n\r");
}

void debug_exception_frame(uint32_t lr, uint32_t sp)
{
    int i;
    int mode = lr & 0x4;

    char exc_sf_verbose[SVC_CX_EXC_SF_SIZE + 1][6] = {
        "r0", "r1", "r2", "r3", "r12",
        "lr", "pc", "xPSR", "align"
    };

    dprintf("* EXCEPTION STACK FRAME\n");
    dprintf("  Exception from %s code\n", mode ? "unprivileged" : "privileged");
    dprintf("    %csp:     0x%08X\n\r", mode ? 'p' : 'm', sp);
    dprintf("    lr:      0x%08X\n\r", lr);

    /* print exception stack frame */
    dprintf("  Exception stack frame:\n");
    i = SVC_CX_EXC_SF_SIZE;
    if(((uint32_t *) sp)[8] & (1 << 9))
    {
        dprintf("    %csp[%02d]: 0x%08X | %s\n", mode ? 'p' : 'm', i, ((uint32_t *) sp)[i], exc_sf_verbose[i]);
    }
    for(i = SVC_CX_EXC_SF_SIZE - 1; i >= 0; --i)
    {
        dprintf("    %csp[%02d]: 0x%08X | %s\n", mode ? 'p' : 'm', i, ((uint32_t *) sp)[i], exc_sf_verbose[i]);
    }
    dprintf("\n");
}

void UVISOR_WEAK debug_fault_memmanage(void)
{
    dprintf("* CFSR  : 0x%08X\n\r\n\r", SCB->CFSR);
    dprintf("* MMFAR : 0x%08X\n\r\n\r", SCB->MMFAR);
}

void UVISOR_WEAK debug_fault_bus(void)
{
    dprintf("* CFSR  : 0x%08X\n\r\n\r", SCB->CFSR);
    dprintf("* BFAR  : 0x%08X\n\r\n\r", SCB->BFAR);
}

void UVISOR_WEAK debug_fault_usage(void)
{
    dprintf("* CFSR  : 0x%08X\n\r\n\r", SCB->CFSR);
}

void UVISOR_WEAK debug_fault_hard(void)
{
    dprintf("* HFSR  : 0x%08X\n\r\n\r", SCB->HFSR);
    dprintf("* CFSR  : 0x%08X\n\r\n\r", SCB->CFSR);
    dprintf("* DFSR  : 0x%08X\n\r\n\r", SCB->DFSR);
    dprintf("* BFAR  : 0x%08X\n\r\n\r", SCB->BFAR);
    dprintf("* MMFAR : 0x%08X\n\r\n\r", SCB->MMFAR);
}

void UVISOR_WEAK debug_fault_debug(void)
{
    dprintf("* DFSR  : 0x%08X\n\r\n\r", SCB->DFSR);
}

void debug_fault(THaltError reason, uint32_t lr, uint32_t sp)
{
    /* fault-specific code */
    switch(reason)
    {
        case FAULT_MEMMANAGE:
            DEBUG_PRINT_HEAD("MEMMANAGE FAULT");
            debug_fault_memmanage();
            break;
        case FAULT_BUS:
            DEBUG_PRINT_HEAD("BUS FAULT");
            debug_fault_bus();
            break;
        case FAULT_USAGE:
            DEBUG_PRINT_HEAD("USAGE FAULT");
            debug_fault_usage();
            break;
        case FAULT_HARD:
            DEBUG_PRINT_HEAD("HARD FAULT");
            debug_fault_hard();
            break;
        case FAULT_DEBUG:
            DEBUG_PRINT_HEAD("DEBUG FAULT");
            debug_fault_debug();
            break;
        default:
            DEBUG_PRINT_HEAD("[unknown fault]");
            break;
    }

    /* blue screen */
    debug_exception_frame(lr, sp);
    debug_mpu_config();
    DEBUG_PRINT_END();
}

static void __debug_reboot(void)
{
    UVISOR_SVC(UVISOR_SVC_ID_DEBUG_REBOOT, "");
}

/* This function must be called as an SVCall handler.
 * All debug handlers that are required to reboot upon exit should use the
 * __debug_reboot function as return value, which triggers the SVCall. This note
 * applies to uVisor internally, as the actual debug box does not need to care
 * about this. */
void debug_reboot(void)
{
    /* FIXME: The halt will be replaced with a proper return code. An ACL will
     *        be created to allow single boxes to reset the device. */
    if (!g_debug_box.initialized || g_active_box != g_debug_box.box_id) {
        HALT_ERROR(NOT_ALLOWED, "This function can only be called from the context of an initialized debug box.\n\r");
    }

    /* Reboot.
     * If called from unprivileged code, NVIC_SystemReset causes a fault. */
    NVIC_SystemReset();
}

/* FIXME: Currently it is not possible to return to a regular execution flow
 *        after the execution of the debug box handler. It is possible to
 *        reboot, though. */
static void debug_deprivilege_and_return(void *debug_handler, void *return_handler,
                                         uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    uint8_t src_id, dst_id;
    uint32_t dst_sp_align;
    uint32_t *src_sp, *dst_sp;

    /* Gather information from the current state.
     * The destination box is always the debug box. */
    /* Note: The source stack pointer is only used to assess the stack
     *       alignment. */
    src_sp = svc_cx_validate_sf((uint32_t *) __get_PSP());
    src_id = g_active_box;
    dst_sp = g_svc_cx_curr_sp[g_debug_box.box_id];
    dst_id = g_debug_box.box_id;

    /* Check source and destination box IDs */
    if (src_id == dst_id) {
        HALT_ERROR(NOT_ALLOWED, "The system is already running in the context of the debug box.\n\r");
    }

    /* Create the exception stack frame. */
    dst_sp_align = ((uint32_t) dst_sp & 0x4) ? 1 : 0;
    dst_sp -= (SVC_CX_EXC_SF_SIZE + dst_sp_align);

    /* Populate the exception stack frame:
     *   - The scratch registers are populated with the handler arguments.
     *   - r12 is cleared.
     *   - lr is the thunk function (die).
     *   - The return address is the destination function. */
    dst_sp[0] = a0;
    dst_sp[1] = a1;
    dst_sp[2] = a2;
    dst_sp[3] = a3;
    dst_sp[4] = 0;
    dst_sp[5] = (uint32_t) return_handler;
    dst_sp[6] = (uint32_t) debug_handler;
    dst_sp[7] = src_sp[7] | (dst_sp_align << 9);

    /* Set the context stack pointer for the destination box. */
    *(__uvisor_config.uvisor_box_context) = g_svc_cx_context_ptr[dst_id];

    /* Switch boxes. */
    vmpu_switch(src_id, dst_id);
    __set_PSP((uint32_t) dst_sp);

    /* Upon execution return debug_handler will be executed. Upon return from
     * debug_handler, return_handler will be executed. */
    return;
}

uint32_t debug_get_version(void)
{
    /* TODO: This function cannot be implemented without a mechanism for
     *       de-privilege, execute, return, and re-privilege. */
    HALT_ERROR(NOT_IMPLEMENTED, "This handler is not implemented yet. Only version 0 is supported.\n\r");
    return 0;
}

void debug_halt_error(THaltError reason)
{
    /* The following arguments are passed to the destination function:
     *   1. reason
     * Upon return from the debug handler, the system will reboot. */
    debug_deprivilege_and_return(g_debug_box.driver->halt_error, __debug_reboot, reason, 0, 0, 0);
}

void debug_register_driver(const TUvisorDebugDriver * const driver)
{
    int i;

    /* Check if already initialized. */
    if (g_debug_box.initialized) {
        HALT_ERROR(NOT_ALLOWED, "The debug box has already been initialized.\n\r");
    }

    /* Check the driver version. */
    /* FIXME: Currently we cannot de-privilege, execute, and return to a
     *        user-provided handler, so we are not calling the get_version()
     *        handler. The version of the driver will be assumed to be 0. */

    /* Check that the debug driver table and all its entries are in public
     * flash. */
    if (!vmpu_flash_addr((uint32_t) driver) || !vmpu_flash_addr((uint32_t) driver + sizeof(TUvisorDebugDriver))) {
        HALT_ERROR(SANITY_CHECK_FAILED, "The debug box driver struct must be stored in public flash.\n\r");
    }
    for (i = 0; i < DEBUG_BOX_HANDLERS_NUMBER; i++) {
        if (!vmpu_flash_addr(*((uint32_t *) driver + i))) {
            HALT_ERROR(SANITY_CHECK_FAILED, "Each handler in the debug box driver struct must be stored in public flash.\n\r");
        }
    }

    /* Register the debug box.
     * The caller of this function is considered the owner of the debug box. */
    g_debug_box.driver = driver;
    g_debug_box.box_id = g_active_box;
    g_debug_box.initialized = 1;
}
