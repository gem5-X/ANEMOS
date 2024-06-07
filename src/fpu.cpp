/* 
 * Copyright EPFL 2021
 * Rafael Medina Morillas
 * 
 * Implementation of the SIMD Floating Point Unit.
 *
 */

#include "fpu.h"

void fpu::multiplex_method() {
    int i;

    // Multiplication Input 1
    switch (mult_in1_sel->read()) {
        case M1_GRF_A1:
            for (i = 0; i < SIMD_WIDTH; i++) {
                mult_in1[i] = grfa_in1[i];
            }
        break;
        case M1_GRF_A2:
            for (i = 0; i < SIMD_WIDTH; i++) {
                mult_in1[i] = grfa_in2[i];
            }
        break;
        case M1_GRF_B1:
            for (i = 0; i < SIMD_WIDTH; i++) {
                mult_in1[i] = grfb_in1[i];
            }
        break;
        case M1_GRF_B2:
            for (i = 0; i < SIMD_WIDTH; i++) {
                mult_in1[i] = grfb_in2[i];
            }
        break;
        case M1_EVEN_BANK:
            for (i = 0; i < SIMD_WIDTH; i++) {
                mult_in1[i] = even_in[i];
            }
        break;
        case M1_ODD_BANK:
            for (i = 0; i < SIMD_WIDTH; i++) {
                mult_in1[i] = odd_in[i];
            }
        break;
        default:
            for (i = 0; i < SIMD_WIDTH; i++) {
                mult_in1[i] = grfa_in1[i];
            }
        break;
    }

    // Multiplication Input 2
    switch (mult_in2_sel->read()) {
        case M2_SRF:
            for (i = 0; i < SIMD_WIDTH; i++) {
                mult_in2[i] = srf_in;
            }
        break;
        case M2_GRF_A1:
            for (i = 0; i < SIMD_WIDTH; i++) {
                mult_in2[i] = grfa_in1[i];
            }
        break;
        case M2_GRF_A2:
            for (i = 0; i < SIMD_WIDTH; i++) {
                mult_in2[i] = grfa_in2[i];
            }
        break;
        case M2_GRF_B1:
            for (i = 0; i < SIMD_WIDTH; i++) {
                mult_in2[i] = grfb_in1[i];
            }
        break;
        case M2_GRF_B2:
            for (i = 0; i < SIMD_WIDTH; i++) {
                mult_in2[i] = grfb_in2[i];
            }
        break;
        case M2_EVEN_BANK:
            for (i = 0; i < SIMD_WIDTH; i++) {
                mult_in2[i] = even_in[i];
            }
        break;
        case M2_ODD_BANK:
            for (i = 0; i < SIMD_WIDTH; i++) {
                mult_in2[i] = odd_in[i];
            }
        break;
        default:
            for (i = 0; i < SIMD_WIDTH; i++) {
                mult_in2[i] = srf_in;
            }
        break;
    }

    // Addition Input 1
    switch (add_in1_sel->read()) {
        case A_MULT_OUT:
            for (i = 0; i < SIMD_WIDTH; i++) {
                add_in1[i] = mult_out[i];
            }
        break;
        case A_SRF:
            for (i = 0; i < SIMD_WIDTH; i++) {
                add_in1[i] = srf_in;
            }
        break;
        case A_GRF_A1:
            for (i = 0; i < SIMD_WIDTH; i++) {
                add_in1[i] = grfa_in1[i];
            }
        break;
        case A_GRF_A2:
            for (i = 0; i < SIMD_WIDTH; i++) {
                add_in1[i] = grfa_in2[i];
            }
        break;
        case A_GRF_B1:
            for (i = 0; i < SIMD_WIDTH; i++) {
                add_in1[i] = grfb_in1[i];
            }
        break;
        case A_GRF_B2:
            for (i = 0; i < SIMD_WIDTH; i++) {
                add_in1[i] = grfb_in2[i];
            }
        break;
        case A_EVEN_BANK:
            for (i = 0; i < SIMD_WIDTH; i++) {
                add_in1[i] = even_in[i];
            }
        break;
        case A_ODD_BANK:
            for (i = 0; i < SIMD_WIDTH; i++) {
                add_in1[i] = odd_in[i];
            }
        break;
        default:
            for (i = 0; i < SIMD_WIDTH; i++) {
                add_in1[i] = mult_out[i];
            }
        break;
    }

    // Addition Input 2
    switch (add_in2_sel->read()) {
        case A_MULT_OUT:
            for (i = 0; i < SIMD_WIDTH; i++) {
                add_in2[i] = mult_out[i];
            }
        break;
        case A_SRF:
            for (i = 0; i < SIMD_WIDTH; i++) {
                add_in2[i] = srf_in;
            }
        break;
        case A_GRF_A1:
            for (i = 0; i < SIMD_WIDTH; i++) {
                add_in2[i] = grfa_in1[i];
            }
        break;
        case A_GRF_A2:
            for (i = 0; i < SIMD_WIDTH; i++) {
                add_in2[i] = grfa_in2[i];
            }
        break;
        case A_GRF_B1:
            for (i = 0; i < SIMD_WIDTH; i++) {
                add_in2[i] = grfb_in1[i];
            }
        break;
        case A_GRF_B2:
            for (i = 0; i < SIMD_WIDTH; i++) {
                add_in2[i] = grfb_in2[i];
            }
        break;
        case A_EVEN_BANK:
            for (i = 0; i < SIMD_WIDTH; i++) {
                add_in2[i] = even_in[i];
            }
        break;
        case A_ODD_BANK:
            for (i = 0; i < SIMD_WIDTH; i++) {
                add_in2[i] = odd_in[i];
            }
        break;
        default:
            for (i = 0; i < SIMD_WIDTH; i++) {
                add_in2[i] = srf_in;
            }
        break;
    }
}

void fpu::update_output() {
    int i;

    if (out_sel->read()) {
        for (i = 0; i < SIMD_WIDTH; i++) {
            output[i]->write(mult_out[i]);
        }
    } else {
        for (i = 0; i < SIMD_WIDTH; i++) {
            output[i]->write(add_out[i]);
        }
    }
}
