#include "fpu_monitor.h"

void fpu_monitor::monitor_thread() {

#if MIXED_SIM
	half *out, *grfa1_aux, *grfa2_aux, *grfb1_aux, *grfb2_aux, *srf_aux, *out_aux;
#endif

    while (1) {
        wait(0.5, SC_NS);

#if MIXED_SIM

		srf_aux = new half(half_float::detail::binary, srf_in->read().to_uint());
		grfa1_aux = new half(half_float::detail::binary, grfa_in1[0]->read().to_uint());
		grfa2_aux = new half(half_float::detail::binary, grfa_in2[0]->read().to_uint());
		grfb1_aux = new half(half_float::detail::binary, grfb_in1[0]->read().to_uint());
		grfb2_aux = new half(half_float::detail::binary, grfb_in2[0]->read().to_uint());
		out_aux = new half(half_float::detail::binary, output[0]->read().to_uint());

		cout << "At time " << sc_time_stamp() << (error ? " an error already happened" : " no error happened yet") << endl;
		cout << "Multiply enabled " << mult_en << " Add enabled " << add_en << endl;
		cout << "SRF= " << *srf_aux;
		cout << " GRF_A1= " << *grfa1_aux << " GRF_A2= " << *grfa2_aux;
		cout << " GRF_B1= " << *grfb1_aux << " GRF_B2= " << *grfb2_aux << endl;
		cout << "Mul_in_1 " << MUL1_SEL_STRING.at(mult_in1_sel->read().to_uint()) << " Mul_in_2 " << MUL2_SEL_STRING.at(mult_in2_sel->read().to_uint());
		cout << " | Add_in_1 " << ADD_SEL_STRING.at(add_in1_sel->read().to_uint()) << " Add_in_2 " << ADD_SEL_STRING.at(add_in2_sel->read().to_uint()) << endl;
		cout << "Mult pipe: " << mpipe[0][0] << " | " << mpipe[1][0] << " | " << mpipe[2][0] << " | " << mpipe[3][0] << " | " << mpipe[4][0] << endl;
		cout << "Add pipe: " << apipe[0][0] << " | " << apipe[1][0] << " | " << apipe[2][0] << " | " << apipe[3][0] << " | " << apipe[4][0] << endl;
		cout << "Out_sel " << out_sel << " Output= " << *out_aux << " predicted= " << pred_out[0] << endl << endl;

		// Automatic check
		for (int i=0; i<SIMD_WIDTH; i++){
			out = new half(half_float::detail::binary, output[i]->read().to_uint());
			if (*out != pred_out[i]) {
				cout << "** Mismatch in the output " << i << " **" << endl << endl;
				error = true;
			}
		}

#else

        cout << "At time " << sc_time_stamp() << (error ? " an error already happened" : " no error happened yet") << endl;
        cout << "Multiply enabled " << mult_en << " Add enabled " << add_en << endl;
        cout << "SRF= " << srf_in;
        cout << " GRF_A1= " << grfa_in1[0] << " GRF_A2= " << grfa_in2[0];
        cout << " GRF_B1= " << grfb_in1[0] << " GRF_B2= " << grfb_in2[0] << endl;
        cout << "Mul_in_1 " << MUL1_SEL_STRING.at(mult_in1_sel) << " Mul_in_2 " << MUL2_SEL_STRING.at(mult_in2_sel);
        cout << " | Add_in_1 " << ADD_SEL_STRING.at(add_in1_sel) << " Add_in_2 " << ADD_SEL_STRING.at(add_in2_sel) << endl;
        cout << "Mult pipe: " << mpipe[0][0] << " | " << mpipe[1][0] << " | " << mpipe[2][0] << " | " << mpipe[3][0] << " | " << mpipe[4][0] << endl;
        cout << "Add pipe: " << apipe[0][0] << " | " << apipe[1][0] << " | " << apipe[2][0] << " | " << apipe[3][0] << " | " << apipe[4][0] << endl;
        cout << "Out_sel " << out_sel << " Output= " << output[0] << " predicted= " << pred_out[0] << endl << endl;

        // Automatic check
        for (int i = 0; i < SIMD_WIDTH; i++) {
            if (output[i] != pred_out[i]) {
                cout << "** Mismatch in the output " << i << " **" << endl << endl;
                error = true;
            }
        }

#endif

        wait();
    }
}

