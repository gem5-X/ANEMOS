#!/usr/bin/tcsh

# C16R4
cd MGC_QSIM
vsim -c -do OUT/DO/sim_mvm180x180_C16R4.do

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
sed -i "s/mvmC16R4S16HBM1pch180x180/mvmC16R8S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C16R8.do

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
sed -i "s/mvmC16R8S16HBM1pch180x180/mvmC16R16S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C16R16.do

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
sed -i "s/mvmC16R16S16HBM1pch180x180/mvmC16R32S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C16R32.do

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
sed -i "s/mvmC16R32S16HBM1pch180x180/mvmC32R4S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C32R4.do

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
sed -i "s/mvmC32R4S16HBM1pch180x180/mvmC32R8S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C32R8.do

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
sed -i "s/mvmC32R8S16HBM1pch180x180/mvmC32R16S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C32R16.do

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
sed -i "s/mvmC32R16S16HBM1pch180x180/mvmC32R32S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C32R32.do

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
sed -i "s/mvmC32R32S16HBM1pch180x180/mvmC64R4S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C64R4.do

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
sed -i "s/mvmC64R4S16HBM1pch180x180/mvmC64R8S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C64R8.do

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
sed -i "s/mvmC64R8S16HBM1pch180x180/mvmC64R16S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C64R16.do

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
sed -i "s/mvmC64R16S16HBM1pch180x180/mvmC64R32S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C64R32.do

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
sed -i "s/mvmC64R32S16HBM1pch180x180/mvmC128R4S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C128R4.do

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
sed -i "s/mvmC128R4S16HBM1pch180x180/mvmC128R8S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C128R8.do

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
sed -i "s/mvmC128R8S16HBM1pch180x180/mvmC128R16S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C128R16.do

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
sed -i "s/mvmC128R16S16HBM1pch180x180/mvmC128R32S16HBM1pch180x180/g" PROJ_CR/pch_main.h
vsim -c -do OUT/DO/sim_mvm180x180_C128R32.do

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
sed -i "s/mvmC128R32S16HBM1pch180x180/mvmC16R4S16HBM1pch180x180/g" PROJ_CR/pch_main.h
cd ..