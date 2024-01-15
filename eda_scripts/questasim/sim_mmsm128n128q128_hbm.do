project open PROJ/PROJ.mpf

sccom -work work -stats=none /scrap/users/rmedina/pim_cores/C_FILES/pch_main.cpp
sccom -link
vsim -L /scrap/users/rmedina/pim_cores/LIBS/TSMC28HPC_CORE_VLOG -sdftyp top/IMCcoreUnderTest_0=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_HBM.sdf -sdftyp top/IMCCoreUnderTest_1=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_HBM.sdf -sdftyp top/IMCcoreUnderTest_2=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_HBM.sdf -sdftyp top/IMCcoreUnderTest_3=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_HBM.sdf -sdftyp top/IMCcoreUnderTest_4=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_HBM.sdf -sdftyp top/IMCcoreUnderTest_5=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_HBM.sdf -sdftyp top/IMCcoreUnderTest_6=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_HBM.sdf -sdftyp top/IMCcoreUnderTest_7=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C32R8S16_HBM.sdf work.top -t ps -sdfnoerror
vcd file /home/rmedina/VCD/imc_C32R8S16_mmsm128n128q128_HBM.vcd.gz
vcd add -r top/IMCcoreUnderTest_0/*
vcd add -r top/IMCcoreUnderTest_1/*
vcd add -r top/IMCcoreUnderTest_2/*
vcd add -r top/IMCcoreUnderTest_3/*
vcd add -r top/IMCcoreUnderTest_4/*
vcd add -r top/IMCcoreUnderTest_5/*
vcd add -r top/IMCcoreUnderTest_6/*
vcd add -r top/IMCcoreUnderTest_7/*
run -all
