
/*
 * Copyright (c) 2017-2018, Alex Taradov <alex@taradov.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <sam.h>
#include <nimolib.h>
#include <stdalign.h>

#include "system.h"

static int app_block_index = -1;
static alignas(4) uint8_t app_flash_buf[ERASE_BLOCK_SIZE];

//-----------------------------------------------------------------------------
void flash_flush(void)
{
    uint32_t addr = FLASH_ADDR + app_block_index * ERASE_BLOCK_SIZE;
    uint32_t *flash_offset = (uint32_t *)addr;
    uint32_t *flash_data = (uint32_t *)app_flash_buf;

    if (-1 == app_block_index)
        return;

    NVMCTRL->ADDR.reg = addr >> 1;

    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_UR;
    while (0 == NVMCTRL->INTFLAG.bit.READY)
        ;

    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
    while (0 == NVMCTRL->INTFLAG.bit.READY)
        ;

    for (int page = 0; page < PAGES_IN_ERASE_BLOCK; page++)
    {
        for (int i = 0; i < FLASH_PAGE_SIZE_WORDS; i++)
            *flash_offset++ = *flash_data++;

        while (0 == NVMCTRL->INTFLAG.bit.READY)
            ;
    }

    app_block_index = -1;
}

//-----------------------------------------------------------------------------
void flash_write(uint32_t addr, uint8_t data)
{
    //int block_index = (addr % FLASH_SIZE) / ERASE_BLOCK_SIZE;
    unsigned int block_index = (addr % 0x40000) / ERASE_BLOCK_SIZE;
    if (block_index != app_block_index)
    {
        uint32_t block_addr = FLASH_ADDR + block_index * ERASE_BLOCK_SIZE;

        flash_flush();

        for (unsigned int i = 0; i < ERASE_BLOCK_SIZE; i++)
            app_flash_buf[i] = ((uint8_t *)block_addr)[i];
        app_block_index = block_index;
    }

    app_flash_buf[addr % ERASE_BLOCK_SIZE] = data;
}

//-----------------------------------------------------------------------------
void run_application(void)
{
#define APP_START_RESET_VEC_ADDRESS (APPLICATION_START + (uint32_t)0x00000004)

    uint32_t msp = *(uint32_t *)(APPLICATION_START);

    if (0xffffffff == msp)
        return;

    /* Pointer to the Application Section */
    void (*application_code_entry)(void);

    __disable_irq();
    /* Rebase the Stack Pointer */
    __set_MSP(msp);

    /* Rebase the vector table base address */
    SCB->VTOR = ((uint32_t)APPLICATION_START & SCB_VTOR_TBLOFF_Msk);

    /* Load the Reset Handler address of the application */
    application_code_entry = (void (*)(void))(unsigned *)(*(unsigned *)(APP_START_RESET_VEC_ADDRESS));

    /* Jump to user Reset Handler in the application */
    application_code_entry();
}