/*
 * Copyright EPFL 2021
 * Rafael Medina Morillas
 *
 * Implementation of the Interface Unit.
 *
 */

#include "interface_unit.h"

#if VAR_DQ_CLK
void interface_unit::clk_thread() {
    // Reset all registers and pipelines
    grf_wr_cnt_reg = 0;
    grf_ser2par_reg = 0;
#if DQ_BITS == 16
    crf_wr_cnt_reg = 0;
    crf_ser2par_reg = 0;
#endif

    wait();

    // Clocked behaviour
    while (1) {
        grf_wr_cnt_reg = grf_wr_cnt_nxt;
        grf_ser2par_reg = grf_ser2par_nxt;
#if DQ_BITS == 16
        crf_wr_cnt_reg = crf_wr_cnt_nxt;
        crf_ser2par_reg = crf_ser2par_nxt;
#endif

        wait();
    }
}
#endif

void interface_unit::comb_method() {
    // Separate row address in MSB and rest
    sc_uint<ROW_BITS> row = row_addr->read();
    bool rmsb = row.range(ROW_BITS - 1, ROW_BITS - 1);
    sc_uint<ROW_BITS - 1> rlsb = row.range(ROW_BITS - 2, 0);

    // Defaults
    rf_access->write(false);
    decode_en->write(false);
    data_out->write(0);
#if VAR_DQ_CLK
    grf_wr_cnt_nxt = grf_wr_cnt_reg;
    grf_ser2par_nxt = grf_ser2par_reg;
    grf_ser2par_aux = grf_ser2par_reg;
#endif
#if DQ_BITS == 16
    crf_wr_cnt_nxt = crf_wr_cnt_reg;
    crf_ser2par_nxt = crf_ser2par_reg;
#endif
    data_out_aux = 0;

    if (pim_mode) {
        if (!rmsb) { // PIM mode, enable instruction decoding if RD/WR command and Row MSB = 0
            if (RD || WR) {
                decode_en->write(true);
            }
#if VAR_DQ_CLK
            if (grf_wr_cnt_reg) {
                grf_wr_cnt_nxt = 0;
            }
#if DQ_BITS == 16
        } else if (!grf_wr_cnt_reg && !crf_wr_cnt_reg) {
#else
        } else if (!grf_wr_cnt_reg) { // We assume that RD/WR is only set in the cycle 1
                                      // of the possible 4 cycles DQ sequence but the col+row addr are maintained
                                      // TODO check if this complies with JEDEC
#endif
            // Write to RFs
            if (rlsb <= RF_GRF_B) {
#if DQ_BITS == 16
                if (rlsb == RF_CRF && WR && !RD) {
                    crf_wr_cnt_nxt = crf_wr_cnt_reg + 1;
                    crf_ser2par_nxt = DQ;
                } else
#endif
                if (rlsb < RF_GRF_A) {	// Write to CRF or SRF
                    data_out_aux.range(DQ_BITS - 1, 0) = DQ;
                    data_out->write(data_out_aux);
                    rf_access->write(WR && !RD);
                } else if (WR && !RD) {	// Write to GRF
                    grf_wr_cnt_nxt = grf_wr_cnt_reg + 1;
                    grf_ser2par_aux.range(DQ_BITS - 1, 0) = DQ;
                    grf_ser2par_nxt = grf_ser2par_aux;
                }
            }
        } else {	// Serial to parallel conversion for writing to the GRF
#if DQ_BITS == 16
            if (crf_wr_cnt_reg) {
                crf_wr_cnt_nxt = 0;
                data_out_aux.range(DQ_BITS - 1, 0) = crf_ser2par_reg;
                data_out_aux.range(DQ_BITS * 2 - 1, DQ_BITS) = DQ;
                data_out->write(data_out_aux);
                rf_access->write(true);
            } else
#endif
            if (grf_wr_cnt_reg < DQ_CLK - 1) {
                grf_wr_cnt_nxt = grf_wr_cnt_reg + 1;
                switch (grf_wr_cnt_reg) {	// SystemC synthesis-friendly
#if DQ_CLK_GT_2
                    case 1:     grf_ser2par_aux.range(DQ_BITS * 2 - 1, DQ_BITS * 1) = DQ;   break;
                    case 2:     grf_ser2par_aux.range(DQ_BITS * 3 - 1, DQ_BITS * 2) = DQ;   break;
#if DQ_CLK_GT_4
                    case 3:     grf_ser2par_aux.range(DQ_BITS * 4 - 1, DQ_BITS * 3) = DQ;   break;
                    case 4:     grf_ser2par_aux.range(DQ_BITS * 5 - 1, DQ_BITS * 4) = DQ;   break;
                    case 5:     grf_ser2par_aux.range(DQ_BITS * 6 - 1, DQ_BITS * 5) = DQ;   break;
                    case 6:     grf_ser2par_aux.range(DQ_BITS * 7 - 1, DQ_BITS * 6) = DQ;   break;
#if DQ_CLK_GT_8
                    case 7:     grf_ser2par_aux.range(DQ_BITS * 8 - 1, DQ_BITS * 7) = DQ;   break;
                    case 8:     grf_ser2par_aux.range(DQ_BITS * 9 - 1, DQ_BITS * 8) = DQ;   break;
                    case 9:     grf_ser2par_aux.range(DQ_BITS * 10 - 1, DQ_BITS * 9) = DQ;  break;
                    case 10:    grf_ser2par_aux.range(DQ_BITS * 11 - 1, DQ_BITS * 10) = DQ; break;
                    case 11:    grf_ser2par_aux.range(DQ_BITS * 12 - 1, DQ_BITS * 11) = DQ; break;
                    case 12:    grf_ser2par_aux.range(DQ_BITS * 13 - 1, DQ_BITS * 12) = DQ; break;
                    case 13:    grf_ser2par_aux.range(DQ_BITS * 14 - 1, DQ_BITS * 13) = DQ; break;
                    case 14:    grf_ser2par_aux.range(DQ_BITS * 15 - 1, DQ_BITS * 14) = DQ; break;
#endif
#endif
#endif
                    default:    grf_wr_cnt_nxt = 0;                                         break;
                }
                grf_ser2par_nxt = grf_ser2par_aux;
            } else {
                grf_wr_cnt_nxt = 0;
                data_out_aux.range(DQ_BITS * DQ_CLK - 1, DQ_BITS * (DQ_CLK-1)) = DQ;
                data_out_aux.range(DQ_BITS * (DQ_CLK-1) - 1, 0) = grf_ser2par_reg;
                data_out->write(data_out_aux);
                rf_access->write(true);
            }
        }
#else
        } else {// Write to RFs
            if (rlsb <= RF_GRF_B) {
                data_out_aux.range(DQ_BITS - 1, 0) = DQ;
                data_out->write(data_out_aux);
                rf_access->write(WR && !RD);
            }
        }
#endif
    }
}
