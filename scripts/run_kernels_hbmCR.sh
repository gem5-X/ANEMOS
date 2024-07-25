
#!/bin/bash

echo "REMEMEBER ALL HEADER FILES SHOULD START WITH C=32, R=8 and HBM original configuration"

sed -i "s/configs\/HBM_AB-config.cfg/configs\/HBM2_AB-config.cfg/g" ../inputs/assembly2sc.sh

make -C $HOME/Documents/ramulator-AB/ -j4

echo "" > ../scripts/kernels_hbmCR.times
echo "----------C = 16, R = 4, S = 16----------" >> ../scripts/kernels_hbmCR.times
sed -i "s/#define CRF_ENTRIES     32/#define CRF_ENTRIES     16/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   8/#define SRF_A_ENTRIES   4/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   8/#define SRF_M_ENTRIES   4/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     8/#define GRF_ENTRIES     4/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   3/#define AAM_ADDR_BITS   2/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC16R4S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC16R4S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC16R4S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC16R4S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC16R4S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC16R4S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ewarwC16R4S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh dpC16R4S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC16R4S16HBM1pch180x180 >> ../scripts/kernels_hbmCR.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC16R4S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mmsC16R4S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ccwwrC16R4S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR.times

echo "" >> ../scripts/kernels_hbmCR.times
echo "----------C = 16, R = 8, S = 16----------" >> ../scripts/kernels_hbmCR.times
sed -i "s/#define CRF_ENTRIES     16/#define CRF_ENTRIES     16/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   4/#define SRF_A_ENTRIES   8/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   4/#define SRF_M_ENTRIES   8/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     4/#define GRF_ENTRIES     8/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   2/#define AAM_ADDR_BITS   3/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC16R8S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC16R8S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC16R8S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC16R8S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC16R8S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC16R8S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ewarwC16R8S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh dpC16R8S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC16R8S16HBM1pch180x180 >> ../scripts/kernels_hbmCR.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC16R8S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mmsC16R8S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ccwwrC16R8S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR.times

echo "" >> ../scripts/kernels_hbmCR.times
echo "----------C = 16, R = 16, S = 16----------" >> ../scripts/kernels_hbmCR.times
sed -i "s/#define CRF_ENTRIES     16/#define CRF_ENTRIES     16/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   8/#define SRF_A_ENTRIES   16/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   8/#define SRF_M_ENTRIES   16/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     8/#define GRF_ENTRIES     16/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   3/#define AAM_ADDR_BITS   4/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC16R16S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC16R16S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC16R16S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC16R16S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC16R16S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC16R16S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ewarwC16R16S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh dpC16R16S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC16R16S16HBM1pch180x180 >> ../scripts/kernels_hbmCR.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC16R16S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mmsC16R16S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ccwwrC16R16S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR.times

echo "" >> ../scripts/kernels_hbmCR.times
echo "----------C = 16, R = 32, S = 16----------" >> ../scripts/kernels_hbmCR.times
sed -i "s/#define CRF_ENTRIES     16/#define CRF_ENTRIES     16/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   16/#define SRF_A_ENTRIES   32/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   16/#define SRF_M_ENTRIES   32/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     16/#define GRF_ENTRIES     32/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   4/#define AAM_ADDR_BITS   5/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC16R32S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC16R32S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC16R32S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC16R32S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC16R32S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC16R32S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ewarwC16R32S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh dpC16R32S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC16R32S16HBM1pch180x180 >> ../scripts/kernels_hbmCR.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC16R32S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mmsC16R32S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ccwwrC16R32S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR.times

echo "" >> ../scripts/kernels_hbmCR.times
echo "----------C = 32, R = 4, S = 16----------" >> ../scripts/kernels_hbmCR.times
sed -i "s/#define CRF_ENTRIES     16/#define CRF_ENTRIES     32/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   32/#define SRF_A_ENTRIES   4/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   32/#define SRF_M_ENTRIES   4/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     32/#define GRF_ENTRIES     4/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   5/#define AAM_ADDR_BITS   2/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC32R4S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC32R4S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC32R4S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC32R4S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC32R4S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC32R4S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ewarwC32R4S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh dpC32R4S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC32R4S16HBM1pch180x180 >> ../scripts/kernels_hbmCR.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC32R4S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mmsC32R4S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ccwwrC32R4S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR.times

