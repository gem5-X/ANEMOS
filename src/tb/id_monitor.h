#include "systemc.h"
#include <bitset>

#include "../cnm_base.h"

SC_MODULE(id_monitor) {
    sc_in<bool>                 clk;
    sc_in<bool>                 rst;
    sc_in<bool>                 rf_access;	// Enables rd/wr to the RFs while not in PIM mode
//	sc_in<bool>					rf_wr_nrd;	// Signals if reading (low) or writing (high)
    sc_in<bool>                 decode_en;	// Enables decoding of the next CRF instruction
    sc_in<uint8_t>              pc_in;		// Program Counter
    sc_in<uint32_t>             instr;		// Instruction input from CRF
    sc_in<sc_uint<ROW_BITS> >   row_addr;	// Address of the bank row
    sc_in<sc_uint<COL_BITS> >   col_addr;	// Address of the bank column
    // Program Counter Control
    sc_in<bool>     pc_rst;	// Synchronous reset when end of CRF or EXIT instruction
    sc_in<bool>     count_en;	// Enables the increase of PC
    sc_in<bool>     jump_en;// Signals a jump which will translate into a decrease of PC
    sc_in<uint8_t>  jump_num;// Amount to subtract from the PC when doing a jump
    // CRF Control
    sc_in<bool> crf_wr_en;		//Enables writing of a received instruction
    sc_in<uint> crf_wr_addr;//Index for writing the received instructions from processor
    // SRF Control
    sc_in<uint>     srf_rd_addr;	// Index read
    sc_in<bool>     srf_rd_a_nm; // Signals if reading from SRF_A (high) or SRF_M (low)
    sc_in<bool>     srf_wr_en;		// Enable writing
    sc_in<uint>     srf_wr_addr;	// Index the address to be written
    sc_in<bool>     srf_wr_a_nm;// Signals if writing to SRF_A (high) or SRF_M (low)
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
    sc_in<bool>     fpu_mult_en;// Signals that a multiplication computation step should be performed
    sc_in<bool>     fpu_add_en;	// Signals that an addition computation step should be performed
    sc_in<uint8_t>  fpu_mult_in1_sel;   // Selects input 1 for multiplication
    sc_in<uint8_t>  fpu_mult_in2_sel;   // Selects input 2 for multiplication
    sc_in<uint8_t>  fpu_add_in1_sel;    // Selects input 1 for addition
    sc_in<uint8_t>  fpu_add_in2_sel;    // Selects input 2 for addition
    sc_in<bool>     fpu_out_sel; // Selects the output: 0 for adder output, 1 for multiplier output

    // Internal events

    // Internal variables and signals for checking

    SC_CTOR(id_monitor) {
        SC_THREAD(monitor_thread);
        sensitive << clk.pos() << rst.neg();
    }

    void monitor_thread(); // Outputs the behavior for the first SIMD rail, and automatically checks the functionality
//    void mirror_thread();		// Mirror FPU behavior for checking its functionality
//    void out_mirror_thread();	// Mirror FPU output for checking functionality
};
