onerror {resume}
radix define float#10#decimal -float -fraction 11 -base signed -precision 5
quietly virtual signal -install /top { /top/even_bus(255 downto 240)} even_bus_15
quietly virtual signal -install /top { /top/even_bus(239 downto 224)} even_bus_14
quietly virtual signal -install /top { /top/even_bus(223 downto 208)} even_bus_13
quietly virtual signal -install /top { /top/even_bus(207 downto 192)} even_bus_12
quietly virtual signal -install /top { /top/even_bus(191 downto 176)} even_bus_11
quietly virtual signal -install /top { /top/even_bus(175 downto 160)} even_bus_10
quietly virtual signal -install /top { /top/even_bus(159 downto 144)} even_bus_9
quietly virtual signal -install /top { /top/even_bus(143 downto 128)} even_bus_8
quietly virtual signal -install /top { /top/even_bus(127 downto 112)} even_bus_7
quietly virtual signal -install /top { /top/even_bus(111 downto 96)} even_bus_6
quietly virtual signal -install /top { /top/even_bus(95 downto 80)} even_bus_5
quietly virtual signal -install /top { /top/even_bus(79 downto 64)} even_bus_4
quietly virtual signal -install /top { /top/even_bus(63 downto 48)} even_bus_3
quietly virtual signal -install /top { /top/even_bus(47 downto 32)} even_bus_2
quietly virtual signal -install /top { /top/even_bus(31 downto 16)} even_bus_1
quietly virtual signal -install /top { /top/even_bus(15 downto 0)} even_bus_0
quietly virtual signal -install /top { /top/odd_bus(255 downto 240)} odd_bus_15
quietly virtual signal -install /top { /top/odd_bus(239 downto 224)} odd_bus_14
quietly virtual signal -install /top { /top/odd_bus(223 downto 208)} odd_bus_13
quietly virtual signal -install /top { /top/odd_bus(207 downto 192)} odd_bus_12
quietly virtual signal -install /top { /top/odd_bus(191 downto 176)} odd_bus_11
quietly virtual signal -install /top { /top/odd_bus(175 downto 160)} odd_bus_10
quietly virtual signal -install /top { /top/odd_bus(159 downto 144)} odd_bus_9
quietly virtual signal -install /top { /top/odd_bus(143 downto 128)} odd_bus_8
quietly virtual signal -install /top { /top/odd_bus(127 downto 112)} odd_bus_7
quietly virtual signal -install /top { /top/odd_bus(111 downto 96)} odd_bus_6
quietly virtual signal -install /top { /top/odd_bus(95 downto 80)} odd_bus_5
quietly virtual signal -install /top { /top/odd_bus(79 downto 64)} odd_bus_4
quietly virtual signal -install /top { /top/odd_bus(63 downto 48)} odd_bus_3
quietly virtual signal -install /top { /top/odd_bus(47 downto 32)} odd_bus_2
quietly virtual signal -install /top { /top/odd_bus(31 downto 16)} odd_bus_1
quietly virtual signal -install /top { /top/odd_bus(15 downto 0)} odd_bus_0
quietly WaveActivateNextPane {} 0
add wave -noupdate /top/IMCCoreUnderTest/rst
add wave -noupdate /top/IMCCoreUnderTest/clk.m_cur_val
add wave -noupdate /top/IMCCoreUnderTest/RD
add wave -noupdate /top/IMCCoreUnderTest/WR
add wave -noupdate /top/IMCCoreUnderTest/ACT
add wave -noupdate /top/IMCCoreUnderTest/AB_mode
add wave -noupdate /top/IMCCoreUnderTest/pim_mode
add wave -noupdate /top/IMCCoreUnderTest/bank_addr
add wave -noupdate /top/IMCCoreUnderTest/control_unit/rf_access
add wave -noupdate /top/IMCCoreUnderTest/control_unit/decode_en
add wave -noupdate -radix decimal /top/IMCCoreUnderTest/row_addr
add wave -noupdate -radix decimal /top/IMCCoreUnderTest/col_addr
add wave -noupdate /top/IMCCoreUnderTest/DQ
add wave -noupdate /top/IMCCoreUnderTest/data_out
add wave -noupdate /top/IMCCoreUnderTest/ext2crf
add wave -noupdate -radix hexadecimal /top/IMCCoreUnderTest/ext2srf
add wave -noupdate -childformat {{/top/IMCCoreUnderTest/ext2grf(0) -radix hexadecimal} {/top/IMCCoreUnderTest/ext2grf(1) -radix hexadecimal} {/top/IMCCoreUnderTest/ext2grf(2) -radix hexadecimal} {/top/IMCCoreUnderTest/ext2grf(3) -radix hexadecimal} {/top/IMCCoreUnderTest/ext2grf(4) -radix hexadecimal} {/top/IMCCoreUnderTest/ext2grf(5) -radix hexadecimal} {/top/IMCCoreUnderTest/ext2grf(6) -radix hexadecimal} {/top/IMCCoreUnderTest/ext2grf(7) -radix hexadecimal} {/top/IMCCoreUnderTest/ext2grf(8) -radix hexadecimal} {/top/IMCCoreUnderTest/ext2grf(9) -radix hexadecimal} {/top/IMCCoreUnderTest/ext2grf(10) -radix hexadecimal} {/top/IMCCoreUnderTest/ext2grf(11) -radix hexadecimal} {/top/IMCCoreUnderTest/ext2grf(12) -radix hexadecimal} {/top/IMCCoreUnderTest/ext2grf(13) -radix hexadecimal} {/top/IMCCoreUnderTest/ext2grf(14) -radix hexadecimal} {/top/IMCCoreUnderTest/ext2grf(15) -radix hexadecimal}} -subitemconfig {/top/IMCCoreUnderTest/ext2grf(0) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/ext2grf(1) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/ext2grf(2) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/ext2grf(3) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/ext2grf(4) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/ext2grf(5) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/ext2grf(6) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/ext2grf(7) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/ext2grf(8) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/ext2grf(9) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/ext2grf(10) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/ext2grf(11) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/ext2grf(12) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/ext2grf(13) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/ext2grf(14) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/ext2grf(15) {-height 16 -radix hexadecimal}} /top/IMCCoreUnderTest/ext2grf
add wave -noupdate -radix decimal /top/IMCCoreUnderTest/PC
add wave -noupdate /top/IMCCoreUnderTest/instr
add wave -noupdate /top/IMCCoreUnderTest/even_out_en
add wave -noupdate -expand -group even_bus -radix hexadecimal /top/even_bus_15
add wave -noupdate -expand -group even_bus -radix hexadecimal /top/even_bus_14
add wave -noupdate -expand -group even_bus -radix hexadecimal /top/even_bus_13
add wave -noupdate -expand -group even_bus -radix hexadecimal /top/even_bus_12
add wave -noupdate -expand -group even_bus -radix hexadecimal /top/even_bus_11
add wave -noupdate -expand -group even_bus -radix hexadecimal /top/even_bus_10
add wave -noupdate -expand -group even_bus -radix hexadecimal /top/even_bus_9
add wave -noupdate -expand -group even_bus -radix hexadecimal /top/even_bus_8
add wave -noupdate -expand -group even_bus -radix hexadecimal /top/even_bus_7
add wave -noupdate -expand -group even_bus -radix hexadecimal /top/even_bus_6
add wave -noupdate -expand -group even_bus -radix hexadecimal /top/even_bus_5
add wave -noupdate -expand -group even_bus -radix hexadecimal /top/even_bus_4
add wave -noupdate -expand -group even_bus -radix hexadecimal /top/even_bus_3
add wave -noupdate -expand -group even_bus -radix hexadecimal /top/even_bus_2
add wave -noupdate -expand -group even_bus -radix hexadecimal /top/even_bus_1
add wave -noupdate -expand -group even_bus -radix hexadecimal /top/even_bus_0
add wave -noupdate /top/IMCCoreUnderTest/odd_out_en
add wave -noupdate -expand -group odd_bus -radix hexadecimal /top/odd_bus_15
add wave -noupdate -expand -group odd_bus -radix hexadecimal /top/odd_bus_14
add wave -noupdate -expand -group odd_bus -radix hexadecimal /top/odd_bus_13
add wave -noupdate -expand -group odd_bus -radix hexadecimal /top/odd_bus_12
add wave -noupdate -expand -group odd_bus -radix hexadecimal /top/odd_bus_11
add wave -noupdate -expand -group odd_bus -radix hexadecimal /top/odd_bus_10
add wave -noupdate -expand -group odd_bus -radix hexadecimal /top/odd_bus_9
add wave -noupdate -expand -group odd_bus -radix hexadecimal /top/odd_bus_8
add wave -noupdate -expand -group odd_bus -radix hexadecimal /top/odd_bus_7
add wave -noupdate -expand -group odd_bus -radix hexadecimal /top/odd_bus_6
add wave -noupdate -expand -group odd_bus -radix hexadecimal /top/odd_bus_5
add wave -noupdate -expand -group odd_bus -radix hexadecimal /top/odd_bus_4
add wave -noupdate -expand -group odd_bus -radix hexadecimal /top/odd_bus_3
add wave -noupdate -expand -group odd_bus -radix hexadecimal /top/odd_bus_2
add wave -noupdate -expand -group odd_bus -radix hexadecimal /top/odd_bus_1
add wave -noupdate -expand -group odd_bus -radix hexadecimal /top/odd_bus_0
add wave -noupdate -divider CRF
add wave -noupdate /top/IMCCoreUnderTest/crf_wr_en
add wave -noupdate /top/IMCCoreUnderTest/crf_wr_addr
add wave -noupdate -divider SRF
add wave -noupdate /top/IMCCoreUnderTest/srf_rd_addr
add wave -noupdate /top/IMCCoreUnderTest/srf_wr_addr
add wave -noupdate /top/IMCCoreUnderTest/srf_rd_a_nm
add wave -noupdate /top/IMCCoreUnderTest/srf_wr_a_nm
add wave -noupdate /top/IMCCoreUnderTest/srf_wr_en
add wave -noupdate /top/IMCCoreUnderTest/srf_wr_from
add wave -noupdate -radix hexadecimal -childformat {{/top/IMCCoreUnderTest/srf_out.data_ -radix float#10#decimal}} -subitemconfig {/top/IMCCoreUnderTest/srf_out.data_ {-height 16 -radix float#10#decimal}} /top/IMCCoreUnderTest/srf_out
add wave -noupdate -divider {GRF A}
add wave -noupdate /top/IMCCoreUnderTest/grfa_rd_addr1
add wave -noupdate /top/IMCCoreUnderTest/grfa_rd_addr2
add wave -noupdate /top/IMCCoreUnderTest/grfa_wr_addr
add wave -noupdate /top/IMCCoreUnderTest/grfa_wr_en
add wave -noupdate /top/IMCCoreUnderTest/grfa_relu_en
add wave -noupdate /top/IMCCoreUnderTest/grfa_wr_from
add wave -noupdate -childformat {{/top/IMCCoreUnderTest/grfa_out1(0) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out1(1) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out1(2) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out1(3) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out1(4) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out1(5) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out1(6) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out1(7) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out1(8) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out1(9) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out1(10) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out1(11) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out1(12) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out1(13) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out1(14) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out1(15) -radix hexadecimal}} -subitemconfig {/top/IMCCoreUnderTest/grfa_out1(0) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out1(1) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out1(2) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out1(3) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out1(4) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out1(5) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out1(6) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out1(7) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out1(8) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out1(9) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out1(10) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out1(11) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out1(12) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out1(13) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out1(14) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out1(15) {-height 16 -radix hexadecimal}} /top/IMCCoreUnderTest/grfa_out1
add wave -noupdate -childformat {{/top/IMCCoreUnderTest/grfa_out2(0) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out2(1) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out2(2) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out2(3) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out2(4) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out2(5) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out2(6) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out2(7) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out2(8) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out2(9) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out2(10) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out2(11) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out2(12) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out2(13) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out2(14) -radix hexadecimal} {/top/IMCCoreUnderTest/grfa_out2(15) -radix hexadecimal}} -subitemconfig {/top/IMCCoreUnderTest/grfa_out2(0) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out2(1) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out2(2) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out2(3) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out2(4) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out2(5) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out2(6) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out2(7) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out2(8) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out2(9) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out2(10) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out2(11) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out2(12) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out2(13) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out2(14) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfa_out2(15) {-height 16 -radix hexadecimal}} /top/IMCCoreUnderTest/grfa_out2
add wave -noupdate -divider {GRF B}
add wave -noupdate /top/IMCCoreUnderTest/grfb_rd_addr1
add wave -noupdate /top/IMCCoreUnderTest/grfb_rd_addr2
add wave -noupdate /top/IMCCoreUnderTest/grfb_wr_addr
add wave -noupdate /top/IMCCoreUnderTest/grfb_wr_en
add wave -noupdate /top/IMCCoreUnderTest/grfb_relu_en
add wave -noupdate /top/IMCCoreUnderTest/grfb_wr_from
add wave -noupdate -childformat {{/top/IMCCoreUnderTest/grfb_out1(0) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out1(1) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out1(2) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out1(3) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out1(4) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out1(5) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out1(6) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out1(7) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out1(8) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out1(9) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out1(10) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out1(11) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out1(12) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out1(13) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out1(14) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out1(15) -radix hexadecimal}} -subitemconfig {/top/IMCCoreUnderTest/grfb_out1(0) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out1(1) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out1(2) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out1(3) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out1(4) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out1(5) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out1(6) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out1(7) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out1(8) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out1(9) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out1(10) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out1(11) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out1(12) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out1(13) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out1(14) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out1(15) {-height 16 -radix hexadecimal}} /top/IMCCoreUnderTest/grfb_out1
add wave -noupdate -childformat {{/top/IMCCoreUnderTest/grfb_out2(0) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out2(1) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out2(2) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out2(3) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out2(4) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out2(5) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out2(6) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out2(7) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out2(8) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out2(9) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out2(10) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out2(11) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out2(12) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out2(13) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out2(14) -radix hexadecimal} {/top/IMCCoreUnderTest/grfb_out2(15) -radix hexadecimal}} -subitemconfig {/top/IMCCoreUnderTest/grfb_out2(0) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out2(1) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out2(2) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out2(3) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out2(4) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out2(5) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out2(6) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out2(7) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out2(8) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out2(9) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out2(10) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out2(11) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out2(12) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out2(13) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out2(14) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/grfb_out2(15) {-height 16 -radix hexadecimal}} /top/IMCCoreUnderTest/grfb_out2
add wave -noupdate -divider FPU
add wave -noupdate /top/IMCCoreUnderTest/fpu_mult_en
add wave -noupdate /top/IMCCoreUnderTest/fpu_add_en
add wave -noupdate /top/IMCCoreUnderTest/fpu_out_sel
add wave -noupdate /top/IMCCoreUnderTest/fpu_mult_in1_sel
add wave -noupdate /top/IMCCoreUnderTest/fpu_mult_in2_sel
add wave -noupdate /top/IMCCoreUnderTest/fpu_add_in1_sel
add wave -noupdate /top/IMCCoreUnderTest/fpu_add_in2_sel
add wave -noupdate -childformat {{/top/IMCCoreUnderTest/fpu_out(0) -radix hexadecimal} {/top/IMCCoreUnderTest/fpu_out(1) -radix hexadecimal} {/top/IMCCoreUnderTest/fpu_out(2) -radix hexadecimal} {/top/IMCCoreUnderTest/fpu_out(3) -radix hexadecimal} {/top/IMCCoreUnderTest/fpu_out(4) -radix hexadecimal} {/top/IMCCoreUnderTest/fpu_out(5) -radix hexadecimal} {/top/IMCCoreUnderTest/fpu_out(6) -radix hexadecimal} {/top/IMCCoreUnderTest/fpu_out(7) -radix hexadecimal} {/top/IMCCoreUnderTest/fpu_out(8) -radix hexadecimal} {/top/IMCCoreUnderTest/fpu_out(9) -radix hexadecimal} {/top/IMCCoreUnderTest/fpu_out(10) -radix hexadecimal} {/top/IMCCoreUnderTest/fpu_out(11) -radix hexadecimal} {/top/IMCCoreUnderTest/fpu_out(12) -radix hexadecimal} {/top/IMCCoreUnderTest/fpu_out(13) -radix hexadecimal} {/top/IMCCoreUnderTest/fpu_out(14) -radix hexadecimal} {/top/IMCCoreUnderTest/fpu_out(15) -radix hexadecimal}} -subitemconfig {/top/IMCCoreUnderTest/fpu_out(0) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/fpu_out(1) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/fpu_out(2) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/fpu_out(3) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/fpu_out(4) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/fpu_out(5) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/fpu_out(6) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/fpu_out(7) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/fpu_out(8) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/fpu_out(9) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/fpu_out(10) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/fpu_out(11) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/fpu_out(12) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/fpu_out(13) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/fpu_out(14) {-height 16 -radix hexadecimal} /top/IMCCoreUnderTest/fpu_out(15) {-height 16 -radix hexadecimal}} /top/IMCCoreUnderTest/fpu_out
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {38 ns} 0}
quietly wave cursor active 1
configure wave -namecolwidth 268
configure wave -valuecolwidth 68
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ns
update
WaveRestoreZoom {0 ns} {78 ns}
