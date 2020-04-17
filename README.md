# Introduction 
A small (<4k) bootloader for the ATSAMD21 using the USB HID protocol.

# Getting Started
To build this project you will need the arm-none-eabi toolchain and umake (https://github.com/nimo-labs/umake).
umake does the hardwork of downloading the appropriate libraries and generating a suitible makefile. Currently, the default program target for make requires openocd, however the build process generates a standard HEX file which can be programmed using standard tools.

# Build and Test
