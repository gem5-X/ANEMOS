/* 
 * Copyright EPFL 2023
 * Rafael Medina Morillas
 * 
 * Riselda Kodra
 *
 * Description of the SIMD Arithmetic Unit.
 *
 */

#ifndef FPU_H
#define FPU_H

#include "systemc.h"

#include "cnm_base.h"
#include "fp_multiplier.h"
#include "fp_adder.h"

class fpu: public sc_module {
public:

#ifdef __SYNTHESIS__

    sc_in_clk           		clk;
    sc_in<bool>         		rst;
    sc_in<bool>         		mult_en;             	// Signals that a multiplication computation step should be performed
    sc_in<bool>         		add_en;             	// Signals that an addition computation step should be performed
    sc_in<cnm_synth>     	srf_in;                	// Scalar operand from SRF
    sc_in<cnm_synth>     	grfa_in1[SIMD_WIDTH];   // Input 1 from GRF_A
    sc_in<cnm_synth>     	grfa_in2[SIMD_WIDTH];   // Input 2 from GRF_A
    sc_in<cnm_synth>     	grfb_in1[SIMD_WIDTH];   // Input 1 from GRF_B
    sc_in<cnm_synth>		grfb_in2[SIMD_WIDTH];   // Input 2 from GRF_B
    sc_in<cnm_synth>    	even_in[SIMD_WIDTH];    // Input from EVEN_BANK
    sc_in<cnm_synth>     	odd_in[SIMD_WIDTH];     // Input from ODD_BANK
    sc_in<uint8_t>    			mult_in1_sel;           // Selects input 1 for multiplication
    sc_in<uint8_t>    			mult_in2_sel;           // Selects input 2 for multiplication
    sc_in<uint8_t>    			add_in1_sel;            // Selects input 1 for addition
    sc_in<uint8_t>				add_in2_sel;            // Selects input 2 for addition
    sc_in<bool>					out_sel;				// Selects the output: 0 for adder output, 1 for multiplier output
    sc_out<cnm_synth>		output[SIMD_WIDTH];     // Output of the Floating Point Unit

    // Internal signals
    sc_signal<cnm_synth>	mult_in1[SIMD_WIDTH];   // Input 1 for multiplication
    sc_signal<cnm_synth>  mult_in2[SIMD_WIDTH];   // Input 2 for multiplication
    sc_signal<cnm_synth> 	mult_out[SIMD_WIDTH];   // Multiplication output
    sc_signal<cnm_synth> 	add_in1[SIMD_WIDTH];    // Input 1 for addition
    sc_signal<cnm_synth> 	add_in2[SIMD_WIDTH];    // Input 2 for addition
    sc_signal<cnm_synth> 	add_out[SIMD_WIDTH];    // Multiplication output
    
