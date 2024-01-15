/* 
 * Copyright EPFL 2023
 * Rafael Medina Morillas
 * 
 * Riselda Kodra
 *
 * Description of the Scalar Register File (SRF).
 *
 */

#ifndef SRF_H
#define SRF_H

#include "systemc.h"

#include "cnm_base.h"
#include "rf_twoport.h"

class srf: public sc_module {
public:

#ifdef __SYNTHESIS__

	sc_in_clk       		clk;
	sc_in<bool>     		rst;
	sc_in<uint>				rd_addr;		// Index read
	sc_in<bool>				rd_a_nm;        // Signals if reading from SRF_A (high) or SRF_M (low)
	sc_out<cnm_synth>	rd_port;		// Read port
	sc_in<bool>				wr_en;			// Enable writing
	sc_in<uint>				wr_addr;		// Index the address to be written
	sc_in<bool>				wr_a_nm;		// Signals if writing to SRF_A (high) or SRF_M (low)
	sc_in<uint8_t>			wr_from;		// Index the MUX for input data
	sc_in<cnm_synth>	ext_in;			// Data input from external DQ
	sc_in<cnm_synth>  srf_in;         // Data input from SRF
	sc_in<cnm_synth>  grfa_in;        // Data input from GRF_A
	sc_in<cnm_synth>  grfb_in;        // Data input from GRF_B
	// sc_in<cnm_synth>     even_in;        // Internal data input from EVEN_BANK
	// sc_in<cnm_synth>     odd_in;         // Internal data input from ODD_BANK

	// Internal RFs
	rf_twoport<cnm_synth,SRF_M_ENTRIES> *srf_m;
	rf_twoport<cnm_synth,SRF_A_ENTRIES> *srf_a;

	// Internal signals
	sc_signal<cnm_synth>	rd_port_m, rd_port_a, wr_mux_out;
	sc_signal<bool>	wr_en_m, wr_en_a;

	SC_CTOR(srf) {

		// Instantiate the two RFs
		srf_m = new rf_twoport<cnm_synth,SRF_M_ENTRIES>("SRF_M");
		srf_m->clk(clk);
		srf_m->rst(rst);
		srf_m->rd_addr(rd_addr);
		srf_m->rd_port(rd_port_m);
		srf_m->wr_en(wr_en_m);
		srf_m->wr_addr(wr_addr);
		srf_m->wr_port(wr_mux_out);

		srf_a = new rf_twoport<cnm_synth,SRF_A_ENTRIES>("SRF_A");
		srf_a->clk(clk);
		srf_a->rst(rst);
		srf_a->rd_addr(rd_addr);
		srf_a->rd_port(rd_port_a);
		srf_a->wr_en(wr_en_a);
		srf_a->wr_addr(wr_addr);
		srf_a->wr_port(wr_mux_out);

#else

    sc_in_clk clk;
    sc_in<bool>     rst;
    sc_in<uint>     rd_addr;	// Index read
    sc_in<bool>     rd_a_nm;    // Signals if reading from SRF_A (high) or SRF_M (low)
    sc_out<cnm_t>   rd_port;	// Read port
    sc_in<bool>     wr_en;		// Enable writing
    sc_in<uint>     wr_addr;	// Index the address to be written
    sc_in<bool>     wr_a_nm;    // Signals if writing to SRF_A (high) or SRF_M (low)
    sc_in<uint8_t>  wr_from;	// Index the MUX for input data
    sc_in<cnm_t>    ext_in;		// Data input from external DQ
    sc_in<cnm_t>    srf_in;     // Data input from SRF
    sc_in<cnm_t>    grfa_in;    // Data input from GRF_A
    sc_in<cnm_t>    grfb_in;    // Data input from GRF_B
    // sc_in<cnm_t>    even_in; // Internal data input from EVEN_BANK
    // sc_in<cnm_t>    odd_in;  // Internal data input from ODD_BANK

    // Internal RFs
    rf_twoport<cnm_t, SRF_M_ENTRIES> *srf_m;
    rf_twoport<cnm_t, SRF_A_ENTRIES> *srf_a;

    // Internal signals
    sc_signal<cnm_t> rd_port_m, rd_port_a, wr_mux_out;
    sc_signal<bool> wr_en_m, wr_en_a;

    SC_CTOR(srf) {

        // Instantiate the two RFs
        srf_m = new rf_twoport<cnm_t, SRF_M_ENTRIES>("SRF_M");
        srf_m->clk(clk);
        srf_m->rst(rst);
        srf_m->rd_addr(rd_addr);
        srf_m->rd_port(rd_port_m);
        srf_m->wr_en(wr_en_m);
        srf_m->wr_addr(wr_addr);
        srf_m->wr_port(wr_mux_out);

        srf_a = new rf_twoport<cnm_t, SRF_A_ENTRIES>("SRF_A");
        srf_a->clk(clk);
        srf_a->rst(rst);
        srf_a->rd_addr(rd_addr);
        srf_a->rd_port(rd_port_a);
        srf_a->wr_en(wr_en_a);
        srf_a->wr_addr(wr_addr);
        srf_a->wr_port(wr_mux_out);

#endif

        SC_METHOD(comb_method);
        sensitive << rd_a_nm << rd_port_m << rd_port_a;
        sensitive << wr_en << wr_a_nm << wr_from;
        sensitive << ext_in << srf_in << grfa_in << grfb_in;// << even_in << odd_in;
    }

    void comb_method();	// Performs the necessary multiplexing
};

#endif
