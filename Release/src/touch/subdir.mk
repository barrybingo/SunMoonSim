################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/touch/touch.c 

OBJS += \
./src/touch/touch.o 

C_DEPS += \
./src/touch/touch.d 


# Each subdirectory must supply rules for building sources it contributes
src/touch/%.o: ../src/touch/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Linux GCC C Compiler'
	arm-none-eabi-gcc -DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD -I"/home/bbingo/workspace/SunMoonOnSTM32/CMSIS/CM3/CoreSupport" -I"/home/bbingo/workspace/SunMoonOnSTM32/CMSIS/CM3/DeviceSupport/ST/STM32F10x" -I"/home/bbingo/workspace/SunMoonOnSTM32/STM32F10x_StdPeriph_Driver/inc" -I"/home/bbingo/workspace/SunMoonOnSTM32/src/LCD" -I"/home/bbingo/workspace/SunMoonOnSTM32/src/touch" -I"/home/bbingo/workspace/SunMoonOnSTM32/src" -Os -ffunction-sections -fdata-sections -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


