#!/bin/bash

g++ -std=c++11 src/build_addr.cpp ../src/defs.h -o bin/build_addr
g++ -std=c++11 src/decode_results.cpp src/half.hpp src/datatypes.h ../src/defs.h -o bin/decode_results
g++ -std=c++11 src/map_kernel.cpp src/map_kernel.h src/utils.h src/utils.cpp src/map_va.h src/map_va.cpp src/map_dp.h src/map_dp.cpp \
                src/map_mm.h src/map_mm.cpp src/map_conv.h src/map_conv.cpp src/half.hpp src/datatypes.h ../src/defs.h ../src/opcodes.h -o bin/map_kernel
g++ -std=c++11 src/nmc_assembler.cpp src/nmc_assembler.h src/half.hpp src/datatypes.h ../src/defs.h ../src/opcodes.h -o bin/nmc_assembler
g++ -std=c++11 src/ramulator2sc.cpp ../src/defs.h -o bin/ramulator2sc
g++ -std=c++11 src/raw2ramulator.cpp -o bin/raw2ramulator
g++ -std=c++11 src/raw_seq_gen.cpp ../src/defs.h -o bin/raw_seq_gen