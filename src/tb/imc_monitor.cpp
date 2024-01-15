#include "imc_monitor.h"

void imc_monitor::monitor_thread() {
    sc_uint<32> instruction;
    sc_uint<OPCODE_STA - OPCODE_END + 1> OPCODE;

#if MIXED_SIM

	while (1) {
		wait(0.5, SC_NS);

		cout << "At time " << sc_time_stamp() << endl;
		cout << "RD " << RD << " WR " << WR << " ACT " << ACT << " AB_mode " << AB_mode << " pim_mode " << pim_mode << endl;
		cout << " Bank " << bank_addr->read().to_uint() << " Row " << row_addr->read().to_uint() << " Column " << col_addr->read().to_uint() << endl;
		cout << "External data DQ " << std::hex << DQ->read().to_uint() << std::dec << endl;
		cout << "Even in " << even_in->read() << endl << "Odd in " << odd_in->read() << endl;
		cout << "Even out " << even_out->read() << endl << "Odd out " << odd_out->read() << endl;

		wait();
	}

#else

    while (1) {
        wait(0.5, SC_NS);

        cout << "At time " << sc_time_stamp() << endl;
        cout << "RD " << RD << " WR " << WR << " ACT " << ACT << " AB_mode "
                << AB_mode << " pim_mode " << pim_mode << endl;
        cout << " Bank " << bank_addr << " Row " << row_addr << " Column "
                << col_addr << endl;
        cout << "External data DQ " << std::hex << DQ << std::dec << endl;
        cout << "Even bus " << even_bus << endl << "Odd bus " << odd_bus
                << endl;

        wait();
    }

#endif

}

