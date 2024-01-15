/* 
 * Copyright EPFL 2023
 * Rafael Medina Morillas
 * 
 * Riselda Kodra
 *
 * Description of the General Register File (GRF).
 *
 */

#ifndef GRF_H
#define GRF_H

#include "systemc.h"
#include "rf_threeport.h"
#include <string>
#include "cnm_base.h"

class grf: public sc_module {
public:

#ifdef __SYNTHESIS__

    sc_in_clk       		clk;
    sc_in<bool>     		rst;
    sc_in<uint>				rd_addr1;				// Index read at port 1
    sc_in<uint>				rd_addr2;				// Index read at port 2
    sc_out<cnm_synth>	rd_port1[SIMD_WIDTH];	// Read port 1
    sc_out<cnm_synth>	rd_port2[SIMD_WIDTH];	// Read port 2
    sc_in<bool>				wr_en;					// Enables writing
    sc_in<bool>				relu_en;				// Enable ReLU for the MOV instruction
    sc_in<uint>				wr_addr;				// Index the address to be written
    sc_in<uint8_t>			wr_from;				// Index the MUX for input data
    sc_in<cnm_synth>	ext_in[SIMD_WIDTH];		// Data input from external DQ
    sc_in<cnm_synth>  fpu_in[SIMD_WIDTH];		// Data input from FPU
    sc_in<cnm_synth>  srf_in[SIMD_WIDTH];     // Data input from SRF
	sc_in<cnm_synth>  grfa_in[SIMD_WIDTH];    // Data input from GRF_A
	sc_in<cnm_synth>  grfb_in[SIMD_WIDTH];    // Data input from GRF_B
	sc_in<cnm_synth>  bank_in[SIMD_WIDTH];    // Data input from corresponding bank

	// Internal RFs
//	sc_vector<grf_word> rf;

	// Internal signals
	sc_signal<cnm_synth> wr_mux_out[SIMD_WIDTH];

	SC_CTOR(grf) {
		uint i;

		// Internal RFs
		rf_threeport<cnm_synth,GRF_ENTRIES> *grf_channel[SIMD_WIDTH];

		for (i=0; i<SIMD_WIDTH; i++) {
			grf_channel[i] = new rf_threeport<cnm_synth,GRF_ENTRIES>(sc_gen_unique_name("grf_channel"));
		}

#else

    sc_in_clk       clk;
    sc_in<bool>     rst;
    sc_in<uint>     rd_addr1;				// Index read at port 1
    sc_in<uint>     rd_addr2;				// Index read at port 2
    sc_out<cnm_t>   rd_port1[SIMD_WIDTH];	// Read port 1
    sc_out<cnm_t>   rd_port2[SIMD_WIDTH];	// Read port 2
    sc_in<bool>     wr_en;					// Enables writing
    sc_in<bool>     relu_en;				// Enable ReLU for the MOV instruction
    sc_in<uint>     wr_addr;				// Index the address to be written
    sc_in<uint8_t>  wr_from;				// Index the MUX for input data
    sc_in<cnm_t>    ext_in[SIMD_WIDTH];		// Data input from external DQ
    sc_in<cnm_t>    fpu_in[SIMD_WIDTH];		// Data input from FPU
    sc_in<cnm_t>    srf_in[SIMD_WIDTH];     // Data input from SRF
    sc_in<cnm_t>    grfa_in[SIMD_WIDTH];    // Data input from GRF_A
    sc_in<cnm_t>    grfb_in[SIMD_WIDTH];    // Data input from GRF_B
    sc_in<cnm_t>    bank_in[SIMD_WIDTH];    // Data input from corresponding bank

    // Internal RFs
//	sc_vector<grf_word> rf;

    // Internal signals
    sc_signal<cnm_t> wr_mux_out[SIMD_WIDTH];

    SC_CTOR(grf) {
        uint i;

        // Internal RFs
        rf_threeport<cnm_t, GRF_ENTRIES> **grf_channel;
        grf_channel = new rf_threeport<cnm_t, GRF_ENTRIES>*[SIMD_WIDTH];

        for (i = 0; i < SIMD_WIDTH; i++) {
            grf_channel[i] = new rf_threeport<cnm_t, GRF_ENTRIES>(sc_gen_unique_name("grf_channel"));
        }

#endif

        for (i = 0; i < SIMD_WIDTH; i++) {
            grf_channel[i]->clk(clk);
            grf_channel[i]->rst(rst);
            grf_channel[i]->rd_addr1(rd_addr1);
            grf_channel[i]->rd_addr2(rd_addr2);
            grf_channel[i]->rd_port1(rd_port1[i]);
            grf_channel[i]->rd_port2(rd_port2[i]);
            grf_channel[i]->wr_en(wr_en);
            grf_channel[i]->wr_addr(wr_addr);
            grf_channel[i]->wr_port(wr_mux_out[i]);
        }

        SC_METHOD(comb_method);
        sensitive << wr_from << relu_en;
        for (i = 0; i < SIMD_WIDTH; i++) {
            sensitive << ext_in[i] << fpu_in[i] << srf_in[i] << grfa_in[i] << grfb_in[i] << bank_in[i];
        }

    }

    void comb_method();	// Performs the necessary multiplexing and ReLU
};

#endif
