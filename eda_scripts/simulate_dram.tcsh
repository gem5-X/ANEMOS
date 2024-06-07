#!/usr/bin/tcsh

# HBM
cd MGC_QSIM
vsim -c -do OUT/DO/sim_ewarwV256n256_hbm.do
sed -i "s/ewarwC32R8S16HBMV256n256/dpC32R8S16HBMV256n256/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_dpV256n256_hbm.do
sed -i "s/dpC32R8S16HBMV256n256/mvmC32R8S16HBM32x32/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm32x32_hbm.do
sed -i "s/mvmC32R8S16HBM32x32/mvmC32R8S16HBM64x64/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm64x64_hbm.do
sed -i "s/mvmC32R8S16HBM64x64/mvmC32R8S16HBM128x128/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm128x128_hbm.do
sed -i "s/mvmC32R8S16HBM128x128/mvmC32R8S16HBM256x256/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm256x256_hbm.do
sed -i "s/mvmC32R8S16HBM256x256/mvmC32R8S16HBM512x512/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm512x512_hbm.do
sed -i "s/mvmC32R8S16HBM512x512/mvmC32R8S16HBM1024x1024/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_hbm.do
sed -i "s/mvmC32R8S16HBM1024x1024/mmsC32R8S16HBMm128n128q128/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mmsm128n128q128_hbm.do
sed -i "s/mmsC32R8S16HBMm128n128q128/ccwwrC32R8S16HBMi24x24x32o20x20x32k5/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri24x24x32o20x20x32k5_hbm.do

