/*
 * Copyright EPFL 2021
 * Rafael Medina Morillas
 *
 * Description of the Control Unit.
 *
 */

#ifndef CONTROL_UNIT_H_
#define CONTROL_UNIT_H_

#include "interface_unit.h"
#include "instr_decoder.h"
#include "pc_unit.h"

class control_unit: public sc_module {
public:
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
    sc_in<sc_uint<DQ_BITS> >        DQ;	        // Data input from DRAM controller (output makes no sense)
    sc_in<uint32_t>                 instr;		// Instruction input from CRF
    sc_out<uint8_t>                 pc_out;		// PC for the CRF
    sc_out<sc_biguint<GRF_WIDTH> >  data_out;	// Data to the RFs

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
    sc_out<bool> odd_out_en;	// Enables the odd bank tri-state buffer

    // Internal modules
    interface_unit *iu;
    instr_decoder *id;
    pc_unit *pcu;

    // Internal signals and variables
    sc_signal<bool> rf_access, decode_en;
    sc_signal<bool> pc_rst, count_en, jump_en;
    sc_signal<uint8_t> pc;
    sc_signal<uint8_t> jump_num;

    SC_HAS_PROCESS(control_unit);
    control_unit(sc_module_name name) : sc_module(name) {
        iu = new interface_unit("interface_unit");
        // Inputs
        iu->clk(clk);
        iu->rst(rst);
        iu->RD(RD);
        iu->WR(WR);
        iu->ACT(ACT);
//    	iu->RSTB(RSTB);
        iu->AB_mode(AB_mode);
        iu->pim_mode(pim_mode);
        iu->bank_addr(bank_addr);
        iu->row_addr(row_addr);
        iu->col_addr(col_addr);
        iu->DQ(DQ);
        // Outputs
        iu->rf_access(rf_access);
        iu->decode_en(decode_en);
        iu->data_out(data_out);

        id = new instr_decoder("instruction_decoder");
        id->clk(clk);
        id->rst(rst);
        id->rf_access(rf_access);
        id->decode_en(decode_en);
        id->pc_in(pc);
        id->instr(instr);
        id->bank_addr(bank_addr);
        id->row_addr(row_addr);
        id->col_addr(col_addr);
        // Program Counter Control
        id->pc_rst(pc_rst);
        id->count_en(count_en);
        id->jump_en(jump_en);
        id->jump_num(jump_num);
        // CRF Control
        id->crf_wr_en(crf_wr_en);
        id->crf_wr_addr(crf_wr_addr);
        // SRF Control
        id->srf_rd_addr(srf_rd_addr);
        id->srf_rd_a_nm(srf_rd_a_nm);
        id->srf_wr_en(srf_wr_en);
        id->srf_wr_addr(srf_wr_addr);
        id->srf_wr_a_nm(srf_wr_a_nm);
        id->srf_wr_from(srf_wr_from);
        // GRF_A Control
        id->grfa_rd_addr1(grfa_rd_addr1);
        id->grfa_rd_addr2(grfa_rd_addr2);
        id->grfa_wr_en(grfa_wr_en);
        id->grfa_relu_en(grfa_relu_en);
        id->grfa_wr_addr(grfa_wr_addr);
        id->grfa_wr_from(grfa_wr_from);
        // GRF_B Control
        id->grfb_rd_addr1(grfb_rd_addr1);
        id->grfb_rd_addr2(grfb_rd_addr2);
        id->grfb_wr_en(grfb_wr_en);
        id->grfb_relu_en(grfb_relu_en);
        id->grfb_wr_addr(grfb_wr_addr);
        id->grfb_wr_from(grfb_wr_from);
        // FPU Control
        id->fpu_mult_en(fpu_mult_en);
        id->fpu_add_en(fpu_add_en);
        id->fpu_mult_in1_sel(fpu_mult_in1_sel);
        id->fpu_mult_in2_sel(fpu_mult_in2_sel);
        id->fpu_add_in1_sel(fpu_add_in1_sel);
        id->fpu_add_in2_sel(fpu_add_in2_sel);
        id->fpu_out_sel(fpu_out_sel);
        // BANKS Control
        id->even_out_en(even_out_en);
        id->odd_out_en(odd_out_en);

        pcu = new pc_unit("PC_unit");
        pcu->clk(clk);
        pcu->rst(rst);
        pcu->pc_rst(pc_rst);
        pcu->count_en(count_en);
        pcu->jump_en(jump_en);
        pcu->jump_num(jump_num);
        pcu->pc_out(pc);

        SC_METHOD(comb_method);
        sensitive << pc;
    }

    void comb_method();
};

#endif /* CONTROL_UNIT_H_ */
