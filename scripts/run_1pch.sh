
#!/bin/bash

echo "REMEMEBER ALL HEADER FILES SHOULD START WITH C=32, R=8 and HBM original configuration"

echo "" > ../scripts/kernels_1pch.times
echo "----------C = 32, R = 8, S = 16, HBM----------" >> ../scripts/kernels_1pch.times
# sed -i "s/#define CORES_PER_PCH   1/#define CORES_PER_PCH   4/g" ../src/defs.h
cd ../Debug
make all
cd ../inputs
# sed -i "s/#define CORES_PER_PCH   1/#define CORES_PER_PCH   4/g" map_kernel.h
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4
./map_kernel ewarw1pchS16HBM EWARW 100 100
./map_kernel dp1pchS16HBM DP 100 100
./map_kernel mvm1pchS16HBM MMS 1 1024 1024
./map_kernel mms1pchS16HBM MMS 60 60 60
./map_kernel ccwwr1pchS16HBM CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ewarw1pchS16HBM >> ../scripts/kernels_1pch.times
echo "DP V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh dp1pchS16HBM >> ../scripts/kernels_1pch.times
echo "MVM " >> ../scripts/kernels_1pch.times
./assembly2sc.sh mvm1pchS16HBM >> ../scripts/kernels_1pch.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_1pch.times
./assembly2sc.sh mms1pchS16HBM >> ../scripts/kernels_1pch.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ccwwr1pchS16HBM >> ../scripts/kernels_1pch.times

echo "" >> ../scripts/kernels_1pch.times
echo "----------C = 32, R = 8, S = 8, HBM----------" >> ../scripts/kernels_1pch.times
# sed -i "s/#define CORES_PER_PCH   16/#define CORES_PER_PCH   32/g" ../src/defs.h
sed -i "s/#define ROW_BITS        14/#define ROW_BITS        15/g" ../src/defs.h
sed -i "s/#define SIMD_WIDTH		(256 \/ WORD_BITS)/#define SIMD_WIDTH		(128 \/ WORD_BITS)/g" ../src/defs.h
# sed -i "s/#define GLOBAL_OFFSET   6/#define GLOBAL_OFFSET   5/g" ../src/defs.h
cd ../Debug
make all
cd ../inputs
sed -i "s/{4<<10, 128, {0, 0, 4, 4, 1<<14, 1<<(5+2)}},/{4<<10, 128, {0, 0, 4, 4, 1<<15, 1<<(5+2)}},/g" $HOME/Documents/ramulator-AB/src/HBM_AB.h
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4
make -C $HOME/Documents/ramulator-AB/ -j4
./map_kernel ewarw1pchS8HBM EWARW 100 100
./map_kernel dp1pchS8HBM DP 100 100
./map_kernel mvm1pchS8HBM MMS 1 1024 1024
./map_kernel mms1pchS8HBM MMS 60 60 60
./map_kernel ccwwr1pchS8HBM CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ewarw1pchS8HBM >> ../scripts/kernels_1pch.times
echo "DP V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh dp1pchS8HBM >> ../scripts/kernels_1pch.times
echo "MVM " >> ../scripts/kernels_1pch.times
./assembly2sc.sh mvm1pchS8HBM >> ../scripts/kernels_1pch.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_1pch.times
./assembly2sc.sh mms1pchS8HBM >> ../scripts/kernels_1pch.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ccwwr1pchS8HBM >> ../scripts/kernels_1pch.times

