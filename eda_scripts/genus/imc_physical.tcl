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

# Set LEF and QRC files
set_db lef_library ${EDA_PROJECT_ROOT}/LIBS/tcbn28hpmbwp35_8lm5X2ZUTRDL.lef
set_db qrc_tech_file ${EDA_PROJECT_ROOT}/LIBS/qrcTechFile.rcworst

# Elaborate design
elaborate imc_core

# Deactivate hierarchy flattening
set_db auto_ungroup none

# Set the constraints for the design 
set_units -time ps
create_clock -name clk_src -period 3333 [get_db ports *clk]
external_delay -input 100 [all_inputs]
external_delay -output 100 [all_outputs]

# Set the synthesis effort
set_db syn_global_effort high

# Synthesize
syn_gen -create_floorplan -physical
