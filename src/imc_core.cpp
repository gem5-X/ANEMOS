/* 
 * Copyright EPFL 2023
 * Rafael Medina Morillas
 * 
 * Riselda Kodra
 *
 * Implementation of the In-Memory Computing Core.
 *
 */

#include "imc_core.h"
#include <bitset>


#ifdef __SYNTHESIS__

//void imc_core::clk_thread() {
//
//	int i;
//
//	// Reset all registers
//	RD_reg = false;
//	WR_reg = false;
//	ACT_reg = false;
////	RSTB_reg = false;
//	AB_mode_reg = false;
//	pim_mode_reg = false;
//	bank_addr_reg = 0;
//	row_addr_reg = 0;
//	col_addr_reg = 0;
//	DQ_reg = 0;
//	even_in_reg = 0;
//	odd_in_reg = 0;
//
//	wait();
//
//	// Update registers
//	while(1) {
//		RD_reg = RD;
//		WR_reg = WR;
//		ACT_reg = ACT;
////		RSTB_reg = RSTB;
//		AB_mode_reg = AB_mode;
//		pim_mode_reg = pim_mode;
//		bank_addr_reg = bank_addr;
//		row_addr_reg = row_addr;
//		col_addr_reg = col_addr;
//		DQ_reg = DQ;
//		even_in_reg = even_in;
//		odd_in_reg = odd_in;
//
//		wait();
//	}
//}

