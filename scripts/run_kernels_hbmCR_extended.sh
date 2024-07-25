
#!/bin/bash

echo "REMEMEBER ALL HEADER FILES SHOULD START WITH C=32, R=8 and HBM original configuration"

sed -i "s/configs\/HBM_AB-config.cfg/configs\/HBM2_AB-config.cfg/g" ../inputs/assembly2sc.sh

make -C $HOME/Documents/ramulator-AB/ -j4

echo "" > ../scripts/kernels_hbmCR_extended.times
echo "----------C = 4, R = 4, S = 16----------" >> ../scripts/kernels_hbmCR_extended.times
sed -i "s/#define CRF_ENTRIES     32/#define CRF_ENTRIES     4/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   8/#define SRF_A_ENTRIES   4/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   8/#define SRF_M_ENTRIES   4/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     8/#define GRF_ENTRIES     4/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   3/#define AAM_ADDR_BITS   2/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC4R4S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC4R4S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC4R4S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC4R4S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC4R4S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC4R4S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ewarwC4R4S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh dpC4R4S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC4R4S16HBM1pch180x180 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC4R4S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR_extended.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mmsC4R4S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR_extended.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ccwwrC4R4S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR_extended.times


echo "" >> ../scripts/kernels_hbmCR_extended.times
echo "----------C = 4, R = 8, S = 16----------" >> ../scripts/kernels_hbmCR_extended.times
sed -i "s/#define CRF_ENTRIES     4/#define CRF_ENTRIES     4/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   4/#define SRF_A_ENTRIES   8/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   4/#define SRF_M_ENTRIES   8/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     4/#define GRF_ENTRIES     8/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   2/#define AAM_ADDR_BITS   3/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC4R8S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC4R8S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC4R8S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC4R8S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC4R8S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC4R8S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ewarwC4R8S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh dpC4R8S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC4R8S16HBM1pch180x180 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC4R8S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR_extended.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mmsC4R8S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR_extended.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ccwwrC4R8S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR_extended.times


echo "" >> ../scripts/kernels_hbmCR_extended.times
echo "----------C = 8, R = 4, S = 16----------" >> ../scripts/kernels_hbmCR_extended.times
sed -i "s/#define CRF_ENTRIES     4/#define CRF_ENTRIES     8/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   8/#define SRF_A_ENTRIES   4/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   8/#define SRF_M_ENTRIES   4/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     8/#define GRF_ENTRIES     4/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   3/#define AAM_ADDR_BITS   2/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC8R4S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC8R4S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC8R4S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC8R4S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC8R4S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC8R4S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ewarwC8R4S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh dpC8R4S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC8R4S16HBM1pch180x180 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC8R4S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR_extended.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mmsC8R4S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR_extended.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ccwwrC8R4S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR_extended.times


echo "" >> ../scripts/kernels_hbmCR_extended.times
echo "----------C = 8, R = 8, S = 16----------" >> ../scripts/kernels_hbmCR_extended.times
sed -i "s/#define CRF_ENTRIES     8/#define CRF_ENTRIES     8/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   4/#define SRF_A_ENTRIES   8/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   4/#define SRF_M_ENTRIES   8/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     4/#define GRF_ENTRIES     8/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   2/#define AAM_ADDR_BITS   3/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC8R8S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC8R8S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC8R8S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC8R8S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC8R8S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC8R8S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ewarwC8R8S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh dpC8R8S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC8R8S16HBM1pch180x180 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC8R8S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR_extended.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mmsC8R8S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR_extended.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ccwwrC8R8S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR_extended.times


echo "" >> ../scripts/kernels_hbmCR_extended.times
echo "----------C = 8, R = 16, S = 16----------" >> ../scripts/kernels_hbmCR_extended.times
sed -i "s/#define CRF_ENTRIES     8/#define CRF_ENTRIES     8/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   8/#define SRF_A_ENTRIES   16/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   8/#define SRF_M_ENTRIES   16/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     8/#define GRF_ENTRIES     16/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   3/#define AAM_ADDR_BITS   4/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC8R16S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC8R16S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC8R16S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC8R16S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC8R16S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC8R16S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ewarwC8R16S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh dpC8R16S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC8R16S16HBM1pch180x180 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC8R16S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR_extended.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mmsC8R16S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR_extended.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ccwwrC8R16S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR_extended.times


