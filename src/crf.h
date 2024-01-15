/* 
 * Copyright EPFL 2021
 * Rafael Medina Morillas
 * 
 * Description of the Configuration Register File (CRF).
 *
 */

#ifndef CRF_H
#define CRF_H

#include "systemc.h"

#include "cnm_base.h"
#include "rf_twoport.h"

class crf: public sc_module {
public:
    sc_in_clk           clk;
    sc_in<bool>         rst;
    sc_in<uint8_t>      PC;         // Program Counter from control
    sc_out<uint32_t>    instr;      // Instruction indexed by the PC
    sc_in<bool>         wr_en;      // Enables writing of a received instruction
    sc_in<uint>         wr_addr;    // Index for writing the received instructions from processor
    sc_in<uint32_t>     wr_port;	// Port for writing instructions

    // Internal RFs
    rf_twoport<uint32_t, CRF_ENTRIES> *rf;

    sc_signal<uint> PC_casted;

    SC_CTOR(crf) {

        // Instantiate the two RFs
        rf = new rf_twoport<uint32_t, CRF_ENTRIES>("CRF");
        rf->clk(clk);
        rf->rst(rst);
        rf->rd_addr(PC_casted);
        rf->rd_port(instr);
        rf->wr_en(wr_en);
        rf->wr_addr(wr_addr);
        rf->wr_port(wr_port);

        SC_METHOD(comb_method);
        sensitive << PC;
    }

    void comb_method();	// Casts the PC to the necessary format
};

#endif
