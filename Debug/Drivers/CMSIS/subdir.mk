################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/CMSIS/system_stm32f7xx.c 

OBJS += \
./Drivers/CMSIS/system_stm32f7xx.o 

C_DEPS += \
./Drivers/CMSIS/system_stm32f7xx.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/%.o: ../Drivers/CMSIS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -DUSE_HAL_DRIVER -DSTM32F746xx -DUSE_STM32746G_DISCO -DUSE_IOEXPANDER -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/Components/Common" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/Components/wm8994" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/Components/ov9655" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/Components/ft5336" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/STM32746G-Discovery" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/CMSIS/Include" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/STM32F7xx_HAL_Driver/Inc" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Projects/STM32746G-Discovery/Examples/BSP/Inc" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Utilities/CPU" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Utilities/Fonts" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Utilities/Log" -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


