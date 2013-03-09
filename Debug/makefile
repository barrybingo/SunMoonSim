################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/touch/subdir.mk
-include src/LCD/subdir.mk
-include src/subdir.mk
-include STM32F10x_StdPeriph_Driver/src/subdir.mk
-include CMSIS/CM3/DeviceSupport/ST/STM32F10x/subdir.mk
-include CMSIS/CM3/CoreSupport/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
SunMoonOnSTM32.hex \

SECONDARY_LIST += \
SunMoonOnSTM32.lst \

SECONDARY_SIZE += \
SunMoonOnSTM32.siz \


# All Target
all: SunMoonOnSTM32.elf secondary-outputs

# Tool invocations
SunMoonOnSTM32.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Sourcery Linux GCC C++ Linker'
	arm-none-eabi-g++ -T"../LinkerScript/STM32F103-MD.ld" -Xlinker --gc-sections -Wl,-Map,SunMoonOnSTM32.map -mcpu=cortex-m3 -mthumb -g3 -gdwarf-2 -o "SunMoonOnSTM32.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

SunMoonOnSTM32.hex: SunMoonOnSTM32.elf
	@echo 'Invoking: ARM Sourcery Linux GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex SunMoonOnSTM32.elf  "SunMoonOnSTM32.hex"
	@echo 'Finished building: $@'
	@echo ' '

SunMoonOnSTM32.lst: SunMoonOnSTM32.elf
	@echo 'Invoking: ARM Sourcery Linux GNU Create Listing'
	arm-none-eabi-objdump -h -S SunMoonOnSTM32.elf > "SunMoonOnSTM32.lst"
	@echo 'Finished building: $@'
	@echo ' '

SunMoonOnSTM32.siz: SunMoonOnSTM32.elf
	@echo 'Invoking: ARM Sourcery Linux GNU Print Size'
	arm-none-eabi-size  --format=berkeley SunMoonOnSTM32.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(SECONDARY_FLASH)$(CXX_DEPS)$(S_UPPER_DEPS)$(SECONDARY_LIST)$(C++_DEPS)$(SECONDARY_SIZE)$(ASM_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(C_UPPER_DEPS) SunMoonOnSTM32.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
