project open PROJ/PROJ.mpf

# Generate wrapper for HBM
vlog -work work -stats=none /scrap/users/rmedina/pim_cores/HDL/GATE/imc_C32R8S16_HBM_generic.v
scgenmod imc_core > ../C_FILES/imc_wrapped_S16.h

# Compile for HBM
project compileall
vlog -work work -vopt -stats=none +acc=np /scrap/users/rmedina/pim_cores/HDL/GATE/imc_C32R8S16_HBM_generic.v

# Simulate EWARW
sccom -link
vsim -L /scrap/users/rmedina/pim_cores/LIBS/TSMC28HPC_CORE_VLOG -sdftyp top/IMCcoreUnderTest_0=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_HBM.sdf -sdftyp top/IMCCoreUnderTest_1=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_HBM.sdf -sdftyp top/IMCcoreUnderTest_2=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_HBM.sdf -sdftyp top/IMCcoreUnderTest_3=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_HBM.sdf -sdftyp top/IMCcoreUnderTest_4=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_HBM.sdf -sdftyp top/IMCcoreUnderTest_5=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_HBM.sdf -sdftyp top/IMCcoreUnderTest_6=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_HBM.sdf -sdftyp top/IMCcoreUnderTest_7=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_HBM.sdf work.top -t ps -sdfnoerror
vcd file /home/rmedina/VCD/imc_C32R8S16_ewarwV256n256_HBM.vcd.gz
vcd add -r top/IMCcoreUnderTest_0/*
vcd add -r top/IMCcoreUnderTest_1/*
vcd add -r top/IMCcoreUnderTest_2/*
vcd add -r top/IMCcoreUnderTest_3/*
vcd add -r top/IMCcoreUnderTest_4/*
vcd add -r top/IMCcoreUnderTest_5/*
vcd add -r top/IMCcoreUnderTest_6/*
vcd add -r top/IMCcoreUnderTest_7/*
run -all