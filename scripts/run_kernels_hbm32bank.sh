
#!/bin/bash

echo "REMEMEBER ALL HEADER FILES SHOULD START WITH C=32, R=8 and HBM32bank original configuration"

echo "" > ../scripts/kernels_hbm32bank.times
echo "----------C = 32, R = 8, S = 16----------" >> ../scripts/kernels_hbm32bank.times
sed -i "s/#define CORES_PER_PCH   1/#define CORES_PER_PCH   16/g" ../src/defs.h
sed -i "s/#define BG_BITS         2/#define BG_BITS         3/g" ../src/defs.h
sed -i "s/#define ROW_BITS        14/#define ROW_BITS        13/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
sed -i "s/{4<<10, 128, {0, 0, 4, 4, 1<<14, 1<<(5+2)}},/{4<<10, 128, {0, 0, 8, 4, 1<<13, 1<<(5+2)}},/g" $HOME/Documents/ramulator-AB/src/HBM_AB.h
make -C $HOME/Documents/ramulator-AB/ -j4
bin/map_kernel ewarwC32R8S16HBM32bankV256n256 EWARW 256 256
bin/map_kernel dpC32R8S16HBM32bankV256n256 DP 256 256
bin/map_kernel mvmC32R8S16HBM32bank64x64 MMS 1 64 64
bin/map_kernel mvmC32R8S16HBM32bank128x128 MMS 1 128 128
bin/map_kernel mvmC32R8S16HBM32bank1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC32R8S16HBM32bankm256n256q256 MMS 256 256 256
bin/map_kernel ccwwrC32R8S16HBM32banki24x24x32o20x20x64k5 CCWWR 32 24 24 5 1 64 20 20
echo "EWARW V = 256 n = 256" >> ../scripts/kernels_hbm32bank.times
./assembly2sc.sh ewarwC32R8S16HBM32bankV256n256 >> ../scripts/kernels_hbm32bank.times
echo "DP V = 256 n = 256" >> ../scripts/kernels_hbm32bank.times
./assembly2sc.sh dpC32R8S16HBM32bankV256n256 >> ../scripts/kernels_hbm32bank.times
echo "MVM 64x64" >> ../scripts/kernels_hbm32bank.times
./assembly2sc.sh mvmC32R8S16HBM32bank64x64 >> ../scripts/kernels_hbm32bank.times
echo "MVM 128x128" >> ../scripts/kernels_hbm32bank.times
./assembly2sc.sh mvmC32R8S16HBM32bank128x128 >> ../scripts/kernels_hbm32bank.times
echo "MVM 1024x1024" >> ../scripts/kernels_hbm32bank.times
./assembly2sc.sh mvmC32R8S16HBM32bank1024x1024 >> ../scripts/kernels_hbm32bank.times
echo "MMS m = n = q = 256" >> ../scripts/kernels_hbm32bank.times
./assembly2sc.sh mmsC32R8S16HBM32bankm256n256q256 >> ../scripts/kernels_hbm32bank.times
echo "CCWWR 24x24x32 -> 20x20x64" >> ../scripts/kernels_hbm32bank.times
./assembly2sc.sh ccwwrC32R8S16HBM32banki24x24x32o20x20x64k5 >> ../scripts/kernels_hbm32bank.times

# Back to initial state
sed -i "s/#define CORES_PER_PCH   16/#define CORES_PER_PCH   1/g" ../src/defs.h
sed -i "s/#define BG_BITS         3/#define BG_BITS         2/g" ../src/defs.h
sed -i "s/#define ROW_BITS        13/#define ROW_BITS        14/g" ../src/defs.h
# sed -i "s/#define GLOBAL_OFFSET   5/#define GLOBAL_OFFSET   6/g" ../src/defs.h
cd ../build
make all
cd ../inputs
sed -i "s/{4<<10, 128, {0, 0, 8, 4, 1<<13, 1<<(5+2)}},/{4<<10, 128, {0, 0, 4, 4, 1<<14, 1<<(5+2)}},/g" $HOME/Documents/ramulator-AB/src/HBM_AB.h
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4