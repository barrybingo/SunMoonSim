SunMoonSim
==========

Sun Moon simulator for the STM32-MINI-V3.0 dev board.

Built for STM32F103RBT6 with ADS7843 touch screen and LCD 2.8" TFT controller ILI9325

Output
* PB0 - LED Sun
* PB1 - LED Moon
* PA2 - LED1 on-board used for RTC seconds tick tock
* PA3 - LED2 on-board unused

Input
* PA0 - KEY2 on-board go to main menu
* PA1 - KEY1 on-board used to indicate recalibrate touch screen 



No Settings are stored in non-volatile storage.

Power cycle the unit to calibrate the screen.

Build instructions
-------------------

Built using 
Code Sourcery Lite 

Tested on version Sourcery CodeBench Lite 2011.09-69 for Linux AMD64

Build makefiles in Debug and Release folders

    cd Debug && make 
    cd Release && make SunMoonOnSTM32.hex

Includes Eclipse project file for that IDE feeling.  In fact the makefiles are created by Eclipse.

Eclipse plugins:

The CDT repo found at http://download.eclipse.org/tools/cdt/releases/helios

-  C/C++ Development Tools
-  C/C++ GDB Hardware Debugging

GNU ARM Dev repo from http://gnuarmeclipse.sourceforge.net/updates

-  GNU ARM C/C++ Development Support


Check out this [helpful guide](https://sites.google.com/site/stm32discovery/) for building and debugging STM using eclipse, gdb and codesourcery

JTAG and Debugging
-------------------------------

Tested using JLink from [http://segger.com/jlink.html](http://segger.com/jlink.html)

The supplied /etc/udev/rules.d/45-jlink.rules was not working on my system for non root users but these lines worked.

    ACTION=="add", SUBSYSTEM=="usb", ATTRS{idProduct}=="0101", ATTRS{idVendor}=="1366", MODE="0664", GROUP="plugdev"
    ACTION=="add", SUBSYSTEM=="usb", ATTRS{idProduct}=="0102", ATTRS{idVendor}=="1366", MODE="0664", GROUP="plugdev"
    ACTION=="add", SUBSYSTEM=="usb", ATTRS{idProduct}=="0103", ATTRS{idVendor}=="1366", MODE="0664", GROUP="plugdev"
    ACTION=="add", SUBSYSTEM=="usb", ATTRS{idProduct}=="0104", ATTRS{idVendor}=="1366", MODE="0664", GROUP="plugdev"

Also make sure to get the 32bit dependencies 

    sudo apt-get install lib32readline5 lib32tinfo5

Simply as user start JLinkGDBServer with:

    ./JLinkGDBServer

Whilst the server is listening open a GDB connection with:

    arm-none-eabi-gdb -n  -ex 'target remote 127.0.0.1:2331' 

Then test the debugging within GDB:

    monitor speed auto
    monitor flash device=stm32f103rb
    monitor flash breakpoints = 1
    monitor flash download = 1
    monitor reg sp = (0x00000000)
    monitor reg r13 = (0x00000000)
    monitor reg pc = (0x00000004)
    file Debug/SunMoonOnSTM32.elf
    load
    monitor reset
    continue
    .....


Once everything is set up the whole build and debug will look like this:

    cd Debug
    make SunMoonOnSTM32.elf
    arm-none-eabi-gdb

Debug/.gdbinit contains the GDB initialisation commands