echo "" >> ../scripts/kernels_hbmCR_extended.times
echo "----------C = 32, R = 2, S = 16----------" >> ../scripts/kernels_hbmCR_extended.times
sed -i "s/#define CRF_ENTRIES     8/#define CRF_ENTRIES     32/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   16/#define SRF_A_ENTRIES   2/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   16/#define SRF_M_ENTRIES   2/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     16/#define GRF_ENTRIES     2/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   4/#define AAM_ADDR_BITS   1/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC32R2S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC32R2S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC32R2S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC32R2S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC32R2S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC32R2S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ewarwC32R2S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh dpC32R2S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC32R2S16HBM1pch180x180 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC32R2S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR_extended.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mmsC32R2S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR_extended.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ccwwrC32R2S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR_extended.times


echo "" >> ../scripts/kernels_hbmCR_extended.times
echo "----------C = 32, R = 64, S = 16----------" >> ../scripts/kernels_hbmCR_extended.times
sed -i "s/#define CRF_ENTRIES     32/#define CRF_ENTRIES     32/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   2/#define SRF_A_ENTRIES   64/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   2/#define SRF_M_ENTRIES   64/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     2/#define GRF_ENTRIES     64/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   1/#define AAM_ADDR_BITS   6/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC32R64S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC32R64S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC32R64S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC32R64S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC32R64S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC32R64S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ewarwC32R64S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh dpC32R64S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC32R64S16HBM1pch180x180 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC32R64S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR_extended.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mmsC32R64S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR_extended.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ccwwrC32R64S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR_extended.times


echo "" >> ../scripts/kernels_hbmCR_extended.times
echo "----------C = 64, R = 2, S = 16----------" >> ../scripts/kernels_hbmCR_extended.times
sed -i "s/#define CRF_ENTRIES     32/#define CRF_ENTRIES     64/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   64/#define SRF_A_ENTRIES   2/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   64/#define SRF_M_ENTRIES   2/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     64/#define GRF_ENTRIES     2/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   6/#define AAM_ADDR_BITS   1/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC64R2S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC64R2S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC64R2S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC64R2S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC64R2S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC64R2S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ewarwC64R2S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh dpC64R2S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC64R2S16HBM1pch180x180 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC64R2S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR_extended.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mmsC64R2S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR_extended.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ccwwrC64R2S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR_extended.times


echo "" >> ../scripts/kernels_hbmCR_extended.times
echo "----------C = 64, R = 64, S = 16----------" >> ../scripts/kernels_hbmCR_extended.times
sed -i "s/#define CRF_ENTRIES     64/#define CRF_ENTRIES     64/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   2/#define SRF_A_ENTRIES   64/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   2/#define SRF_M_ENTRIES   64/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     2/#define GRF_ENTRIES     64/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   1/#define AAM_ADDR_BITS   6/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC64R64S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC64R64S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC64R64S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC64R64S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC64R64S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC64R64S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ewarwC64R64S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh dpC64R64S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC64R64S16HBM1pch180x180 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC64R64S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR_extended.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mmsC64R64S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR_extended.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ccwwrC64R64S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR_extended.times


echo "" >> ../scripts/kernels_hbmCR_extended.times
echo "----------C = 64, R = 128, S = 16----------" >> ../scripts/kernels_hbmCR_extended.times
sed -i "s/#define CRF_ENTRIES     64/#define CRF_ENTRIES     64/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   64/#define SRF_A_ENTRIES   128/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   64/#define SRF_M_ENTRIES   128/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     64/#define GRF_ENTRIES     128/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   6/#define AAM_ADDR_BITS   7/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC64R128S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC64R128S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC64R128S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC64R128S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC64R128S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC64R128S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ewarwC64R128S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh dpC64R128S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC64R128S16HBM1pch180x180 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC64R128S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR_extended.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mmsC64R128S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR_extended.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ccwwrC64R128S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR_extended.times


