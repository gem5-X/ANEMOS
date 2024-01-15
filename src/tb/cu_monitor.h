#include "systemc.h"
#include <bitset>

#include "../cnm_base.h"

SC_MODULE(cu_monitor) {

#if MIXED_SIM

	sc_in<sc_logic>     		clk;
	sc_in<sc_logic>     		rst;
	sc_in<sc_logic>				RD;			// DRAM read command
	sc_in<sc_logic>				WR;			// DRAM write command
	sc_in<sc_logic>				ACT;		// DRAM activate command
//    sc_in<sc_logic>				RSTB;		//
	sc_in<sc_logic>				AB_mode;	// Signals if the All-Banks mode is enabled
	sc_in<sc_logic>				pim_mode;	// Signals if the PIM mode is enabled
	sc_in<sc_lv<BANK_BITS> >	bank_addr;	// Address of the bank
	sc_in<sc_lv<ROW_BITS> >		row_addr;	// Address of the bank row
	sc_in<sc_lv<COL_BITS> >		col_addr;	// Address of the bank column
	sc_in<sc_lv<64> >			DQ;			// Data input from DRAM controller (output makes no sense
	sc_in<sc_lv<32> >			instr;		// Instruction input from CRF
	sc_in<sc_lv<32> >			pc_out;		// PC to the CRF
	sc_in<sc_lv<GRF_WIDTH> >	data_out;	// Data to the RFs

	// CRF Control
	sc_in<sc_logic>    	crf_wr_en;		//Enables writing of a received instruction
	sc_in<sc_lv<32> >   crf_wr_addr;	//Index for writing the received instructions from processor

	// SRF Control
	sc_in<sc_lv<32> >	srf_rd_addr;	// Index read
	sc_in<sc_logic>		srf_rd_a_nm;    // Signals if reading from SRF_A (high) or SRF_M (low)
	sc_in<sc_logic>		srf_wr_en;		// Enable writing
	sc_in<sc_lv<32> >	srf_wr_addr;	// Index the address to be written
	sc_in<sc_logic>		srf_wr_a_nm;	// Signals if writing to SRF_A (high) or SRF_M (low)
	sc_in<sc_lv<8> >	srf_wr_from;	// Index the MUX for input data

	// GRF_A Control
	sc_in<sc_lv<32> >	grfa_rd_addr1;	// Index read at port 1
	sc_in<sc_lv<32> >	grfa_rd_addr2;	// Index read at port 2
	sc_in<sc_logic>		grfa_wr_en;		// Enables writing
	sc_in<sc_logic>		grfa_relu_en;	// Enable ReLU for the MOV instruction
	sc_in<sc_lv<32> >	grfa_wr_addr;	// Index the address to be written
	sc_in<sc_lv<8> >	grfa_wr_from;	// Index the MUX for input data

	// GRF_B Control
	sc_in<sc_lv<32> >	grfb_rd_addr1;	// Index read at port 1
	sc_in<sc_lv<32> >	grfb_rd_addr2;	// Index read at port 2
	sc_in<sc_logic>		grfb_wr_en;		// Enables writing
	sc_in<sc_logic>		grfb_relu_en;	// Enable ReLU for the MOV instruction
	sc_in<sc_lv<32> >	grfb_wr_addr;	// Index the address to be written
	sc_in<sc_lv<8> >	grfb_wr_from;	// Index the MUX for input data

	// FPU Control
	sc_in<sc_logic>    	fpu_mult_en;		// Signals that a multiplication computation step should be performed
	sc_in<sc_logic>		fpu_add_en;			// Signals that an addition computation step should be performed
	sc_in<sc_lv<8> >	fpu_mult_in1_sel;   // Selects input 1 for multiplication
	sc_in<sc_lv<8> > 	fpu_mult_in2_sel;   // Selects input 2 for multiplication
	sc_in<sc_lv<8> > 	fpu_add_in1_sel;    // Selects input 1 for addition
	sc_in<sc_lv<8> >	fpu_add_in2_sel;    // Selects input 2 for addition
	sc_in<sc_logic>		fpu_out_sel;		// Selects the output: 0 for adder output, 1 for multiplier output

#else

    sc_in_clk                       clk;
    sc_in<bool>                     rst;
    sc_in<bool>                     RD;			// DRAM read command
    sc_in<bool>                     WR;			// DRAM write command
    sc_in<bool>                     ACT;		// DRAM activate command
//    sc_in<bool>						RSTB;		//
    sc_in<bool>                     AB_mode;	// Signals if the All-Banks mode is enabled
    sc_in<bool>                     pim_mode;	// Signals if the PIM mode is enabled
    sc_in<sc_uint<BANK_BITS> >      bank_addr;	// Address of the bank
    sc_in<sc_uint<ROW_BITS> >       row_addr;	// Address of the bank row
    sc_in<sc_uint<COL_BITS> >       col_addr;	// Address of the bank column
    sc_in<uint64_t>                 DQ;	        // Data input from DRAM controller (output makes no sense
    sc_in<uint32_t>                 instr;		// Instruction input from CRF
    sc_in<uint>                     pc_out;		// PC to the CRF
    sc_in<sc_biguint<GRF_WIDTH> >   data_out;	// Data to the RFs

    // CRF Control
    sc_in<bool> crf_wr_en;		// Enables writing of a received instruction
    sc_in<uint> crf_wr_addr;    // Index for writing the received instructions from processor

    // SRF Control
    sc_in<uint>     srf_rd_addr;	// Index read
    sc_in<bool>     srf_rd_a_nm;    // Signals if reading from SRF_A (high) or SRF_M (low)
    sc_in<bool>     srf_wr_en;		// Enable writing
    sc_in<uint>     srf_wr_addr;	// Index the address to be written
    sc_in<bool>     srf_wr_a_nm;    // Signals if writing to SRF_A (high) or SRF_M (low)
    sc_in<uint8_t>  srf_wr_from;	// Index the MUX for input data

    // GRF_A Control
    sc_in<uint>     grfa_rd_addr1;	// Index read at port 1
    sc_in<uint>     grfa_rd_addr2;	// Index read at port 2
    sc_in<bool>     grfa_wr_en;		// Enables writing
    sc_in<bool>     grfa_relu_en;	// Enable ReLU for the MOV instruction
    sc_in<uint>     grfa_wr_addr;	// Index the address to be written
    sc_in<uint8_t>  grfa_wr_from;	// Index the MUX for input data

    // GRF_B Control
    sc_in<uint>     grfb_rd_addr1;	// Index read at port 1
    sc_in<uint>     grfb_rd_addr2;	// Index read at port 2
    sc_in<bool>     grfb_wr_en;		// Enables writing
    sc_in<bool>     grfb_relu_en;	// Enable ReLU for the MOV instruction
    sc_in<uint>     grfb_wr_addr;	// Index the address to be written
    sc_in<uint8_t>  grfb_wr_from;	// Index the MUX for input data

    // FPU Control
    sc_in<bool>     fpu_mult_en;        // Signals that a multiplication computation step should be performed
    sc_in<bool>     fpu_add_en;	        // Signals that an addition computation step should be performed
    sc_in<uint8_t>  fpu_mult_in1_sel;   // Selects input 1 for multiplication
    sc_in<uint8_t>  fpu_mult_in2_sel;   // Selects input 2 for multiplication
    sc_in<uint8_t>  fpu_add_in1_sel;    // Selects input 1 for addition
    sc_in<uint8_t>  fpu_add_in2_sel;    // Selects input 2 for addition
    sc_in<bool>     fpu_out_sel;        // Selects the output: 0 for adder output, 1 for multiplier output

#endif

    // Internal events

    // Internal variables and signals for checking

    SC_CTOR(cu_monitor) {
        SC_THREAD(monitor_thread);
        sensitive << clk.pos() << rst.neg();
    }

    void monitor_thread(); // Outputs the behavior for the first SIMD rail, and automatically checks the functionality
//    void mirror_thread();		// Mirror FPU behavior for checking its functionality
//    void out_mirror_thread();	// Mirror FPU output for checking functionality
};