void fpu_monitor::mirror_thread() {
    int i, j;

#if MIXED_SIM

	half *ga1[SIMD_WIDTH], *ga2[SIMD_WIDTH], *gb1[SIMD_WIDTH], *gb2[SIMD_WIDTH], *srf;//*even[SIMD_WIDTH], *odd[SIMD_WIDTH];

	while (1) {
		if (rst == SC_LOGIC_0){	// Reset
			for (i=0; i<MULT_STAGES; i++)
				for (j=0; j<SIMD_WIDTH; j++)
					mpipe[i][j] = half_float::half_cast<half>(0.0);
			for (i=0; i<ADD_STAGES; i++)
				for (j=0; j<SIMD_WIDTH; j++)
					apipe[i][j] = half_float::half_cast<half>(0.0);
		} else if (mult_en->read() == SC_LOGIC_1 || add_en->read() == SC_LOGIC_1) {

			for (i=0; i<SIMD_WIDTH; i++) { ga1[i] = new half(half_float::detail::binary, grfa_in1[i]->read().to_uint()); }
			for (i=0; i<SIMD_WIDTH; i++) { ga2[i] = new half(half_float::detail::binary, grfa_in2[i]->read().to_uint()); }
			for (i=0; i<SIMD_WIDTH; i++) { gb1[i] = new half(half_float::detail::binary, grfb_in1[i]->read().to_uint()); }
			for (i=0; i<SIMD_WIDTH; i++) { gb2[i] = new half(half_float::detail::binary, grfb_in2[i]->read().to_uint()); }
//			for (i=0; i<SIMD_WIDTH; i++) { even[i] = new half(half_float::detail::binary, even_in[i]->read().to_uint()); }
//			for (i=0; i<SIMD_WIDTH; i++) { odd[i] = new half(half_float::detail::binary, odd_in[i]->read().to_uint()); }
			srf = new half(half_float::detail::binary, srf_in->read().to_uint());

			// Mirroring the 4 input muxes
			switch (mult_in1_sel->read().to_uint()) {
				case M1_GRF_A1:
					for (i=0; i<SIMD_WIDTH; i++) { m1_in[i] = *ga1[i]; }
				break;
				case M1_GRF_A2:
					for (i=0; i<SIMD_WIDTH; i++) { m1_in[i] = *ga2[i]; }
				break;
				case M1_GRF_B1:
					for (i=0; i<SIMD_WIDTH; i++) { m1_in[i] = *gb1[i]; }
				break;
				case M1_GRF_B2:
					for (i=0; i<SIMD_WIDTH; i++) { m1_in[i] = *gb2[i]; }
				break;
				// case M1_EVEN_BANK:
				//     for (i=0; i<SIMD_WIDTH; i++) { m1_in[i] = *even[i]; }
				// break;
				// case M1_ODD_BANK:
				//     for (i=0; i<SIMD_WIDTH; i++) { m1_in[i] = *odd[i]; }
				// break;
				default:
					for (i=0; i<SIMD_WIDTH; i++) { m1_in[i] = *ga1[i]; }
				break;
			}
			switch (mult_in2_sel->read().to_uint()) {
				case M2_SRF:
					for (i=0; i<SIMD_WIDTH; i++) { m2_in[i] = *srf; }
				break;
				case M2_GRF_A1:
					for (i=0; i<SIMD_WIDTH; i++) { m2_in[i] = *ga1[i]; }
				break;
				case M2_GRF_A2:
					for (i=0; i<SIMD_WIDTH; i++) { m2_in[i] = *ga2[i]; }
				break;
				case M2_GRF_B1:
					for (i=0; i<SIMD_WIDTH; i++) { m2_in[i] = *gb1[i]; }
				break;
				case M2_GRF_B2:
					for (i=0; i<SIMD_WIDTH; i++) { m2_in[i] = *gb2[i]; }
				break;
				// case M2_EVEN_BANK:
				//     for (i=0; i<SIMD_WIDTH; i++) { m2_in[i] = *even[i]; }
				// break;
				// case M2_ODD_BANK:
				//     for (i=0; i<SIMD_WIDTH; i++) { m2_in[i] = *odd[i]; }
				// break;
				default:
					for (i=0; i<SIMD_WIDTH; i++) { m2_in[i] = *ga1[i]; }
				break;
			}
			switch (add_in1_sel->read().to_uint()) {
				case A_MULT_OUT:
					for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = mpipe[MULT_STAGES-1][i]; }
				break;
				case A_SRF:
					for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = *srf; }
				break;
				case A_GRF_A1:
					for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = *ga1[i]; }
				break;
				case A_GRF_A2:
					for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = *ga2[i]; }
				break;
				case A_GRF_B1:
					for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = *gb1[i]; }
				break;
				case A_GRF_B2:
					for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = *gb2[i]; }
				break;
				// case A_EVEN_BANK:
				//     for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = *even[i]; }
				// break;
				// case A_ODD_BANK:
				//     for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = *odd[i]; }
				// break;
				default:
					for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = mpipe[MULT_STAGES-1][i]; }
				break;
			}
			switch (add_in2_sel->read().to_uint()) {
				case A_MULT_OUT:
					for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = mpipe[MULT_STAGES-1][i]; }
				break;
				case A_SRF:
					for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = *srf; }
				break;
				case A_GRF_A1:
					for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = *ga1[i]; }
				break;
				case A_GRF_A2:
					for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = *ga2[i]; }
				break;
				case A_GRF_B1:
					for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = *gb1[i]; }
				break;
				case A_GRF_B2:
					for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = *gb2[i]; }
				break;
				// case A_EVEN_BANK:
				//     for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = *odd[i]; }
				// break;
				// case A_ODD_BANK:
				//     for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = *even[i]; }
				// break;
				default:
					for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = mpipe[MULT_STAGES-1][i]; }
				break;
			}

