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
void flash_flush(void);
void flash_write(uint32_t addr, uint8_t data);
void run_application(void);
