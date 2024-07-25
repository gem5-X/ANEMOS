
#!/bin/bash

echo "REMEMEBER ALL HEADER FILES SHOULD START WITH C=32, R=8 and HBM original configuration"

sed -i "s/configs\/HBM_AB-config.cfg/configs\/HBM2_AB-config.cfg/g" ../inputs/assembly2sc.sh

make -C $HOME/Documents/ramulator-AB/ -j4

echo "" >> ../scripts/kernels_datatypes.times
echo "----------INT8, S = 32----------" >> ../scripts/kernels_datatypes.times
sed -i "s/#define DATA_TYPE   0/#define DATA_TYPE   4/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC32R8S32HBMint81pchV128n128 EWARW 128 128
bin/map_kernel dpC32R8S32HBMint81pchV128n128 DP 128 128
bin/map_kernel mvmC32R8S32HBMint81pch180x180 MMS 1 180 180
bin/map_kernel mvmC32R8S32HBMint81pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC32R8S32HBMint81pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC32R8S32HBMint81pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh ewarwC32R8S32HBMint81pchV128n128 >> ../scripts/kernels_datatypes.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh dpC32R8S32HBMint81pchV128n128 >> ../scripts/kernels_datatypes.times
echo "MVM 180x180" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh mvmC32R8S32HBMint81pch180x180 >> ../scripts/kernels_datatypes.times
echo "MVM 1024x1024" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh mvmC32R8S32HBMint81pch1024x1024 >> ../scripts/kernels_datatypes.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh mmsC32R8S32HBMint81pchm60n60q60 >> ../scripts/kernels_datatypes.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh ccwwrC32R8S32HBMint81pchi11x11x34o9x9x16k3 >> ../scripts/kernels_datatypes.times

echo "" >> ../scripts/kernels_datatypes.times
echo "----------INT16, S = 16----------" >> ../scripts/kernels_datatypes.times
sed -i "s/#define DATA_TYPE   4/#define DATA_TYPE   5/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC32R8S16HBMint161pchV128n128 EWARW 128 128
bin/map_kernel dpC32R8S16HBMint161pchV128n128 DP 128 128
bin/map_kernel mvmC32R8S16HBMint161pch180x180 MMS 1 180 180
bin/map_kernel mvmC32R8S16HBMint161pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC32R8S16HBMint161pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC32R8S16HBMint161pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh ewarwC32R8S16HBMint161pchV128n128 >> ../scripts/kernels_datatypes.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh dpC32R8S16HBMint161pchV128n128 >> ../scripts/kernels_datatypes.times
echo "MVM 180x180" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh mvmC32R8S16HBMint161pch180x180 >> ../scripts/kernels_datatypes.times
echo "MVM 1024x1024" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh mvmC32R8S16HBMint161pch1024x1024 >> ../scripts/kernels_datatypes.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh mmsC32R8S16HBMint161pchm60n60q60 >> ../scripts/kernels_datatypes.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh ccwwrC32R8S16HBMint161pchi11x11x34o9x9x16k3 >> ../scripts/kernels_datatypes.times

echo "" >> ../scripts/kernels_datatypes.times
echo "----------INT32, S = 8----------" >> ../scripts/kernels_datatypes.times
sed -i "s/#define DATA_TYPE   5/#define DATA_TYPE   6/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC32R8S8HBMint321pchV128n128 EWARW 128 128
bin/map_kernel dpC32R8S8HBMint321pchV128n128 DP 128 128
bin/map_kernel mvmC32R8S8HBMint321pch180x180 MMS 1 180 180
bin/map_kernel mvmC32R8S8HBMint321pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC32R8S8HBMint321pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC32R8S8HBMint321pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh ewarwC32R8S8HBMint321pchV128n128 >> ../scripts/kernels_datatypes.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh dpC32R8S8HBMint321pchV128n128 >> ../scripts/kernels_datatypes.times
echo "MVM 180x180" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh mvmC32R8S8HBMint321pch180x180 >> ../scripts/kernels_datatypes.times
echo "MVM 1024x1024" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh mvmC32R8S8HBMint321pch1024x1024 >> ../scripts/kernels_datatypes.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh mmsC32R8S8HBMint321pchm60n60q60 >> ../scripts/kernels_datatypes.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh ccwwrC32R8S8HBMint321pchi11x11x34o9x9x16k3 >> ../scripts/kernels_datatypes.times

