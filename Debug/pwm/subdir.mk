################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../pwm/pwm.c \
../pwm/soft_pwm.c 

OBJS += \
./pwm/pwm.o \
./pwm/soft_pwm.o 

C_DEPS += \
./pwm/pwm.d \
./pwm/soft_pwm.d 


# Each subdirectory must supply rules for building sources it contributes
pwm/%.o: ../pwm/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


