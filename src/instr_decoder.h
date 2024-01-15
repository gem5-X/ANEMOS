/*
 * Copyright EPFL 2021
 * Rafael Medina Morillas
 *
 * Description of the Instruction Decoder Unit.
 *
 */

#ifndef INSTR_DECODER_H_
#define INSTR_DECODER_H_

#include "systemc.h"

#include "cnm_base.h"

class instr_decoder: public sc_module {
public:
    sc_in_clk                   clk;
    sc_in<bool>                 rst;
    sc_in<bool>                 rf_access;	// Enables access to the RFs while not in PIM mode
    sc_in<bool>                 decode_en;	// Enables decoding of the next CRF instruction
    sc_in<uint8_t>              pc_in;		// Program Counter
    sc_in<uint32_t>             instr;		// Instruction input from CRF
    sc_in<sc_uint<BANK_BITS> >  bank_addr;   // Address of the bank row
    sc_in<sc_uint<ROW_BITS> >   row_addr;	// Address of the bank row
    sc_in<sc_uint<COL_BITS> >   col_addr;	// Address of the bank column

    // Program Counter Control
    sc_out<bool>    pc_rst;     // Synchronous reset when end of CRF or EXIT instruction
    sc_out<bool>    count_en;   //Enables the increase of PC
    sc_out<bool>    jump_en;    // Signals a jump which will translate into a decrease of PC
    sc_out<uint8_t> jump_num;   // Amount to subtract from the PC when doing a jump

    // CRF Control
    sc_out<bool>    crf_wr_en;		//Enables writing of a received instruction
    sc_out<uint>    crf_wr_addr;    //Index for writing the received instructions from processor

    // SRF Control
    sc_out<uint>    srf_rd_addr;	// Index read
    sc_out<bool>    srf_rd_a_nm;    // Signals if reading from SRF_A (high) or SRF_M (low)
    sc_out<bool>    srf_wr_en;		// Enable writing
    sc_out<uint>    srf_wr_addr;	// Index the address to be written
    sc_out<bool>    srf_wr_a_nm;    // Signals if writing to SRF_A (high) or SRF_M (low)
    sc_out<uint8_t> srf_wr_from;	// Index the MUX for input data

    // GRF_A Control
    sc_out<uint>    grfa_rd_addr1;	// Index read at port 1
    sc_out<uint>    grfa_rd_addr2;	// Index read at port 2
    sc_out<bool>    grfa_wr_en;		// Enables writing
    sc_out<bool>    grfa_relu_en;	// Enable ReLU for the MOV instruction
    sc_out<uint>    grfa_wr_addr;	// Index the address to be written
    sc_out<uint8_t> grfa_wr_from;	// Index the MUX for input data

    // GRF_B Control
    sc_out<uint>    grfb_rd_addr1;	// Index read at port 1
    sc_out<uint>    grfb_rd_addr2;	// Index read at port 2
    sc_out<bool>    grfb_wr_en;		// Enables writing
    sc_out<bool>    grfb_relu_en;	// Enable ReLU for the MOV instruction
    sc_out<uint>    grfb_wr_addr;	// Index the address to be written
    sc_out<uint8_t> grfb_wr_from;	// Index the MUX for input data

    // FPU Control
    sc_out<bool>    fpu_mult_en;        // Signals that a multiplication computation step should be performed
    sc_out<bool>    fpu_add_en;         // Signals that an addition computation step should be performed
    sc_out<uint8_t> fpu_mult_in1_sel;   // Selects input 1 for multiplication
    sc_out<uint8_t> fpu_mult_in2_sel;   // Selects input 2 for multiplication
    sc_out<uint8_t> fpu_add_in1_sel;    // Selects input 1 for addition
    sc_out<uint8_t> fpu_add_in2_sel;    // Selects input 2 for addition
    sc_out<bool>    fpu_out_sel;        // Selects the output: 0 for adder output, 1 for multiplier output

    // BANKS Control
    sc_out<bool> even_out_en;	// Enables the even bank tri-state buffer
    sc_out<bool> odd_out_en;		// Enables the odd bank tri-state buffer

    // ** INTERNAL SIGNALS AND VARIABLES **
    // Name scheme:
    // AftLoad	=	The load stage is present in the pipeline
    // toAdd	=	Controls an input to the adder within a MAD or ADD
    // toWB		=	Controls an input to the writeback stage
    // toMoA	=	Controls an input to the multiplier or adder
    // toMAW	=	Controls an input to the multiplier, adder or writeback stage
    // fromMul	=	Controls an output from a Multiply
    // fromAdd	=	Controls an output from a Add
    // fromMaA	=	Controls an output from a Multiply + Add (MAD or MAC)
    // fromLoad	=	Controls an output from a Load

