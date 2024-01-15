/*
 * Copyright EPFL 2021
 * Rafael Medina Morillas
 *
 * Description of the Interface Unit.
 *
 */

#ifndef INTERFACE_UNIT_H_
#define INTERFACE_UNIT_H_

#include "systemc.h"

#include "cnm_base.h"

class interface_unit: public sc_module {
public:
    sc_in_clk clk;
    sc_in<bool> rst;

    // Interface input
    sc_in<bool>                 RD;			// DRAM read command
    sc_in<bool>                 WR;			// DRAM write command
    sc_in<bool>                 ACT;		// DRAM activate command
//    sc_in<bool>						RSTB;		//
    sc_in<bool>                 AB_mode;	// Signals if the All-Banks mode is enabled
    sc_in<bool>                 pim_mode;	// Signals if the PIM mode is enabled
    sc_in<sc_uint<BANK_BITS> >  bank_addr;	// Address of the bank
    sc_in<sc_uint<ROW_BITS> >   row_addr;	// Address of the bank row
    sc_in<sc_uint<COL_BITS> >   col_addr;	// Address of the bank column
    sc_in<sc_uint<DQ_BITS> >    DQ;	        // Data input from DRAM controller (output makes no sense here)

    // Interface output
    sc_out<bool>                    rf_access;	// Enables access to the RFs while not in PIM mode
//    sc_out<bool>					rf_wr_nrd;	// Signals if reading (low) or writing (high)
    sc_out<bool>                    decode_en;	// Enables decoding of the next CRF instruction
    sc_out<sc_biguint<GRF_WIDTH> >  data_out;	// Data to the RFs

    // Internal signals and variables
#if VAR_DQ_CLK
    sc_biguint<DQ_BITS * (DQ_CLK - 1)> grf_ser2par_aux;
#endif
    sc_biguint<GRF_WIDTH> data_out_aux;	// Variable for formatting the data output

#if VAR_DQ_CLK
    sc_signal<sc_biguint<DQ_BITS * (DQ_CLK - 1)> > grf_ser2par_nxt, grf_ser2par_reg;    // Register for serial to parallel
    sc_signal<uint> grf_wr_cnt_nxt, grf_wr_cnt_reg;	// Counter of cycles for serial to parallel
#endif
#if DQ_BITS == 16
    sc_signal<sc_uint<16> > crf_ser2par_nxt, crf_ser2par_reg;    // Register for serial to parallel
    sc_signal<uint> crf_wr_cnt_nxt, crf_wr_cnt_reg; // Counter of cycles for serial to parallel
#endif


    SC_HAS_PROCESS(interface_unit);
    interface_unit(sc_module_name name) : sc_module(name) {
#if VAR_DQ_CLK
        SC_THREAD(clk_thread);
        sensitive << clk.pos();
        async_reset_signal_is(rst, false);
#endif

        SC_METHOD(comb_method);
        sensitive << RD << WR << ACT << AB_mode << pim_mode;
        sensitive << bank_addr << row_addr << col_addr << DQ;
#if VAR_DQ_CLK
        sensitive << grf_wr_cnt_reg << grf_ser2par_reg;
#endif
#if DQ_BITS == 16
        sensitive << crf_wr_cnt_reg << crf_ser2par_reg;
#endif
    }

#if VAR_DQ_CLK
    void clk_thread();	// Performs sequential logic (and resets)
#endif
    void comb_method(); // Performs the combinational logic
};

#endif /* INTERFACE_UNIT_H_ */
