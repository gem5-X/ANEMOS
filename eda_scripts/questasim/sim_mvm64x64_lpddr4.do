project open PROJ/PROJ.mpf

sccom -work work -stats=none ${EDA_PROJECT_ROOT}/C_FILES/pch_main.cpp
sccom -link
vsim -L ${EDA_PROJECT_ROOT}/LIBS/TSMC28HPC_CORE_VLOG -sdftyp top/IMCcoreUnderTest_0=${EDA_PROJECT_ROOT}/CDS_GENUS/TIM/imc_C32R8S16_LPDDR4.sdf -sdftyp top/IMCCoreUnderTest_1=${EDA_PROJECT_ROOT}/CDS_GENUS/TIM/imc_C32R8S16_LPDDR4.sdf -sdftyp top/IMCcoreUnderTest_2=${EDA_PROJECT_ROOT}/CDS_GENUS/TIM/imc_C32R8S16_LPDDR4.sdf -sdftyp top/IMCcoreUnderTest_3=${EDA_PROJECT_ROOT}/CDS_GENUS/TIM/imc_C32R8S16_LPDDR4.sdf work.top -t ps -sdfnoerror
vcd file ${VCD_FILES}/imc_C32R8S16_mvm64x64_LPDDR4.vcd.gz
vcd add -r top/IMCcoreUnderTest_0/*
vcd add -r top/IMCcoreUnderTest_1/*
vcd add -r top/IMCcoreUnderTest_2/*
vcd add -r top/IMCcoreUnderTest_3/*
run -all