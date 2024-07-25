
#!/bin/bash

echo "REMEMEBER ALL HEADER FILES SHOULD START WITH C=32, R=8 and HBM original configuration"

sed -i "s/configs\/HBM_AB-config.cfg/configs\/GDDR5_AB-config.cfg/g" ../inputs/assembly2sc.sh
sed -i "s/#define CLK_PERIOD 3333/#define CLK_PERIOD 1000/g" ../src/defs.h

echo "" > ../scripts/kernels_gddr5.times
echo "----------C = 32, R = 8, S = 16----------" >> ../scripts/kernels_gddr5.times
sed -i "s/#define CORES_PER_PCH   1/#define CORES_PER_PCH   8/g" ../src/defs.h
sed -i "s/#define SIMD_WIDTH		(256 \/ WORD_BITS)/#define SIMD_WIDTH		(256 \/ WORD_BITS)/g" ../src/defs.h
sed -i "s/#define DQ_BITS         64/#define DQ_BITS         32/g" ../src/defs.h
sed -i "s/#define ROW_BITS        14/#define ROW_BITS        14/g" ../src/defs.h
sed -i "s/#define COL_BITS        5/#define COL_BITS        7/g" ../src/defs.h
# sed -i "s/#define GLOBAL_OFFSET   6/#define GLOBAL_OFFSET   4/g" ../src/defs.h
cd ../build
make all
cd ../inputs
sed -i "s/{4<<10, 16, {0, 1, 4, 4, 1<<14, 1<<(7+3)}},/{4<<10, 16, {0, 1, 4, 4, 1<<14, 1<<(7+3)}},/g" $HOME/Documents/ramulator-AB/src/GDDR5_AB.h
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4
bin/map_kernel ewarwC32R8S16GDDR5V256n256 EWARW 256 256
bin/map_kernel dpC32R8S16GDDR5V256n256 DP 256 256
bin/map_kernel mvmC32R8S16GDDR532x32 MMS 1 32 32
bin/map_kernel mvmC32R8S16GDDR564x64 MMS 1 64 64
bin/map_kernel mvmC32R8S16GDDR5128x128 MMS 1 128 128
bin/map_kernel mvmC32R8S16GDDR5256x256 MMS 1 256 256
bin/map_kernel mvmC32R8S16GDDR5512x512 MMS 1 512 512
bin/map_kernel mvmC32R8S16GDDR51024x1024 MMS 1 1024 1024
bin/map_kernel mmsC32R8S16GDDR5m128n128q128 MMS 128 128 128
bin/map_kernel ccwwrC32R8S16GDDR5i24x24x32o20x20x32k5 CCWWR 32 24 24 5 1 32 20 20
echo "EWARW V = 256 n = 256" >> ../scripts/kernels_gddr5.times
./assembly2sc.sh ewarwC32R8S16GDDR5V256n256 >> ../scripts/kernels_gddr5.times
echo "DP V = 256 n = 256" >> ../scripts/kernels_gddr5.times
./assembly2sc.sh dpC32R8S16GDDR5V256n256 >> ../scripts/kernels_gddr5.times
echo "MVM 32x32" >> ../scripts/kernels_gddr5.times
./assembly2sc.sh mvmC32R8S16GDDR532x32 >> ../scripts/kernels_gddr5.times
echo "MVM 64x64" >> ../scripts/kernels_gddr5.times
./assembly2sc.sh mvmC32R8S16GDDR564x64 >> ../scripts/kernels_gddr5.times
echo "MVM 128x128" >> ../scripts/kernels_gddr5.times
./assembly2sc.sh mvmC32R8S16GDDR5128x128 >> ../scripts/kernels_gddr5.times
echo "MVM 256x256" >> ../scripts/kernels_gddr5.times
./assembly2sc.sh mvmC32R8S16GDDR5256x256 >> ../scripts/kernels_gddr5.times
echo "MVM 512x512" >> ../scripts/kernels_gddr5.times
./assembly2sc.sh mvmC32R8S16GDDR5512x512 >> ../scripts/kernels_gddr5.times
echo "MVM 1024x1024" >> ../scripts/kernels_gddr5.times
./assembly2sc.sh mvmC32R8S16GDDR51024x1024 >> ../scripts/kernels_gddr5.times
echo "MMS m = n = q = 128" >> ../scripts/kernels_gddr5.times
./assembly2sc.sh mmsC32R8S16GDDR5m128n128q128 >> ../scripts/kernels_gddr5.times
echo "CCWWR 24x24x32 -> 20x20x32" >> ../scripts/kernels_gddr5.times
./assembly2sc.sh ccwwrC32R8S16GDDR5i24x24x32o20x20x32k5 >> ../scripts/kernels_gddr5.times

