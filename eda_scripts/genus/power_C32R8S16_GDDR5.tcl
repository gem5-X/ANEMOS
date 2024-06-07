# Cadence Genus(TM) Synthesis Solution, Version 19.16-s111_1, built Feb 12 2021 12:22:37

# Date: Thu Dec 02 08:56:11 2021
# Host: eslsrv12 (x86_64 w/Linux 3.10.0-1160.45.1.el7.x86_64) (18cores*72cpus*2physical cpus*Intel(R) Xeon(R) Gold 6154 CPU @ 3.00GHz 25344KB)
# OS:   CentOS Linux release 7.9.2009 (Core)

# Power analysis

# Read design
read_db designs/imc_C32R8S16_GDDR5.db

# Read VCD and report for every IMC core and testbench
read_vcd ${VCD_FILES}/imc_C32R8S16_dpV256n256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_0
report_power -by_hierarchy > RPT/power/dpV256n256_C32R8S16_GDDR5/imc_0.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_dpV256n256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_1
report_power -by_hierarchy > RPT/power/dpV256n256_C32R8S16_GDDR5/imc_1.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_dpV256n256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_2
report_power -by_hierarchy > RPT/power/dpV256n256_C32R8S16_GDDR5/imc_2.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_dpV256n256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_3
report_power -by_hierarchy > RPT/power/dpV256n256_C32R8S16_GDDR5/imc_3.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_dpV256n256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_4
report_power -by_hierarchy > RPT/power/dpV256n256_C32R8S16_GDDR5/imc_4.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_dpV256n256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_5
report_power -by_hierarchy > RPT/power/dpV256n256_C32R8S16_GDDR5/imc_5.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_dpV256n256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_6
report_power -by_hierarchy > RPT/power/dpV256n256_C32R8S16_GDDR5/imc_6.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_dpV256n256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_7
report_power -by_hierarchy > RPT/power/dpV256n256_C32R8S16_GDDR5/imc_7.rep

read_vcd ${VCD_FILES}/imc_C32R8S16_ewarwV256n256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_0
report_power -by_hierarchy > RPT/power/ewarwV256n256_C32R8S16_GDDR5/imc_0.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_ewarwV256n256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_1
report_power -by_hierarchy > RPT/power/ewarwV256n256_C32R8S16_GDDR5/imc_1.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_ewarwV256n256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_2
report_power -by_hierarchy > RPT/power/ewarwV256n256_C32R8S16_GDDR5/imc_2.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_ewarwV256n256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_3
report_power -by_hierarchy > RPT/power/ewarwV256n256_C32R8S16_GDDR5/imc_3.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_ewarwV256n256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_4
report_power -by_hierarchy > RPT/power/ewarwV256n256_C32R8S16_GDDR5/imc_4.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_ewarwV256n256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_5
report_power -by_hierarchy > RPT/power/ewarwV256n256_C32R8S16_GDDR5/imc_5.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_ewarwV256n256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_6
report_power -by_hierarchy > RPT/power/ewarwV256n256_C32R8S16_GDDR5/imc_6.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_ewarwV256n256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_7
report_power -by_hierarchy > RPT/power/ewarwV256n256_C32R8S16_GDDR5/imc_7.rep

read_vcd ${VCD_FILES}/imc_C32R8S16_mvm32x32_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_0
report_power -by_hierarchy > RPT/power/mvm32x32_C32R8S16_GDDR5/imc_0.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm32x32_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_1
report_power -by_hierarchy > RPT/power/mvm32x32_C32R8S16_GDDR5/imc_1.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm32x32_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_2
report_power -by_hierarchy > RPT/power/mvm32x32_C32R8S16_GDDR5/imc_2.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm32x32_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_3
report_power -by_hierarchy > RPT/power/mvm32x32_C32R8S16_GDDR5/imc_3.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm32x32_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_4
report_power -by_hierarchy > RPT/power/mvm32x32_C32R8S16_GDDR5/imc_4.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm32x32_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_5
report_power -by_hierarchy > RPT/power/mvm32x32_C32R8S16_GDDR5/imc_5.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm32x32_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_6
report_power -by_hierarchy > RPT/power/mvm32x32_C32R8S16_GDDR5/imc_6.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm32x32_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_7
report_power -by_hierarchy > RPT/power/mvm32x32_C32R8S16_GDDR5/imc_7.rep