echo "" >> ../scripts/kernels_hbmCR.times
echo "----------C = 32, R = 8, S = 16----------" >> ../scripts/kernels_hbmCR.times
sed -i "s/#define CRF_ENTRIES     32/#define CRF_ENTRIES     32/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   4/#define SRF_A_ENTRIES   8/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   4/#define SRF_M_ENTRIES   8/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     4/#define GRF_ENTRIES     8/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   2/#define AAM_ADDR_BITS   3/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC32R8S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC32R8S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC32R8S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC32R8S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC32R8S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC32R8S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ewarwC32R8S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh dpC32R8S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC32R8S16HBM1pch180x180 >> ../scripts/kernels_hbmCR.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC32R8S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mmsC32R8S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ccwwrC32R8S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR.times

echo "" >> ../scripts/kernels_hbmCR.times
echo "----------C = 32, R = 16, S = 16----------" >> ../scripts/kernels_hbmCR.times
sed -i "s/#define CRF_ENTRIES     32/#define CRF_ENTRIES     32/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   8/#define SRF_A_ENTRIES   16/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   8/#define SRF_M_ENTRIES   16/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     8/#define GRF_ENTRIES     16/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   3/#define AAM_ADDR_BITS   4/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC32R16S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC32R16S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC32R16S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC32R16S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC32R16S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC32R16S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ewarwC32R16S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh dpC32R16S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC32R16S16HBM1pch180x180 >> ../scripts/kernels_hbmCR.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC32R16S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mmsC32R16S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ccwwrC32R16S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR.times

echo "" >> ../scripts/kernels_hbmCR.times
echo "----------C = 32, R = 32, S = 16----------" >> ../scripts/kernels_hbmCR.times
sed -i "s/#define CRF_ENTRIES     32/#define CRF_ENTRIES     32/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   16/#define SRF_A_ENTRIES   32/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   16/#define SRF_M_ENTRIES   32/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     16/#define GRF_ENTRIES     32/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   4/#define AAM_ADDR_BITS   5/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC32R32S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC32R32S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC32R32S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC32R32S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC32R32S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC32R32S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ewarwC32R32S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh dpC32R32S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC32R32S16HBM1pch180x180 >> ../scripts/kernels_hbmCR.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC32R32S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mmsC32R32S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ccwwrC32R32S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR.times

make -C $HOME/Documents/ramulator-AB/ -j4

echo "" >> ../scripts/kernels_hbmCR.times
echo "----------C = 64, R = 4, S = 16----------" >> ../scripts/kernels_hbmCR.times
sed -i "s/#define CRF_ENTRIES     32/#define CRF_ENTRIES     64/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   32/#define SRF_A_ENTRIES   4/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   32/#define SRF_M_ENTRIES   4/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     32/#define GRF_ENTRIES     4/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   5/#define AAM_ADDR_BITS   2/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC64R4S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC64R4S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC64R4S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC64R4S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC64R4S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC64R4S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ewarwC64R4S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh dpC64R4S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC64R4S16HBM1pch180x180 >> ../scripts/kernels_hbmCR.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC64R4S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mmsC64R4S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ccwwrC64R4S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR.times

echo "" >> ../scripts/kernels_hbmCR.times
echo "----------C = 64, R = 8, S = 16----------" >> ../scripts/kernels_hbmCR.times
sed -i "s/#define CRF_ENTRIES     64/#define CRF_ENTRIES     64/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   4/#define SRF_A_ENTRIES   8/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   4/#define SRF_M_ENTRIES   8/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     4/#define GRF_ENTRIES     8/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   2/#define AAM_ADDR_BITS   3/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC64R8S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC64R8S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC64R8S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC64R8S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC64R8S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC64R8S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ewarwC64R8S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh dpC64R8S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC64R8S16HBM1pch180x180 >> ../scripts/kernels_hbmCR.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC64R8S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mmsC64R8S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ccwwrC64R8S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR.times

echo "" >> ../scripts/kernels_hbmCR.times
echo "----------C = 64, R = 16, S = 16----------" >> ../scripts/kernels_hbmCR.times
sed -i "s/#define CRF_ENTRIES     64/#define CRF_ENTRIES     64/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   8/#define SRF_A_ENTRIES   16/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   8/#define SRF_M_ENTRIES   16/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     8/#define GRF_ENTRIES     16/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   3/#define AAM_ADDR_BITS   4/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC64R16S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC64R16S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC64R16S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC64R16S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC64R16S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC64R16S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ewarwC64R16S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh dpC64R16S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC64R16S16HBM1pch180x180 >> ../scripts/kernels_hbmCR.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC64R16S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mmsC64R16S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ccwwrC64R16S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR.times

