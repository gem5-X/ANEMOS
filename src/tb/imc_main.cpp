#include "imc_main.h"

#ifdef MTI_SYSTEMC

	SC_MODULE_EXPORT(top);

#if MIXED_SIM

	void top::clock_method() {
		clk = static_cast<sc_logic>(clk_g);
	}

#endif

#else

int sc_main(int argc, char *argv[]) {
    sc_clock                        clk("clk", CLK_PERIOD, RESOLUTION);
    sc_signal<bool>                 rst;
    sc_signal<bool>                 RD;			// DRAM read command
    sc_signal<bool>                 WR;			// DRAM write command
    sc_signal<bool>                 ACT;		// DRAM activate command
//    sc_signal<bool>					RSTB;		//
    sc_signal<bool>                 AB_mode;	// Signals if the All-Banks mode is enabled
    sc_signal<bool>                 pim_mode;	// Signals if the PIM mode is enabled
    sc_signal<sc_uint<BANK_BITS> >  bank_addr;	// Address of the bank
    sc_signal<sc_uint<ROW_BITS> >   row_addr;	// Address of the bank row
    sc_signal<sc_uint<COL_BITS> >   col_addr;	// Address of the bank column
    sc_signal<uint64_t>             DQ;	        // Data input from DRAM controller (output makes no sense
    sc_signal_rv<GRF_WIDTH>         even_bus;	// Direct data in/out to the even bank
    sc_signal_rv<GRF_WIDTH>         odd_bus;	// Direct data in/out to the odd bank

    imc_core dut("IMCCoreUnderTest", 1);
    dut.clk(clk);
    dut.rst(rst);
    dut.RD(RD);
    dut.WR(WR);
    dut.ACT(ACT);
//    dut.RSTB(RSTB);
    dut.AB_mode(AB_mode);
    dut.pim_mode(pim_mode);
    dut.bank_addr(bank_addr);
    dut.row_addr(row_addr);
    dut.col_addr(col_addr);
    dut.DQ(DQ);
    dut.even_bus(even_bus);
    dut.odd_bus(odd_bus);

    imc_driver driver("Driver");
    driver.rst(rst);
    driver.RD(RD);
    driver.WR(WR);
    driver.ACT(ACT);
//    driver.RSTB(RSTB);
    driver.AB_mode(AB_mode);
    driver.pim_mode(pim_mode);
    driver.bank_addr(bank_addr);
    driver.row_addr(row_addr);
    driver.col_addr(col_addr);
    driver.DQ(DQ);
    driver.even_bus(even_bus);
    driver.odd_bus(odd_bus);

    imc_monitor monitor("Monitor");
    monitor.clk(clk);
    monitor.rst(rst);
    monitor.RD(RD);
    monitor.WR(WR);
    monitor.ACT(ACT);
//    monitor.RSTB(RSTB);
    monitor.AB_mode(AB_mode);
    monitor.pim_mode(pim_mode);
    monitor.bank_addr(bank_addr);
    monitor.row_addr(row_addr);
    monitor.col_addr(col_addr);
    monitor.DQ(DQ);
    monitor.even_bus(even_bus);
    monitor.odd_bus(odd_bus);

    sc_report_handler::set_actions(SC_ID_VECTOR_CONTAINS_LOGIC_VALUE_,
            SC_DO_NOTHING);

    sc_trace_file *tracefile;
    tracefile = sc_create_vcd_trace_file("waveforms/imc_wave");

    sc_trace(tracefile, clk, "clk");
    sc_trace(tracefile, rst, "rst");
    sc_trace(tracefile, RD, "RD");
    sc_trace(tracefile, WR, "WR");
    sc_trace(tracefile, ACT, "ACT");
//    sc_trace(tracefile, RSTB, "RSTB");
    sc_trace(tracefile, AB_mode, "AB_mode");
    sc_trace(tracefile, pim_mode, "pim_mode");
    sc_trace(tracefile, bank_addr, "bank_addr");
    sc_trace(tracefile, row_addr, "row_addr");
    sc_trace(tracefile, col_addr, "col_addr");
    sc_trace(tracefile, DQ, "DQ");
    sc_trace(tracefile, dut.data_out, "data_out");
    sc_trace(tracefile, dut.ext2crf, "ext2crf");
    sc_trace(tracefile, dut.ext2srf, "ext2srf");
    sc_trace(tracefile, dut.ext2grf[1], "ext2grf");
    sc_trace(tracefile, dut.PC, "PC");
    sc_trace(tracefile, dut.instr, "instr");
    sc_trace(tracefile, even_bus, "even_bus");
    sc_trace(tracefile, odd_bus, "odd_bus");
    sc_trace(tracefile, dut.even2grfa[1], "even2grfa");
    sc_trace(tracefile, dut.odd2grfb[1], "odd2grfb");
    sc_trace(tracefile, dut.crf_wr_en, "crf_wr_en");
    sc_trace(tracefile, dut.crf_wr_addr, "crf_wr_addr");
    sc_trace(tracefile, dut.srf_rd_addr, "srf_rd_addr");
    sc_trace(tracefile, dut.srf_wr_addr, "srf_wr_addr");
    sc_trace(tracefile, dut.srf_rd_a_nm, "srf_rd_a_nm");
    sc_trace(tracefile, dut.srf_wr_a_nm, "srf_wr_a_nm");
    sc_trace(tracefile, dut.srf_wr_en, "srf_wr_en");
    sc_trace(tracefile, dut.srf_wr_from, "srf_wr_from");
    sc_trace(tracefile, dut.srf_out, "srf_out");
    sc_trace(tracefile, dut.grfa_rd_addr1, "grfa_rd_addr1");
    sc_trace(tracefile, dut.grfa_rd_addr2, "grfa_rd_addr2");
    sc_trace(tracefile, dut.grfa_wr_addr, "grfa_wr_addr");
    sc_trace(tracefile, dut.grfa_wr_en, "grfa_wr_en");
    sc_trace(tracefile, dut.grfa_relu_en, "grfa_relu_en");
    sc_trace(tracefile, dut.grfa_wr_from, "grfa_wr_from");
    sc_trace(tracefile, dut.grfa_out1[1], "grfa_out1");
    sc_trace(tracefile, dut.grfa_out2[1], "grfa_out2");
    sc_trace(tracefile, dut.grfb_rd_addr1, "grfb_rd_addr1");
    sc_trace(tracefile, dut.grfb_rd_addr2, "grfb_rd_addr2");
    sc_trace(tracefile, dut.grfb_wr_addr, "grfb_wr_addr");
    sc_trace(tracefile, dut.grfb_wr_en, "grfb_wr_en");
    sc_trace(tracefile, dut.grfb_relu_en, "grfb_relu_en");
    sc_trace(tracefile, dut.grfb_wr_from, "grfb_wr_from");
    sc_trace(tracefile, dut.grfb_out1[1], "grfb_out1");
    sc_trace(tracefile, dut.grfb_out2[1], "grfb_out2");
    sc_trace(tracefile, dut.fpu_mult_en, "fpu_mult_en");
    sc_trace(tracefile, dut.fpu_add_en, "fpu_add_en");
    sc_trace(tracefile, dut.fpu_out_sel, "fpu_out_sel");
    sc_trace(tracefile, dut.fpu_mult_in1_sel, "fpu_mult_in1_sel");
    sc_trace(tracefile, dut.fpu_mult_in2_sel, "fpu_mult_in2_sel");
    sc_trace(tracefile, dut.fpu_add_in1_sel, "fpu_add_in1_sel");
    sc_trace(tracefile, dut.fpu_add_in2_sel, "fpu_add_in2_sel");
    sc_trace(tracefile, dut.fpu_out[1], "fpu_out");
//    sc_trace(tracefile, dut.fpunit->multipliers[1].pipeline[0], "mpipeline[0]");
//    sc_trace(tracefile, dut.fpunit->multipliers[1].pipeline[1], "mpipeline[1]");
//    sc_trace(tracefile, dut.fpunit->multipliers[1].pipeline[2], "mpipeline[2]");
//    sc_trace(tracefile, dut.fpunit->multipliers[1].pipeline[3], "mpipeline[3]");
//    sc_trace(tracefile, dut.fpunit->multipliers[1].pipeline[4], "mpipeline[4]");
//    sc_trace(tracefile, dut.fpunit->adders[1].pipeline[0], "apipeline[0]");
//    sc_trace(tracefile, dut.fpunit->adders[1].pipeline[1], "apipeline[1]");
//    sc_trace(tracefile, dut.fpunit->adders[1].pipeline[2], "apipeline[2]");
//    sc_trace(tracefile, dut.fpunit->adders[1].pipeline[3], "apipeline[3]");
//    sc_trace(tracefile, dut.fpunit->adders[1].pipeline[4], "apipeline[4]");
//    sc_trace(tracefile, dut.grfa->rd_addr1, "grfach0_rd_addr1");
//    sc_trace(tracefile, dut.grfa->rd_addr2, "grfach0_rd_addr2");
//    sc_trace(tracefile, dut.grfa->rd_port1[0], "grfach0_rd_port1");
//    sc_trace(tracefile, dut.grfa->rd_port2[0], "grfach0_rd_port2");
//    sc_trace(tracefile, dut.grfa->wr_en, "grfach0_wr_en");
//    sc_trace(tracefile, dut.grfa->wr_addr, "grfach0_wr_addr");
//    sc_trace(tracefile, dut.grfa->rd_addr1, "grfach0_rd_addr1");
//    sc_trace(tracefile, dut.grfa->wr_mux_out[0], "grfach0_wr_port");

    sc_start(850 * CLK_PERIOD, RESOLUTION);

    sc_close_vcd_trace_file(tracefile);

    return 0;
}

#endif