void imc_core::comb_method() {

	// NOTE: We make up for lack of tri-state buffers
//	even_out = even_in_reg;
//	odd_out = odd_in_reg;
	even_out = even_in;
	odd_out = odd_in;

	sc_biguint<GRF_WIDTH> data_tmp = data_out;
//	sc_biguint<GRF_WIDTH> even_tmp = even_in_reg;
//	sc_biguint<GRF_WIDTH> odd_tmp = odd_in_reg;
	sc_biguint<GRF_WIDTH> even_tmp = even_in;
	sc_biguint<GRF_WIDTH> odd_tmp = odd_in;
	sc_uint<INSTR_BITS> ext2crf_tmp;
	sc_uint<WORD_BITS> ext2srf_tmp, ext2grf_tmp[SIMD_WIDTH];

	sc_lv<WORD_BITS> even2grfa_tmp[SIMD_WIDTH], odd2grfb_tmp[SIMD_WIDTH];

	cnm_synth synth_aux;
	sc_uint<WORD_BITS> grfa_tmp[SIMD_WIDTH], grfb_tmp[SIMD_WIDTH];
	sc_lv<GRF_WIDTH> grfa2even_tmp, grfb2odd_tmp;

	// Adapt DQ to RF widths
	ext2crf_tmp = data_tmp.range(INSTR_BITS-1,0);
	ext2crf = (uint32_t) ext2crf_tmp;
	ext2srf_tmp = data_tmp.range(WORD_BITS-1,0);
	#if INT_TYPE
	        ext2srf = ext2srf_tmp.to_uint64();
        #else
                synth_aux.set_data(ext2srf_tmp.to_uint64());
                ext2srf = synth_aux;
        #endif

	for (int i=0; i<SIMD_WIDTH; i++) {
		ext2grf_tmp[i] = data_tmp.range(WORD_BITS*(i+1)-1, WORD_BITS*i);
	        #if INT_TYPE
		        ext2grf[i] = ext2grf_tmp[i].to_uint64(); 
                #else        
                        synth_aux.set_data(ext2grf_tmp[i].to_uint64());
                        ext2grf[i] = synth_aux;
                #endif        
	}

	if (even_out_en) {
		// Adapt GRFs to bank buses
		for (int i=0; i<SIMD_WIDTH; i++) {
			even2grfa[i] = grfa_out1[i];
                        #if INT_TYPE
			        grfa2even_tmp.range(WORD_BITS*(i+1)-1, WORD_BITS*i) = grfa_out1[i];
                        #else
			        synth_aux = grfa_out1[i];
                                grfa_tmp[i] = synth_aux.data();
                                grfa2even_tmp.range(WORD_BITS*(i+1)-1, WORD_BITS*i) = grfa_tmp[i];
                        #endif
		}
		even_out = grfa2even_tmp;
	} else {
		// Adapt bank buses to GRFs
		for (int i=0; i<SIMD_WIDTH; i++) {
			even2grfa_tmp[i] = even_tmp.range(WORD_BITS*(i+1)-1, WORD_BITS*i);
                        #if INT_TYPE
			        even2grfa[i] = even2grfa_tmp[i].to_uint64();
                        #else
                                synth_aux.set_data(even2grfa_tmp[i].to_uint64());
                                even2grfa[i] = synth_aux;
                        #endif
		}
	}

	if (odd_out_en) {
		// Adapt GRFs to bank buses
		for (int i=0; i<SIMD_WIDTH; i++) {
			odd2grfb[i] = grfb_out1[i];
                        #if INT_TYPE
			        grfb2odd_tmp.range(WORD_BITS*(i+1)-1, WORD_BITS*i) = grfb_out1[i];
                        #else
                                synth_aux = grfb_out1[i];
                                grfb_tmp[i] = synth_aux.data();
                                grfb2odd_tmp.range(WORD_BITS*(i+1)-1, WORD_BITS*i) = grfb_tmp[i];
                        #endif
		}
		odd_out = grfb2odd_tmp;
	} else {
		// Adapt bank buses to GRFs
		for (int i=0; i<SIMD_WIDTH; i++) {
			odd2grfb_tmp[i] = odd_tmp.range(WORD_BITS*(i+1)-1, WORD_BITS*i);
                        #if INT_TYPE
			        odd2grfb[i] = odd2grfb_tmp[i].to_uint64();
			#else
                                synth_aux.set_data(odd2grfb_tmp[i].to_uint64());
                                odd2grfb[i] = synth_aux;
                        #endif
		}
	}

#else

void imc_core::comb_method() {



    sc_biguint<GRF_WIDTH> data_tmp = data_out;
    sc_uint<INSTR_BITS> ext2crf_tmp;
    sc_uint<WORD_BITS> ext2srf_tmp, ext2grf_tmp[SIMD_WIDTH];

    sc_lv<GRF_WIDTH> even_tmp = even_bus;
    sc_lv<GRF_WIDTH> odd_tmp = odd_bus;
    sc_lv<WORD_BITS> even2grfa_tmp[SIMD_WIDTH], odd2grfb_tmp[SIMD_WIDTH];
    cnm_t cnm_aux;
    sc_int<WORD_BITS> grfa_tmp[SIMD_WIDTH], grfb_tmp[SIMD_WIDTH];  
    sc_lv<GRF_WIDTH> grfa2even_tmp, grfb2odd_tmp;

#if (!(HALF_FLOAT) && !(INT_TYPE))
    // Union variables for conversion between FP and binary
    cnm_union union_aux, union_ext2grf[SIMD_WIDTH];
    cnm_union union_even2grfa[SIMD_WIDTH], union_odd2grfb[SIMD_WIDTH];
#endif

    // Adapt DQ to RF widths
    ext2crf_tmp = data_tmp.range(INSTR_BITS - 1, 0);
    ext2crf = ext2crf_tmp.to_uint64();

    ext2srf_tmp = data_tmp.range(WORD_BITS - 1, 0);

#if HALF_FLOAT
    ext2srf = half_float::half(half_float::detail::binary,
            ext2srf_tmp.to_uint());
#else
        #if INT_TYPE
                ext2srf = (cnm_t) ext2srf_tmp.to_uint64();
        #else
                union_aux.bin = ext2srf_tmp.to_uint64();
                ext2srf = union_aux.data;
        #endif
#endif

    for (int i = 0; i < SIMD_WIDTH; i++) {
        ext2grf_tmp[i] = data_tmp.range(WORD_BITS * (i + 1) - 1, WORD_BITS * i);
#if HALF_FLOAT
        ext2grf[i] = half_float::half(half_float::detail::binary,
                ext2grf_tmp[i].to_uint());
#else
        #if INT_TYPE
                ext2grf[i] = ext2grf_tmp[i].to_uint64();
        #else
                union_ext2grf[i].bin = ext2grf_tmp[i].to_uint64();
                ext2grf[i] = union_ext2grf[i].data;
        #endif
#endif

    }

    // Adapt bank buses to GRFs
    for (int i = 0; i < SIMD_WIDTH; i++) {
        even2grfa_tmp[i] = even_tmp.range(WORD_BITS * (i + 1) - 1, WORD_BITS * i);
        odd2grfb_tmp[i] = odd_tmp.range(WORD_BITS * (i + 1) - 1, WORD_BITS * i);
#if HALF_FLOAT
        even2grfa[i] = half_float::half(half_float::detail::binary,
                even2grfa_tmp[i].to_uint());
        odd2grfb[i] = half_float::half(half_float::detail::binary,
                odd2grfb_tmp[i].to_uint());
#else
        #if INT_TYPE
                even2grfa[i] = even2grfa_tmp[i].to_uint64();
                odd2grfb[i] = odd2grfb_tmp[i].to_uint64();
        #else
                union_even2grfa[i].bin = even2grfa_tmp[i].to_uint64();
                even2grfa[i] = union_even2grfa[i].data;
                union_odd2grfb[i].bin = odd2grfb_tmp[i].to_uint64();
                odd2grfb[i] = union_odd2grfb[i].data;
        #endif
#endif
    }

    // Adapt GRFs to bank buses
    for (int i = 0; i < SIMD_WIDTH; i++) {
#if HALF_FLOAT
        cnm_aux = grfa_out1[i];
        grfa_tmp[i] = cnm_aux.bin_word();
#else
        #if INT_TYPE
                grfa_tmp[i] = grfa_out1[i];
        #else
                union_aux.data = (grfa_out1[i]);
                grfa_tmp[i] = union_aux.bin;
        #endif
#endif
        grfa2even_tmp.range(WORD_BITS * (i + 1) - 1, WORD_BITS * i) =
                grfa_tmp[i];
#if HALF_FLOAT
        cnm_aux = grfb_out1[i];
        grfb_tmp[i] = cnm_aux.bin_word();
#else
        #if INT_TYPE
                grfb_tmp[i] = grfb_out1[i];
        #else
                union_aux.data = (grfb_out1[i]);
                grfb_tmp[i] = union_aux.bin;
        #endif
#endif
        grfb2odd_tmp.range(WORD_BITS * (i + 1) - 1, WORD_BITS * i) =
                grfb_tmp[i];
    }
    grfa2even = grfa2even_tmp;
    grfb2odd = grfb2odd_tmp;

#endif

}

