################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/led.c \
../src/main.c \
../src/stm32f10x_it.c 

S_UPPER_SRCS += \
../src/startup_stm32f10x_md.S 

OBJS += \
./src/led.o \
./src/main.o \
./src/startup_stm32f10x_md.o \
./src/stm32f10x_it.o 

C_DEPS += \
./src/led.d \
./src/main.d \
./src/stm32f10x_it.d 

S_UPPER_DEPS += \
./src/startup_stm32f10x_md.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Linux GCC C Compiler'
	arm-none-eabi-gcc -DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD -I"/home/bbingo/workspace/SunMoonOnSTM32/CMSIS/CM3/CoreSupport" -I"/home/bbingo/workspace/SunMoonOnSTM32/CMSIS/CM3/DeviceSupport/ST/STM32F10x" -I"/home/bbingo/workspace/SunMoonOnSTM32/STM32F10x_StdPeriph_Driver/inc" -I"/home/bbingo/workspace/SunMoonOnSTM32/src" -Os -ffunction-sections -fdata-sections -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Linux GCC Assembler'
	arm-none-eabi-gcc -x assembler-with-cpp -DUSE_STDPERIPH_DRIVER -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


