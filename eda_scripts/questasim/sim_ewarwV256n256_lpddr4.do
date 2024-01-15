project open PROJ/PROJ.mpf

# Generate wrapper for LPDDR4
vlog -work work -stats=none /scrap/users/rmedina/pim_cores/HDL/GATE/imc_C32R8S16_LPDDR4_generic.v
scgenmod imc_core > ../C_FILES/imc_wrapped_S16.h

# Compile for LPDDR4
project compileall
vlog -work work -vopt -stats=none +acc=np /scrap/users/rmedina/pim_cores/HDL/GATE/imc_C32R8S16_LPDDR4_generic.v

# Simulate EWARW
sccom -work work -stats=none /scrap/users/rmedina/pim_cores/C_FILES/pch_main.cpp
sccom -link
vsim -L /scrap/users/rmedina/pim_cores/LIBS/TSMC28HPC_CORE_VLOG -sdftyp top/IMCcoreUnderTest_0=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_LPDDR4.sdf -sdftyp top/IMCCoreUnderTest_1=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_LPDDR4.sdf -sdftyp top/IMCcoreUnderTest_2=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_LPDDR4.sdf -sdftyp top/IMCcoreUnderTest_3=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_LPDDR4.sdf work.top -t ps -sdfnoerror
vcd file /home/rmedina/VCD/imc_C32R8S16_ewarwV256n256_LPDDR4.vcd.gz
vcd add -r top/IMCcoreUnderTest_0/*
vcd add -r top/IMCcoreUnderTest_1/*
vcd add -r top/IMCcoreUnderTest_2/*
vcd add -r top/IMCcoreUnderTest_3/*
run -all