# echo "" >> ../scripts/kernels_gddr5.times
# echo "----------C = 32, R = 8, S = 8----------" >> ../scripts/kernels_gddr5.times
# sed -i "s/#define CORES_PER_PCH   8/#define CORES_PER_PCH   4/g" ../src/defs.h
# sed -i "s/#define SIMD_WIDTH		(64 \/ WORD_BITS)/#define SIMD_WIDTH		(128 \/ WORD_BITS)/g" ../src/defs.h
# sed -i "s/#define ROW_BITS        14/#define ROW_BITS        13/g" ../src/defs.h
# cd ../build
# make all
# cd ../inputs
# sed -i "s/{4<<10, 16, {0, 1, 4, 4, 1<<14, 1<<(7+3)}},/{4<<10, 16, {0, 1, 4, 4, 1<<13, 1<<(7+3)}},/g" $HOME/Documents/ramulator-AB/src/GDDR5_AB.h
# ./compile_all.sh
# make -C $HOME/Documents/ramulator-AB/ -j4
# bin/map_kernel ewarwC32R8S8GDDR5V256n256 EWARW 256 256
# bin/map_kernel dpC32R8S8GDDR5V256n256 DP 256 256
# bin/map_kernel mvmC32R8S8GDDR564x64 MMS 1 64 64
# bin/map_kernel mvmC32R8S8GDDR5128x128 MMS 1 128 128
# bin/map_kernel mvmC32R8S8GDDR51024x1024 MMS 1 1024 1024
# bin/map_kernel mmsC32R8S8GDDR5m128n128q128 MMS 128 128 128
# bin/map_kernel ccwwrC32R8S8GDDR5i24x24x32o20x20x32k5 CCWWR 32 24 24 5 1 32 20 20
# echo "EWARW V = 256 n = 256" >> ../scripts/kernels_gddr5.times
# ./assembly2sc.sh ewarwC32R8S8GDDR5V256n256 >> ../scripts/kernels_gddr5.times
# echo "DP V = 256 n = 256" >> ../scripts/kernels_gddr5.times
# ./assembly2sc.sh dpC32R8S8GDDR5V256n256 >> ../scripts/kernels_gddr5.times
# echo "MVM 64x64" >> ../scripts/kernels_gddr5.times
# ./assembly2sc.sh mvmC32R8S8GDDR564x64 >> ../scripts/kernels_gddr5.times
# echo "MVM 128x128" >> ../scripts/kernels_gddr5.times
# ./assembly2sc.sh mvmC32R8S8GDDR5128x128 >> ../scripts/kernels_gddr5.times
# echo "MVM 1024x1024" >> ../scripts/kernels_gddr5.times
# ./assembly2sc.sh mvmC32R8S8GDDR51024x1024 >> ../scripts/kernels_gddr5.times
# echo "MMS m = n = q = 128" >> ../scripts/kernels_gddr5.times
# ./assembly2sc.sh mmsC32R8S8GDDR5m128n128q128 >> ../scripts/kernels_gddr5.times
# echo "CCWWR 24x24x32 -> 20x20x32" >> ../scripts/kernels_gddr5.times
# ./assembly2sc.sh ccwwrC32R8S8GDDR5i24x24x32o20x20x32k5 >> ../scripts/kernels_gddr5.times

# echo "" >> ../scripts/kernels_gddr5.times
# echo "----------C = 32, R = 8, S = 16----------" >> ../scripts/kernels_gddr5.times
# sed -i "s/#define CORES_PER_PCH   4/#define CORES_PER_PCH   2/g" ../src/defs.h
# sed -i "s/#define SIMD_WIDTH		(128 \/ WORD_BITS)/#define SIMD_WIDTH		(256 \/ WORD_BITS)/g" ../src/defs.h
# sed -i "s/#define ROW_BITS        13/#define ROW_BITS        12/g" ../src/defs.h
# cd ../build
# make all
# cd ../inputs
# sed -i "s/{4<<10, 16, {0, 1, 4, 4, 1<<13, 1<<(7+3)}},/{4<<10, 16, {0, 1, 4, 4, 1<<12, 1<<(7+3)}},/g" $HOME/Documents/ramulator-AB/src/GDDR5_AB.h
# ./compile_all.sh
# make -C $HOME/Documents/ramulator-AB/ -j4
# bin/map_kernel ewarwC32R8S16GDDR5V256n256 EWARW 256 256
# bin/map_kernel dpC32R8S16GDDR5V256n256 DP 256 256
# bin/map_kernel mvmC32R8S16GDDR564x64 MMS 1 64 64
# bin/map_kernel mvmC32R8S16GDDR5128x128 MMS 1 128 128
# bin/map_kernel mvmC32R8S16GDDR51024x1024 MMS 1 1024 1024
# bin/map_kernel mmsC32R8S16GDDR5m128n128q128 MMS 128 128 128
# bin/map_kernel ccwwrC32R8S16GDDR5i24x24x32o20x20x32k5 CCWWR 32 24 24 5 1 32 20 20
# echo "EWARW V = 256 n = 256" >> ../scripts/kernels_gddr5.times
# ./assembly2sc.sh ewarwC32R8S16GDDR5V256n256 >> ../scripts/kernels_gddr5.times
# echo "DP V = 256 n = 256" >> ../scripts/kernels_gddr5.times
# ./assembly2sc.sh dpC32R8S16GDDR5V256n256 >> ../scripts/kernels_gddr5.times
# echo "MVM 64x64" >> ../scripts/kernels_gddr5.times
# ./assembly2sc.sh mvmC32R8S16GDDR564x64 >> ../scripts/kernels_gddr5.times
# echo "MVM 128x128" >> ../scripts/kernels_gddr5.times
# ./assembly2sc.sh mvmC32R8S16GDDR5128x128 >> ../scripts/kernels_gddr5.times
# echo "MVM 1024x1024" >> ../scripts/kernels_gddr5.times
# ./assembly2sc.sh mvmC32R8S16GDDR51024x1024 >> ../scripts/kernels_gddr5.times
# echo "MMS m = n = q = 128" >> ../scripts/kernels_gddr5.times
# ./assembly2sc.sh mmsC32R8S16GDDR5m128n128q128 >> ../scripts/kernels_gddr5.times
# echo "CCWWR 24x24x32 -> 20x20x32" >> ../scripts/kernels_gddr5.times
# ./assembly2sc.sh ccwwrC32R8S16GDDR5i24x24x32o20x20x32k5 >> ../scripts/kernels_gddr5.times
# echo "" >> ../scripts/kernels_gddr5.times