    // NOP signals and variables
    sc_signal<uint8_t> nop_cnt_nxt, nop_cnt_reg;

    // Jump signals and variables
    sc_signal<bool> jmp_act_nxt, jmp_act_reg;
    sc_signal<uint> jmp_cnt_nxt, jmp_cnt_reg;

    // GRF_A signals, variables and pipelines
    sc_signal<uint>     grfa_rd_addr1_comb, grfa_rd_addr1_toAddAftLoad, grfa_rd_addr1_toAdd;
    sc_signal<uint>     grfa_rd_addr1_toMAWAftLoad, grfa_rd_addr1_pipe[1 + MULT_STAGES];
    sc_signal<uint>     grfa_rd_addr2_comb, grfa_rd_addr2_nxt, grfa_rd_addr2_reg;
    sc_signal<bool>     grfa_wr_en_comb, grfa_wr_en_fromMaAAftLoad, grfa_wr_en_fromMaA;
    sc_signal<bool>     grfa_wr_en_fromMulAftLoad, grfa_wr_en_fromMul;
    sc_signal<bool>     grfa_wr_en_fromAddAftLoad, grfa_wr_en_fromAdd;
    sc_signal<bool>     grfa_wr_en_fromLoad, grfa_wr_en_pipe[1 + MULT_STAGES + ADD_STAGES];
    sc_signal<uint>     grfa_wr_addr_comb, grfa_wr_addr_fromMaAAftLoad, grfa_wr_addr_fromMaA;
    sc_signal<uint>     grfa_wr_addr_fromMulAftLoad, grfa_wr_addr_fromMul;
    sc_signal<uint>     grfa_wr_addr_fromAddAftLoad, grfa_wr_addr_fromAdd;
    sc_signal<uint>     grfa_wr_addr_fromLoad, grfa_wr_addr_pipe[1 + MULT_STAGES + ADD_STAGES];
    sc_signal<uint8_t>  grfa_wr_from_comb, grfa_wr_from_fromMaAAftLoad, grfa_wr_from_fromMaA;
    sc_signal<uint8_t>  grfa_wr_from_fromMulAftLoad, grfa_wr_from_fromMul;
    sc_signal<uint8_t>  grfa_wr_from_fromAddAftLoad, grfa_wr_from_fromAdd;
    sc_signal<uint8_t>  grfa_wr_from_fromLoad, grfa_wr_from_pipe[1 + MULT_STAGES + ADD_STAGES];
    sc_signal<bool>     grfa_relu_en_comb, grfa_relu_en_nxt, grfa_relu_en_reg;

    // GRF_B signals, variables and pipelines
    sc_signal<uint>     grfb_rd_addr1_comb, grfb_rd_addr1_toAddAftLoad, grfb_rd_addr1_toAdd;
    sc_signal<uint>     grfb_rd_addr1_toMAWAftLoad, grfb_rd_addr1_pipe[1 + MULT_STAGES];
    sc_signal<uint>     grfb_rd_addr2_comb, grfb_rd_addr2_nxt, grfb_rd_addr2_reg;
    sc_signal<bool>     grfb_wr_en_comb, grfb_wr_en_fromMaAAftLoad, grfb_wr_en_fromMaA;
    sc_signal<bool>     grfb_wr_en_fromMulAftLoad, grfb_wr_en_fromMul;
    sc_signal<bool>     grfb_wr_en_fromAddAftLoad, grfb_wr_en_fromAdd;
    sc_signal<bool>     grfb_wr_en_fromLoad, grfb_wr_en_pipe[1 + MULT_STAGES + ADD_STAGES];
    sc_signal<uint>     grfb_wr_addr_comb, grfb_wr_addr_fromMaAAftLoad, grfb_wr_addr_fromMaA;
    sc_signal<uint>     grfb_wr_addr_fromMulAftLoad, grfb_wr_addr_fromMul;
    sc_signal<uint>     grfb_wr_addr_fromAddAftLoad, grfb_wr_addr_fromAdd;
    sc_signal<uint>     grfb_wr_addr_fromLoad, grfb_wr_addr_pipe[1 + MULT_STAGES + ADD_STAGES];
    sc_signal<uint8_t>  grfb_wr_from_comb, grfb_wr_from_fromMaAAftLoad, grfb_wr_from_fromMaA;
    sc_signal<uint8_t>  grfb_wr_from_fromMulAftLoad, grfb_wr_from_fromMul;
    sc_signal<uint8_t>  grfb_wr_from_fromAddAftLoad, grfb_wr_from_fromAdd;
    sc_signal<uint8_t>  grfb_wr_from_fromLoad, grfb_wr_from_pipe[1 + MULT_STAGES + ADD_STAGES];
    sc_signal<bool>     grfb_relu_en_comb, grfb_relu_en_nxt, grfb_relu_en_reg;

