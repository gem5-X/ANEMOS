
#!/bin/bash

echo "REMEMEBER ALL HEADER FILES SHOULD START WITH C=32, R=8 and HBM original configuration"

sed -i "s/configs\/HBM_AB-config.cfg/configs\/HBM2_AB-config.cfg/g" ../inputs/assembly2sc.sh

echo "" > ../scripts/kernels_hbm.times
echo "----------C = 32, R = 8, S = 16----------" >> ../scripts/kernels_hbm.times
sed -i "s/#define CORES_PER_PCH   1/#define CORES_PER_PCH   8/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4
bin/map_kernel ewarwC32R8S16HBMV256n256 EWARW 256 256
bin/map_kernel dpC32R8S16HBMV256n256 DP 256 256
bin/map_kernel mvmC32R8S16HBM16x16 MMS 1 16 16
bin/map_kernel mvmC32R8S16HBM32x32 MMS 1 32 32
bin/map_kernel mvmC32R8S16HBM64x64 MMS 1 64 64
bin/map_kernel mvmC32R8S16HBM128x128 MMS 1 128 128
bin/map_kernel mvmC32R8S16HBM256x256 MMS 1 256 256
bin/map_kernel mvmC32R8S16HBM512x512 MMS 1 512 512
bin/map_kernel mvmC32R8S16HBM1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC32R8S16HBMm128n128q128 MMS 128 128 128
bin/map_kernel ccwwrC32R8S16HBMi24x24x32o20x20x32k5 CCWWR 32 24 24 5 1 32 20 20
echo "EWARW V = 256 n = 256" >> ../scripts/kernels_hbm.times
./assembly2sc.sh ewarwC32R8S16HBMV256n256 >> ../scripts/kernels_hbm.times
echo "DP V = 256 n = 256" >> ../scripts/kernels_hbm.times
./assembly2sc.sh dpC32R8S16HBMV256n256 >> ../scripts/kernels_hbm.times
echo "MVM 16x16" >> ../scripts/kernels_hbm.times
./assembly2sc.sh mvmC32R8S16HBM16x16 >> ../scripts/kernels_hbm.times
echo "MVM 32x32" >> ../scripts/kernels_hbm.times
./assembly2sc.sh mvmC32R8S16HBM32x32 >> ../scripts/kernels_hbm.times
echo "MVM 64x64" >> ../scripts/kernels_hbm.times
./assembly2sc.sh mvmC32R8S16HBM64x64 >> ../scripts/kernels_hbm.times
echo "MVM 128x128" >> ../scripts/kernels_hbm.times
./assembly2sc.sh mvmC32R8S16HBM128x128 >> ../scripts/kernels_hbm.times
echo "MVM 256x256" >> ../scripts/kernels_hbm.times
./assembly2sc.sh mvmC32R8S16HBM256x256 >> ../scripts/kernels_hbm.times
echo "MVM 512x512" >> ../scripts/kernels_hbm.times
./assembly2sc.sh mvmC32R8S16HBM512x512 >> ../scripts/kernels_hbm.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbm.times
./assembly2sc.sh mvmC32R8S16HBM1024x1024 >> ../scripts/kernels_hbm.times
echo "MMS m = n = q = 128" >> ../scripts/kernels_hbm.times
./assembly2sc.sh mmsC32R8S16HBMm128n128q128 >> ../scripts/kernels_hbm.times
echo "CCWWR 24x24x32 -> 20x20x32" >> ../scripts/kernels_hbm.times
./assembly2sc.sh ccwwrC32R8S16HBMi24x24x32o20x20x32k5 >> ../scripts/kernels_hbm.times

