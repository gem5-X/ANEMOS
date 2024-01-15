/*
 * Copyright EPFL 2021
 * Rafael Medina Morillas
 *
 * Description of the Program Counter Unit
 *
 */

#ifndef PC_UNIT_H_
#define PC_UNIT_H_

#include "systemc.h"

#include "cnm_base.h"

class pc_unit: public sc_module {
public:
    sc_in_clk       clk;
    sc_in<bool>     rst;
    sc_in<bool>     pc_rst;	    // Synchronous reset when end of CRF or EXIT instruction
    sc_in<bool>     count_en;   // Enables the increase of PC
    sc_in<bool>     jump_en;    // Signals a jump which will translate into a decrease of PC
    sc_in<uint8_t>  jump_num;   // Amount to subtract from the PC when doing a jump
    sc_out<uint8_t> pc_out;     // Value of the Program Counter

    sc_signal<uint8_t> pc_nxt, pc_reg;

    SC_CTOR(pc_unit) {
        SC_THREAD(clk_thread);
        sensitive << clk.pos();
        async_reset_signal_is(rst, false);

        SC_METHOD(comb_method);
        sensitive << pc_rst << count_en << jump_en << jump_num << pc_reg;

        pc_nxt = 0;
        pc_reg = 0;
    }

    void clk_thread();	// Performs sequential logic (and resets)
    void comb_method(); // Performs the combinational logic
};

#endif /* PC_UNIT_H_ */
