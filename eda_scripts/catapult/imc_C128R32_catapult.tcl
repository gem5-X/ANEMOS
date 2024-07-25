## TCL script for Catapult HLS Synthesis of the pim-core control unit

# Initial configuration values
solution new -state initial
solution options defaults
solution options set /Input/Compiler Custom
solution options set /Input/CompilerHome ${HLS_GCC}/bin
solution options set /Input/CppStandard c++11

# Add the C++/SystemC files
solution file add ../C_FILES/control_unit.cpp -type C++
solution file add ../C_FILES/control_unit.h -type CHEADER
solution file add ../C_FILES/crf.cpp -type C++
solution file add ../C_FILES/crf.h -type CHEADER
solution file add ../C_FILES/defs.h -type CHEADER
solution file add ../C_FILES/fp_adder.cpp -type C++
solution file add ../C_FILES/fp_adder.h -type CHEADER
solution file add ../C_FILES/fp_multiplier.cpp -type C++
solution file add ../C_FILES/fp_multiplier.h -type CHEADER
solution file add ../C_FILES/fpu.cpp -type C++
solution file add ../C_FILES/fpu.h -type CHEADER
solution file add ../C_FILES/grf.cpp -type C++
solution file add ../C_FILES/grf.h -type CHEADER
solution file add ../C_FILES/imc_core.cpp -type C++
solution file add ../C_FILES/imc_core.h -type CHEADER
solution file add ../C_FILES/imc_pch.cpp -type C++
solution file add ../C_FILES/imc_pch.h -type CHEADER
solution file add ../C_FILES/instr_decoder.cpp -type C++
solution file add ../C_FILES/instr_decoder.h -type CHEADER
solution file add ../C_FILES/interface_unit.cpp -type C++
solution file add ../C_FILES/interface_unit.h -type CHEADER
solution file add ../C_FILES/log2.h -type CHEADER -exclude true
solution file add ../C_FILES/pch_main.h -type CHEADER -exclude true
solution file add ../C_FILES/pch_main.cpp -type C++ -exclude true
solution file add ../C_FILES/pch_driver.h -type CHEADER -exclude true
solution file add ../C_FILES/pch_driver.cpp -type C++ -exclude true
solution file add ../C_FILES/pch_monitor.h -type CHEADER -exclude true
solution file add ../C_FILES/pch_monitor.cpp -type C++ -exclude true
solution file add ../C_FILES/pc_unit.cpp -type C++
solution file add ../C_FILES/pc_unit.h -type CHEADER
solution file add ../C_FILES/rf_threeport.h -type CHEADER
solution file add ../C_FILES/rf_twoport.h -type CHEADER
solution file add ../C_FILES/srf.cpp -type C++
solution file add ../C_FILES/srf.h -type CHEADER
solution file add ../C_FILES/tristate_buffer.h -type CHEADER

# Set the design directives
directive set -DESIGN_GOAL area
directive set -SPECULATE true
directive set -MERGEABLE true
directive set -REGISTER_THRESHOLD 256
directive set -MEM_MAP_THRESHOLD 32
directive set -LOGIC_OPT false
directive set -LOGIC_OPT_WIDTH 0
directive set -FSM_ENCODING none
directive set -FSM_BINARY_ENCODING_THRESHOLD 64
directive set -REG_MAX_FANOUT 0
directive set -NO_X_ASSIGNMENTS true
directive set -SAFE_FSM false
directive set -REGISTER_SHARING_MAX_WIDTH_DIFFERENCE 8
directive set -REGISTER_SHARING_LIMIT 0
directive set -ASSIGN_OVERHEAD 0
directive set -TIMING_CHECKS true
directive set -MUXPATH true
directive set -REALLOC true
directive set -UNROLL no
directive set -IO_MODE super
directive set -CHAN_IO_PROTOCOL use_library
directive set -ARRAY_SIZE 1024
directive set -REGISTER_IDLE_SIGNAL false
directive set -IDLE_SIGNAL {}
directive set -STALL_FLAG false
directive set -TRANSACTION_DONE_SIGNAL true
directive set -DONE_FLAG {}
directive set -READY_FLAG {}
directive set -START_FLAG {}
directive set -RESET_CLEARS_ALL_REGS use_library
directive set -CLOCK_OVERHEAD 20.000000
directive set -OPT_CONST_MULTS use_library
directive set -CHARACTERIZE_ROM false
directive set -PROTOTYPE_ROM true
directive set -ROM_THRESHOLD 64
directive set -CLUSTER_ADDTREE_IN_WIDTH_THRESHOLD 0
directive set -CLUSTER_ADDTREE_IN_COUNT_THRESHOLD 0
directive set -CLUSTER_OPT_CONSTANT_INPUTS true
directive set -CLUSTER_RTL_SYN false
directive set -CLUSTER_FAST_MODE false
directive set -CLUSTER_TYPE combinational
directive set -PROTOTYPING_ENGINE oasys
directive set -PIPELINE_RAMP_UP true

# Analyze the hierarchy
go new

# Identify the bocks and the top block
#solution design set control_unit -block
#solution design set {rf_twoport<cnm_synth, 8U>} -block
#solution design set {rf_twoport<uint32_t, 32U>} -block
#solution design set srf -block
#solution design set {rf_threeport<cnm_synth, 8U>} -block
#solution design set pc_unit -block
#solution design set interface_unit -block
#solution design set instr_decoder -block
#solution design set fp_multiplier -block
#solution design set fp_adder -block
#solution design set fpu -block
#solution design set crf -block
#solution design set grf -block
#solution design set imc_core -block
#solution design set ac::fx_div<8> -block
solution design set imc_core -top

# Analyze C files
go analyze

# Select library and synthesis tool
solution library add nangate-45nm_beh -- -rtlsyntool DesignCompiler -vendor Nangate -technology 045nm

# Compile
go libraries

# Set the clock
directive set -CLOCKS {clk {-CLOCK_PERIOD 3.33 -CLOCK_UNCERTAINTY 0.0 -CLOCK_HIGH_TIME 1.665}}

# Architectural assembly
go assembly

# Unroll loops for maintaining coded scheduling
directive set /imc_core/rf_twoport<cnm_synth,32U>/write_update_thread/for -UNROLL yes
directive set /imc_core/instr_decoder/clk_thread/while:for#1 -UNROLL yes
directive set /imc_core/instr_decoder/clk_thread/while:for#3 -UNROLL yes
directive set /imc_core/rf_threeport<cnm_synth,32U>/write_update_thread/for -UNROLL yes
directive set /imc_core/rf_twoport<uint32_t,128U>/write_update_thread/for -UNROLL yes

# Infer resources
go architect

# Generate scheduling and RTL
go extract
