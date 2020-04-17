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
#include <sam.h>
#include <gpio.h>
#include <uart.h>
#include <osc.h>
#include <string.h>
#include <simpleHid.h>
#include <delay.h>

#include "nimolib.h"
#include "system.h"
#include "hidBlProtocol.h"
#include "intFlash.h"

#define SYS_STATE_SLEEP 0
#define SYS_STATE_IDLE_IN 1
#define SYS_STATE_IDLE 2
#define SYS_STATE_LOG_START 3
#define SYS_STATE_LOG 4
#define SYS_STATE_LOG_STOP 5
#define SYS_STATE_LOG_DUMP 6

unsigned long flashCtr = 0;

#define BOOT_MAGIC_ADDRESS           (0x20007FFCul)
#define BOOT_MAGIC_VALUE (*((volatile uint32_t *) BOOT_MAGIC_ADDRESS))

void UART_STR(unsigned char uart, char *str)
{
    while (*str)
    {
        uartTx(uart, *str++);
    }
}
void printHex(unsigned char data)
{
    unsigned char hex[]= {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    uartTx(PRINTF_UART, hex[data >>4]);
    uartTx(PRINTF_UART, hex[data &0x0f]);
}

/**************************USB stuff *******************/
void usbHidProcess(uint8_t *req)
{
    struct hidBlProtocolPacket_s recvPacket;
    struct hidBlProtocolPacket_s sendPacket;
    unsigned char serPkt[64];

    hidBlProtocolDeSerialisePacket(&recvPacket, req);

    switch (recvPacket.packetType)
    {
    case HID_BL_PROTOCOL_PTYPE_GET_VER:
        if (hidBlProtocolEncodePacket(&sendPacket,0, HID_BL_PROTOCOL_PTYPE_SEND_VER, (unsigned char *)"1.0", 3))
            return;
        break;
    case HID_BL_PROTOCOL_WRITE_INT_FLASH:
        for(unsigned int i=0; i < recvPacket.dataLen; i++)
            flash_write(recvPacket.address+i, recvPacket.data[i]);
        if (hidBlProtocolEncodePacket(&sendPacket,0, HID_BL_PROTOCOL_PTYPE_ACK, NULL, 0))
        {
            UART_STR(PRINTF_UART,"protocol encode error\r\n");
            return;
        }
        break;
    case HID_BL_PROTOCOL_RUN_INT:
        flash_flush();
        BOOT_MAGIC_VALUE = 0x00000000; /*Make sure that we boot the application and not us*/
        UART_STR(UART_CHAN0, "Run application\r\n\r\n\r\n");
        NVIC_SystemReset();
        break;
    default:
        hidBlProtocolEncodePacket(&sendPacket,0, HID_BL_PROTOCOL_PTYPE_ACK, NULL, 0);
        break;
    }
    hidBlProtocolSerialisePacket(&sendPacket, serPkt,sizeof(serPkt));
    usbSendWait(USB_EP_SEND, serPkt, sizeof(sendPacket));

}

//-----------------------------------------------------------------------------


void *memcpy (void * restrict dst,const void * src,size_t n)
{
    int i;
    char* d = dst;
    char* s = src;
    for (i = 0; i < n; i++)
    {
        d[i] = s[i];
    }
    return dst;
}

void main(void)
{
    unsigned char state = SYS_STATE_IDLE_IN;
    oscSet(OSC_48DFLL);
//   GPIO_PIN_DIR(SAM_GPIO_PORTA, 6, GPIO_DIR_OUT);
//    GPIO_PIN_OUT(SAM_GPIO_PORTA, 6, GPIO_OUT_LOW);

    if(0x0000DEAD != BOOT_MAGIC_VALUE)
    {
        delaySetup(DELAY_BASE_MICRO_SEC); /*Clock timer at 1mS*/
        delayMs(2000);/*Small delay to allow us to recover from a broken app osc config*/
        run_application();
        uartInit(UART_CHAN0, UART_BAUD_9600);
        UART_STR(PRINTF_UART, "\r\nHID Bootloader\r\n");
        UART_STR(PRINTF_UART, "(C)2020 Nimolabs Ltd\r\n");
        UART_STR(PRINTF_UART,"No valid application found\r\n");
    }
    else
    {
        uartInit(UART_CHAN0, UART_BAUD_9600);
        UART_STR(PRINTF_UART, "\r\nHID Bootloader\r\n");
        UART_STR(PRINTF_UART, "(C)2020 Nimolabs Ltd\r\n");
        UART_STR(PRINTF_UART,"App requested reset\r\n");
    }
    // UART_STR(PRINTF_UART, "BOOT_MAGIC_ADDRESS: 0x");
    // printHex(BOOT_MAGIC_VALUE >> 24);
    // printHex(BOOT_MAGIC_VALUE >> 16);
    // printHex(BOOT_MAGIC_VALUE >> 8);
    // printHex(BOOT_MAGIC_VALUE &0xff);
    // UART_STR(PRINTF_UART, "\r\n");

    //  GPIO_PIN_OUT(SAM_GPIO_PORTA, 6, GPIO_OUT_HIGH);

    usbInit();

    while (1)
    {
        switch (state)
        {
        case SYS_STATE_IDLE_IN:
            //    UART_STR(PRINTF_UART, "State idle in\r\n");
            state = SYS_STATE_IDLE;
            break;
        case SYS_STATE_IDLE:
            break;

        default:
            UART_STR(PRINTF_UART, "State error\r\n");
            while (1)
                ;
            break;
        }
        usbTask();
    }
}