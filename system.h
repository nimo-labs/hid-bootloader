/*
 * This file is part of the hid-bootloader distribution (https://github.com/nimo-labs/hid-bootloader).
 * Copyright (c) 2020 Nimolabs Ltd. www.nimo.uk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*Main is providing this*/
extern void UART_STR(unsigned char uart, char *str);

#define APPLICATION_START 0x00001000
#define PAGES_IN_ERASE_BLOCK NVMCTRL_ROW_PAGES
#define ERASE_BLOCK_SIZE NVMCTRL_ROW_SIZE
#define FLASH_PAGE_SIZE_WORDS (int)(FLASH_PAGE_SIZE / sizeof(uint32_t))