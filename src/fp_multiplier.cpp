 /* 
 * Copyright EPFL 2023
 * Rafael Medina Morillas
 * 
 * Riselda Kodra
 * 
 * Implementation of the Multiplier.
 *
 */

#include "fp_multiplier.h"

void fp_multiplier::clk_thread() {
    int i;

    // Reset behaviour
    for (int i = 0; i < MULT_STAGES; i++) {
#if __SYNTHESIS__
        pipeline[i] = cnm_synth(0);
#elif  HALF_FLOAT
        pipeline[i] = half_float::half_cast<half>(0.0);
#else
        pipeline[i] = cnm_t(0);
#endif

    }

    wait();

    // Clocked behaviour
    while (1) {
        if (compute_en->read()) {
            for (i = MULT_STAGES - 1; i > 0; i--) {
                pipeline[i] = pipeline[i - 1];
            }
            pipeline[0] = mul_res;
        }
        wait();
    }
}

void fp_multiplier::comb_method() {
    mul_res = op1->read() * op2->read();
    output->write(pipeline[MULT_STAGES - 1]);
}