    // SRF signals, variables and pipelines
    sc_signal<uint>     srf_rd_addr_comb, srf_rd_addr_toAddAftLoad, srf_rd_addr_toAdd;
    sc_signal<uint>     srf_rd_addr_toMAWAftLoad, srf_rd_addr_pipe[1 + MULT_STAGES];
    sc_signal<bool>     srf_rd_a_nm_comb, srf_rd_a_nm_toAddAftLoad, srf_rd_a_nm_toAdd;
    sc_signal<bool>     srf_rd_a_nm_toMAWAftLoad, srf_rd_a_nm_pipe[1 + MULT_STAGES];
    sc_signal<bool>     srf_wr_en_comb, srf_wr_en_nxt, srf_wr_en_reg;
    sc_signal<uint8_t>  srf_wr_from_comb, srf_wr_from_nxt, srf_wr_from_reg;
    sc_signal<uint>     srf_wr_addr_comb, srf_wr_addr_nxt, srf_wr_addr_reg;
    sc_signal<bool>     srf_wr_a_nm_comb, srf_wr_a_nm_nxt, srf_wr_a_nm_reg;

    // FPU signals, variables and pipelines
    sc_signal<uint8_t>  fpu_add_in1_sel_comb, fpu_add_in1_sel_toAddAftLoad, fpu_add_in1_sel_toAdd;
    sc_signal<uint8_t>  fpu_add_in1_sel_toMoAAftLoad, fpu_add_in1_sel_pipe[1 + MULT_STAGES];
    sc_signal<uint8_t>  fpu_add_in2_sel_comb, fpu_add_in2_sel_toAddAftLoad, fpu_add_in2_sel_toAdd;
    sc_signal<uint8_t>  fpu_add_in2_sel_toMoAAftLoad, fpu_add_in2_sel_pipe[1 + MULT_STAGES];
    sc_signal<uint8_t>  fpu_mult_in1_sel_comb, fpu_mult_in1_sel_nxt, fpu_mult_in1_sel_reg;
    sc_signal<uint8_t>  fpu_mult_in2_sel_comb, fpu_mult_in2_sel_nxt, fpu_mult_in2_sel_reg;
    bool                add_en_or;
    sc_signal<bool>     add_en_toAddAftLoad, add_en_toAdd, add_en_toMoA, add_en_toMoAAftLoad;
    sc_signal<bool>     add_en_pipe[MULT_STAGES + ADD_STAGES];
    bool                mul_en_or;
    sc_signal<bool>     mul_en_toMoA, mul_en_toMoAAftLoad, mul_en_pipe[MULT_STAGES];
    sc_signal<bool>     fpu_out_sel_fromMulAftLoad, fpu_out_sel_fromMul, fpu_out_sel_pipe[1 + MULT_STAGES];

