################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/fsp/src/r_dac/r_dac.c 

C_DEPS += \
./ra/fsp/src/r_dac/r_dac.d 

CREF += \
llc_dcx.cref 

OBJS += \
./ra/fsp/src/r_dac/r_dac.o 

MAP += \
llc_dcx.map 


# Each subdirectory must supply rules for building sources it contributes
ra/fsp/src/r_dac/%.o: ../ra/fsp/src/r_dac/%.c
	@echo 'Building file: $<'
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mlittle-endian -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -ffunction-sections -fdata-sections -fno-strict-aliasing -fmessage-length=0 -funsigned-char -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Waggregate-return -Wno-parentheses-equality -Wfloat-equal -g3 -std=c99 -fshort-enums -fno-unroll-loops -I"C:\\Users\\navee\\e2_studio\\workspace\\llc_dcx\\ra_cfg\\fsp_cfg\\bsp" -I"." -I"C:\\Users\\navee\\e2_studio\\workspace\\llc_dcx\\ra_gen" -I"C:\\Users\\navee\\e2_studio\\workspace\\llc_dcx\\ra_cfg\\fsp_cfg" -I"C:\\Users\\navee\\e2_studio\\workspace\\llc_dcx\\src" -I"C:\\Users\\navee\\e2_studio\\workspace\\llc_dcx\\ra\\fsp\\inc" -I"C:\\Users\\navee\\e2_studio\\workspace\\llc_dcx\\ra\\fsp\\inc\\api" -I"C:\\Users\\navee\\e2_studio\\workspace\\llc_dcx\\ra\\fsp\\inc\\instances" -I"C:\\Users\\navee\\e2_studio\\workspace\\llc_dcx\\ra\\arm\\CMSIS_6\\CMSIS\\Core\\Include" -D_RENESAS_RA_ -D_RA_CORE=CM33 -D_RA_ORDINAL=1 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -x c "$<" -c -o "$@")
	@clang --target=arm-none-eabi @"$@.in"

