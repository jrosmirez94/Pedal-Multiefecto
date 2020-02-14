################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/adc.c \
../Src/audio.c \
../Src/dma.c \
../Src/fuzz.c \
../Src/gpio.c \
../Src/i2c.c \
../Src/i2s.c \
../Src/main.c \
../Src/main_funcs.c \
../Src/stm32f4xx_hal_msp.c \
../Src/stm32f4xx_it.c \
../Src/syscalls.c \
../Src/system_stm32f4xx.c \
../Src/tim.c \
../Src/wahwah.c 

OBJS += \
./Src/adc.o \
./Src/audio.o \
./Src/dma.o \
./Src/fuzz.o \
./Src/gpio.o \
./Src/i2c.o \
./Src/i2s.o \
./Src/main.o \
./Src/main_funcs.o \
./Src/stm32f4xx_hal_msp.o \
./Src/stm32f4xx_it.o \
./Src/syscalls.o \
./Src/system_stm32f4xx.o \
./Src/tim.o \
./Src/wahwah.o 

C_DEPS += \
./Src/adc.d \
./Src/audio.d \
./Src/dma.d \
./Src/fuzz.d \
./Src/gpio.d \
./Src/i2c.d \
./Src/i2s.d \
./Src/main.d \
./Src/main_funcs.d \
./Src/stm32f4xx_hal_msp.d \
./Src/stm32f4xx_it.d \
./Src/syscalls.d \
./Src/system_stm32f4xx.d \
./Src/tim.d \
./Src/wahwah.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DUSE_HAL_DRIVER -DARM_MATH_CM4 -DSTM32F411xE -I"C:/Users/pablo/OneDrive/Desktop/pedal_multiefecto/Pedal_Multiefecto_guitarraelectrica - v08/Inc" -I"C:/Users/pablo/OneDrive/Desktop/pedal_multiefecto/Pedal_Multiefecto_guitarraelectrica - v08/Inc/CMSIS" -I"C:/Users/pablo/OneDrive/Desktop/pedal_multiefecto/Pedal_Multiefecto_guitarraelectrica - v08/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/pablo/OneDrive/Desktop/pedal_multiefecto/Pedal_Multiefecto_guitarraelectrica - v08/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"C:/Users/pablo/OneDrive/Desktop/pedal_multiefecto/Pedal_Multiefecto_guitarraelectrica - v08/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/pablo/OneDrive/Desktop/pedal_multiefecto/Pedal_Multiefecto_guitarraelectrica - v08/Drivers/CMSIS/Include"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