echo "" >> ../scripts/kernels_datatypes.times
echo "----------INT64, S = 4----------" >> ../scripts/kernels_datatypes.times
sed -i "s/#define DATA_TYPE   6/#define DATA_TYPE   7/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC32R8S4HBMint641pchV128n128 EWARW 128 128
bin/map_kernel dpC32R8S4HBMint641pchV128n128 DP 128 128
bin/map_kernel mvmC32R8S4HBMint641pch180x180 MMS 1 180 180
bin/map_kernel mvmC32R8S4HBMint641pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC32R8S4HBMint641pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC32R8S4HBMint641pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh ewarwC32R8S4HBMint641pchV128n128 >> ../scripts/kernels_datatypes.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh dpC32R8S4HBMint641pchV128n128 >> ../scripts/kernels_datatypes.times
echo "MVM 180x180" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh mvmC32R8S4HBMint641pch180x180 >> ../scripts/kernels_datatypes.times
echo "MVM 1024x1024" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh mvmC32R8S4HBMint641pch1024x1024 >> ../scripts/kernels_datatypes.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh mmsC32R8S4HBMint641pchm60n60q60 >> ../scripts/kernels_datatypes.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh ccwwrC32R8S4HBMint641pchi11x11x34o9x9x16k3 >> ../scripts/kernels_datatypes.times

echo "" >> ../scripts/kernels_datatypes.times
echo "----------FP32, S = 8----------" >> ../scripts/kernels_datatypes.times
sed -i "s/#define DATA_TYPE   7/#define DATA_TYPE   1/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC32R8S8HBMfp321pchV128n128 EWARW 128 128
bin/map_kernel dpC32R8S8HBMfp321pchV128n128 DP 128 128
bin/map_kernel mvmC32R8S8HBMfp321pch180x180 MMS 1 180 180
bin/map_kernel mvmC32R8S8HBMfp321pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC32R8S8HBMfp321pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC32R8S8HBMfp321pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh ewarwC32R8S8HBMfp321pchV128n128 >> ../scripts/kernels_datatypes.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh dpC32R8S8HBMfp321pchV128n128 >> ../scripts/kernels_datatypes.times
echo "MVM 180x180" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh mvmC32R8S8HBMfp321pch180x180 >> ../scripts/kernels_datatypes.times
echo "MVM 1024x1024" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh mvmC32R8S8HBMfp321pch1024x1024 >> ../scripts/kernels_datatypes.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh mmsC32R8S8HBMfp321pchm60n60q60 >> ../scripts/kernels_datatypes.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh ccwwrC32R8S8HBMfp321pchi11x11x34o9x9x16k3 >> ../scripts/kernels_datatypes.times

echo "" >> ../scripts/kernels_datatypes.times
echo "----------FP64, S = 4----------" >> ../scripts/kernels_datatypes.times
sed -i "s/#define DATA_TYPE   1/#define DATA_TYPE   2/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
bin/map_kernel ewarwC32R8S4HBMfp641pchV128n128 EWARW 128 128
bin/map_kernel dpC32R8S4HBMfp641pchV128n128 DP 128 128
bin/map_kernel mvmC32R8S4HBMfp641pch180x180 MMS 1 180 180
bin/map_kernel mvmC32R8S4HBMfp641pch1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC32R8S4HBMfp641pchm60n60q60 MMS 60 60 60
bin/map_kernel ccwwrC32R8S4HBMfp641pchi11x11x34o9x9x16k3 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 128 n = 128" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh ewarwC32R8S4HBMfp641pchV128n128 >> ../scripts/kernels_datatypes.times
echo "DP V = 128 n = 128" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh dpC32R8S4HBMfp641pchV128n128 >> ../scripts/kernels_datatypes.times
echo "MVM 180x180" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh mvmC32R8S4HBMfp641pch180x180 >> ../scripts/kernels_datatypes.times
echo "MVM 1024x1024" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh mvmC32R8S4HBMfp641pch1024x1024 >> ../scripts/kernels_datatypes.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh mmsC32R8S4HBMfp641pchm60n60q60 >> ../scripts/kernels_datatypes.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_datatypes.times
./assembly2sc.sh ccwwrC32R8S4HBMfp641pchi11x11x34o9x9x16k3 >> ../scripts/kernels_datatypes.times


# Back to initial state

sed -i "s/configs\/HBM2_AB-config.cfg/configs\/HBM_AB-config.cfg/g" ../inputs/assembly2sc.sh

make -C $HOME/Documents/ramulator-AB/ -j4

sed -i "s/#define DATA_TYPE   2/#define DATA_TYPE   0/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh