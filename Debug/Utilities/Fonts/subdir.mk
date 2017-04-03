################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/Fonts/font12.c \
../Utilities/Fonts/font16.c \
../Utilities/Fonts/font20.c \
../Utilities/Fonts/font24.c \
../Utilities/Fonts/font8.c 

OBJS += \
./Utilities/Fonts/font12.o \
./Utilities/Fonts/font16.o \
./Utilities/Fonts/font20.o \
./Utilities/Fonts/font24.o \
./Utilities/Fonts/font8.o 

C_DEPS += \
./Utilities/Fonts/font12.d \
./Utilities/Fonts/font16.d \
./Utilities/Fonts/font20.d \
./Utilities/Fonts/font24.d \
./Utilities/Fonts/font8.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/Fonts/%.o: ../Utilities/Fonts/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -DUSE_HAL_DRIVER -DSTM32F746xx -DUSE_STM32746G_DISCO -DUSE_IOEXPANDER -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/Components/Common" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/Components/wm8994" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/Components/ov9655" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/Components/ft5336" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/STM32746G-Discovery" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/CMSIS/Include" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/STM32F7xx_HAL_Driver/Inc" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Projects/STM32746G-Discovery/Examples/BSP/Inc" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Utilities/CPU" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Utilities/Fonts" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Utilities/Log" -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


