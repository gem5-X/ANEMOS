# Cadence Genus(TM) Synthesis Solution, Version 19.16-s111_1, built Feb 12 2021 12:22:37

# Date: Thu Dec 02 08:56:11 2021
# Host: eslsrv12 (x86_64 w/Linux 3.10.0-1160.45.1.el7.x86_64) (18cores*72cpus*2physical cpus*Intel(R) Xeon(R) Gold 6154 CPU @ 3.00GHz 25344KB)
# OS:   CentOS Linux release 7.9.2009 (Core)

# Configure Genus and the search paths
set_db information_level 7
set_db init_lib_search_path ${EDA_PROJECT_ROOT}/LIBS
set_db init_hdl_search_path ${EDA_PROJECT_ROOT}/HDL/RTL

# Read the tech library and the HDL file
set_db library tcbn28hpmbwp35tt0p9v25c.lib
read_hdl imc_core.v

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
create_clock -name clk_src -period 333 [get_db ports *clk]
external_delay -input 30 [all_inputs]
external_delay -output 30 [all_outputs]

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

# Add the vcd file
#read_vcd ${EDA_PROJECT_ROOT}/MGC_QSIM/OUT/imc_3000.vcd

report_area > RPT/imc_area_3000tt.rep
report_timing > RPT/imc_timing_3000tt.rep
#report_power -by_hierarchy > RPT/imc_power_3000tt.rep

write_hdl -generic > HDL/GATE/imc_generic3000.v
write_sdf > TIM/imc_clk0ns33.sdf

write_design -base_name TEMP/imc_3000