#else

    while (1) {
        if (!rst) {	// Reset
            for (i = 0; i < MULT_STAGES; i++)
                for (j = 0; j < SIMD_WIDTH; j++)
                    mpipe[i][j] = half_float::half_cast<half>(0.0);
            for (i = 0; i < ADD_STAGES; i++)
                for (j = 0; j < SIMD_WIDTH; j++)
                    apipe[i][j] = half_float::half_cast<half>(0.0);
        } else if (mult_en->read() || add_en->read()) {

            // Mirroring the 4 input muxes
            switch (mult_in1_sel->read()) {
            case M1_GRF_A1:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    m1_in[i] = grfa_in1[i];
                }
                break;
            case M1_GRF_A2:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    m1_in[i] = grfa_in2[i];
                }
                break;
            case M1_GRF_B1:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    m1_in[i] = grfb_in1[i];
                }
                break;
            case M1_GRF_B2:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    m1_in[i] = grfb_in2[i];
                }
                break;
                // case M1_EVEN_BANK:
                //     for (i=0; i<SIMD_WIDTH; i++) { m1_in[i] = even_in[i]; }
                // break;
                // case M1_ODD_BANK:
                //     for (i=0; i<SIMD_WIDTH; i++) { m1_in[i] = odd_in[i]; }
                // break;
            default:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    m1_in[i] = grfa_in1[i];
                }
                break;
            }
            switch (mult_in2_sel->read()) {
            case M2_SRF:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    m2_in[i] = srf_in;
                }
                break;
            case M2_GRF_A1:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    m2_in[i] = grfa_in1[i];
                }
                break;
            case M2_GRF_A2:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    m2_in[i] = grfa_in2[i];
                }
                break;
            case M2_GRF_B1:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    m2_in[i] = grfb_in1[i];
                }
                break;
            case M2_GRF_B2:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    m2_in[i] = grfb_in2[i];
                }
                break;
                // case M2_EVEN_BANK:
                //     for (i=0; i<SIMD_WIDTH; i++) { m2_in[i] = even_in[i]; }
                // break;
                // case M2_ODD_BANK:
                //     for (i=0; i<SIMD_WIDTH; i++) { m2_in[i] = odd_in[i]; }
                // break;
            default:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    m2_in[i] = srf_in;
                }
                break;
            }
            switch (add_in1_sel->read()) {
            case A_MULT_OUT:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    a1_in[i] = mpipe[MULT_STAGES - 1][i];
                }
                break;
            case A_SRF:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    a1_in[i] = srf_in;
                }
                break;
            case A_GRF_A1:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    a1_in[i] = grfa_in1[i];
                }
                break;
            case A_GRF_A2:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    a1_in[i] = grfa_in2[i];
                }
                break;
            case A_GRF_B1:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    a1_in[i] = grfb_in1[i];
                }
                break;
            case A_GRF_B2:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    a1_in[i] = grfb_in2[i];
                }
                break;
                // case A_EVEN_BANK:
                //     for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = even_in[i]; }
                // break;
                // case A_ODD_BANK:
                //     for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = odd_in[i]; }
                // break;
            default:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    a1_in[i] = mpipe[MULT_STAGES - 1][i];
                }
                break;
            }
            switch (add_in2_sel->read()) {
            case A_MULT_OUT:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    a2_in[i] = mpipe[MULT_STAGES - 1][i];
                }
                break;
            case A_SRF:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    a2_in[i] = srf_in;
                }
                break;
            case A_GRF_A1:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    a2_in[i] = grfa_in1[i];
                }
                break;
            case A_GRF_A2:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    a2_in[i] = grfa_in2[i];
                }
                break;
            case A_GRF_B1:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    a2_in[i] = grfb_in1[i];
                }
                break;
            case A_GRF_B2:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    a2_in[i] = grfb_in2[i];
                }
                break;
                // case A_EVEN_BANK:
                //     for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = even_in[i]; }
                // break;
                // case A_ODD_BANK:
                //     for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = odd_in[i]; }
                // break;
            default:
                for (i = 0; i < SIMD_WIDTH; i++) {
                    a2_in[i] = mpipe[MULT_STAGES - 1][i];
                }
                break;
            }