# # echo "----------C = 32, R = 8, S = 64----------" >> ../scripts/kernels_gddr5.times
# # sed -i "s/#define CORES_PER_PCH   2/#define CORES_PER_PCH   1/g" ../src/defs.h
# # sed -i "s/#define SIMD_WIDTH		512 \/ WORD_BITS/#define SIMD_WIDTH		1024 \/ WORD_BITS/g" ../src/defs.h
# # sed -i "s/#define COL_BITS        6/#define COL_BITS        5/g" ../src/defs.h
# # cd ../build
# # make all
# # cd ../inputs
# # sed -i "s/{4<<10, 128, {0, 0, 4, 4, 1<<13, 1<<(6+3)}},/{4<<10, 128, {0, 0, 4, 4, 1<<13, 1<<(5+3)}},/g" $HOME/Documents/ramulator-AB/src/HBM_AB.h
# # ./compile_all.sh
# # make -C $HOME/Documents/ramulator-AB/ -j4
# # bin/map_kernel ewarwC32R8S64V256n256 EWARW 256 256
# # bin/map_kernel dpC32R8S64V256n256 DP 256 256
# # bin/map_kernel mvmC32R8S641024x1024 MMS 1 1024 1024
# # bin/map_kernel mmsC32R8S64m128n128q128 MMS 128 128 128
# # bin/map_kernel ccwwrC32R8S64i24x24x32o20x20x32k5 CCWWR 32 24 24 5 1 32 20 20
# # echo "EWARW V = 256 n = 256" >> ../scripts/kernels_gddr5.times
# # ./assembly2sc.sh ewarwC32R8S64V256n256 >> ../scripts/kernels_gddr5.times
# # echo "DP V = 256 n = 256" >> ../scripts/kernels_gddr5.times
# # ./assembly2sc.sh dpC32R8S64V256n256 >> ../scripts/kernels_gddr5.times
# # echo "MVM 1024x1024" >> ../scripts/kernels_gddr5.times
# # ./assembly2sc.sh mvmC32R8S641024x1024 >> ../scripts/kernels_gddr5.times
# # echo "MMS m = n = q = 128" >> ../scripts/kernels_gddr5.times
# # ./assembly2sc.sh mmsC32R8S64m128n128q128 >> ../scripts/kernels_gddr5.times
# # echo "CCWWR 24x24x32 -> 20x20x32" >> ../scripts/kernels_gddr5.times
# # ./assembly2sc.sh ccwwrC32R8S64i24x24x32o20x20x32k5 >> ../scripts/kernels_gddr5.times

sed -i "s/configs\/GDDR5_AB-config.cfg/configs\/HBM_AB-config.cfg/g" ../inputs/assembly2sc.sh
sed -i "s/#define CLK_PERIOD 1000/#define CLK_PERIOD 3333/g" ../src/defs.h


# Back to initial state
sed -i "s/#define CORES_PER_PCH   8/#define CORES_PER_PCH   1/g" ../src/defs.h
sed -i "s/#define SIMD_WIDTH		(256 \/ WORD_BITS)/#define SIMD_WIDTH		(256 \/ WORD_BITS)/g" ../src/defs.h
sed -i "s/#define DQ_BITS         32/#define DQ_BITS         64/g" ../src/defs.h
sed -i "s/#define COL_BITS        7/#define COL_BITS        5/g" ../src/defs.h
sed -i "s/#define ROW_BITS        14/#define ROW_BITS        14/g" ../src/defs.h
# sed -i "s/#define GLOBAL_OFFSET   4/#define GLOBAL_OFFSET   6/g" ../src/defs.h
cd ../build
make all
cd ../inputs
sed -i "s/{4<<10, 16, {0, 1, 4, 4, 1<<12, 1<<(7+3)}},/{4<<10, 16, {0, 1, 4, 4, 1<<14, 1<<(7+3)}},/g" $HOME/Documents/ramulator-AB/src/GDDR5_AB.h
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4