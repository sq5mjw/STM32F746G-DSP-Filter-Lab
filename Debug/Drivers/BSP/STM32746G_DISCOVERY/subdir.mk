################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery.c \
../Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_audio.c \
../Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_camera.c \
../Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_eeprom.c \
../Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_lcd.c \
../Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_qspi.c \
../Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_sd.c \
../Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_sdram.c \
../Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_ts.c 

OBJS += \
./Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery.o \
./Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_audio.o \
./Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_camera.o \
./Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_eeprom.o \
./Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_lcd.o \
./Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_qspi.o \
./Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_sd.o \
./Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_sdram.o \
./Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_ts.o 

C_DEPS += \
./Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery.d \
./Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_audio.d \
./Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_camera.d \
./Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_eeprom.d \
./Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_lcd.d \
./Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_qspi.d \
./Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_sd.d \
./Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_sdram.d \
./Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_ts.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/STM32746G_DISCOVERY/%.o: ../Drivers/BSP/STM32746G_DISCOVERY/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -DUSE_HAL_DRIVER -DSTM32F746xx -DUSE_STM32746G_DISCO -DUSE_IOEXPANDER -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/Components/Common" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/Components/wm8994" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/Components/ov9655" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/Components/ft5336" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/BSP/STM32746G-Discovery" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/CMSIS/Include" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Drivers/STM32F7xx_HAL_Driver/Inc" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Projects/STM32746G-Discovery/Examples/BSP/Inc" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Utilities/CPU" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Utilities/Fonts" -I"/home/r2d2/workspace/cwfilter_sq5mjw/Inc/Utilities/Log" -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


