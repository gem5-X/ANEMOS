
#!/bin/bash

echo "REMEMEBER ALL HEADER FILES SHOULD START WITH C=32, R=8 and HBM original configuration"

sed -i "s/configs\/HBM_AB-config.cfg/configs\/PCM_AB-config.cfg/g" ../inputs/assembly2sc.sh
sed -i "s/#define CLK_PERIOD 3333/#define CLK_PERIOD 2500/g" ../src/defs.h

echo "" > ../scripts/kernels_nvm.times
echo "---------- PCM, C = 32, R = 8, S = 4----------" >> ../scripts/kernels_nvm.times
sed -i "s/#define CORES_PER_PCH   1/#define CORES_PER_PCH   8/g" ../src/defs.h
sed -i "s/#define SIMD_WIDTH      (256 \/ WORD_BITS)/#define SIMD_WIDTH      (64 \/ WORD_BITS)/g" ../src/defs.h
sed -i "s/#define ROW_BITS        14/#define ROW_BITS        15/g" ../src/defs.h
sed -i "s/#define COL_BITS        5/#define COL_BITS        7/g" ../src/defs.h
# sed -i "s/#define GLOBAL_OFFSET   6/#define GLOBAL_OFFSET   4/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4
bin/map_kernel ewarwC32R8S4PCMV256n256 EWARW 256 256
bin/map_kernel dpC32R8S4PCMV256n256 DP 256 256
bin/map_kernel mvmC32R8S4PCM16x16 MMS 1 16 16
bin/map_kernel mvmC32R8S4PCM32x32 MMS 1 32 32
bin/map_kernel mvmC32R8S4PCM64x64 MMS 1 64 64
bin/map_kernel mvmC32R8S4PCM128x128 MMS 1 128 128
bin/map_kernel mvmC32R8S4PCM256x256 MMS 1 256 256
bin/map_kernel mvmC32R8S4PCM512x512 MMS 1 512 512
bin/map_kernel mvmC32R8S4PCM1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC32R8S4PCMm128n128q128 MMS 128 128 128
bin/map_kernel ccwwrC32R8S4PCMi24x24x32o20x20x32k5 CCWWR 32 24 24 5 1 32 20 20
echo "EWARW V = 256 n = 256" >> ../scripts/kernels_nvm.times
./assembly2sc.sh ewarwC32R8S4PCMV256n256 >> ../scripts/kernels_nvm.times
echo "DP V = 256 n = 256" >> ../scripts/kernels_nvm.times
./assembly2sc.sh dpC32R8S4PCMV256n256 >> ../scripts/kernels_nvm.times
echo "MVM 16x16" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4PCM16x16 >> ../scripts/kernels_nvm.times
echo "MVM 32x32" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4PCM32x32 >> ../scripts/kernels_nvm.times
echo "MVM 64x64" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4PCM64x64 >> ../scripts/kernels_nvm.times
echo "MVM 128x128" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4PCM128x128 >> ../scripts/kernels_nvm.times
echo "MVM 256x256" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4PCM256x256 >> ../scripts/kernels_nvm.times
echo "MVM 512x512" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4PCM512x512 >> ../scripts/kernels_nvm.times
echo "MVM 1024x1024" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4PCM1024x1024 >> ../scripts/kernels_nvm.times
echo "MMS m = n = q = 128" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mmsC32R8S4PCMm128n128q128 >> ../scripts/kernels_nvm.times
echo "CCWWR 24x24x32 -> 20x20x32" >> ../scripts/kernels_nvm.times
./assembly2sc.sh ccwwrC32R8S4PCMi24x24x32o20x20x32k5 >> ../scripts/kernels_nvm.times

echo "" >> ../scripts/kernels_nvm.times
echo "---------- RRAM, C = 32, R = 8, S = 4----------" >> ../scripts/kernels_nvm.times
sed -i "s/configs\/PCM_AB-config.cfg/configs\/RRAM_AB-config.cfg/g" ../inputs/assembly2sc.sh
cd ../build
make all
cd ../inputs
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4
bin/map_kernel ewarwC32R8S4RRAMV256n256 EWARW 256 256
bin/map_kernel dpC32R8S4RRAMV256n256 DP 256 256
bin/map_kernel mvmC32R8S4RRAM16x16 MMS 1 16 16
bin/map_kernel mvmC32R8S4RRAM32x32 MMS 1 32 32
bin/map_kernel mvmC32R8S4RRAM64x64 MMS 1 64 64
bin/map_kernel mvmC32R8S4RRAM128x128 MMS 1 128 128
bin/map_kernel mvmC32R8S4RRAM256x256 MMS 1 256 256
bin/map_kernel mvmC32R8S4RRAM512x512 MMS 1 512 512
bin/map_kernel mvmC32R8S4RRAM1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC32R8S4RRAMm128n128q128 MMS 128 128 128
bin/map_kernel ccwwrC32R8S4RRAMi24x24x32o20x20x32k5 CCWWR 32 24 24 5 1 32 20 20
echo "EWARW V = 256 n = 256" >> ../scripts/kernels_nvm.times
./assembly2sc.sh ewarwC32R8S4RRAMV256n256 >> ../scripts/kernels_nvm.times
echo "DP V = 256 n = 256" >> ../scripts/kernels_nvm.times
./assembly2sc.sh dpC32R8S4RRAMV256n256 >> ../scripts/kernels_nvm.times
echo "MVM 16x16" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4RRAM16x16 >> ../scripts/kernels_nvm.times
echo "MVM 32x32" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4RRAM32x32 >> ../scripts/kernels_nvm.times
echo "MVM 64x64" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4RRAM64x64 >> ../scripts/kernels_nvm.times
echo "MVM 128x128" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4RRAM128x128 >> ../scripts/kernels_nvm.times
echo "MVM 256x256" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4RRAM256x256 >> ../scripts/kernels_nvm.times
echo "MVM 512x512" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4RRAM512x512 >> ../scripts/kernels_nvm.times
echo "MVM 1024x1024" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4RRAM1024x1024 >> ../scripts/kernels_nvm.times
echo "MMS m = n = q = 128" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mmsC32R8S4RRAMm128n128q128 >> ../scripts/kernels_nvm.times
echo "CCWWR 24x24x32 -> 20x20x32" >> ../scripts/kernels_nvm.times
./assembly2sc.sh ccwwrC32R8S4RRAMi24x24x32o20x20x32k5 >> ../scripts/kernels_nvm.times