echo "" >> ../scripts/kernels_hbmCR.times
echo "----------C = 64, R = 32, S = 16----------" >> ../scripts/kernels_hbmCR.times
sed -i "s/#define CRF_ENTRIES     64/#define CRF_ENTRIES     64/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   16/#define SRF_A_ENTRIES   32/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   16/#define SRF_M_ENTRIES   32/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     16/#define GRF_ENTRIES     32/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   4/#define AAM_ADDR_BITS   5/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC64R32S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC64R32S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC64R32S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC64R32S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC64R32S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC64R32S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ewarwC64R32S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh dpC64R32S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC64R32S16HBM1pch180x180 >> ../scripts/kernels_hbmCR.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC64R32S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mmsC64R32S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ccwwrC64R32S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR.times

make -C $HOME/Documents/ramulator-AB/ -j4

echo "" >> ../scripts/kernels_hbmCR.times
echo "----------C = 128, R = 4, S = 16----------" >> ../scripts/kernels_hbmCR.times
sed -i "s/#define CRF_ENTRIES     64/#define CRF_ENTRIES     128/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   32/#define SRF_A_ENTRIES   4/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   32/#define SRF_M_ENTRIES   4/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     32/#define GRF_ENTRIES     4/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   5/#define AAM_ADDR_BITS   2/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC128R4S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC128R4S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC128R4S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC128R4S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC128R4S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC128R4S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ewarwC128R4S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh dpC128R4S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC128R4S16HBM1pch180x180 >> ../scripts/kernels_hbmCR.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC128R4S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mmsC128R4S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ccwwrC128R4S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR.times

echo "" >> ../scripts/kernels_hbmCR.times
echo "----------C = 128, R = 8, S = 16----------" >> ../scripts/kernels_hbmCR.times
sed -i "s/#define CRF_ENTRIES     128/#define CRF_ENTRIES     128/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   4/#define SRF_A_ENTRIES   8/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   4/#define SRF_M_ENTRIES   8/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     4/#define GRF_ENTRIES     8/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   2/#define AAM_ADDR_BITS   3/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC128R8S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC128R8S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC128R8S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC128R8S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC128R8S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC128R8S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ewarwC128R8S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh dpC128R8S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC128R8S16HBM1pch180x180 >> ../scripts/kernels_hbmCR.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC128R8S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mmsC128R8S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ccwwrC128R8S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR.times

echo "" >> ../scripts/kernels_hbmCR.times
echo "----------C = 128, R = 16, S = 16----------" >> ../scripts/kernels_hbmCR.times
sed -i "s/#define CRF_ENTRIES     128/#define CRF_ENTRIES     128/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   8/#define SRF_A_ENTRIES   16/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   8/#define SRF_M_ENTRIES   16/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     8/#define GRF_ENTRIES     16/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   3/#define AAM_ADDR_BITS   4/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC128R16S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC128R16S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC128R16S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC128R16S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC128R16S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC128R16S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ewarwC128R16S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh dpC128R16S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC128R16S16HBM1pch180x180 >> ../scripts/kernels_hbmCR.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC128R16S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mmsC128R16S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ccwwrC128R16S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR.times

echo "" >> ../scripts/kernels_hbmCR.times
echo "----------C = 128, R = 32, S = 16----------" >> ../scripts/kernels_hbmCR.times
sed -i "s/#define CRF_ENTRIES     128/#define CRF_ENTRIES     128/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   16/#define SRF_A_ENTRIES   32/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   16/#define SRF_M_ENTRIES   32/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     16/#define GRF_ENTRIES     32/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   4/#define AAM_ADDR_BITS   5/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
# bin/map_kernel ewarwC128R32S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC128R32S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC128R32S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC128R32S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC128R32S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC128R32S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
# ./assembly2sc.sh ewarwC128R32S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
cd ..
build/pim-cores ewarwC128R32S16HBM1pchV128n128 >> scripts/kernels_hbmCR.times
cd inputs 
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh dpC128R32S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC128R32S16HBM1pch180x180 >> ../scripts/kernels_hbmCR.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mvmC128R32S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh mmsC128R32S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR.times
./assembly2sc.sh ccwwrC128R32S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR.times


# Back to initial state

sed -i "s/configs\/HBM2_AB-config.cfg/configs\/HBM_AB-config.cfg/g" ../inputs/assembly2sc.sh

make -C $HOME/Documents/ramulator-AB/ -j4

sed -i "s/#define CRF_ENTRIES     128/#define CRF_ENTRIES     32/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   32/#define SRF_A_ENTRIES   8/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   32/#define SRF_M_ENTRIES   8/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     32/#define GRF_ENTRIES     8/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   5/#define AAM_ADDR_BITS   3/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh