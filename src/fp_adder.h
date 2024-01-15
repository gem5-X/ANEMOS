/* 
 * Copyright EPFL 2023
 * Rafael Medina Morillas
 * 
 * Riselda Kodra
 *
 * Description of the Adder.
 *
 */

#ifndef FP_ADDER_H
#define FP_ADDER_H

#include "systemc.h"

#include "cnm_base.h"

#ifdef __SYNTHESIS__
#include <ac_std_float.h>
#include <ac_sc.h>
#else
#include "half.hpp"
#endif

#ifdef __SYNTHESIS__

class fp_adder : public sc_module {
public:
    sc_in_clk       			clk;
    sc_in<bool>     			rst;
    sc_in<bool>					compute_en; // Signals that a computation step should be performed
    sc_in<cnm_synth>    	op1;        // First operand
    sc_in<cnm_synth>    	op2;        // Second operand
    sc_out<cnm_synth>		output;     // Output of the addition

    sc_signal<cnm_synth>	add_res;
    sc_signal<cnm_synth>	pipeline[ADD_STAGES];   // Pipelined addition results

    SC_CTOR(fp_adder) {
        SC_THREAD(clk_thread);
        sensitive << clk.pos();
        async_reset_signal_is(rst, false);

        SC_METHOD(comb_method);
        sensitive << op1 << op2 << pipeline[ADD_STAGES-1];

        for (int i=0; i<ADD_STAGES; i++) {
            pipeline[i] = cnm_synth(0);
        }
    }

    void clk_thread();  	// Performs the FP addition and advances the pipeline
    void comb_method();   // Connects the last pipeline register with the output
};

#else

using half_float::half;

class fp_adder: public sc_module {
public:
    sc_in_clk       clk;
    sc_in<bool>     rst;
    sc_in<bool>     compute_en; // Signals that a computation step should be performed
    sc_in<cnm_t> 	op1;        // First operand
    sc_in<cnm_t> 	op2;        // Second operand
    sc_out<cnm_t>   output;     // Output of the addition

    sc_signal<cnm_t>    add_res;
    sc_signal<cnm_t>    pipeline[ADD_STAGES];   // Pipelined addition results

    SC_CTOR(fp_adder) {
        SC_THREAD(clk_thread);
        sensitive << clk.pos();
        async_reset_signal_is(rst, false);

        SC_METHOD(comb_method);
        sensitive << op1 << op2 << pipeline[ADD_STAGES - 1];

        for (int i = 0; i < ADD_STAGES; i++)
#if HALF_FLOAT
            pipeline[i] = half_float::half_cast<half>(0.0);
#else
            pipeline[i] = cnm_t(0);
#endif
    }

    void clk_thread();  // Performs the FP addition and advances the pipeline
    void comb_method();   // Connects the last pipeline register with the output
};

#endif

#endif
