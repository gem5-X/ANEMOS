/* 
 * Copyright EPFL 2023
 * Rafael Medina Morillas
 * 
 * Riselda Kodra
 *
 * Implementation of the Scalar Register File (SRF).
 *
 */

#include "srf.h"

void srf::comb_method() {
    // Choose the read RF
    if (rd_a_nm) {
        rd_port->write(rd_port_a);
    } else {
        rd_port->write(rd_port_m);
    }

    // Choose the RF to be written to
    wr_en_m = wr_en && !wr_a_nm;
    wr_en_a = wr_en && wr_a_nm;

    // Select the write input
    switch (wr_from->read()) {
    case MUX_EXT:
        wr_mux_out = ext_in->read();
        break;
    case MUX_SRF:
        wr_mux_out = srf_in->read();
        break;
    case MUX_GRF_A:
        wr_mux_out = grfa_in->read();
        break;
    case MUX_GRF_B:
        wr_mux_out = grfb_in->read();
        break;
//		case MUX_EVEN_BANK:	wr_mux_out = even_in->read();  break;
//		case MUX_ODD_BANK:  wr_mux_out = odd_in->read();   break;
#ifdef __SYNTHESIS__
		default:        	wr_mux_out = cnm_synth(0);	break;
#else
    default:
#if HALF_FLOAT
        wr_mux_out = half_float::half_cast<half>(0.0);
#else
        wr_mux_out = cnm_t(0);
#endif
        break;
#endif
    }
}