    SC_CTOR(instr_decoder) {

        int i;

        SC_THREAD(clk_thread);
        sensitive << clk.pos();
        async_reset_signal_is(rst, false);

        SC_METHOD(comb_method);
        sensitive << instr << rf_access << decode_en << bank_addr << row_addr << col_addr;
        sensitive << nop_cnt_reg;
        sensitive << jmp_act_reg << jmp_cnt_reg;

        SC_METHOD(out_method);
        sensitive << srf_rd_addr_comb << srf_rd_addr_pipe[MULT_STAGES];
        sensitive << srf_rd_a_nm_comb << srf_rd_a_nm_pipe[MULT_STAGES];
        sensitive << srf_wr_en_comb << srf_wr_en_reg;
        sensitive << srf_wr_addr_comb << srf_wr_addr_reg;
        sensitive << srf_wr_a_nm_comb << srf_wr_a_nm_reg;
        sensitive << srf_wr_from_comb << srf_wr_from_reg;
        sensitive << grfa_rd_addr1_comb << grfa_rd_addr1_pipe[MULT_STAGES];
        sensitive << grfa_rd_addr2_comb << grfa_rd_addr2_reg;
        sensitive << grfa_wr_en_comb
                << grfa_wr_en_pipe[MULT_STAGES + ADD_STAGES];
        sensitive << grfa_relu_en_comb << grfa_relu_en_reg;
        sensitive << grfa_wr_addr_comb
                << grfa_wr_addr_pipe[MULT_STAGES + ADD_STAGES];
        sensitive << grfa_wr_from_comb
                << grfa_wr_from_pipe[MULT_STAGES + ADD_STAGES];
        sensitive << grfb_rd_addr1_comb << grfb_rd_addr1_pipe[MULT_STAGES];
        sensitive << grfb_rd_addr2_comb << grfb_rd_addr2_reg;
        sensitive << grfb_wr_en_comb
                << grfb_wr_en_pipe[MULT_STAGES + ADD_STAGES];
        sensitive << grfb_relu_en_comb << grfb_relu_en_reg;
        sensitive << grfb_wr_addr_comb
                << grfb_wr_addr_pipe[MULT_STAGES + ADD_STAGES];
        sensitive << grfb_wr_from_comb
                << grfb_wr_from_pipe[MULT_STAGES + ADD_STAGES];
        for (i = 0; i < MULT_STAGES; i++) {
            sensitive << mul_en_pipe[i];
        }
        for (i = MULT_STAGES; i < MULT_STAGES + ADD_STAGES; i++) {
            sensitive << add_en_pipe[i];
        }
        sensitive << mul_en_toMoA << add_en_toMoA;
        sensitive << fpu_mult_in1_sel_comb << fpu_mult_in1_sel_reg;
        sensitive << fpu_mult_in2_sel_comb << fpu_mult_in2_sel_reg;
        sensitive << fpu_add_in1_sel_comb << fpu_add_in1_sel_pipe[MULT_STAGES];
        sensitive << fpu_add_in2_sel_comb << fpu_add_in2_sel_pipe[MULT_STAGES];
        sensitive << fpu_out_sel_pipe[MULT_STAGES];

        nop_cnt_reg = false;

        jmp_act_reg = false;
        jmp_cnt_reg = false;

        grfa_rd_addr1_comb = 0;
        grfa_rd_addr2_comb = 0;
        grfa_rd_addr2_reg = 0;
        grfa_wr_en_comb = false;
        grfa_wr_addr_comb = 0;
        grfa_wr_from_comb = 0;
        grfa_relu_en_comb = false;
        grfa_relu_en_reg = false;

        grfb_rd_addr1_comb = 0;
        grfb_rd_addr2_comb = 0;
        grfb_rd_addr2_reg = 0;
        grfb_wr_en_comb = false;
        grfb_wr_addr_comb = 0;
        grfb_wr_from_comb = 0;
        grfb_relu_en_comb = false;
        grfb_relu_en_reg = false;

        srf_rd_addr_comb = 0;
        srf_rd_a_nm_comb = false;
        srf_wr_en_comb = false;
        srf_wr_en_reg = false;
        srf_wr_from_comb = 0;
        srf_wr_from_reg = 0;
        srf_wr_addr_comb = 0;
        srf_wr_addr_reg = 0;
        srf_wr_a_nm_comb = false;
        srf_wr_a_nm_reg = false;

        fpu_add_in1_sel_comb = 0;
        fpu_add_in2_sel_comb = 0;
        fpu_mult_in1_sel_comb = 0;
        fpu_mult_in1_sel_reg = 0;
        fpu_mult_in2_sel_comb = 0;
        fpu_mult_in2_sel_reg = 0;
        mul_en_toMoA = false;
        mul_en_or = false;
        add_en_toMoA = false;
        add_en_or = false;

        for (i = 0; i < MULT_STAGES; i++) {
            mul_en_pipe[i] = false;
        }

        for (i = 0; i < (1 + MULT_STAGES); i++) {
            grfa_rd_addr1_pipe[i] = 0;

            srf_rd_addr_pipe[i] = 0;
            srf_rd_a_nm_pipe[i] = false;

            fpu_add_in1_sel_pipe[i] = 0;
            fpu_add_in2_sel_pipe[i] = 0;
            fpu_out_sel_pipe[i] = false;
        }

        for (i = 0; i < (MULT_STAGES + ADD_STAGES); i++) {
            add_en_pipe[i] = false;
        }

        for (i = 0; i < (1 + MULT_STAGES + ADD_STAGES); i++) {
            grfa_wr_en_pipe[i] = false;
            grfa_wr_addr_pipe[i] = 0;
            grfa_wr_from_pipe[i] = 0;

            grfb_wr_en_pipe[i] = false;
            grfb_wr_addr_pipe[i] = 0;
            grfb_wr_from_pipe[i] = 0;
        }
    }

    void clk_thread();	// Performs sequential logic (and resets)
    void comb_method(); // Performs the combinational logic
    void out_method();	// Performs output combinational logic
};

#endif /* INSTR_DECODER_H_ */
