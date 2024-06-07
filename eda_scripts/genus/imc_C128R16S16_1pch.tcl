# Cadence Genus(TM) Synthesis Solution, Version 19.16-s111_1, built Feb 12 2021 12:22:37

# Date: Thu Dec 02 08:56:11 2021
# Host: eslsrv12 (x86_64 w/Linux 3.10.0-1160.45.1.el7.x86_64) (18cores*72cpus*2physical cpus*Intel(R) Xeon(R) Gold 6154 CPU @ 3.00GHz 25344KB)
# OS:   CentOS Linux release 7.9.2009 (Core)

# Configure Genus and the search paths
set_db information_level 7
set_db init_lib_search_path ${EDA_PROJECT_ROOT}/LIBS
set_db init_hdl_search_path ${EDA_PROJECT_ROOT}/HDL/RTL

# Read the tech library and the HDL file
set_db library tcbn28hpcbwp30p140tt0p8v25c_ccs.lib
read_hdl imc_C128R16S16_HBM.v

# Enable insertion of clock-gating logic
set_db lp_insert_clock_gating true

# Ensure separate clock-gating for same functions and not same clock
set_db lp_clock_gating_exceptions_aware true

# Make genus work to optimize all critical paths, not only worst
set_db tns_opto true

# Enable aggressive power optimization, even at the cost of area incrase
set_db design_power_effort high

# Increase core count
set_db / .max_cpus_per_server 32
set_multi_cpu_usage -local_cpu 32

# Elaborate the design
elaborate imc_core

# Set the constraints for the design 
set_units -time ps
create_clock -name clk_src -period 3333 [get_db ports *clk]
external_delay -input 100 [all_inputs]
external_delay -output 100 [all_outputs]

# Set the synthesis effort
set_db syn_global_effort high

# Deactivate hierarchy flattening only for the level 1
set_db auto_ungroup both
set_db hinst:imc_core/control_unit_1 .ungroup_ok false
set_db hinst:imc_core/crf_1 .ungroup_ok false
set_db hinst:imc_core/fpu_1 .ungroup_ok false
set_db hinst:imc_core/grf_1 .ungroup_ok false
set_db hinst:imc_core/grf_2 .ungroup_ok false
set_db hinst:imc_core/srf_1 .ungroup_ok false
ungroup -all -flatten

# Enable retiming for all the design
set_db design:imc_core .retime true

# Synthesize for a generic technology, then map to the technology and optimize
syn_generic
syn_map
syn_opt

# Report the area and timing of the design
report_area -physical > RPT/area/imc_C128R16S16_1pch.rep
report_timing > RPT/timing/imc_C128R16S16_1pch.rep

# Write the netlist and the timing SDF
write_hdl -generic > HDL/GATE/imc_C128R16S16_1pch_generic.v
write_sdf > TIM/imc_C128R16S16_1pch.sdf

write_db -to_file designs/imc_C128R16S16_1pch.db -all_root_attributes

# Power analysis
#read_vcd ${EDA_PROJECT_ROOT}/MGC_QSIM/OUT/VCD/pch_C128R16S16_dpV100n100.vcd -vcd_scope /top/IMCcoreUnderTest_0
#report_power -by_hierarchy > RPT/power/pch_C128R16S16_dpV100n100.rep
#read_vcd ${EDA_PROJECT_ROOT}/MGC_QSIM/OUT/VCD/pch_C128R16S16_ewarwV100n100.vcd
#report_power -by_hierarchy > RPT/power/pch_C128R16S16_ewarwV100n100.rep
#read_vcd ${EDA_PROJECT_ROOT}/MGC_QSIM/OUT/VCD/pch_C128R16S16_ewacwV100n100.vcd
#report_power -by_hierarchy > RPT/power/pch_C128R16S16_ewacwV100n100.rep

#read_db designs/pch_C128R16S16.db