# echo "" >> ../scripts/kernels_hbm.times
# echo "----------C = 32, R = 8, S = 8----------" >> ../scripts/kernels_hbm.times
# sed -i "s/#define CORES_PER_PCH   8/#define CORES_PER_PCH   16/g" ../src/defs.h
# sed -i "s/#define ROW_BITS        14/#define ROW_BITS        15/g" ../src/defs.h
# sed -i "s/#define SIMD_WIDTH		(256 \/ WORD_BITS)/#define SIMD_WIDTH		(128 \/ WORD_BITS)/g" ../src/defs.h
# # sed -i "s/#define GLOBAL_OFFSET   6/#define GLOBAL_OFFSET   5/g" ../src/defs.h
# cd ../build
# make all
# cd ../inputs
# sed -i "s/{4<<10, 128, {0, 0, 4, 4, 1<<14, 1<<(5+2)}},/{4<<10, 128, {0, 0, 4, 4, 1<<15, 1<<(5+2)}},/g" $HOME/Documents/ramulator-AB/src/HBM_AB.h
# ./compile_all.sh
# make -C $HOME/Documents/ramulator-AB/ -j4
# bin/map_kernel ewarwC32R8S8HBMV256n256 EWARW 256 256
# bin/map_kernel dpC32R8S8HBMV256n256 DP 256 256
# bin/map_kernel mvmC32R8S8HBM64x64 MMS 1 64 64
# bin/map_kernel mvmC32R8S8HBM128x128 MMS 1 128 128
# bin/map_kernel mvmC32R8S8HBM1024x1024 MMS 1 1024 1024
# bin/map_kernel mmsC32R8S8HBMm128n128q128 MMS 128 128 128
# bin/map_kernel ccwwrC32R8S8HBMi24x24x32o20x20x32k5 CCWWR 32 24 24 5 1 32 20 20
# echo "EWARW V = 256 n = 256" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh ewarwC32R8S8HBMV256n256 >> ../scripts/kernels_hbm.times
# echo "DP V = 256 n = 256" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh dpC32R8S8HBMV256n256 >> ../scripts/kernels_hbm.times
# echo "MVM 64x64" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh mvmC32R8S8HBM64x64 >> ../scripts/kernels_hbm.times
# echo "MVM 128x128" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh mvmC32R8S8HBM128x128 >> ../scripts/kernels_hbm.times
# echo "MVM 1024x1024" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh mvmC32R8S8HBM1024x1024 >> ../scripts/kernels_hbm.times
# echo "MMS m = n = q = 128" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh mmsC32R8S8HBMm128n128q128 >> ../scripts/kernels_hbm.times
# echo "CCWWR 24x24x32 -> 20x20x32" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh ccwwrC32R8S8HBMi24x24x32o20x20x32k5 >> ../scripts/kernels_hbm.times

# echo "" >> ../scripts/kernels_hbm.times
# echo "----------C = 32, R = 8, S = 32----------" >> ../scripts/kernels_hbm.times
# sed -i "s/#define CORES_PER_PCH   16/#define CORES_PER_PCH   4/g" ../src/defs.h
# sed -i "s/#define ROW_BITS        15/#define ROW_BITS        13/g" ../src/defs.h
# sed -i "s/#define SIMD_WIDTH		(128 \/ WORD_BITS)/#define SIMD_WIDTH		(512 \/ WORD_BITS)/g" ../src/defs.h
# # sed -i "s/#define GLOBAL_OFFSET   6/#define GLOBAL_OFFSET   5/g" ../src/defs.h
# cd ../build
# make all
# cd ../inputs
# sed -i "s/{4<<10, 128, {0, 0, 4, 4, 1<<15, 1<<(5+2)}},/{4<<10, 128, {0, 0, 4, 4, 1<<13, 1<<(5+2)}},/g" $HOME/Documents/ramulator-AB/src/HBM_AB.h
# ./compile_all.sh
# make -C $HOME/Documents/ramulator-AB/ -j4
# bin/map_kernel ewarwC32R8S32HBMV256n256 EWARW 256 256
# bin/map_kernel dpC32R8S32HBMV256n256 DP 256 256
# bin/map_kernel mvmC32R8S32HBM64x64 MMS 1 64 64
# bin/map_kernel mvmC32R8S32HBM128x128 MMS 1 128 128
# bin/map_kernel mvmC32R8S32HBM1024x1024 MMS 1 1024 1024
# bin/map_kernel mmsC32R8S32HBMm128n128q128 MMS 128 128 128
# bin/map_kernel ccwwrC32R8S32HBMi24x24x32o20x20x32k5 CCWWR 32 24 24 5 1 32 20 20
# echo "EWARW V = 256 n = 256" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh ewarwC32R8S32HBMV256n256 >> ../scripts/kernels_hbm.times
# echo "DP V = 256 n = 256" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh dpC32R8S32HBMV256n256 >> ../scripts/kernels_hbm.times
# echo "MVM 64x64" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh mvmC32R8S32HBM64x64 >> ../scripts/kernels_hbm.times
# echo "MVM 128x128" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh mvmC32R8S32HBM128x128 >> ../scripts/kernels_hbm.times
# echo "MVM 1024x1024" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh mvmC32R8S32HBM1024x1024 >> ../scripts/kernels_hbm.times
# echo "MMS m = n = q = 128" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh mmsC32R8S32HBMm128n128q128 >> ../scripts/kernels_hbm.times
# echo "CCWWR 24x24x32 -> 20x20x32" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh ccwwrC32R8S32HBMi24x24x32o20x20x32k5 >> ../scripts/kernels_hbm.times
# echo "" >> ../scripts/kernels_hbm.times

