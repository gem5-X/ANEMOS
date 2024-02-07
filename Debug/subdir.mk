################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../softsimd_dlb_mov_translate.c \
../softsimd_dlb_pm_translate.c \
../softsimd_dlb_sa_translate.c \
../softsimd_slb_instr_translate.c 

C_DEPS += \
./softsimd_dlb_mov_translate.d \
./softsimd_dlb_pm_translate.d \
./softsimd_dlb_sa_translate.d \
./softsimd_slb_instr_translate.d 

OBJS += \
./softsimd_dlb_mov_translate.o \
./softsimd_dlb_pm_translate.o \
./softsimd_dlb_sa_translate.o \
./softsimd_slb_instr_translate.o 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean--2e-

clean--2e-:
	-$(RM) ./softsimd_dlb_mov_translate.d ./softsimd_dlb_mov_translate.o ./softsimd_dlb_pm_translate.d ./softsimd_dlb_pm_translate.o ./softsimd_dlb_sa_translate.d ./softsimd_dlb_sa_translate.o ./softsimd_slb_instr_translate.d ./softsimd_slb_instr_translate.o

.PHONY: clean--2e-

