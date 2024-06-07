# Cadence Genus(TM) Synthesis Solution, Version 19.16-s111_1, built Feb 12 2021 12:22:37

# Date: Thu Dec 02 08:56:11 2021
# Host: eslsrv12 (x86_64 w/Linux 3.10.0-1160.45.1.el7.x86_64) (18cores*72cpus*2physical cpus*Intel(R) Xeon(R) Gold 6154 CPU @ 3.00GHz 25344KB)
# OS:   CentOS Linux release 7.9.2009 (Core)

cdnshelp

# Configure Genus and the search paths
set_db information_level 7
set_db init_lib_search_path ${EDA_PROJECT_ROOT}/LIBS
set_db init_hdl_search_path ${EDA_PROJECT_ROOT}/HDL/RTL

# Read the tech library and the HDL file
set_db library tcbn28hpmbwp35ss0p81v125c.lib
read_hdl fpu.v

# Elaborate the design
elaborate fpu 

# Set the constraints for the design 
create_clock -name clk_src -period 3.333 [get_db ports *clk]

# Set the synthesis effort
set_db syn_global_effort high

# Synthesize for a generic technology, then map to the technology and optimize
syn_generic
syn_map
syn_opt

# Report the area and timing of the design
report_area > RPT/fpu_area_ss.rep
report_timing > RPT/fpu_timing_ss.rep

# Remap and optimize for the rest of technology cases
set_db library tcbn28hpmbwp35ff0p99v0c.lib
syn_gen
syn_map
syn_opt
report_area > RPT/fpu_area_ff.rep
report_timing > RPT/fpu_timing_ff.rep

set_db library tcbn28hpmbwp35tt0p9v25c.lib
syn_gen
syn_map
syn_opt
report_area > RPT/fpu_area_tt.rep
report_timing > RPT/fpu_timing_tt.rep

# Write the netlist and the timing SDF
write_hdl -generic > HDL/GATE/fpu_generic.v
write_sdf > TIM/fpu_clk3ns.sdf