read_vcd ${VCD_FILES}/imc_C32R8S16_mvm64x64_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_0
report_power -by_hierarchy > RPT/power/mvm64x64_C32R8S16_GDDR5/imc_0.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm64x64_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_1
report_power -by_hierarchy > RPT/power/mvm64x64_C32R8S16_GDDR5/imc_1.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm64x64_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_2
report_power -by_hierarchy > RPT/power/mvm64x64_C32R8S16_GDDR5/imc_2.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm64x64_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_3
report_power -by_hierarchy > RPT/power/mvm64x64_C32R8S16_GDDR5/imc_3.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm64x64_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_4
report_power -by_hierarchy > RPT/power/mvm64x64_C32R8S16_GDDR5/imc_4.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm64x64_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_5
report_power -by_hierarchy > RPT/power/mvm64x64_C32R8S16_GDDR5/imc_5.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm64x64_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_6
report_power -by_hierarchy > RPT/power/mvm64x64_C32R8S16_GDDR5/imc_6.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm64x64_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_7
report_power -by_hierarchy > RPT/power/mvm64x64_C32R8S16_GDDR5/imc_7.rep

read_vcd ${VCD_FILES}/imc_C32R8S16_mvm128x128_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_0
report_power -by_hierarchy > RPT/power/mvm128x128_C32R8S16_GDDR5/imc_0.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm128x128_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_1
report_power -by_hierarchy > RPT/power/mvm128x128_C32R8S16_GDDR5/imc_1.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm128x128_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_2
report_power -by_hierarchy > RPT/power/mvm128x128_C32R8S16_GDDR5/imc_2.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm128x128_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_3
report_power -by_hierarchy > RPT/power/mvm128x128_C32R8S16_GDDR5/imc_3.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm128x128_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_4
report_power -by_hierarchy > RPT/power/mvm128x128_C32R8S16_GDDR5/imc_4.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm128x128_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_5
report_power -by_hierarchy > RPT/power/mvm128x128_C32R8S16_GDDR5/imc_5.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm128x128_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_6
report_power -by_hierarchy > RPT/power/mvm128x128_C32R8S16_GDDR5/imc_6.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm128x128_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_7
report_power -by_hierarchy > RPT/power/mvm128x128_C32R8S16_GDDR5/imc_7.rep

read_vcd ${VCD_FILES}/imc_C32R8S16_mvm256x256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_0
report_power -by_hierarchy > RPT/power/mvm256x256_C32R8S16_GDDR5/imc_0.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm256x256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_1
report_power -by_hierarchy > RPT/power/mvm256x256_C32R8S16_GDDR5/imc_1.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm256x256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_2
report_power -by_hierarchy > RPT/power/mvm256x256_C32R8S16_GDDR5/imc_2.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm256x256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_3
report_power -by_hierarchy > RPT/power/mvm256x256_C32R8S16_GDDR5/imc_3.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm256x256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_4
report_power -by_hierarchy > RPT/power/mvm256x256_C32R8S16_GDDR5/imc_4.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm256x256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_5
report_power -by_hierarchy > RPT/power/mvm256x256_C32R8S16_GDDR5/imc_5.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm256x256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_6
report_power -by_hierarchy > RPT/power/mvm256x256_C32R8S16_GDDR5/imc_6.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm256x256_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_7
report_power -by_hierarchy > RPT/power/mvm256x256_C32R8S16_GDDR5/imc_7.rep

read_vcd ${VCD_FILES}/imc_C32R8S16_mvm512x512_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_0
report_power -by_hierarchy > RPT/power/mvm512x512_C32R8S16_GDDR5/imc_0.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm512x512_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_1
report_power -by_hierarchy > RPT/power/mvm512x512_C32R8S16_GDDR5/imc_1.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm512x512_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_2
report_power -by_hierarchy > RPT/power/mvm512x512_C32R8S16_GDDR5/imc_2.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm512x512_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_3
report_power -by_hierarchy > RPT/power/mvm512x512_C32R8S16_GDDR5/imc_3.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm512x512_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_4
report_power -by_hierarchy > RPT/power/mvm512x512_C32R8S16_GDDR5/imc_4.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm512x512_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_5
report_power -by_hierarchy > RPT/power/mvm512x512_C32R8S16_GDDR5/imc_5.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm512x512_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_6
report_power -by_hierarchy > RPT/power/mvm512x512_C32R8S16_GDDR5/imc_6.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm512x512_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_7
report_power -by_hierarchy > RPT/power/mvm512x512_C32R8S16_GDDR5/imc_7.rep

