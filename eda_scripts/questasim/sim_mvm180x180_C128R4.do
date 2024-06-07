project open PROJ_CR/HBM_CR.mpf

# Generate wrapper
vlog -work work -stats=none ${EDA_PROJECT_ROOT}/HDL/GATE/imc_C128R4S16_1pch_generic.v
scgenmod imc_core > imc_wrapped_S16.h

# Compile for C128R4
project compileall
vlog -work work -vopt -stats=none +acc=np ${EDA_PROJECT_ROOT}/HDL/GATE/imc_C128R4S16_1pch_generic.v

# Simulate EWARW
sccom -link
vsim -L ${EDA_PROJECT_ROOT}/LIBS/TSMC28HPC_CORE_VLOG -sdftyp top/IMCcoreUnderTest_0=${EDA_PROJECT_ROOT}/CDS_GENUS/TIM/imc_C128R4S16_1pch.sdf work.top -t ps -sdfnoerror
vcd file ${VCD_FILES}/hbmCR/imc_C128R4S16_mvm180x180_HBM.vcd.gz
vcd add -r top/IMCcoreUnderTest_0/*
run -all