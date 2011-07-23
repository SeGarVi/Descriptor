################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/descriptor.c \
../src/descriptor_debug.c \
../src/lista_puntos.c \
../src/punto.c \
../src/test.c 

OBJS += \
./src/descriptor.o \
./src/descriptor_debug.o \
./src/lista_puntos.o \
./src/punto.o \
./src/test.o 

C_DEPS += \
./src/descriptor.d \
./src/descriptor_debug.d \
./src/lista_puntos.d \
./src/punto.d \
./src/test.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