echo "" >> ../scripts/kernels_1pch.times
echo "----------C = 32, R = 8, S = 32, HBM----------" >> ../scripts/kernels_1pch.times
# sed -i "s/#define CORES_PER_PCH   32/#define CORES_PER_PCH   8/g" ../src/defs.h
sed -i "s/#define ROW_BITS        15/#define ROW_BITS        13/g" ../src/defs.h
sed -i "s/#define SIMD_WIDTH		(128 \/ WORD_BITS)/#define SIMD_WIDTH		(512 \/ WORD_BITS)/g" ../src/defs.h
# sed -i "s/#define GLOBAL_OFFSET   6/#define GLOBAL_OFFSET   5/g" ../src/defs.h
cd ../Debug
make all
cd ../inputs
sed -i "s/{4<<10, 128, {0, 0, 4, 4, 1<<15, 1<<(5+2)}},/{4<<10, 128, {0, 0, 4, 4, 1<<13, 1<<(5+2)}},/g" $HOME/Documents/ramulator-AB/src/HBM_AB.h
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4
./map_kernel ewarw1pchS32HBM EWARW 100 100
./map_kernel dp1pchS32HBM DP 100 100
./map_kernel mvm1pchS32HBM MMS 1 1024 1024
./map_kernel mms1pchS32HBM MMS 60 60 60
./map_kernel ccwwr1pchS32HBM CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ewarw1pchS32HBM >> ../scripts/kernels_1pch.times
echo "DP V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh dp1pchS32HBM >> ../scripts/kernels_1pch.times
echo "MVM " >> ../scripts/kernels_1pch.times
./assembly2sc.sh mvm1pchS32HBM >> ../scripts/kernels_1pch.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_1pch.times
./assembly2sc.sh mms1pchS32HBM >> ../scripts/kernels_1pch.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ccwwr1pchS32HBM >> ../scripts/kernels_1pch.times
echo "" >> ../scripts/kernels_1pch.times

echo "----------C = 32, R = 8, S = 64, HBM----------" >> ../scripts/kernels_1pch.times
# sed -i "s/#define CORES_PER_PCH   8/#define CORES_PER_PCH   4/g" ../src/defs.h
sed -i "s/#define ROW_BITS        13/#define ROW_BITS        12/g" ../src/defs.h
sed -i "s/#define SIMD_WIDTH		(512 \/ WORD_BITS)/#define SIMD_WIDTH		(1024 \/ WORD_BITS)/g" ../src/defs.h
# sed -i "s/#define GLOBAL_OFFSET   6/#define GLOBAL_OFFSET   5/g" ../src/defs.h
cd ../Debug
make all
cd ../inputs
sed -i "s/{4<<10, 128, {0, 0, 4, 4, 1<<13, 1<<(5+2)}},/{4<<10, 128, {0, 0, 4, 4, 1<<12, 1<<(5+2)}},/g" $HOME/Documents/ramulator-AB/src/HBM_AB.h
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4
./map_kernel ewarw1pchS64HBM EWARW 100 100
./map_kernel dp1pchS64HBM DP 100 100
./map_kernel mvm1pchS64HBM MMS 1 1024 1024
./map_kernel mms1pchS64HBM MMS 60 60 60
./map_kernel ccwwr1pchS64HBM CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ewarw1pchS64HBM >> ../scripts/kernels_1pch.times
echo "DP V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh dp1pchS64HBM >> ../scripts/kernels_1pch.times
echo "MVM " >> ../scripts/kernels_1pch.times
./assembly2sc.sh mvm1pchS64HBM >> ../scripts/kernels_1pch.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_1pch.times
./assembly2sc.sh mms1pchS64HBM >> ../scripts/kernels_1pch.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ccwwr1pchS64HBM >> ../scripts/kernels_1pch.times
echo "" >> ../scripts/kernels_1pch.times

sed -i "s/configs\/HBM_AB-config.cfg/configs\/DDR4_AB-config.cfg/g" ../inputs/assembly2sc.sh
sed -i "s/#define CLK_PERIOD 3333/#define CLK_PERIOD 1250/g" ../src/defs.sh

echo "----------C = 32, R = 8, S = 4, DDR4----------" >> ../scripts/kernels_1pch.times
# sed -i "s/#define CORES_PER_PCH   4/#define CORES_PER_PCH   8/g" ../src/defs.h
sed -i "s/#define SIMD_WIDTH		(1024 \/ WORD_BITS)/#define SIMD_WIDTH		(64 \/ WORD_BITS)/g" ../src/defs.h
sed -i "s/#define ROW_BITS        12/#define ROW_BITS        15/g" ../src/defs.h
sed -i "s/#define COL_BITS        5/#define COL_BITS        10/g" ../src/defs.h
# sed -i "s/#define GLOBAL_OFFSET   6/#define GLOBAL_OFFSET   5/g" ../src/defs.h
cd ../Debug
make all
cd ../inputs
sed -i "s/{4<<10,  8, {0, 0, 4, 4, 1<<15, 1<<10}},/{4<<10,  8, {0, 0, 4, 4, 1<<15, 1<<(10+3)}},/g" $HOME/Documents/ramulator-AB/src/DDR4_AB.h
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4
./map_kernel ewarw1pchS4DDR4 EWARW 100 100
./map_kernel dp1pchS4DDR4 DP 100 100
./map_kernel mvm1pchS4DDR4 MMS 1 1024 1024
./map_kernel mms1pchS4DDR4 MMS 60 60 60
./map_kernel ccwwr1pchS4DDR4 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ewarw1pchS4DDR4 >> ../scripts/kernels_1pch.times
echo "DP V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh dp1pchS4DDR4 >> ../scripts/kernels_1pch.times
echo "MVM " >> ../scripts/kernels_1pch.times
./assembly2sc.sh mvm1pchS4DDR4 >> ../scripts/kernels_1pch.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_1pch.times
./assembly2sc.sh mms1pchS4DDR4 >> ../scripts/kernels_1pch.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ccwwr1pchS4DDR4 >> ../scripts/kernels_1pch.times
echo "" >> ../scripts/kernels_1pch.times