//void id_monitor::mirror_thread() {
//	int i, j;
//
//	while (1) {
//		if (!rst){	// Reset
//			for (i=0; i<MULT_STAGES; i++)
//				for (j=0; j<SIMD_WIDTH; j++)
//					mpipe[i][j] = half_float::half_cast<half>(0.0);
//			for (i=0; i<ADD_STAGES; i++)
//				for (j=0; j<SIMD_WIDTH; j++)
//					apipe[i][j] = half_float::half_cast<half>(0.0);
//		} else if (mult_en->read() || add_en->read()) {
//
//			// Mirroring the 4 input muxes
//			switch (mult_in1_sel->read()) {
//				case M1_GRF_A1:
//					for (i=0; i<SIMD_WIDTH; i++) { m1_in[i] = grfa_in1[i]; }
//				break;
//				case M1_GRF_A2:
//					for (i=0; i<SIMD_WIDTH; i++) { m1_in[i] = grfa_in2[i]; }
//				break;
//				case M1_GRF_B1:
//					for (i=0; i<SIMD_WIDTH; i++) { m1_in[i] = grfb_in1[i]; }
//				break;
//				case M1_GRF_B2:
//					for (i=0; i<SIMD_WIDTH; i++) { m1_in[i] = grfb_in1[i]; }
//				break;
//				// case M1_EVEN_BANK:
//				//     for (i=0; i<SIMD_WIDTH; i++) { m1_in[i] = even_in[i]; }
//				// break;
//				// case M1_ODD_BANK:
//				//     for (i=0; i<SIMD_WIDTH; i++) { m1_in[i] = odd_in[i]; }
//				// break;
//				default:
//					for (i=0; i<SIMD_WIDTH; i++) { m1_in[i] = grfa_in1[i]; }
//				break;
//			}
//			switch (mult_in2_sel->read()) {
//				case M2_SRF:
//					for (i=0; i<SIMD_WIDTH; i++) { m2_in[i] = srf_in; }
//				break;
//				case M2_GRF_A1:
//					for (i=0; i<SIMD_WIDTH; i++) { m2_in[i] = grfa_in1[i]; }
//				break;
//				case M2_GRF_A2:
//					for (i=0; i<SIMD_WIDTH; i++) { m2_in[i] = grfa_in2[i]; }
//				break;
//				case M2_GRF_B1:
//					for (i=0; i<SIMD_WIDTH; i++) { m2_in[i] = grfb_in1[i]; }
//				break;
//				case M2_GRF_B2:
//					for (i=0; i<SIMD_WIDTH; i++) { m2_in[i] = grfb_in2[i]; }
//				break;
//				// case M2_EVEN_BANK:
//				//     for (i=0; i<SIMD_WIDTH; i++) { m2_in[i] = even_in[i]; }
//				// break;
//				// case M2_ODD_BANK:
//				//     for (i=0; i<SIMD_WIDTH; i++) { m2_in[i] = odd_in[i]; }
//				// break;
//				default:
//					for (i=0; i<SIMD_WIDTH; i++) { m2_in[i] = srf_in; }
//				break;
//			}
//			switch (add_in1_sel->read()) {
//				case A_MULT_OUT:
//					for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = mpipe[MULT_STAGES-1][i]; }
//				break;
//				case A_SRF:
//					for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = srf_in; }
//				break;
//				case A_GRF_A1:
//					for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = grfa_in1[i]; }
//				break;
//				case A_GRF_A2:
//					for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = grfa_in2[i]; }
//				break;
//				case A_GRF_B1:
//					for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = grfb_in1[i]; }
//				break;
//				case A_GRF_B2:
//					for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = grfb_in1[i]; }
//				break;
//				// case A_EVEN_BANK:
//				//     for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = even_in[i]; }
//				// break;
//				// case A_ODD_BANK:
//				//     for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = odd_in[i]; }
//				// break;
//				default:
//					for (i=0; i<SIMD_WIDTH; i++) { a1_in[i] = mpipe[MULT_STAGES-1][i]; }
//				break;
//			}
//			switch (add_in2_sel->read()) {
//				case A_MULT_OUT:
//					for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = mpipe[MULT_STAGES-1][i]; }
//				break;
//				case A_SRF:
//					for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = srf_in; }
//				break;
//				case A_GRF_A1:
//					for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = grfa_in1[i]; }
//				break;
//				case A_GRF_A2:
//					for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = grfa_in2[i]; }
//				break;
//				case A_GRF_B1:
//					for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = grfb_in1[i]; }
//				break;
//				case A_GRF_B2:
//					for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = grfb_in2[i]; }
//				break;
//				// case A_EVEN_BANK:
//				//     for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = even_in[i]; }
//				// break;
//				// case A_ODD_BANK:
//				//     for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = odd_in[i]; }
//				// break;
//				default:
//					for (i=0; i<SIMD_WIDTH; i++) { a2_in[i] = mpipe[MULT_STAGES-1][i]; }
//				break;
//			}
//
//			// Mirroring the addition, multiplication and their pipelines
//			for (i=MULT_STAGES-1; i>0; i--) {
//				for (j=0; j<SIMD_WIDTH; j++)
//					mpipe[i][j] = mpipe[i-1][j];
//			}
//			for (i=ADD_STAGES-1; i>0; i--) {
//				for (j=0; j<SIMD_WIDTH; j++)
//					apipe[i][j] = apipe[i-1][j];
//			}
//			for (j=0; j<SIMD_WIDTH; j++) {
//				mpipe[0][j] = m1_in[j] * m2_in[j];
//				apipe[0][j] = a1_in[j] + a2_in[j];
//			}
//		}
//		wait();
//	}
//}
//
//void id_monitor::out_mirror_thread(){
//	int i;
//
//	// Mirroring the output selection
//	if (out_sel->read()) {
//		for (i=0; i<SIMD_WIDTH; i++) {
//			pred_out[i] = mpipe[MULT_STAGES-1][i];
//		}
//	} else {
//		for (i=0; i<SIMD_WIDTH; i++) {
//			pred_out[i] = apipe[ADD_STAGES-1][i];
//		}
//	}
//	next_trigger(output_change_evnt);
//}
