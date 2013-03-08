echo SunMoonOnSTM32 debugging starting......\n
target remote 127.0.0.1:2331
monitor speed auto
monitor flash device=stm32f103rb
monitor flash breakpoints = 1
monitor flash download = 1
monitor reg sp = (0x00000000)
monitor reg r13 = (0x00000000)
monitor reg pc = (0x00000004)
file SunMoonOnSTM32.elf
load
monitor reset
break main
continue
