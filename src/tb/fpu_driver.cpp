#include "fpu_driver.h"
#include <random>

void fpu_driver::driver_thread() {
    int i;

    // Randomization tools
    std::mt19937 gen(11111);    // Standard mersenne_twister_engine seeded
    std::uniform_real_distribution<float> dis_reg(-10.0, 10.0);
    std::uniform_int_distribution<> dis_mul1_sel(M1_GRF_A1, M1_GRF_B2);
    std::uniform_int_distribution<> dis_mul2_sel(M2_SRF, M2_GRF_B2);
    std::uniform_int_distribution<> dis_add_sel(A_MULT_OUT, A_GRF_B2);
    std::uniform_int_distribution<> dis_bool(0, 1);

#if MIXED_SIM

    half half_aux;

    // Initialization
	rst->write(SC_LOGIC_0);
	mult_en->write(SC_LOGIC_0);
	add_en->write(SC_LOGIC_0);
	mult_in1_sel->write(M1_GRF_A1);
	mult_in2_sel->write(M2_SRF);
	add_in1_sel->write(A_MULT_OUT);
	add_in2_sel->write(A_SRF);
	out_sel->write(SC_LOGIC_0);
	half_aux = half_float::half_cast<half>(dis_reg(gen));
	srf_in->write(half_aux.bin_word());
	for (i=0; i<SIMD_WIDTH; i++) {
		half_aux = half_float::half_cast<half>(dis_reg(gen));
		grfa_in1[i]->write(half_aux.bin_word());
		half_aux = half_float::half_cast<half>(dis_reg(gen));
		grfa_in2[i]->write(half_aux.bin_word());
		half_aux = half_float::half_cast<half>(dis_reg(gen));
		grfb_in1[i]->write(half_aux.bin_word());
		half_aux = half_float::half_cast<half>(dis_reg(gen));
		grfb_in2[i]->write(half_aux.bin_word());
	}

	wait(5, SC_NS);
	rst->write(SC_LOGIC_1);
	wait(0, SC_NS);
	mult_en->write(SC_LOGIC_0);
	add_en->write(SC_LOGIC_0);
	wait(6, SC_NS);

	while (1) {
		mult_en->write(SC_LOGIC_1);
		add_en->write(SC_LOGIC_1);
		wait(0, SC_NS);

		mult_in1_sel->write(dis_mul1_sel(gen));
		mult_in2_sel->write(dis_mul2_sel(gen));
		add_in1_sel->write(dis_add_sel(gen));
		add_in2_sel->write(dis_add_sel(gen));
		out_sel->write(static_cast<sc_logic>(dis_bool(gen)));
		half_aux = half_float::half_cast<half>(dis_reg(gen));
		srf_in->write(half_aux.bin_word());
		for (i=0; i<SIMD_WIDTH; i++) {
			half_aux = half_float::half_cast<half>(dis_reg(gen));
			grfa_in1[i]->write(half_aux.bin_word());
			half_aux = half_float::half_cast<half>(dis_reg(gen));
			grfa_in2[i]->write(half_aux.bin_word());
			half_aux = half_float::half_cast<half>(dis_reg(gen));
			grfb_in1[i]->write(half_aux.bin_word());
			half_aux = half_float::half_cast<half>(dis_reg(gen));
			grfb_in2[i]->write(half_aux.bin_word());
		}
		wait(10, SC_NS);
	}

#else

    // Initialization
    rst->write(0);
    mult_en->write(0);
    add_en->write(0);
    mult_in1_sel->write(M1_GRF_A1);
    mult_in2_sel->write(M2_SRF);
    add_in1_sel->write(A_MULT_OUT);
    add_in2_sel->write(A_SRF);
    out_sel->write(0);
    srf_in->write(half_float::half_cast<half>(dis_reg(gen)));
    for (i = 0; i < SIMD_WIDTH; i++) {
        grfa_in1[i]->write(half_float::half_cast<half>(dis_reg(gen)));
        grfa_in2[i]->write(half_float::half_cast<half>(dis_reg(gen)));
        grfb_in1[i]->write(half_float::half_cast<half>(dis_reg(gen)));
        grfb_in2[i]->write(half_float::half_cast<half>(dis_reg(gen)));
    }

    wait(5, SC_NS);
    rst->write(1);
    wait(0, SC_NS);
    mult_en->write(0);
    add_en->write(0);
    wait(6, SC_NS);

    while (1) {
        mult_en->write(1);
        add_en->write(1);
        wait(0, SC_NS);

        mult_in1_sel->write(dis_mul1_sel(gen));
        mult_in2_sel->write(dis_mul2_sel(gen));
        add_in1_sel->write(dis_add_sel(gen));
        add_in2_sel->write(dis_add_sel(gen));
        out_sel->write(dis_bool(gen));
//        out_sel->write(1);
        srf_in->write(half_float::half_cast<half>(dis_reg(gen)));
        for (i = 0; i < SIMD_WIDTH; i++) {
            grfa_in1[i]->write(half_float::half_cast<half>(dis_reg(gen)));
            grfa_in2[i]->write(half_float::half_cast<half>(dis_reg(gen)));
            grfb_in1[i]->write(half_float::half_cast<half>(dis_reg(gen)));
            grfb_in2[i]->write(half_float::half_cast<half>(dis_reg(gen)));
        }
        wait(10, SC_NS);
    }

#endif

}
