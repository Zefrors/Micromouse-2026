################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../X-CUBE-MEMS1/Target/custom_motion_sensors.c \
../X-CUBE-MEMS1/Target/custom_motion_sensors_ex.c 

OBJS += \
./X-CUBE-MEMS1/Target/custom_motion_sensors.o \
./X-CUBE-MEMS1/Target/custom_motion_sensors_ex.o 

C_DEPS += \
./X-CUBE-MEMS1/Target/custom_motion_sensors.d \
./X-CUBE-MEMS1/Target/custom_motion_sensors_ex.d 


# Each subdirectory must supply rules for building sources it contributes
X-CUBE-MEMS1/Target/%.o X-CUBE-MEMS1/Target/%.su X-CUBE-MEMS1/Target/%.cyclo: ../X-CUBE-MEMS1/Target/%.c X-CUBE-MEMS1/Target/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../X-CUBE-MEMS1/Target -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/Components/ism330dhcx -I../Drivers/BSP/Components/Common -I../Middlewares/ST/STM32_MotionGC_Library/Inc -I../Middlewares/ST/STM32_MotionAC_Library/Inc -I../Middlewares/ST/STM32_MotionFX_Library/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-X-2d-CUBE-2d-MEMS1-2f-Target

clean-X-2d-CUBE-2d-MEMS1-2f-Target:
	-$(RM) ./X-CUBE-MEMS1/Target/custom_motion_sensors.cyclo ./X-CUBE-MEMS1/Target/custom_motion_sensors.d ./X-CUBE-MEMS1/Target/custom_motion_sensors.o ./X-CUBE-MEMS1/Target/custom_motion_sensors.su ./X-CUBE-MEMS1/Target/custom_motion_sensors_ex.cyclo ./X-CUBE-MEMS1/Target/custom_motion_sensors_ex.d ./X-CUBE-MEMS1/Target/custom_motion_sensors_ex.o ./X-CUBE-MEMS1/Target/custom_motion_sensors_ex.su

.PHONY: clean-X-2d-CUBE-2d-MEMS1-2f-Target