echo "----------C = 32, R = 8, S = 8, DDR4----------" >> ../scripts/kernels_1pch.times
# sed -i "s/#define CORES_PER_PCH   4/#define CORES_PER_PCH   8/g" ../src/defs.h
sed -i "s/#define SIMD_WIDTH		(64 \/ WORD_BITS)/#define SIMD_WIDTH		(128 \/ WORD_BITS)/g" ../src/defs.h
sed -i "s/#define ROW_BITS        15/#define ROW_BITS        14/g" ../src/defs.h
# sed -i "s/#define GLOBAL_OFFSET   6/#define GLOBAL_OFFSET   5/g" ../src/defs.h
cd ../Debug
make all
cd ../inputs
sed -i "s/{4<<10,  8, {0, 0, 4, 4, 1<<15, 1<<(10+3)}},/{4<<10,  8, {0, 0, 4, 4, 1<<14, 1<<(10+3)}},/g" $HOME/Documents/ramulator-AB/src/DDR4_AB.h
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4
./map_kernel ewarw1pchS8DDR4 EWARW 100 100
./map_kernel dp1pchS8DDR4 DP 100 100
./map_kernel mvm1pchS8DDR4 MMS 1 1024 1024
./map_kernel mms1pchS8DDR4 MMS 60 60 60
./map_kernel ccwwr1pchS8DDR4 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ewarw1pchS8DDR4 >> ../scripts/kernels_1pch.times
echo "DP V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh dp1pchS8DDR4 >> ../scripts/kernels_1pch.times
echo "MVM " >> ../scripts/kernels_1pch.times
./assembly2sc.sh mvm1pchS8DDR4 >> ../scripts/kernels_1pch.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_1pch.times
./assembly2sc.sh mms1pchS8DDR4 >> ../scripts/kernels_1pch.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ccwwr1pchS8DDR4 >> ../scripts/kernels_1pch.times
echo "" >> ../scripts/kernels_1pch.times

echo "----------C = 32, R = 8, S = 16, DDR4----------" >> ../scripts/kernels_1pch.times
# sed -i "s/#define CORES_PER_PCH   4/#define CORES_PER_PCH   8/g" ../src/defs.h
sed -i "s/#define SIMD_WIDTH		(128 \/ WORD_BITS)/#define SIMD_WIDTH		(256 \/ WORD_BITS)/g" ../src/defs.h
sed -i "s/#define ROW_BITS        14/#define ROW_BITS        13/g" ../src/defs.h
# sed -i "s/#define GLOBAL_OFFSET   6/#define GLOBAL_OFFSET   5/g" ../src/defs.h
cd ../Debug
make all
cd ../inputs
sed -i "s/{4<<10,  8, {0, 0, 4, 4, 1<<14, 1<<(10+3)}},/{4<<10,  8, {0, 0, 4, 4, 1<<13, 1<<(10+3)}},/g" $HOME/Documents/ramulator-AB/src/DDR4_AB.h
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4
./map_kernel ewarw1pchS16DDR4 EWARW 100 100
./map_kernel dp1pchS16DDR4 DP 100 100
./map_kernel mvm1pchS16DDR4 MMS 1 1024 1024
./map_kernel mms1pchS16DDR4 MMS 60 60 60
./map_kernel ccwwr1pchS16DDR4 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ewarw1pchS16DDR4 >> ../scripts/kernels_1pch.times
echo "DP V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh dp1pchS16DDR4 >> ../scripts/kernels_1pch.times
echo "MVM " >> ../scripts/kernels_1pch.times
./assembly2sc.sh mvm1pchS16DDR4 >> ../scripts/kernels_1pch.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_1pch.times
./assembly2sc.sh mms1pchS16DDR4 >> ../scripts/kernels_1pch.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ccwwr1pchS16DDR4 >> ../scripts/kernels_1pch.times
echo "" >> ../scripts/kernels_1pch.times