#endif

            // Mirroring the addition, multiplication and their pipelines
            for (i = MULT_STAGES - 1; i > 0; i--) {
                for (j = 0; j < SIMD_WIDTH; j++)
                    mpipe[i][j] = mpipe[i - 1][j];
            }
            for (i = ADD_STAGES - 1; i > 0; i--) {
                for (j = 0; j < SIMD_WIDTH; j++)
                    apipe[i][j] = apipe[i - 1][j];
            }
            for (j = 0; j < SIMD_WIDTH; j++) {
                mpipe[0][j] = m1_in[j] * m2_in[j];
                apipe[0][j] = a1_in[j] + a2_in[j];
            }
        }
        wait();
    }
}

void fpu_monitor::out_mirror_thread() {
    int i;

    // Mirroring the output selection
#if MIXED_SIM
	if (out_sel->read() == SC_LOGIC_1) {
#else
    if (out_sel->read()) {
#endif
        for (i = 0; i < SIMD_WIDTH; i++) {
            pred_out[i] = mpipe[MULT_STAGES - 1][i];
        }
    } else {
        for (i = 0; i < SIMD_WIDTH; i++) {
            pred_out[i] = apipe[ADD_STAGES - 1][i];
        }
    }
    next_trigger(output_change_evnt);
}
