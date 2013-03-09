################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c 

OBJS += \
./CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.o 

C_DEPS += \
./CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/CM3/DeviceSupport/ST/STM32F10x/%.o: ../CMSIS/CM3/DeviceSupport/ST/STM32F10x/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Linux GCC C Compiler'
	arm-none-eabi-gcc -DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD -I"/home/bbingo/workspace/SunMoonOnSTM32/CMSIS/CM3/CoreSupport" -I"/home/bbingo/workspace/SunMoonOnSTM32/CMSIS/CM3/DeviceSupport/ST/STM32F10x" -I"/home/bbingo/workspace/SunMoonOnSTM32/STM32F10x_StdPeriph_Driver/inc" -I"/home/bbingo/workspace/SunMoonOnSTM32/src/LCD" -I"/home/bbingo/workspace/SunMoonOnSTM32/src/touch" -I"/home/bbingo/workspace/SunMoonOnSTM32/src" -O0 -ffunction-sections -fdata-sections -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