sed -i "s/configs\/DDR4_AB-config.cfg/configs\/GDDR5_AB-config.cfg/g" ../inputs/assembly2sc.sh
sed -i "s/#define CLK_PERIOD 1250/#define CLK_PERIOD 1000/g" ../src/defs.sh

echo "----------C = 32, R = 8, S = 4, GDDR5----------" >> ../scripts/kernels_1pch.times
# sed -i "s/#define CORES_PER_PCH   4/#define CORES_PER_PCH   8/g" ../src/defs.h
sed -i "s/#define SIMD_WIDTH		(256 \/ WORD_BITS)/#define SIMD_WIDTH		(64 \/ WORD_BITS)/g" ../src/defs.h
sed -i "s/#define ROW_BITS        13/#define ROW_BITS        14/g" ../src/defs.h
sed -i "s/#define COL_BITS        10/#define COL_BITS        7/g" ../src/defs.h
# sed -i "s/#define GLOBAL_OFFSET   6/#define GLOBAL_OFFSET   5/g" ../src/defs.h
cd ../Debug
make all
cd ../inputs
sed -i "s/{4<<10, 16, {0, 1, 4, 4, 1<<14, 1<<(7+3)}},/{4<<10, 16, {0, 1, 4, 4, 1<<14, 1<<(7+3)}},/g" $HOME/Documents/ramulator-AB/src/GDDR5_AB.h
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4
./map_kernel ewarw1pchS4GDDR5 EWARW 100 100
./map_kernel dp1pchS4GDDR5 DP 100 100
./map_kernel mvm1pchS4GDDR5 MMS 1 1024 1024
./map_kernel mms1pchS4GDDR5 MMS 60 60 60
./map_kernel ccwwr1pchS4GDDR5 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ewarw1pchS4GDDR5 >> ../scripts/kernels_1pch.times
echo "DP V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh dp1pchS4GDDR5 >> ../scripts/kernels_1pch.times
echo "MVM " >> ../scripts/kernels_1pch.times
./assembly2sc.sh mvm1pchS4GDDR5 >> ../scripts/kernels_1pch.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_1pch.times
./assembly2sc.sh mms1pchS4GDDR5 >> ../scripts/kernels_1pch.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ccwwr1pchS4GDDR5 >> ../scripts/kernels_1pch.times
echo "" >> ../scripts/kernels_1pch.times

echo "----------C = 32, R = 8, S = 8, GDDR5----------" >> ../scripts/kernels_1pch.times
# sed -i "s/#define CORES_PER_PCH   4/#define CORES_PER_PCH   8/g" ../src/defs.h
sed -i "s/#define SIMD_WIDTH		(64 \/ WORD_BITS)/#define SIMD_WIDTH		(128 \/ WORD_BITS)/g" ../src/defs.h
sed -i "s/#define ROW_BITS        14/#define ROW_BITS        13/g" ../src/defs.h
# sed -i "s/#define GLOBAL_OFFSET   6/#define GLOBAL_OFFSET   5/g" ../src/defs.h
cd ../Debug
make all
cd ../inputs
sed -i "s/{4<<10, 16, {0, 1, 4, 4, 1<<14, 1<<(7+3)}},/{4<<10, 16, {0, 1, 4, 4, 1<<13, 1<<(7+3)}},/g" $HOME/Documents/ramulator-AB/src/GDDR5_AB.h
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4
./map_kernel ewarw1pchS8GDDR5 EWARW 100 100
./map_kernel dp1pchS8GDDR5 DP 100 100
./map_kernel mvm1pchS8GDDR5 MMS 1 1024 1024
./map_kernel mms1pchS8GDDR5 MMS 60 60 60
./map_kernel ccwwr1pchS8GDDR5 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ewarw1pchS8GDDR5 >> ../scripts/kernels_1pch.times
echo "DP V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh dp1pchS8GDDR5 >> ../scripts/kernels_1pch.times
echo "MVM " >> ../scripts/kernels_1pch.times
./assembly2sc.sh mvm1pchS8GDDR5 >> ../scripts/kernels_1pch.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_1pch.times
./assembly2sc.sh mms1pchS8GDDR5 >> ../scripts/kernels_1pch.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ccwwr1pchS8GDDR5 >> ../scripts/kernels_1pch.times
echo "" >> ../scripts/kernels_1pch.times

