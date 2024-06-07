#!/usr/bin/tcsh

# C16R4
cd MGC_QSIM
vsim -c -do OUT/DO/sim_ewarwV128n128_C16R4.do
sed -i "s/ewarwC16R4S16HBM1pchV128n128/dpC16R4S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_dpV128n128_C16R4.do
sed -i "s/dpC16R4S16HBM1pchV128n128/mvmC16R4S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C16R4.do
sed -i "s/mvmC16R4S16HBM1pch180x180/mvmC16R4S16HBM1pch1024x1024/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_C16R4.do
sed -i "s/mvmC16R4S16HBM1pch1024x1024/mmsC16R4S16HBM1pchm60n60q60/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mmsm60n60q60_C16R4.do
sed -i "s/mmsC16R4S16HBM1pchm60n60q60/ccwwrC16R4S16HBM1pchi11x11x34o9x9x16k3/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri11x11x34o9x9x16k3_C16R4.do

cd ../CDS_GENUS
genus -files TCL/power_C16R4S16_1pch.tcl
rm ${VCD_FILES}/hbmCR/*.vcd.gz

# C16R8
cd ../MGC_QSIM
sed -i "s/#define CRF_ENTRIES     16/#define CRF_ENTRIES     16/g" PROJ_CR/defs.h
sed -i "s/#define SRF_A_ENTRIES   4/#define SRF_A_ENTRIES   8/g" PROJ_CR/defs.h
sed -i "s/#define SRF_M_ENTRIES   4/#define SRF_M_ENTRIES   8/g" PROJ_CR/defs.h
sed -i "s/#define GRF_ENTRIES     4/#define GRF_ENTRIES     8/g" PROJ_CR/defs.h
sed -i "s/#define AAM_ADDR_BITS   2/#define AAM_ADDR_BITS   3/g" PROJ_CR/defs.h
sed -i "s/ccwwrC16R4S16HBM1pchi11x11x34o9x9x16k3/ewarwC16R8S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ewarwV128n128_C16R8.do
sed -i "s/ewarwC16R8S16HBM1pchV128n128/dpC16R8S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_dpV128n128_C16R8.do
sed -i "s/dpC16R8S16HBM1pchV128n128/mvmC16R8S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C16R8.do
sed -i "s/mvmC16R8S16HBM1pch180x180/mvmC16R8S16HBM1pch1024x1024/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_C16R8.do
sed -i "s/mvmC16R8S16HBM1pch1024x1024/mmsC16R8S16HBM1pchm60n60q60/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mmsm60n60q60_C16R8.do
sed -i "s/mmsC16R8S16HBM1pchm60n60q60/ccwwrC16R8S16HBM1pchi11x11x34o9x9x16k3/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri11x11x34o9x9x16k3_C16R8.do

cd ../CDS_GENUS
genus -files TCL/power_C16R8S16_1pch.tcl
rm ${VCD_FILES}/hbmCR/*.vcd.gz

# C16R16
cd ../MGC_QSIM
sed -i "s/#define CRF_ENTRIES     16/#define CRF_ENTRIES     16/g" PROJ_CR/defs.h
sed -i "s/#define SRF_A_ENTRIES   8/#define SRF_A_ENTRIES   16/g" PROJ_CR/defs.h
sed -i "s/#define SRF_M_ENTRIES   8/#define SRF_M_ENTRIES   16/g" PROJ_CR/defs.h
sed -i "s/#define GRF_ENTRIES     8/#define GRF_ENTRIES     16/g" PROJ_CR/defs.h
sed -i "s/#define AAM_ADDR_BITS   3/#define AAM_ADDR_BITS   4/g" PROJ_CR/defs.h
sed -i "s/ccwwrC16R8S16HBM1pchi11x11x34o9x9x16k3/ewarwC16R16S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ewarwV128n128_C16R16.do
sed -i "s/ewarwC16R16S16HBM1pchV128n128/dpC16R16S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_dpV128n128_C16R16.do
sed -i "s/dpC16R16S16HBM1pchV128n128/mvmC16R16S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C16R16.do
sed -i "s/mvmC16R16S16HBM1pch180x180/mvmC16R16S16HBM1pch1024x1024/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_C16R16.do
sed -i "s/mvmC16R16S16HBM1pch1024x1024/mmsC16R16S16HBM1pchm60n60q60/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mmsm60n60q60_C16R16.do
sed -i "s/mmsC16R16S16HBM1pchm60n60q60/ccwwrC16R16S16HBM1pchi11x11x34o9x9x16k3/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri11x11x34o9x9x16k3_C16R16.do

cd ../CDS_GENUS
genus -files TCL/power_C16R16S16_1pch.tcl
rm ${VCD_FILES}/hbmCR/*.vcd.gz

# C16R32
cd ../MGC_QSIM
sed -i "s/#define CRF_ENTRIES     16/#define CRF_ENTRIES     16/g" PROJ_CR/defs.h
sed -i "s/#define SRF_A_ENTRIES   16/#define SRF_A_ENTRIES   32/g" PROJ_CR/defs.h
sed -i "s/#define SRF_M_ENTRIES   16/#define SRF_M_ENTRIES   32/g" PROJ_CR/defs.h
sed -i "s/#define GRF_ENTRIES     16/#define GRF_ENTRIES     32/g" PROJ_CR/defs.h
sed -i "s/#define AAM_ADDR_BITS   4/#define AAM_ADDR_BITS   5/g" PROJ_CR/defs.h
sed -i "s/ccwwrC16R16S16HBM1pchi11x11x34o9x9x16k3/ewarwC16R32S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ewarwV128n128_C16R32.do
sed -i "s/ewarwC16R32S16HBM1pchV128n128/dpC16R32S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_dpV128n128_C16R32.do
sed -i "s/dpC16R32S16HBM1pchV128n128/mvmC16R32S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C16R32.do
sed -i "s/mvmC16R32S16HBM1pch180x180/mvmC16R32S16HBM1pch1024x1024/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_C16R32.do
sed -i "s/mvmC16R32S16HBM1pch1024x1024/mmsC16R32S16HBM1pchm60n60q60/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mmsm60n60q60_C16R32.do
sed -i "s/mmsC16R32S16HBM1pchm60n60q60/ccwwrC16R32S16HBM1pchi11x11x34o9x9x16k3/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri11x11x34o9x9x16k3_C16R32.do

cd ../CDS_GENUS
genus -files TCL/power_C16R32S16_1pch.tcl
rm ${VCD_FILES}/hbmCR/*.vcd.gz

# C32R4
cd ../MGC_QSIM
sed -i "s/#define CRF_ENTRIES     16/#define CRF_ENTRIES     32/g" PROJ_CR/defs.h
sed -i "s/#define SRF_A_ENTRIES   32/#define SRF_A_ENTRIES   4/g" PROJ_CR/defs.h
sed -i "s/#define SRF_M_ENTRIES   32/#define SRF_M_ENTRIES   4/g" PROJ_CR/defs.h
sed -i "s/#define GRF_ENTRIES     32/#define GRF_ENTRIES     4/g" PROJ_CR/defs.h
sed -i "s/#define AAM_ADDR_BITS   5/#define AAM_ADDR_BITS   2/g" PROJ_CR/defs.h
sed -i "s/ccwwrC16R32S16HBM1pchi11x11x34o9x9x16k3/ewarwC32R4S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ewarwV128n128_C32R4.do
sed -i "s/ewarwC32R4S16HBM1pchV128n128/dpC32R4S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_dpV128n128_C32R4.do
sed -i "s/dpC32R4S16HBM1pchV128n128/mvmC32R4S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C32R4.do
sed -i "s/mvmC32R4S16HBM1pch180x180/mvmC32R4S16HBM1pch1024x1024/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_C32R4.do
sed -i "s/mvmC32R4S16HBM1pch1024x1024/mmsC32R4S16HBM1pchm60n60q60/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mmsm60n60q60_C32R4.do
sed -i "s/mmsC32R4S16HBM1pchm60n60q60/ccwwrC32R4S16HBM1pchi11x11x34o9x9x16k3/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri11x11x34o9x9x16k3_C32R4.do

cd ../CDS_GENUS
genus -files TCL/power_C32R4S16_1pch.tcl
rm ${VCD_FILES}/hbmCR/*.vcd.gz

# C32R8
cd ../MGC_QSIM
sed -i "s/#define CRF_ENTRIES     32/#define CRF_ENTRIES     32/g" PROJ_CR/defs.h
sed -i "s/#define SRF_A_ENTRIES   4/#define SRF_A_ENTRIES   8/g" PROJ_CR/defs.h
sed -i "s/#define SRF_M_ENTRIES   4/#define SRF_M_ENTRIES   8/g" PROJ_CR/defs.h
sed -i "s/#define GRF_ENTRIES     4/#define GRF_ENTRIES     8/g" PROJ_CR/defs.h
sed -i "s/#define AAM_ADDR_BITS   2/#define AAM_ADDR_BITS   3/g" PROJ_CR/defs.h
sed -i "s/ccwwrC32R4S16HBM1pchi11x11x34o9x9x16k3/ewarwC32R8S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ewarwV128n128_C32R8.do
sed -i "s/ewarwC32R8S16HBM1pchV128n128/dpC32R8S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_dpV128n128_C32R8.do
sed -i "s/dpC32R8S16HBM1pchV128n128/mvmC32R8S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C32R8.do
sed -i "s/mvmC32R8S16HBM1pch180x180/mvmC32R8S16HBM1pch1024x1024/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_C32R8.do
sed -i "s/mvmC32R8S16HBM1pch1024x1024/mmsC32R8S16HBM1pchm60n60q60/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mmsm60n60q60_C32R8.do
sed -i "s/mmsC32R8S16HBM1pchm60n60q60/ccwwrC32R8S16HBM1pchi11x11x34o9x9x16k3/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri11x11x34o9x9x16k3_C32R8.do

cd ../CDS_GENUS
genus -files TCL/power_C32R8S16_1pch.tcl
rm ${VCD_FILES}/hbmCR/*.vcd.gz

# C32R16
cd ../MGC_QSIM
sed -i "s/#define CRF_ENTRIES     32/#define CRF_ENTRIES     32/g" PROJ_CR/defs.h
sed -i "s/#define SRF_A_ENTRIES   8/#define SRF_A_ENTRIES   16/g" PROJ_CR/defs.h
sed -i "s/#define SRF_M_ENTRIES   8/#define SRF_M_ENTRIES   16/g" PROJ_CR/defs.h
sed -i "s/#define GRF_ENTRIES     8/#define GRF_ENTRIES     16/g" PROJ_CR/defs.h
sed -i "s/#define AAM_ADDR_BITS   3/#define AAM_ADDR_BITS   4/g" PROJ_CR/defs.h
sed -i "s/ccwwrC32R8S16HBM1pchi11x11x34o9x9x16k3/ewarwC32R16S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ewarwV128n128_C32R16.do
sed -i "s/ewarwC32R16S16HBM1pchV128n128/dpC32R16S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_dpV128n128_C32R16.do
sed -i "s/dpC32R16S16HBM1pchV128n128/mvmC32R16S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C32R16.do
sed -i "s/mvmC32R16S16HBM1pch180x180/mvmC32R16S16HBM1pch1024x1024/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_C32R16.do
sed -i "s/mvmC32R16S16HBM1pch1024x1024/mmsC32R16S16HBM1pchm60n60q60/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mmsm60n60q60_C32R16.do
sed -i "s/mmsC32R16S16HBM1pchm60n60q60/ccwwrC32R16S16HBM1pchi11x11x34o9x9x16k3/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri11x11x34o9x9x16k3_C32R16.do

cd ../CDS_GENUS
genus -files TCL/power_C32R16S16_1pch.tcl
rm ${VCD_FILES}/hbmCR/*.vcd.gz

# C32R32
cd ../MGC_QSIM
sed -i "s/#define CRF_ENTRIES     32/#define CRF_ENTRIES     32/g" PROJ_CR/defs.h
sed -i "s/#define SRF_A_ENTRIES   16/#define SRF_A_ENTRIES   32/g" PROJ_CR/defs.h
sed -i "s/#define SRF_M_ENTRIES   16/#define SRF_M_ENTRIES   32/g" PROJ_CR/defs.h
sed -i "s/#define GRF_ENTRIES     16/#define GRF_ENTRIES     32/g" PROJ_CR/defs.h
sed -i "s/#define AAM_ADDR_BITS   4/#define AAM_ADDR_BITS   5/g" PROJ_CR/defs.h
sed -i "s/ccwwrC32R16S16HBM1pchi11x11x34o9x9x16k3/ewarwC32R32S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ewarwV128n128_C32R32.do
sed -i "s/ewarwC32R32S16HBM1pchV128n128/dpC32R32S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_dpV128n128_C32R32.do
sed -i "s/dpC32R32S16HBM1pchV128n128/mvmC32R32S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C32R32.do
sed -i "s/mvmC32R32S16HBM1pch180x180/mvmC32R32S16HBM1pch1024x1024/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_C32R32.do
sed -i "s/mvmC32R32S16HBM1pch1024x1024/mmsC32R32S16HBM1pchm60n60q60/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mmsm60n60q60_C32R32.do
sed -i "s/mmsC32R32S16HBM1pchm60n60q60/ccwwrC32R32S16HBM1pchi11x11x34o9x9x16k3/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri11x11x34o9x9x16k3_C32R32.do

cd ../CDS_GENUS
genus -files TCL/power_C32R32S16_1pch.tcl
rm ${VCD_FILES}/hbmCR/*.vcd.gz

# C64R4
cd ../MGC_QSIM
sed -i "s/#define CRF_ENTRIES     32/#define CRF_ENTRIES     64/g" PROJ_CR/defs.h
sed -i "s/#define SRF_A_ENTRIES   32/#define SRF_A_ENTRIES   4/g" PROJ_CR/defs.h
sed -i "s/#define SRF_M_ENTRIES   32/#define SRF_M_ENTRIES   4/g" PROJ_CR/defs.h
sed -i "s/#define GRF_ENTRIES     32/#define GRF_ENTRIES     4/g" PROJ_CR/defs.h
sed -i "s/#define AAM_ADDR_BITS   5/#define AAM_ADDR_BITS   2/g" PROJ_CR/defs.h
sed -i "s/ccwwrC32R32S16HBM1pchi11x11x34o9x9x16k3/ewarwC64R4S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ewarwV128n128_C64R4.do
sed -i "s/ewarwC64R4S16HBM1pchV128n128/dpC64R4S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_dpV128n128_C64R4.do
sed -i "s/dpC64R4S16HBM1pchV128n128/mvmC64R4S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C64R4.do
sed -i "s/mvmC64R4S16HBM1pch180x180/mvmC64R4S16HBM1pch1024x1024/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_C64R4.do
sed -i "s/mvmC64R4S16HBM1pch1024x1024/mmsC64R4S16HBM1pchm60n60q60/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mmsm60n60q60_C64R4.do
sed -i "s/mmsC64R4S16HBM1pchm60n60q60/ccwwrC64R4S16HBM1pchi11x11x34o9x9x16k3/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri11x11x34o9x9x16k3_C64R4.do

cd ../CDS_GENUS
genus -files TCL/power_C64R4S16_1pch.tcl
rm ${VCD_FILES}/hbmCR/*.vcd.gz

# C64R8
cd ../MGC_QSIM
sed -i "s/#define CRF_ENTRIES     64/#define CRF_ENTRIES     64/g" PROJ_CR/defs.h
sed -i "s/#define SRF_A_ENTRIES   4/#define SRF_A_ENTRIES   8/g" PROJ_CR/defs.h
sed -i "s/#define SRF_M_ENTRIES   4/#define SRF_M_ENTRIES   8/g" PROJ_CR/defs.h
sed -i "s/#define GRF_ENTRIES     4/#define GRF_ENTRIES     8/g" PROJ_CR/defs.h
sed -i "s/#define AAM_ADDR_BITS   2/#define AAM_ADDR_BITS   3/g" PROJ_CR/defs.h
sed -i "s/ccwwrC64R4S16HBM1pchi11x11x34o9x9x16k3/ewarwC64R8S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ewarwV128n128_C64R8.do
sed -i "s/ewarwC64R8S16HBM1pchV128n128/dpC64R8S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_dpV128n128_C64R8.do
sed -i "s/dpC64R8S16HBM1pchV128n128/mvmC64R8S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C64R8.do
sed -i "s/mvmC64R8S16HBM1pch180x180/mvmC64R8S16HBM1pch1024x1024/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_C64R8.do
sed -i "s/mvmC64R8S16HBM1pch1024x1024/mmsC64R8S16HBM1pchm60n60q60/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mmsm60n60q60_C64R8.do
sed -i "s/mmsC64R8S16HBM1pchm60n60q60/ccwwrC64R8S16HBM1pchi11x11x34o9x9x16k3/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri11x11x34o9x9x16k3_C64R8.do

cd ../CDS_GENUS
genus -files TCL/power_C64R8S16_1pch.tcl
rm ${VCD_FILES}/hbmCR/*.vcd.gz

# C64R16
cd ../MGC_QSIM
sed -i "s/#define CRF_ENTRIES     64/#define CRF_ENTRIES     64/g" PROJ_CR/defs.h
sed -i "s/#define SRF_A_ENTRIES   8/#define SRF_A_ENTRIES   16/g" PROJ_CR/defs.h
sed -i "s/#define SRF_M_ENTRIES   8/#define SRF_M_ENTRIES   16/g" PROJ_CR/defs.h
sed -i "s/#define GRF_ENTRIES     8/#define GRF_ENTRIES     16/g" PROJ_CR/defs.h
sed -i "s/#define AAM_ADDR_BITS   3/#define AAM_ADDR_BITS   4/g" PROJ_CR/defs.h
sed -i "s/ccwwrC64R8S16HBM1pchi11x11x34o9x9x16k3/ewarwC64R16S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ewarwV128n128_C64R16.do
sed -i "s/ewarwC64R16S16HBM1pchV128n128/dpC64R16S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_dpV128n128_C64R16.do
sed -i "s/dpC64R16S16HBM1pchV128n128/mvmC64R16S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C64R16.do
sed -i "s/mvmC64R16S16HBM1pch180x180/mvmC64R16S16HBM1pch1024x1024/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_C64R16.do
sed -i "s/mvmC64R16S16HBM1pch1024x1024/mmsC64R16S16HBM1pchm60n60q60/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mmsm60n60q60_C64R16.do
sed -i "s/mmsC64R16S16HBM1pchm60n60q60/ccwwrC64R16S16HBM1pchi11x11x34o9x9x16k3/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri11x11x34o9x9x16k3_C64R16.do

cd ../CDS_GENUS
genus -files TCL/power_C64R16S16_1pch.tcl
rm ${VCD_FILES}/hbmCR/*.vcd.gz

# C64R32
cd ../MGC_QSIM
sed -i "s/#define CRF_ENTRIES     64/#define CRF_ENTRIES     64/g" PROJ_CR/defs.h
sed -i "s/#define SRF_A_ENTRIES   16/#define SRF_A_ENTRIES   32/g" PROJ_CR/defs.h
sed -i "s/#define SRF_M_ENTRIES   16/#define SRF_M_ENTRIES   32/g" PROJ_CR/defs.h
sed -i "s/#define GRF_ENTRIES     16/#define GRF_ENTRIES     32/g" PROJ_CR/defs.h
sed -i "s/#define AAM_ADDR_BITS   4/#define AAM_ADDR_BITS   5/g" PROJ_CR/defs.h
sed -i "s/ccwwrC64R16S16HBM1pchi11x11x34o9x9x16k3/ewarwC64R32S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ewarwV128n128_C64R32.do
sed -i "s/ewarwC64R32S16HBM1pchV128n128/dpC64R32S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_dpV128n128_C64R32.do
sed -i "s/dpC64R32S16HBM1pchV128n128/mvmC64R32S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C64R32.do
sed -i "s/mvmC64R32S16HBM1pch180x180/mvmC64R32S16HBM1pch1024x1024/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_C64R32.do
sed -i "s/mvmC64R32S16HBM1pch1024x1024/mmsC64R32S16HBM1pchm60n60q60/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mmsm60n60q60_C64R32.do
sed -i "s/mmsC64R32S16HBM1pchm60n60q60/ccwwrC64R32S16HBM1pchi11x11x34o9x9x16k3/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri11x11x34o9x9x16k3_C64R32.do

cd ../CDS_GENUS
genus -files TCL/power_C64R32S16_1pch.tcl
rm ${VCD_FILES}/hbmCR/*.vcd.gz

# C128R4
cd ../MGC_QSIM
sed -i "s/#define CRF_ENTRIES     64/#define CRF_ENTRIES     128/g" PROJ_CR/defs.h
sed -i "s/#define SRF_A_ENTRIES   32/#define SRF_A_ENTRIES   4/g" PROJ_CR/defs.h
sed -i "s/#define SRF_M_ENTRIES   32/#define SRF_M_ENTRIES   4/g" PROJ_CR/defs.h
sed -i "s/#define GRF_ENTRIES     32/#define GRF_ENTRIES     4/g" PROJ_CR/defs.h
sed -i "s/#define AAM_ADDR_BITS   5/#define AAM_ADDR_BITS   2/g" PROJ_CR/defs.h
sed -i "s/ccwwrC64R32S16HBM1pchi11x11x34o9x9x16k3/ewarwC128R4S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ewarwV128n128_C128R4.do
sed -i "s/ewarwC128R4S16HBM1pchV128n128/dpC128R4S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_dpV128n128_C128R4.do
sed -i "s/dpC128R4S16HBM1pchV128n128/mvmC128R4S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C128R4.do
sed -i "s/mvmC128R4S16HBM1pch180x180/mvmC128R4S16HBM1pch1024x1024/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_C128R4.do
sed -i "s/mvmC128R4S16HBM1pch1024x1024/mmsC128R4S16HBM1pchm60n60q60/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mmsm60n60q60_C128R4.do
sed -i "s/mmsC128R4S16HBM1pchm60n60q60/ccwwrC128R4S16HBM1pchi11x11x34o9x9x16k3/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri11x11x34o9x9x16k3_C128R4.do

cd ../CDS_GENUS
genus -files TCL/power_C128R4S16_1pch.tcl
rm ${VCD_FILES}/hbmCR/*.vcd.gz

# C128R8
cd ../MGC_QSIM
sed -i "s/#define CRF_ENTRIES     128/#define CRF_ENTRIES     128/g" PROJ_CR/defs.h
sed -i "s/#define SRF_A_ENTRIES   4/#define SRF_A_ENTRIES   8/g" PROJ_CR/defs.h
sed -i "s/#define SRF_M_ENTRIES   4/#define SRF_M_ENTRIES   8/g" PROJ_CR/defs.h
sed -i "s/#define GRF_ENTRIES     4/#define GRF_ENTRIES     8/g" PROJ_CR/defs.h
sed -i "s/#define AAM_ADDR_BITS   2/#define AAM_ADDR_BITS   3/g" PROJ_CR/defs.h
sed -i "s/ccwwrC128R4S16HBM1pchi11x11x34o9x9x16k3/ewarwC128R8S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ewarwV128n128_C128R8.do
sed -i "s/ewarwC128R8S16HBM1pchV128n128/dpC128R8S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_dpV128n128_C128R8.do
sed -i "s/dpC128R8S16HBM1pchV128n128/mvmC128R8S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C128R8.do
sed -i "s/mvmC128R8S16HBM1pch180x180/mvmC128R8S16HBM1pch1024x1024/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_C128R8.do
sed -i "s/mvmC128R8S16HBM1pch1024x1024/mmsC128R8S16HBM1pchm60n60q60/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mmsm60n60q60_C128R8.do
sed -i "s/mmsC128R8S16HBM1pchm60n60q60/ccwwrC128R8S16HBM1pchi11x11x34o9x9x16k3/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri11x11x34o9x9x16k3_C128R8.do

cd ../CDS_GENUS
genus -files TCL/power_C128R8S16_1pch.tcl
rm ${VCD_FILES}/hbmCR/*.vcd.gz

# C128R16
cd ../MGC_QSIM
sed -i "s/#define CRF_ENTRIES     128/#define CRF_ENTRIES     128/g" PROJ_CR/defs.h
sed -i "s/#define SRF_A_ENTRIES   8/#define SRF_A_ENTRIES   16/g" PROJ_CR/defs.h
sed -i "s/#define SRF_M_ENTRIES   8/#define SRF_M_ENTRIES   16/g" PROJ_CR/defs.h
sed -i "s/#define GRF_ENTRIES     8/#define GRF_ENTRIES     16/g" PROJ_CR/defs.h
sed -i "s/#define AAM_ADDR_BITS   3/#define AAM_ADDR_BITS   4/g" PROJ_CR/defs.h
sed -i "s/ccwwrC128R8S16HBM1pchi11x11x34o9x9x16k3/ewarwC128R16S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ewarwV128n128_C128R16.do
sed -i "s/ewarwC128R16S16HBM1pchV128n128/dpC128R16S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_dpV128n128_C128R16.do
sed -i "s/dpC128R16S16HBM1pchV128n128/mvmC128R16S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C128R16.do
sed -i "s/mvmC128R16S16HBM1pch180x180/mvmC128R16S16HBM1pch1024x1024/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_C128R16.do
sed -i "s/mvmC128R16S16HBM1pch1024x1024/mmsC128R16S16HBM1pchm60n60q60/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mmsm60n60q60_C128R16.do
sed -i "s/mmsC128R16S16HBM1pchm60n60q60/ccwwrC128R16S16HBM1pchi11x11x34o9x9x16k3/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri11x11x34o9x9x16k3_C128R16.do

cd ../CDS_GENUS
genus -files TCL/power_C128R16S16_1pch.tcl
rm ${VCD_FILES}/hbmCR/*.vcd.gz

# C128R32
cd ../MGC_QSIM
sed -i "s/#define CRF_ENTRIES     128/#define CRF_ENTRIES     128/g" PROJ_CR/defs.h
sed -i "s/#define SRF_A_ENTRIES   16/#define SRF_A_ENTRIES   32/g" PROJ_CR/defs.h
sed -i "s/#define SRF_M_ENTRIES   16/#define SRF_M_ENTRIES   32/g" PROJ_CR/defs.h
sed -i "s/#define GRF_ENTRIES     16/#define GRF_ENTRIES     32/g" PROJ_CR/defs.h
sed -i "s/#define AAM_ADDR_BITS   4/#define AAM_ADDR_BITS   5/g" PROJ_CR/defs.h
sed -i "s/ccwwrC128R16S16HBM1pchi11x11x34o9x9x16k3/ewarwC128R32S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ewarwV128n128_C128R32.do
sed -i "s/ewarwC128R32S16HBM1pchV128n128/dpC128R32S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_dpV128n128_C128R32.do
sed -i "s/dpC128R32S16HBM1pchV128n128/mvmC128R32S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C128R32.do
sed -i "s/mvmC128R32S16HBM1pch180x180/mvmC128R32S16HBM1pch1024x1024/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm1024x1024_C128R32.do
sed -i "s/mvmC128R32S16HBM1pch1024x1024/mmsC128R32S16HBM1pchm60n60q60/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mmsm60n60q60_C128R32.do
sed -i "s/mmsC128R32S16HBM1pchm60n60q60/ccwwrC128R32S16HBM1pchi11x11x34o9x9x16k3/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_ccwwri11x11x34o9x9x16k3_C128R32.do

cd ../CDS_GENUS
genus -files TCL/power_C128R32S16_1pch.tcl
rm ${VCD_FILES}/hbmCR/*.vcd.gz


# Go back to original
cd ../MGC_QSIM
sed -i "s/#define CRF_ENTRIES     128/#define CRF_ENTRIES     32/g" PROJ_CR/defs.h
sed -i "s/#define SRF_A_ENTRIES   32/#define SRF_A_ENTRIES   8/g" PROJ_CR/defs.h
sed -i "s/#define SRF_M_ENTRIES   32/#define SRF_M_ENTRIES   8/g" PROJ_CR/defs.h
sed -i "s/#define GRF_ENTRIES     32/#define GRF_ENTRIES     8/g" PROJ_CR/defs.h
sed -i "s/#define AAM_ADDR_BITS   5/#define AAM_ADDR_BITS   3/g" PROJ_CR/defs.h
sed -i "s/ccwwrC128R32S16HBM1pchi11x11x34o9x9x16k3/ewarwC16R4S16HBM1pchV128n128/g" PROJ_CR/pch_main.h
cd ..