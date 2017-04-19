################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Example/User/audio_rec.c \
../Example/User/camera.c \
../Example/User/filter.c \
../Example/User/lcd.c \
../Example/User/log.c \
../Example/User/main.c \
../Example/User/sdram.c \
../Example/User/sdram_dma.c \
../Example/User/stm32f7xx_it.c \
../Example/User/touchscreen.c 

OBJS += \
./Example/User/audio_rec.o \
./Example/User/camera.o \
./Example/User/filter.o \
./Example/User/lcd.o \
./Example/User/log.o \
./Example/User/main.o \
./Example/User/sdram.o \
./Example/User/sdram_dma.o \
./Example/User/stm32f7xx_it.o \
./Example/User/touchscreen.o 

C_DEPS += \
./Example/User/audio_rec.d \
./Example/User/camera.d \
./Example/User/filter.d \
./Example/User/lcd.d \
./Example/User/log.d \
./Example/User/main.d \
./Example/User/sdram.d \
./Example/User/sdram_dma.d \
./Example/User/stm32f7xx_it.d \
./Example/User/touchscreen.d 


# Each subdirectory must supply rules for building sources it contributes
Example/User/%.o: ../Example/User/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -DUSE_HAL_DRIVER -DSTM32F746xx -DUSE_STM32746G_DISCO -DUSE_IOEXPANDER -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/Components/Common" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/Components/wm8994" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/Components/ov9655" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/Components/ft5336" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/STM32746G-Discovery" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/CMSIS/Include" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/STM32F7xx_HAL_Driver/Inc" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Projects/STM32746G-Discovery/Examples/BSP/Inc" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Utilities/CPU" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Utilities/Fonts" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Utilities/Log" -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


