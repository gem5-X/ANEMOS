project open PROJ_CR/HBM_CR.mpf

sccom -work work -stats=none ${EDA_PROJECT_ROOT}/MGC_QSIM/PROJ_CR/pch_main.cpp
sccom -link
vsim -L ${EDA_PROJECT_ROOT}/LIBS/TSMC28HPC_CORE_VLOG -sdftyp top/IMCcoreUnderTest_0=${EDA_PROJECT_ROOT}/CDS_GENUS/TIM/imc_C64R4S16_1pch.sdf work.top -t ps -sdfnoerror
vcd file ${VCD_FILES}/hbmCR/imc_C64R4S16_dpV128n128_HBM.vcd.gz
vcd add -r top/IMCcoreUnderTest_0/*
run -all