    SC_CTOR(fpu) {

    	uint i;

    	fp_multiplier *multipliers[SIMD_WIDTH];
    	fp_adder *adders[SIMD_WIDTH];

    	for (i=0; i<SIMD_WIDTH; i++) {
			multipliers[i] = new fp_multiplier(sc_gen_unique_name("multiplier"));
    		multipliers[i]->clk(clk);
			multipliers[i]->rst(rst);
			multipliers[i]->compute_en(mult_en);
			multipliers[i]->op1(mult_in1[i]);
			multipliers[i]->op2(mult_in2[i]);
			multipliers[i]->output(mult_out[i]);

			adders[i] = new fp_adder(sc_gen_unique_name("adder"));
			adders[i]->clk(clk);
			adders[i]->rst(rst);
			adders[i]->compute_en(add_en);
			adders[i]->op1(add_in1[i]);
			adders[i]->op2(add_in2[i]);
			adders[i]->output(add_out[i]);
		}

#else

    sc_in_clk       clk;
    sc_in<bool>     rst;
    sc_in<bool>     mult_en;                // Signals that a multiplication computation step should be performed
    sc_in<bool>     add_en;                 // Signals that an addition computation step should be performed
    sc_in<cnm_t>    srf_in;                	// Scalar operand from SRF
    sc_in<cnm_t>    grfa_in1[SIMD_WIDTH];   // Input 1 from GRF_A
    sc_in<cnm_t>    grfa_in2[SIMD_WIDTH];   // Input 2 from GRF_A
    sc_in<cnm_t>    grfb_in1[SIMD_WIDTH];   // Input 1 from GRF_B
    sc_in<cnm_t>    grfb_in2[SIMD_WIDTH];   // Input 2 from GRF_B
    sc_in<cnm_t>    even_in[SIMD_WIDTH];    // Input from EVEN_BANK
    sc_in<cnm_t>    odd_in[SIMD_WIDTH];     // Input from ODD_BANK
    sc_in<uint8_t>  mult_in1_sel;           // Selects input 1 for multiplication
    sc_in<uint8_t>  mult_in2_sel;           // Selects input 2 for multiplication
    sc_in<uint8_t>  add_in1_sel;            // Selects input 1 for addition
    sc_in<uint8_t>  add_in2_sel;            // Selects input 2 for addition
    sc_in<bool>     out_sel;                // Selects the output: 0 for adder output, 1 for multiplier output
    sc_out<cnm_t>   output[SIMD_WIDTH];     // Output of the Floating Point Unit

    // Internal signals
    sc_signal<cnm_t>    mult_in1[SIMD_WIDTH];   // Input 1 for multiplication
    sc_signal<cnm_t>    mult_in2[SIMD_WIDTH];   // Input 2 for multiplication
    sc_signal<cnm_t>    mult_out[SIMD_WIDTH];   // Multiplication output
    sc_signal<cnm_t>    add_in1[SIMD_WIDTH];    // Input 1 for addition
    sc_signal<cnm_t>    add_in2[SIMD_WIDTH];    // Input 2 for addition
    sc_signal<cnm_t>    add_out[SIMD_WIDTH];    // Multiplication output

    // Submodules
    sc_vector<fp_multiplier>    multipliers;    // Vector of FP Multipliers
    sc_vector<fp_adder>         adders;         // Vector of FP Adders

    SC_HAS_PROCESS(fpu);
    fpu(sc_module_name name_) : sc_module(name_), multipliers("multiplier", SIMD_WIDTH), adders("adder", SIMD_WIDTH) {

        int i;

        for (i = 0; i < SIMD_WIDTH; i++) {
            multipliers[i].clk(clk);
            multipliers[i].rst(rst);
            multipliers[i].compute_en(mult_en);
            multipliers[i].op1(mult_in1[i]);
            multipliers[i].op2(mult_in2[i]);
            multipliers[i].output(mult_out[i]);

            adders[i].clk(clk);
            adders[i].rst(rst);
            adders[i].compute_en(add_en);
            adders[i].op1(add_in1[i]);
            adders[i].op2(add_in2[i]);
            adders[i].output(add_out[i]);
        }

#endif

        SC_METHOD(multiplex_method);
        sensitive << mult_in1_sel << mult_in2_sel << add_in1_sel << add_in2_sel;
        sensitive << srf_in;
        for (i = 0; i < SIMD_WIDTH; i++) {
            sensitive << grfa_in1[i] << grfa_in2[i] << grfb_in1[i] << grfb_in2[i] << mult_out[i];
            sensitive << even_in[i] << odd_in[i];
        }

        SC_METHOD(update_output);
        sensitive << out_sel;
        for (i = 0; i < SIMD_WIDTH; i++) {
            sensitive << mult_out[i] << add_out[i];
        }
    }

    // ~fpu() {
    //     delete multipliers;
    //     delete adders;
    // }

    void multiplex_method(); // Handles multiplexing of the inputs of the adders and multipliers
    void update_output();       // Handles connection from add_out to output
};

#endif
