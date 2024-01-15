#ifndef CU_MAIN_H_
#define CU_MAIN_H_

#include "cu_driver.h"
#include "cu_monitor.h"
#include <string>

#if MIXED_SIM
#include "../cu_wrapped.h"
#else
#include "../control_unit.h"
#endif

#ifdef MTI_SYSTEMC

SC_MODULE(top) {

#if MIXED_SIM

	sc_clock       					clk_g;
	sc_signal<sc_logic>				clk;
	sc_signal<sc_logic>     		rst;
	sc_signal<sc_logic>				RD;			// DRAM read command
	sc_signal<sc_logic>				WR;			// DRAM write command
	sc_signal<sc_logic>				ACT;		// DRAM activate command
//    sc_signal<sc_logic>				RSTB;		//
	sc_signal<sc_logic>				AB_mode;	// Signals if the All-Banks mode is enabled
	sc_signal<sc_logic>				pim_mode;	// Signals if the PIM mode is enabled
	sc_signal<sc_lv<BANK_BITS> >	bank_addr;	// Address of the bank
	sc_signal<sc_lv<ROW_BITS> >		row_addr;	// Address of the bank row
	sc_signal<sc_lv<COL_BITS> >		col_addr;	// Address of the bank column
	sc_signal<sc_lv<64> >			DQ;			// Data input from DRAM controller (output makes no sense
	sc_signal<sc_lv<32> >			instr;		// Instruction input from CRF
	sc_signal<sc_lv<32> >			pc_out;		// PC to the CRF
	sc_signal<sc_lv<GRF_WIDTH> >	data_out;	// Data to the RFs
	// CRF Control
	sc_signal<sc_logic>    	crf_wr_en;		//Enables writing of a received instruction
	sc_signal<sc_lv<32> >   crf_wr_addr;	//Index for writing the received instructions from processor
	// SRF Control
	sc_signal<sc_lv<32> >	srf_rd_addr;	// Index read
	sc_signal<sc_logic>		srf_rd_a_nm;    // Signals if reading from SRF_A (high) or SRF_M (low)
	sc_signal<sc_logic>		srf_wr_en;		// Enable writing
	sc_signal<sc_lv<32> >	srf_wr_addr;	// Index the address to be written
	sc_signal<sc_logic>		srf_wr_a_nm;	// Signals if writing to SRF_A (high) or SRF_M (low)
	sc_signal<sc_lv<8> >	srf_wr_from;	// Index the MUX for input data
	// GRF_A Control
	sc_signal<sc_lv<32> >	grfa_rd_addr1;	// Index read at port 1
	sc_signal<sc_lv<32> >	grfa_rd_addr2;	// Index read at port 2
	sc_signal<sc_logic>		grfa_wr_en;		// Enables writing
	sc_signal<sc_logic>		grfa_relu_en;	// Enable ReLU for the MOV instruction
	sc_signal<sc_lv<32> >	grfa_wr_addr;	// Index the address to be written
	sc_signal<sc_lv<8> >	grfa_wr_from;	// Index the MUX for input data
	// GRF_B Control
	sc_signal<sc_lv<32> >	grfb_rd_addr1;	// Index read at port 1
	sc_signal<sc_lv<32> >	grfb_rd_addr2;	// Index read at port 2
	sc_signal<sc_logic>		grfb_wr_en;		// Enables writing
	sc_signal<sc_logic>		grfb_relu_en;	// Enable ReLU for the MOV instruction
	sc_signal<sc_lv<32> >	grfb_wr_addr;	// Index the address to be written
	sc_signal<sc_lv<8> >	grfb_wr_from;	// Index the MUX for input data
	// FPU Control
	sc_signal<sc_logic>    	fpu_mult_en;		// Signals that a multiplication computation step should be performed
	sc_signal<sc_logic>    	fpu_add_en;			// Signals that an addition computation step should be performed
	sc_signal<sc_lv<8> >	fpu_mult_in1_sel;   // Selects input 1 for multiplication
	sc_signal<sc_lv<8> > 	fpu_mult_in2_sel;   // Selects input 2 for multiplication
	sc_signal<sc_lv<8> > 	fpu_add_in1_sel;    // Selects input 1 for addition
	sc_signal<sc_lv<8> >	fpu_add_in2_sel;    // Selects input 2 for addition
	sc_signal<sc_logic>		fpu_out_sel;		// Selects the output: 0 for adder output, 1 for multiplier output

	control_unit dut;
	cu_driver driver;
	cu_monitor monitor;

	SC_CTOR(top) : clk_g("clk", 10, SC_NS), dut("ControlUnitUnderTest", "control_unit"),
					driver("Driver"), monitor("Monitor")
	{
		SC_METHOD(clock_method);
		sensitive << clk_g;

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
		dut.instr(instr);
		dut.pc_out(pc_out);
		dut.data_out(data_out);
		// CRF Control
		dut.crf_wr_en(crf_wr_en);
		dut.crf_wr_addr(crf_wr_addr);
		// SRF Control
		dut.srf_rd_addr(srf_rd_addr);
		dut.srf_rd_a_nm(srf_rd_a_nm);
		dut.srf_wr_en(srf_wr_en);
		dut.srf_wr_addr(srf_wr_addr);
		dut.srf_wr_a_nm(srf_wr_a_nm);
		dut.srf_wr_from(srf_wr_from);
		// GRF_A Control
		dut.grfa_rd_addr1(grfa_rd_addr1);
		dut.grfa_rd_addr2(grfa_rd_addr2);
		dut.grfa_wr_en(grfa_wr_en);
		dut.grfa_relu_en(grfa_relu_en);
		dut.grfa_wr_addr(grfa_wr_addr);
		dut.grfa_wr_from(grfa_wr_from);
		// GRF_B Control
		dut.grfb_rd_addr1(grfb_rd_addr1);
		dut.grfb_rd_addr2(grfb_rd_addr2);
		dut.grfb_wr_en(grfb_wr_en);
		dut.grfb_relu_en(grfb_relu_en);
		dut.grfb_wr_addr(grfb_wr_addr);
		dut.grfb_wr_from(grfb_wr_from);
		// FPU Control
		dut.fpu_mult_en(fpu_mult_en);
		dut.fpu_add_en(fpu_add_en);
		dut.fpu_mult_in1_sel(fpu_mult_in1_sel);
		dut.fpu_mult_in2_sel(fpu_mult_in2_sel);
		dut.fpu_add_in1_sel(fpu_add_in1_sel);
		dut.fpu_add_in2_sel(fpu_add_in2_sel);
		dut.fpu_out_sel(fpu_out_sel);

#else

	sc_clock       						clk;
	sc_signal<bool>     				rst;
	sc_signal<bool>						RD;			// DRAM read command
	sc_signal<bool>						WR;			// DRAM write command
	sc_signal<bool>						ACT;		// DRAM activate command
//    sc_signal<bool>						RSTB;		//
	sc_signal<bool>						AB_mode;	// Signals if the All-Banks mode is enabled
	sc_signal<bool>						pim_mode;	// Signals if the PIM mode is enabled
	sc_signal<sc_uint<BANK_BITS> >		bank_addr;	// Address of the bank
	sc_signal<sc_uint<ROW_BITS> >		row_addr;	// Address of the bank row
	sc_signal<sc_uint<COL_BITS> >		col_addr;	// Address of the bank column
	sc_signal<uint64_t>					DQ;			// Data input from DRAM controller (output makes no sense
	sc_signal<uint32_t>					instr;		// Instruction input from CRF
	sc_signal<uint>						pc_out;		// PC to the CRF
	sc_signal<sc_biguint<GRF_WIDTH> >	data_out;	// Data to the RFs
	// CRF Control
	sc_signal<bool>    	crf_wr_en;		//Enables writing of a received instruction
	sc_signal<uint>    	crf_wr_addr;	//Index for writing the received instructions from processor
	// SRF Control
	sc_signal<uint>		srf_rd_addr;	// Index read
	sc_signal<bool>		srf_rd_a_nm;    // Signals if reading from SRF_A (high) or SRF_M (low)
	sc_signal<bool>		srf_wr_en;		// Enable writing
	sc_signal<uint>		srf_wr_addr;	// Index the address to be written
	sc_signal<bool>		srf_wr_a_nm;	// Signals if writing to SRF_A (high) or SRF_M (low)
	sc_signal<uint8_t>	srf_wr_from;	// Index the MUX for input data
	// GRF_A Control
	sc_signal<uint>		grfa_rd_addr1;	// Index read at port 1
	sc_signal<uint>		grfa_rd_addr2;	// Index read at port 2
	sc_signal<bool>		grfa_wr_en;		// Enables writing
	sc_signal<bool>		grfa_relu_en;	// Enable ReLU for the MOV instruction
	sc_signal<uint>		grfa_wr_addr;	// Index the address to be written
	sc_signal<uint8_t>	grfa_wr_from;	// Index the MUX for input data
	// GRF_B Control
	sc_signal<uint>		grfb_rd_addr1;	// Index read at port 1
	sc_signal<uint>		grfb_rd_addr2;	// Index read at port 2
	sc_signal<bool>		grfb_wr_en;		// Enables writing
	sc_signal<bool>		grfb_relu_en;	// Enable ReLU for the MOV instruction
	sc_signal<uint>		grfb_wr_addr;	// Index the address to be written
	sc_signal<uint8_t>	grfb_wr_from;	// Index the MUX for input data
	// FPU Control
	sc_signal<bool>    	fpu_mult_en;		// Signals that a multiplication computation step should be performed
	sc_signal<bool>    	fpu_add_en;			// Signals that an addition computation step should be performed
	sc_signal<uint8_t>	fpu_mult_in1_sel;   // Selects input 1 for multiplication
	sc_signal<uint8_t> 	fpu_mult_in2_sel;   // Selects input 2 for multiplication
	sc_signal<uint8_t> 	fpu_add_in1_sel;    // Selects input 1 for addition
	sc_signal<uint8_t>	fpu_add_in2_sel;    // Selects input 2 for addition
	sc_signal<bool>		fpu_out_sel;		// Selects the output: 0 for adder output, 1 for multiplier output

	control_unit dut;
	cu_driver driver;
	cu_monitor monitor;

	SC_CTOR(top) : clk("clk", 10, SC_NS), dut("ControlUnitUnderTest", 1),
					driver("Driver"), monitor("Monitor")
	{
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
		dut.instr(instr);
		dut.pc_out(pc_out);
		dut.data_out(data_out);
		// CRF Control
		dut.crf_wr_en(crf_wr_en);
		dut.crf_wr_addr(crf_wr_addr);
		// SRF Control
		dut.srf_rd_addr(srf_rd_addr);
		dut.srf_rd_a_nm(srf_rd_a_nm);
		dut.srf_wr_en(srf_wr_en);
		dut.srf_wr_addr(srf_wr_addr);
		dut.srf_wr_a_nm(srf_wr_a_nm);
		dut.srf_wr_from(srf_wr_from);
		// GRF_A Control
		dut.grfa_rd_addr1(grfa_rd_addr1);
		dut.grfa_rd_addr2(grfa_rd_addr2);
		dut.grfa_wr_en(grfa_wr_en);
		dut.grfa_relu_en(grfa_relu_en);
		dut.grfa_wr_addr(grfa_wr_addr);
		dut.grfa_wr_from(grfa_wr_from);
		// GRF_B Control
		dut.grfb_rd_addr1(grfb_rd_addr1);
		dut.grfb_rd_addr2(grfb_rd_addr2);
		dut.grfb_wr_en(grfb_wr_en);
		dut.grfb_relu_en(grfb_relu_en);
		dut.grfb_wr_addr(grfb_wr_addr);
		dut.grfb_wr_from(grfb_wr_from);
		// FPU Control
		dut.fpu_mult_en(fpu_mult_en);
		dut.fpu_add_en(fpu_add_en);
		dut.fpu_mult_in1_sel(fpu_mult_in1_sel);
		dut.fpu_mult_in2_sel(fpu_mult_in2_sel);
		dut.fpu_add_in1_sel(fpu_add_in1_sel);
		dut.fpu_add_in2_sel(fpu_add_in2_sel);
		dut.fpu_out_sel(fpu_out_sel);

#endif

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
		driver.instr(instr);

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
		monitor.instr(instr);
		monitor.pc_out(pc_out);
		monitor.data_out(data_out);
		// CRF Control
		monitor.crf_wr_en(crf_wr_en);
		monitor.crf_wr_addr(crf_wr_addr);
		// SRF Control
		monitor.srf_rd_addr(srf_rd_addr);
		monitor.srf_rd_a_nm(srf_rd_a_nm);
		monitor.srf_wr_en(srf_wr_en);
		monitor.srf_wr_addr(srf_wr_addr);
		monitor.srf_wr_a_nm(srf_wr_a_nm);
		monitor.srf_wr_from(srf_wr_from);
		// GRF_A Control
		monitor.grfa_rd_addr1(grfa_rd_addr1);
		monitor.grfa_rd_addr2(grfa_rd_addr2);
		monitor.grfa_wr_en(grfa_wr_en);
		monitor.grfa_relu_en(grfa_relu_en);
		monitor.grfa_wr_addr(grfa_wr_addr);
		monitor.grfa_wr_from(grfa_wr_from);
		// GRF_B Control
		monitor.grfb_rd_addr1(grfb_rd_addr1);
		monitor.grfb_rd_addr2(grfb_rd_addr2);
		monitor.grfb_wr_en(grfb_wr_en);
		monitor.grfb_relu_en(grfb_relu_en);
		monitor.grfb_wr_addr(grfb_wr_addr);
		monitor.grfb_wr_from(grfb_wr_from);
		// FPU Control
		monitor.fpu_mult_en(fpu_mult_en);
		monitor.fpu_add_en(fpu_add_en);
		monitor.fpu_mult_in1_sel(fpu_mult_in1_sel);
		monitor.fpu_mult_in2_sel(fpu_mult_in2_sel);
		monitor.fpu_add_in1_sel(fpu_add_in1_sel);
		monitor.fpu_add_in2_sel(fpu_add_in2_sel);
		monitor.fpu_out_sel(fpu_out_sel);
	}

#if MIXED_SIM
	void clock_method();	// Turns bool clk_g into sc_logic clk
#endif

};

#endif

#endif /* CU_MAIN_H_ */