# echo "----------C = 32, R = 8, S = 64----------" >> ../scripts/kernels_hbm.times
# sed -i "s/#define CORES_PER_PCH   4/#define CORES_PER_PCH   2/g" ../src/defs.h
# sed -i "s/#define ROW_BITS        13/#define ROW_BITS        12/g" ../src/defs.h
# sed -i "s/#define SIMD_WIDTH		(512 \/ WORD_BITS)/#define SIMD_WIDTH		(1024 \/ WORD_BITS)/g" ../src/defs.h
# # sed -i "s/#define GLOBAL_OFFSET   6/#define GLOBAL_OFFSET   5/g" ../src/defs.h
# cd ../build
# make all
# cd ../inputs
# sed -i "s/{4<<10, 128, {0, 0, 4, 4, 1<<13, 1<<(5+2)}},/{4<<10, 128, {0, 0, 4, 4, 1<<12, 1<<(5+2)}},/g" $HOME/Documents/ramulator-AB/src/HBM_AB.h
# ./compile_all.sh
# make -C $HOME/Documents/ramulator-AB/ -j4
# bin/map_kernel ewarwC32R8S64HBMV256n256 EWARW 256 256
# bin/map_kernel dpC32R8S64HBMV256n256 DP 256 256
# bin/map_kernel mvmC32R8S64HBM64x64 MMS 1 64 64
# bin/map_kernel mvmC32R8S64HBM128x128 MMS 1 128 128
# bin/map_kernel mvmC32R8S64HBM1024x1024 MMS 1 1024 1024
# bin/map_kernel mmsC32R8S64HBMm128n128q128 MMS 128 128 128
# bin/map_kernel ccwwrC32R8S64HBMi24x24x32o20x20x32k5 CCWWR 32 24 24 5 1 32 20 20
# echo "EWARW V = 256 n = 256" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh ewarwC32R8S64HBMV256n256 >> ../scripts/kernels_hbm.times
# echo "DP V = 256 n = 256" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh dpC32R8S64HBMV256n256 >> ../scripts/kernels_hbm.times
# echo "MVM 64x64" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh mvmC32R8S64HBM64x64 >> ../scripts/kernels_hbm.times
# echo "MVM 128x128" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh mvmC32R8S64HBM128x128 >> ../scripts/kernels_hbm.times
# echo "MVM 1024x1024" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh mvmC32R8S64HBM1024x1024 >> ../scripts/kernels_hbm.times
# echo "MMS m = n = q = 128" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh mmsC32R8S64HBMm128n128q128 >> ../scripts/kernels_hbm.times
# echo "CCWWR 24x24x32 -> 20x20x32" >> ../scripts/kernels_hbm.times
# ./assembly2sc.sh ccwwrC32R8S64HBMi24x24x32o20x20x32k5 >> ../scripts/kernels_hbm.times

sed -i "s/configs\/HBM2_AB-config.cfg/configs\/HBM_AB-config.cfg/g" ../inputs/assembly2sc.sh


# Back to initial state
sed -i "s/#define CORES_PER_PCH   8/#define CORES_PER_PCH   1/g" ../src/defs.h
sed -i "s/#define ROW_BITS        12/#define ROW_BITS        14/g" ../src/defs.h
sed -i "s/#define SIMD_WIDTH		(1024 \/ WORD_BITS)/#define SIMD_WIDTH		(256 \/ WORD_BITS)/g" ../src/defs.h
# sed -i "s/#define GLOBAL_OFFSET   5/#define GLOBAL_OFFSET   6/g" ../src/defs.h
cd ../build
make all
cd ../inputs
sed -i "s/{4<<10, 128, {0, 0, 4, 4, 1<<12, 1<<(5+2)}},/{4<<10, 128, {0, 0, 4, 4, 1<<14, 1<<(5+2)}},/g" $HOME/Documents/ramulator-AB/src/HBM_AB.h
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4