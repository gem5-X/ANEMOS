project open PROJ_CR/HBM_CR.mpf

# Generate wrapper
vlog -work work -stats=none /scrap/users/rmedina/pim_cores/HDL/GATE/imc_C16R32S16_1pch_generic.v
scgenmod imc_core > imc_wrapped_S16.h

# Compile for C16R32
project compileall
vlog -work work -vopt -stats=none +acc=np /scrap/users/rmedina/pim_cores/HDL/GATE/imc_C16R32S16_1pch_generic.v

# Simulate EWARW
sccom -link
vsim -L /scrap/users/rmedina/pim_cores/LIBS/TSMC28HPC_CORE_VLOG -sdftyp top/IMCcoreUnderTest_0=/scrap/users/rmedina/pim_cores/CDS_GENUS/TIM/imc_C16R32S16_1pch.sdf work.top -t ps -sdfnoerror
vcd file /home/rmedina/VCD/hbmCR/imc_C16R32S16_mvm180x180_HBM.vcd.gz
vcd add -r top/IMCcoreUnderTest_0/*
run -all