echo "" >> ../scripts/kernels_nvm.times
echo "---------- STTRAM, C = 32, R = 8, S = 4----------" >> ../scripts/kernels_nvm.times
sed -i "s/configs\/RRAM_AB-config.cfg/configs\/STTRAM_AB-config.cfg/g" ../inputs/assembly2sc.sh
cd ../build
make all
cd ../inputs
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4
bin/map_kernel ewarwC32R8S4STTRAMV256n256 EWARW 256 256
bin/map_kernel dpC32R8S4STTRAMV256n256 DP 256 256
bin/map_kernel mvmC32R8S4STTRAM16x16 MMS 1 16 16
bin/map_kernel mvmC32R8S4STTRAM32x32 MMS 1 32 32
bin/map_kernel mvmC32R8S4STTRAM64x64 MMS 1 64 64
bin/map_kernel mvmC32R8S4STTRAM128x128 MMS 1 128 128
bin/map_kernel mvmC32R8S4STTRAM256x256 MMS 1 256 256
bin/map_kernel mvmC32R8S4STTRAM512x512 MMS 1 512 512
bin/map_kernel mvmC32R8S4STTRAM1024x1024 MMS 1 1024 1024
bin/map_kernel mmsC32R8S4STTRAMm128n128q128 MMS 128 128 128
bin/map_kernel ccwwrC32R8S4STTRAMi24x24x32o20x20x32k5 CCWWR 32 24 24 5 1 32 20 20
echo "EWARW V = 256 n = 256" >> ../scripts/kernels_nvm.times
./assembly2sc.sh ewarwC32R8S4STTRAMV256n256 >> ../scripts/kernels_nvm.times
echo "DP V = 256 n = 256" >> ../scripts/kernels_nvm.times
./assembly2sc.sh dpC32R8S4STTRAMV256n256 >> ../scripts/kernels_nvm.times
echo "MVM 16x16" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4STTRAM16x16 >> ../scripts/kernels_nvm.times
echo "MVM 32x32" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4STTRAM32x32 >> ../scripts/kernels_nvm.times
echo "MVM 64x64" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4STTRAM64x64 >> ../scripts/kernels_nvm.times
echo "MVM 128x128" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4STTRAM128x128 >> ../scripts/kernels_nvm.times
echo "MVM 256x256" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4STTRAM256x256 >> ../scripts/kernels_nvm.times
echo "MVM 512x512" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4STTRAM512x512 >> ../scripts/kernels_nvm.times
echo "MVM 1024x1024" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mvmC32R8S4STTRAM1024x1024 >> ../scripts/kernels_nvm.times
echo "MMS m = n = q = 128" >> ../scripts/kernels_nvm.times
./assembly2sc.sh mmsC32R8S4STTRAMm128n128q128 >> ../scripts/kernels_nvm.times
echo "CCWWR 24x24x32 -> 20x20x32" >> ../scripts/kernels_nvm.times
./assembly2sc.sh ccwwrC32R8S4STTRAMi24x24x32o20x20x32k5 >> ../scripts/kernels_nvm.times


sed -i "s/configs\/STTRAM_AB-config.cfg/configs\/HBM_AB-config.cfg/g" ../inputs/assembly2sc.sh
sed -i "s/#define CLK_PERIOD 2500/#define CLK_PERIOD 3333/g" ../src/defs.h


# Back to initial state
sed -i "s/#define CORES_PER_PCH   8/#define CORES_PER_PCH   1/g" ../src/defs.h
sed -i "s/#define SIMD_WIDTH      (64 \/ WORD_BITS)/#define SIMD_WIDTH      (256 \/ WORD_BITS)/g" ../src/defs.h
sed -i "s/#define COL_BITS        7/#define COL_BITS        5/g" ../src/defs.h
sed -i "s/#define ROW_BITS        15/#define ROW_BITS        14/g" ../src/defs.h
# sed -i "s/#define GLOBAL_OFFSET   4/#define GLOBAL_OFFSET   6/g" ../src/defs.h
cd ../build
make all
cd ../inputs
./compile_all.sh
make -C $HOME/Documents/ramulator-AB/ -j4