read_vcd ${VCD_FILES}/imc_C32R8S16_mvm1024x1024_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_0
report_power -by_hierarchy > RPT/power/mvm1024x1024_C32R8S16_GDDR5/imc_0.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm1024x1024_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_1
report_power -by_hierarchy > RPT/power/mvm1024x1024_C32R8S16_GDDR5/imc_1.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm1024x1024_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_2
report_power -by_hierarchy > RPT/power/mvm1024x1024_C32R8S16_GDDR5/imc_2.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm1024x1024_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_3
report_power -by_hierarchy > RPT/power/mvm1024x1024_C32R8S16_GDDR5/imc_3.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm1024x1024_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_4
report_power -by_hierarchy > RPT/power/mvm1024x1024_C32R8S16_GDDR5/imc_4.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm1024x1024_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_5
report_power -by_hierarchy > RPT/power/mvm1024x1024_C32R8S16_GDDR5/imc_5.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm1024x1024_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_6
report_power -by_hierarchy > RPT/power/mvm1024x1024_C32R8S16_GDDR5/imc_6.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mvm1024x1024_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_7
report_power -by_hierarchy > RPT/power/mvm1024x1024_C32R8S16_GDDR5/imc_7.rep

read_vcd ${VCD_FILES}/imc_C32R8S16_mmsm128n128q128_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_0
report_power -by_hierarchy > RPT/power/mmsm128n128q128_C32R8S16_GDDR5/imc_0.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mmsm128n128q128_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_1
report_power -by_hierarchy > RPT/power/mmsm128n128q128_C32R8S16_GDDR5/imc_1.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mmsm128n128q128_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_2
report_power -by_hierarchy > RPT/power/mmsm128n128q128_C32R8S16_GDDR5/imc_2.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mmsm128n128q128_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_3
report_power -by_hierarchy > RPT/power/mmsm128n128q128_C32R8S16_GDDR5/imc_3.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mmsm128n128q128_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_4
report_power -by_hierarchy > RPT/power/mmsm128n128q128_C32R8S16_GDDR5/imc_4.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mmsm128n128q128_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_5
report_power -by_hierarchy > RPT/power/mmsm128n128q128_C32R8S16_GDDR5/imc_5.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mmsm128n128q128_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_6
report_power -by_hierarchy > RPT/power/mmsm128n128q128_C32R8S16_GDDR5/imc_6.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_mmsm128n128q128_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_7
report_power -by_hierarchy > RPT/power/mmsm128n128q128_C32R8S16_GDDR5/imc_7.rep

read_vcd ${VCD_FILES}/imc_C32R8S16_ccwwri24x24x32o20x20x32k5_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_0
report_power -by_hierarchy > RPT/power/ccwwri24x24x32o20x20x32k5_C32R8S16_GDDR5/imc_0.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_ccwwri24x24x32o20x20x32k5_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_1
report_power -by_hierarchy > RPT/power/ccwwri24x24x32o20x20x32k5_C32R8S16_GDDR5/imc_1.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_ccwwri24x24x32o20x20x32k5_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_2
report_power -by_hierarchy > RPT/power/ccwwri24x24x32o20x20x32k5_C32R8S16_GDDR5/imc_2.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_ccwwri24x24x32o20x20x32k5_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_3
report_power -by_hierarchy > RPT/power/ccwwri24x24x32o20x20x32k5_C32R8S16_GDDR5/imc_3.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_ccwwri24x24x32o20x20x32k5_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_4
report_power -by_hierarchy > RPT/power/ccwwri24x24x32o20x20x32k5_C32R8S16_GDDR5/imc_4.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_ccwwri24x24x32o20x20x32k5_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_5
report_power -by_hierarchy > RPT/power/ccwwri24x24x32o20x20x32k5_C32R8S16_GDDR5/imc_5.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_ccwwri24x24x32o20x20x32k5_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_6
report_power -by_hierarchy > RPT/power/ccwwri24x24x32o20x20x32k5_C32R8S16_GDDR5/imc_6.rep
read_vcd ${VCD_FILES}/imc_C32R8S16_ccwwri24x24x32o20x20x32k5_GDDR5.vcd.gz -vcd_scope /top/IMCcoreUnderTest_7
report_power -by_hierarchy > RPT/power/ccwwri24x24x32o20x20x32k5_C32R8S16_GDDR5/imc_7.rep

quit
