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
#define UP_CLK 48000000

#define PERHIP_CLK_GEN 0

#define PRINTF_UART UART_CHAN0
#define PRINTF_BUFF_SIZE 0

/*Debug UART*/
#define UART_CHAN0 0
#define UART_CHAN0_SERCOM 0
#define UART_CHAN0_IRQ void irq_handler_sercom0(void)
#define UART_CHAN0_FIFO_LEN 10
#define UART_CHAN0_PORT SAM_GPIO_PORTA
#define UART_CHAN0_RX_PIN 5
#define UART_CHAN0_TX_PIN 4
#define UART_CHAN0_RX_PAD 1
#define UART_CHAN0_TX_PAD 0 /*1 is pad 2*/
#define UART_CHAN0_PERHIPH_RX_MUX SAM_GPIO_PMUX_D
#define UART_CHAN0_PERHIPH_TX_MUX SAM_GPIO_PMUX_D

/*USB HID */
#define USB_EP_SEND 1
#define USB_EP_RECV 2
#define USB_BUFFER_SIZE 64
