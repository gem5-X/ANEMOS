project open PROJ/PROJ.mpf

sccom -work work -stats=none ${EDA_PROJECT_ROOT}/C_FILES/pch_main.cpp
sccom -link
vsim -L ${EDA_PROJECT_ROOT}/LIBS/TSMC28HPC_CORE_VLOG -sdftyp top/IMCcoreUnderTest_0=${EDA_PROJECT_ROOT}/CDS_GENUS/TIM/imc_C32R8S4_DDR4.sdf -sdftyp top/IMCCoreUnderTest_1=${EDA_PROJECT_ROOT}/CDS_GENUS/TIM/imc_C32R8S4_DDR4.sdf -sdftyp top/IMCcoreUnderTest_2=${EDA_PROJECT_ROOT}/CDS_GENUS/TIM/imc_C32R8S4_DDR4.sdf -sdftyp top/IMCcoreUnderTest_3=${EDA_PROJECT_ROOT}/CDS_GENUS/TIM/imc_C32R8S4_DDR4.sdf -sdftyp top/IMCcoreUnderTest_4=${EDA_PROJECT_ROOT}/CDS_GENUS/TIM/imc_C32R8S4_DDR4.sdf -sdftyp top/IMCcoreUnderTest_5=${EDA_PROJECT_ROOT}/CDS_GENUS/TIM/imc_C32R8S4_DDR4.sdf -sdftyp top/IMCcoreUnderTest_6=${EDA_PROJECT_ROOT}/CDS_GENUS/TIM/imc_C32R8S4_DDR4.sdf -sdftyp top/IMCcoreUnderTest_7=${EDA_PROJECT_ROOT}/CDS_GENUS/TIM/imc_C32R8S4_DDR4.sdf work.top -t ps -sdfnoerror
vcd file ${VCD_FILES}/imc_C32R8S4_dpV256n256_DDR4.vcd.gz
vcd add -r top/IMCcoreUnderTest_0/*
vcd add -r top/IMCcoreUnderTest_1/*
vcd add -r top/IMCcoreUnderTest_2/*
vcd add -r top/IMCcoreUnderTest_3/*
vcd add -r top/IMCcoreUnderTest_4/*
vcd add -r top/IMCcoreUnderTest_5/*
vcd add -r top/IMCcoreUnderTest_6/*
vcd add -r top/IMCcoreUnderTest_7/*
run -all