echo "----------C = 32, R = 8, S = 16, GDDR5----------" >> ../scripts/kernels_1pch.times
# sed -i "s/#define CORES_PER_PCH   4/#define CORES_PER_PCH   8/g" ../src/defs.h
sed -i "s/#define SIMD_WIDTH		(128 \/ WORD_BITS)/#define SIMD_WIDTH		(256 \/ WORD_BITS)/g" ../src/defs.h
sed -i "s/#define ROW_BITS        13/#define ROW_BITS        12/g" ../src/defs.h
# sed -i "s/#define GLOBAL_OFFSET   6/#define GLOBAL_OFFSET   5/g" ../src/defs.h
cd ../Debug
make all
cd ../inputs
sed -i "s/{4<<10, 16, {0, 1, 4, 4, 1<<13, 1<<(7+3)}},/{4<<10, 16, {0, 1, 4, 4, 1<<12, 1<<(7+3)}},/g" $HOME/Documents/ramulator-AB/src/GDDR5_AB.h
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4
./map_kernel ewarw1pchS16GDDR5 EWARW 100 100
./map_kernel dp1pchS16GDDR5 DP 100 100
./map_kernel mvm1pchS16GDDR5 MMS 1 1024 1024
./map_kernel mms1pchS16GDDR5 MMS 60 60 60
./map_kernel ccwwr1pchS16GDDR5 CCWWR 34 11 11 3 1 16 9 9
echo "EWARW V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ewarw1pchS16GDDR5 >> ../scripts/kernels_1pch.times
echo "DP V = 100 n = 100" >> ../scripts/kernels_1pch.times
./assembly2sc.sh dp1pchS16GDDR5 >> ../scripts/kernels_1pch.times
echo "MVM " >> ../scripts/kernels_1pch.times
./assembly2sc.sh mvm1pchS16GDDR5 >> ../scripts/kernels_1pch.times
echo "MMS m = n = q = 60" >> ../scripts/kernels_1pch.times
./assembly2sc.sh mms1pchS16GDDR5 >> ../scripts/kernels_1pch.times
echo "CCWWR 11x11x34 -> 9x9x16" >> ../scripts/kernels_1pch.times
./assembly2sc.sh ccwwr1pchS16GDDR5 >> ../scripts/kernels_1pch.times
echo "" >> ../scripts/kernels_1pch.times

sed -i "s/configs\/GDDR5_AB-config.cfg/configs\/HBM_AB-config.cfg/g" ../inputs/assembly2sc.sh
sed -i "s/#define CLK_PERIOD 1000/#define CLK_PERIOD 3333/g" ../src/defs.sh

# Back to initial state
sed -i "s/#define SIMD_WIDTH		(256 \/ WORD_BITS)/#define SIMD_WIDTH		(256 \/ WORD_BITS)/g" ../src/defs.h
sed -i "s/#define ROW_BITS        12/#define ROW_BITS        14/g" ../src/defs.h
sed -i "s/#define COL_BITS        7/#define COL_BITS        5/g" ../src/defs.h
# sed -i "s/#define GLOBAL_OFFSET   5/#define GLOBAL_OFFSET   6/g" ../src/defs.h
cd ../Debug
make all
cd ../inputs
sed -i "s/{4<<10, 128, {0, 0, 4, 4, 1<<12, 1<<(5+2)}},/{4<<10, 128, {0, 0, 4, 4, 1<<14, 1<<(5+2)}},/g" $HOME/Documents/ramulator-AB/src/HBM_AB.h
sed -i "s/{4<<10,  8, {0, 0, 4, 4, 1<<13, 1<<(10+3)}},/{4<<10,  8, {0, 0, 4, 4, 1<<15, 1<<10}},/g" $HOME/Documents/ramulator-AB/src/DDR4_AB.h
sed -i "s/{4<<10, 16, {0, 1, 4, 4, 1<<12, 1<<(7+3)}},/{4<<10, 16, {0, 1, 4, 4, 1<<14, 1<<(7+3)}},/g" $HOME/Documents/ramulator-AB/src/GDDR5_AB.h
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4