#!/bin/bash

bin/nmc_assembler assembly-input/$1.asm raw/$1.seq data-input/$1.data address-input/$1.addr

bin/raw2ramulator raw/$1.seq ramulator-in/$1.trace

${RAMULATOR_ROOT}/ramulator ${RAMULATOR_ROOT}/configs/HBM_AB-config.cfg --mode=dram ramulator-in/$1.trace > ramulator-out/$1.cmd

bin/ramulator2sc raw/$1.seq ramulator-out/$1.cmd SystemC/$1.sci 1

cd ..
build/pim-cores $1
cd inputs

# ./decode_results results/$1.results
