/* 
 * Copyright EPFL 2023
 * Rafael Medina Morillas
 * 
 * Riselda Kodra
 *
* Implementation of the General Register File (GRF).
 *
 */

#include "grf.h"

void grf::comb_method() {
    int i;

#ifdef __SYNTHESIS__

    cnm_synth intN_zero(0);

    switch (wr_from->read()) {
		case MUX_EXT:
			for (i=0; i<SIMD_WIDTH; i++)
				wr_mux_out[i] = (relu_en->read() && ext_in[i]->read() < intN_zero) ? intN_zero : ext_in[i]->read();
		break;
		case MUX_SRF:
			for (i=0; i<SIMD_WIDTH; i++)
				wr_mux_out[i] = (relu_en->read() && srf_in[i]->read() < intN_zero) ? intN_zero : srf_in[i]->read();
		break;
		case MUX_GRF_A:
			for (i=0; i<SIMD_WIDTH; i++)
				wr_mux_out[i] = (relu_en->read() && grfa_in[i]->read() < intN_zero) ? intN_zero : grfa_in[i]->read();
		break;
		case MUX_GRF_B:
			for (i=0; i<SIMD_WIDTH; i++)
				wr_mux_out[i] = (relu_en->read() && grfb_in[i]->read() < intN_zero) ? intN_zero : grfb_in[i]->read();
		break;
		// TODO for now, we'll say if any of both happen, it will read from bank input and trust controller to do it well
		case MUX_EVEN_BANK:
			for (i=0; i<SIMD_WIDTH; i++)
				wr_mux_out[i] = (relu_en->read() && bank_in[i]->read() < intN_zero) ? intN_zero : bank_in[i]->read();
		break;
		case MUX_ODD_BANK:
			for (i=0; i<SIMD_WIDTH; i++)
				wr_mux_out[i] = (relu_en->read() && bank_in[i]->read() < intN_zero) ? intN_zero : bank_in[i]->read();
		break;
		case MUX_FPU:
			for (i=0; i<SIMD_WIDTH; i++)
				wr_mux_out[i] = (relu_en->read() && fpu_in[i]->read() < intN_zero) ? intN_zero : fpu_in[i]->read();
		break;
		default:
			for (i=0; i<SIMD_WIDTH; i++)
				wr_mux_out[i] = intN_zero;
		break;
	}

#else

    switch (wr_from->read()) {
    case MUX_EXT:
        for (i = 0; i < SIMD_WIDTH; i++)
            wr_mux_out[i] = (relu_en->read() && ext_in[i]->read() < 0) ?
#if HALF_FLOAT
                            half_float::half_cast<half>(0.0) :
#else
                            cnm_t(0) :
#endif
                            ext_in[i]->read();
        break;
    case MUX_SRF:
        for (i = 0; i < SIMD_WIDTH; i++)
            wr_mux_out[i] = (relu_en->read() && srf_in[i]->read() < 0) ?
#if HALF_FLOAT
                            half_float::half_cast<half>(0.0) :
#else
                            cnm_t(0) :
#endif
                            srf_in[i]->read();
        break;
    case MUX_GRF_A:
        for (i = 0; i < SIMD_WIDTH; i++)
            wr_mux_out[i] = (relu_en->read() && grfa_in[i]->read() < 0) ?
#if HALF_FLOAT
                            half_float::half_cast<half>(0.0) :
#else
                            cnm_t(0) :
#endif
                            grfa_in[i]->read();
        break;
    case MUX_GRF_B:
        for (i = 0; i < SIMD_WIDTH; i++)
            wr_mux_out[i] = (relu_en->read() && grfb_in[i]->read() < 0) ?
#if HALF_FLOAT
                            half_float::half_cast<half>(0.0) :
#else
                            cnm_t(0) :
#endif
                            grfb_in[i]->read();
        break;
        // TODO for now, we'll say if any of both happen, it will read from bank input and trust controller to do it well
    case MUX_EVEN_BANK:
        for (i = 0; i < SIMD_WIDTH; i++)
            wr_mux_out[i] = (relu_en->read() && bank_in[i]->read() < 0) ?
#if HALF_FLOAT
                            half_float::half_cast<half>(0.0) :
#else
                            cnm_t(0) :
#endif
                            bank_in[i]->read();
        break;
    case MUX_ODD_BANK:
        for (i = 0; i < SIMD_WIDTH; i++)
            wr_mux_out[i] = (relu_en->read() && bank_in[i]->read() < 0) ?
#if HALF_FLOAT
                            half_float::half_cast<half>(0.0) :
#else
                            cnm_t(0) :
#endif
                            bank_in[i]->read();
        break;
    case MUX_FPU:
        for (i = 0; i < SIMD_WIDTH; i++)
            wr_mux_out[i] = (relu_en->read() && fpu_in[i]->read() < 0) ?
#if HALF_FLOAT
                            half_float::half_cast<half>(0.0) :
#else
                            cnm_t(0) :
#endif
                            fpu_in[i]->read();
        break;
    default:
        for (i = 0; i < SIMD_WIDTH; i++)
#if HALF_FLOAT
            wr_mux_out[i] = half_float::half_cast<half>(0.0);
#else
            wr_mux_out[i] = cnm_t(0);
#endif
        break;
    }

#endif

}