echo "" >> ../scripts/kernels_hbmCR_extended.times
echo "----------C = 256, R = 8, S = 16----------" >> ../scripts/kernels_hbmCR_extended.times
sed -i "s/#define CRF_ENTRIES     64/#define CRF_ENTRIES     256/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   128/#define SRF_A_ENTRIES   8/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   128/#define SRF_M_ENTRIES   8/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     128/#define GRF_ENTRIES     8/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   7/#define AAM_ADDR_BITS   3/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC256R8S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC256R8S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC256R8S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC256R8S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC256R8S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC256R8S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ewarwC256R8S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh dpC256R8S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC256R8S16HBM1pch180x180 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC256R8S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR_extended.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mmsC256R8S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR_extended.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ccwwrC256R8S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR_extended.times


echo "" >> ../scripts/kernels_hbmCR_extended.times
echo "----------C = 256, R = 16, S = 16----------" >> ../scripts/kernels_hbmCR_extended.times
sed -i "s/#define CRF_ENTRIES     256/#define CRF_ENTRIES     256/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   8/#define SRF_A_ENTRIES   16/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   8/#define SRF_M_ENTRIES   16/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     8/#define GRF_ENTRIES     16/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   3/#define AAM_ADDR_BITS   4/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC256R16S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC256R16S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC256R16S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC256R16S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC256R16S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC256R16S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ewarwC256R16S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh dpC256R16S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC256R16S16HBM1pch180x180 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC256R16S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR_extended.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mmsC256R16S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR_extended.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ccwwrC256R16S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR_extended.times


echo "" >> ../scripts/kernels_hbmCR_extended.times
echo "----------C = 256, R = 32, S = 16----------" >> ../scripts/kernels_hbmCR_extended.times
sed -i "s/#define CRF_ENTRIES     256/#define CRF_ENTRIES     256/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   16/#define SRF_A_ENTRIES   32/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   16/#define SRF_M_ENTRIES   32/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     16/#define GRF_ENTRIES     32/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   4/#define AAM_ADDR_BITS   5/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC256R32S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC256R32S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC256R32S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC256R32S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC256R32S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC256R32S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ewarwC256R32S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh dpC256R32S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC256R32S16HBM1pch180x180 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC256R32S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR_extended.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mmsC256R32S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR_extended.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ccwwrC256R32S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR_extended.times


echo "" >> ../scripts/kernels_hbmCR_extended.times
echo "----------C = 512, R = 32, S = 16----------" >> ../scripts/kernels_hbmCR_extended.times
sed -i "s/#define CRF_ENTRIES     256/#define CRF_ENTRIES     512/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   32/#define SRF_A_ENTRIES   32/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   32/#define SRF_M_ENTRIES   32/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     132/#define GRF_ENTRIES     32/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   5/#define AAM_ADDR_BITS   5/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC512R32S16HBM1pchV128n128 EWARW 128 128
bin/map_kernel dpC512R32S16HBM1pchV128n128 DP 128 128
bin/map_kernel mvmC512R32S16HBM1pch180x180 MMS 1 180 180
bin/map_kernel mvmC512R32S16HBM1pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC512R32S16HBM1pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC512R32S16HBM1pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ewarwC512R32S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh dpC512R32S16HBM1pchV128n128 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 180x180" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC512R32S16HBM1pch180x180 >> ../scripts/kernels_hbmCR_extended.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mvmC512R32S16HBM1pch1024x1024 >> ../scripts/kernels_hbmCR_extended.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh mmsC512R32S16HBM1pchm60n60q60 >> ../scripts/kernels_hbmCR_extended.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_hbmCR_extended.times
./assembly2sc.sh ccwwrC512R32S16HBM1pchi11x11x34o9x9x16k3 >> ../scripts/kernels_hbmCR_extended.times


# Back to initial state

sed -i "s/configs\/HBM2_AB-config.cfg/configs\/HBM_AB-config.cfg/g" ../inputs/assembly2sc.sh

make -C $HOME/Documents/ramulator-AB/ -j4

sed -i "s/#define CRF_ENTRIES     512/#define CRF_ENTRIES     32/g" ../src/defs.h
sed -i "s/#define SRF_A_ENTRIES   32/#define SRF_A_ENTRIES   8/g" ../src/defs.h
sed -i "s/#define SRF_M_ENTRIES   32/#define SRF_M_ENTRIES   8/g" ../src/defs.h
sed -i "s/#define GRF_ENTRIES     32/#define GRF_ENTRIES     8/g" ../src/defs.h
sed -i "s/#define AAM_ADDR_BITS   5/#define AAM_ADDR_BITS   3/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh