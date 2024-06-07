# Cadence Genus(TM) Synthesis Solution, Version 19.16-s111_1, built Feb 12 2021 12:22:37

# Date: Thu Dec 02 08:56:11 2021
# Host: eslsrv12 (x86_64 w/Linux 3.10.0-1160.45.1.el7.x86_64) (18cores*72cpus*2physical cpus*Intel(R) Xeon(R) Gold 6154 CPU @ 3.00GHz 25344KB)
# OS:   CentOS Linux release 7.9.2009 (Core)

# Power analysis

# Read design
read_db designs/imc_C128R8S16_1pch.db

# Read VCD/hbmCR and report for every IMC core and testbench
read_vcd ${VCD_FILES}/hbmCR/imc_C128R8S16_dpV128n128_HBM.vcd.gz -vcd_scope /top/IMCcoreUnderTest_0
report_power -by_hierarchy > RPT/power/dpV128n128_C128R8S16_1pch/imc_0.rep

read_vcd ${VCD_FILES}/hbmCR/imc_C128R8S16_ewarwV128n128_HBM.vcd.gz -vcd_scope /top/IMCcoreUnderTest_0
report_power -by_hierarchy > RPT/power/ewarwV128n128_C128R8S16_1pch/imc_0.rep

read_vcd ${VCD_FILES}/hbmCR/imc_C128R8S16_mvm180x180_HBM.vcd.gz -vcd_scope /top/IMCcoreUnderTest_0
report_power -by_hierarchy > RPT/power/mvm180x180_C128R8S16_1pch/imc_0.rep

read_vcd ${VCD_FILES}/hbmCR/imc_C128R8S16_mvm1024x1024_HBM.vcd.gz -vcd_scope /top/IMCcoreUnderTest_0
report_power -by_hierarchy > RPT/power/mvm1024x1024_C128R8S16_1pch/imc_0.rep

read_vcd ${VCD_FILES}/hbmCR/imc_C128R8S16_mmsm60n60q60_HBM.vcd.gz -vcd_scope /top/IMCcoreUnderTest_0
report_power -by_hierarchy > RPT/power/mmsm60n60q60_C128R8S16_1pch/imc_0.rep

read_vcd ${VCD_FILES}/hbmCR/imc_C128R8S16_ccwwri11x11x34o9x9x16k3_HBM.vcd.gz -vcd_scope /top/IMCcoreUnderTest_0
report_power -by_hierarchy > RPT/power/ccwwri11x11x34o9x9x16k3_C128R8S16_1pch/imc_0.rep

quit
