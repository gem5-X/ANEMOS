#!/usr/bin/tcsh

# HBM 32 banks
cd MGC_QSIM
vsim -c -do OUT/DO/sim_ewarwV256n256_hbm32bank.do
sed -i "s/ewarwC32R8S16HBM32bankV256n256/dpC32R8S16HBM32bankV256n256/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_dpV256n256_hbm32bank.do
sed -i "s/dpC32R8S16HBM32bankV256n256/mvmC32R8S16HBM32bank64x64/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm64x64_hbm32bank.do
sed -i "s/mvmC32R8S16HBM32bank64x64/mvmC32R8S16HBM32bank128x128/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm128x128_hbm32bank.do
sed -i "s/mvmC32R8S16HBM32bank128x128/mvmC32R8S16HBM32bank1024x1024/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_hbm32bank.do
sed -i "s/mvmC32R8S16HBM32bank1024x1024/mmsC32R8S16HBM32bankm256n256q256/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mmsm256n256q256_hbm32bank.do
sed -i "s/mmsC32R8S16HBM32bankm256n256q256/ccwwrC32R8S16HBM32banki24x24x32o20x20x64k5/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri24x24x32o20x20x64k5_hbm32bank.do

cd ../CDS_GENUS
genus -files TCL/power_C32R8S16_HBM.tcl
rm ${VCD_FILES}/*.vcd.gz


# Go back to original
cd ../MGC_QSIM
sed -i "s/ccwwrC32R8S4GDDR5i24x24x32o20x20x64k5/ewarwC32R8S16HBM32bankV256n256/g" C_FILES/pch_main.h
sed -i "s/#define CLK_PERIOD 1000/#define CLK_PERIOD 3333/g" C_FILES/defs.h
sed -i "s/#define SIMD_WIDTH		(64/#define SIMD_WIDTH		(256/g" C_FILES/defs.h
sed -i "s/#define COL_BITS        7/#define COL_BITS        5/g" C_FILES/defs.h
sed -i "s/#define ROW_BITS        14/#define ROW_BITS        14/g" C_FILES/defs.h
cd ..