cd ../CDS_GENUS
genus -files TCL/power_C32R8S16_HBM.tcl
rm ${VCD_FILES}/*.vcd.gz

# DDR4
cd ../MGC_QSIM
sed -i "s/#define CLK_PERIOD 3333/#define CLK_PERIOD 2500/g" C_FILES/defs.h
sed -i "s/#define SIMD_WIDTH		(256/#define SIMD_WIDTH		(64/g" C_FILES/defs.h
sed -i "s/#define ROW_BITS        14/#define ROW_BITS        15/g" C_FILES/defs.h
sed -i "s/#define COL_BITS        5/#define COL_BITS        7/g" C_FILES/defs.h
sed -i "s/ccwwrC32R8S16HBMi24x24x32o20x20x32k5/ewarwC32R8S4DDR4V256n256/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_ewarwV256n256_ddr4.do
sed -i "s/ewarwC32R8S4DDR4V256n256/dpC32R8S4DDR4V256n256/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_dpV256n256_ddr4.do
sed -i "s/dpC32R8S4DDR4V256n256/mvmC32R8S4DDR432x32/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm32x32_ddr4.do
sed -i "s/mvmC32R8S4DDR432x32/mvmC32R8S4DDR464x64/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm64x64_ddr4.do
sed -i "s/mvmC32R8S4DDR464x64/mvmC32R8S4DDR4128x128/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm128x128_ddr4.do
sed -i "s/mvmC32R8S4DDR4128x128/mvmC32R8S4DDR4256x256/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm256x256_ddr4.do
sed -i "s/mvmC32R8S4DDR4256x256/mvmC32R8S4DDR4512x512/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm512x512_ddr4.do
sed -i "s/mvmC32R8S4DDR4512x512/mvmC32R8S4DDR41024x1024/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_ddr4.do
sed -i "s/mvmC32R8S4DDR41024x1024/mmsC32R8S4DDR4m128n128q128/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mmsm128n128q128_ddr4.do
sed -i "s/mmsC32R8S4DDR4m128n128q128/ccwwrC32R8S4DDR4i24x24x32o20x20x32k5/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri24x24x32o20x20x32k5_ddr4.do

cd ../CDS_GENUS
genus -files TCL/power_C32R8S4_DDR4.tcl
rm ${VCD_FILES}/*.vcd.gz

# GDDR5
cd ../MGC_QSIM
sed -i "s/#define CLK_PERIOD 2500/#define CLK_PERIOD 1000/g" C_FILES/defs.h
sed -i "s/#define SIMD_WIDTH		(64/#define SIMD_WIDTH		(256/g" C_FILES/defs.h
sed -i "s/#define ROW_BITS        15/#define ROW_BITS        14/g" C_FILES/defs.h
sed -i "s/#define COL_BITS        7/#define COL_BITS        7/g" C_FILES/defs.h
sed -i "s/#define DQ_BITS         64/#define DQ_BITS         32/g" C_FILES/defs.h
sed -i "s/ccwwrC32R8S4DDR4i24x24x32o20x20x32k5/ewarwC32R8S16GDDR5V256n256/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_ewarwV256n256_gddr5.do
sed -i "s/ewarwC32R8S16GDDR5V256n256/dpC32R8S16GDDR5V256n256/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_dpV256n256_gddr5.do
sed -i "s/dpC32R8S16GDDR5V256n256/mvmC32R8S16GDDR532x32/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm32x32_gddr5.do
sed -i "s/mvmC32R8S16GDDR532x32/mvmC32R8S16GDDR564x64/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm64x64_gddr5.do
sed -i "s/mvmC32R8S16GDDR564x64/mvmC32R8S16GDDR5128x128/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm128x128_gddr5.do
sed -i "s/mvmC32R8S16GDDR5128x128/mvmC32R8S16GDDR5256x256/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm256x256_gddr5.do
sed -i "s/mvmC32R8S16GDDR5256x256/mvmC32R8S16GDDR5512x512/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm512x512_gddr5.do
sed -i "s/mvmC32R8S16GDDR5512x512/mvmC32R8S16GDDR51024x1024/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_gddr5.do
sed -i "s/mvmC32R8S16GDDR51024x1024/mmsC32R8S16GDDR5m128n128q128/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mmsm128n128q128_gddr5.do
sed -i "s/mmsC32R8S16GDDR5m128n128q128/ccwwrC32R8S16GDDR5i24x24x32o20x20x32k5/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri24x24x32o20x20x32k5_gddr5.do

cd ../CDS_GENUS
genus -files TCL/power_C32R8S16_GDDR5.tcl
rm ${VCD_FILES}/*.vcd.gz

# LPDDR4
cd ../MGC_QSIM
sed -i "s/#define CLK_PERIOD 1000/#define CLK_PERIOD 5000/g" C_FILES/defs.h
sed -i "s/#define CORES_PER_PCH   8/#define CORES_PER_PCH   4/g" C_FILES/defs.h
sed -i "s/#define SIMD_WIDTH		(256/#define SIMD_WIDTH		(256/g" C_FILES/defs.h
sed -i "s/#define BG_BITS         2/#define BG_BITS         0/g" C_FILES/defs.h
sed -i "s/#define BANK_BITS       2/#define BANK_BITS       3/g" C_FILES/defs.h
sed -i "s/#define ROW_BITS        14/#define ROW_BITS        15/g" C_FILES/defs.h
sed -i "s/#define COL_BITS        7/#define COL_BITS        6/g" C_FILES/defs.h
sed -i "s/#define DQ_BITS         32/#define DQ_BITS         16/g" C_FILES/defs.h
sed -i "s/ccwwrC32R8S16GDDR5i24x24x32o20x20x32k5/ewarwC32R8S16LPDDR4V256n256/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_ewarwV256n256_lpddr4.do
sed -i "s/ewarwC32R8S16LPDDR4V256n256/dpC32R8S16LPDDR4V256n256/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_dpV256n256_lpddr4.do
sed -i "s/dpC32R8S16LPDDR4V256n256/mvmC32R8S16LPDDR432x32/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm32x32_lpddr4.do
sed -i "s/mvmC32R8S16LPDDR432x32/mvmC32R8S16LPDDR464x64/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm64x64_lpddr4.do
sed -i "s/mvmC32R8S16LPDDR464x64/mvmC32R8S16LPDDR4128x128/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm128x128_lpddr4.do
sed -i "s/mvmC32R8S16LPDDR4128x128/mvmC32R8S16LPDDR4256x256/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm256x256_lpddr4.do
sed -i "s/mvmC32R8S16LPDDR4256x256/mvmC32R8S16LPDDR4512x512/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm512x512_lpddr4.do
sed -i "s/mvmC32R8S16LPDDR4512x512/mvmC32R8S16LPDDR41024x1024/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_lpddr4.do
sed -i "s/mvmC32R8S16LPDDR41024x1024/mmsC32R8S16LPDDR4m128n128q128/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_mmsm128n128q128_lpddr4.do
sed -i "s/mmsC32R8S16LPDDR4m128n128q128/ccwwrC32R8S16LPDDR4i24x24x32o20x20x32k5/g" C_FILES/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri24x24x32o20x20x32k5_lpddr4.do

cd ../CDS_GENUS
genus -files TCL/power_C32R8S16_LPDDR4.tcl
rm ${VCD_FILES}/*.vcd.gz

# Go back to original
cd ../MGC_QSIM
sed -i "s/ccwwrC32R8S16LPDDR4i24x24x32o20x20x32k5/ewarwC32R8S16HBMV256n256/g" C_FILES/pch_main.h
sed -i "s/#define CLK_PERIOD 5000/#define CLK_PERIOD 3333/g" C_FILES/defs.h
sed -i "s/#define CORES_PER_PCH   4/#define CORES_PER_PCH   8/g" C_FILES/defs.h
sed -i "s/#define SIMD_WIDTH		(256/#define SIMD_WIDTH		(256/g" C_FILES/defs.h
sed -i "s/#define BG_BITS         0/#define BG_BITS         2/g" C_FILES/defs.h
sed -i "s/#define BANK_BITS       3/#define BANK_BITS       2/g" C_FILES/defs.h
sed -i "s/#define COL_BITS        6/#define COL_BITS        5/g" C_FILES/defs.h
sed -i "s/#define ROW_BITS        15/#define ROW_BITS        14/g" C_FILES/defs.h
sed -i "s/#define DQ_BITS         16/#define DQ_BITS         64/g" C_FILES/defs.h
cd ..