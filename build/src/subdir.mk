################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/control_unit.cpp \
../src/crf.cpp \
../src/fp_adder.cpp \
../src/fp_multiplier.cpp \
../src/fpu.cpp \
../src/grf.cpp \
../src/imc_core.cpp \
../src/imc_pch.cpp \
../src/instr_decoder.cpp \
../src/interface_unit.cpp \
../src/pc_unit.cpp \
../src/srf.cpp 

CPP_DEPS += \
./src/control_unit.d \
./src/crf.d \
./src/fp_adder.d \
./src/fp_multiplier.d \
./src/fpu.d \
./src/grf.d \
./src/imc_core.d \
./src/imc_pch.d \
./src/instr_decoder.d \
./src/interface_unit.d \
./src/pc_unit.d \
./src/srf.d 

OBJS += \
./src/control_unit.o \
./src/crf.o \
./src/fp_adder.o \
./src/fp_multiplier.o \
./src/fpu.o \
./src/grf.o \
./src/imc_core.o \
./src/imc_pch.o \
./src/instr_decoder.o \
./src/interface_unit.o \
./src/pc_unit.o \
./src/srf.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/opt/systemc/include -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/control_unit.d ./src/control_unit.o ./src/crf.d ./src/crf.o ./src/fp_adder.d ./src/fp_adder.o ./src/fp_multiplier.d ./src/fp_multiplier.o ./src/fpu.d ./src/fpu.o ./src/grf.d ./src/grf.o ./src/imc_core.d ./src/imc_core.o ./src/imc_pch.d ./src/imc_pch.o ./src/instr_decoder.d ./src/instr_decoder.o ./src/interface_unit.d ./src/interface_unit.o ./src/pc_unit.d ./src/pc_unit.o ./src/srf.d ./src/srf.o

.PHONY: clean-src

