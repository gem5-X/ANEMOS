################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/tb/cu_driver.cpp \
../src/tb/cu_monitor.cpp \
../src/tb/fpu_driver.cpp \
../src/tb/fpu_monitor.cpp \
../src/tb/id_driver.cpp \
../src/tb/id_monitor.cpp \
../src/tb/imc_driver.cpp \
../src/tb/imc_driver_mixed.cpp \
../src/tb/imc_monitor.cpp \
../src/tb/pch_driver.cpp \
../src/tb/pch_driver_mixed.cpp \
../src/tb/pch_main.cpp \
../src/tb/pch_monitor.cpp 

CPP_DEPS += \
./src/tb/cu_driver.d \
./src/tb/cu_monitor.d \
./src/tb/fpu_driver.d \
./src/tb/fpu_monitor.d \
./src/tb/id_driver.d \
./src/tb/id_monitor.d \
./src/tb/imc_driver.d \
./src/tb/imc_driver_mixed.d \
./src/tb/imc_monitor.d \
./src/tb/pch_driver.d \
./src/tb/pch_driver_mixed.d \
./src/tb/pch_main.d \
./src/tb/pch_monitor.d 

OBJS += \
./src/tb/cu_driver.o \
./src/tb/cu_monitor.o \
./src/tb/fpu_driver.o \
./src/tb/fpu_monitor.o \
./src/tb/id_driver.o \
./src/tb/id_monitor.o \
./src/tb/imc_driver.o \
./src/tb/imc_driver_mixed.o \
./src/tb/imc_monitor.o \
./src/tb/pch_driver.o \
./src/tb/pch_driver_mixed.o \
./src/tb/pch_main.o \
./src/tb/pch_monitor.o 


# Each subdirectory must supply rules for building sources it contributes
src/tb/%.o: ../src/tb/%.cpp src/tb/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/opt/systemc/include -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-tb

clean-src-2f-tb:
	-$(RM) ./src/tb/cu_driver.d ./src/tb/cu_driver.o ./src/tb/cu_monitor.d ./src/tb/cu_monitor.o ./src/tb/fpu_driver.d ./src/tb/fpu_driver.o ./src/tb/fpu_monitor.d ./src/tb/fpu_monitor.o ./src/tb/id_driver.d ./src/tb/id_driver.o ./src/tb/id_monitor.d ./src/tb/id_monitor.o ./src/tb/imc_driver.d ./src/tb/imc_driver.o ./src/tb/imc_driver_mixed.d ./src/tb/imc_driver_mixed.o ./src/tb/imc_monitor.d ./src/tb/imc_monitor.o ./src/tb/pch_driver.d ./src/tb/pch_driver.o ./src/tb/pch_driver_mixed.d ./src/tb/pch_driver_mixed.o ./src/tb/pch_main.d ./src/tb/pch_main.o ./src/tb/pch_monitor.d ./src/tb/pch_monitor.o

.PHONY: clean-src-2f-tb

