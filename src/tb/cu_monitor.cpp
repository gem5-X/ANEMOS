#include "cu_monitor.h"

void cu_monitor::monitor_thread() {
    sc_uint<32> instruction;
    sc_uint<OPCODE_STA - OPCODE_END + 1> OPCODE;

    cout << "c'est parti" << endl;

    while (1) {
        wait(1, SC_NS);

        instruction = instr;
        OPCODE = instruction.range(OPCODE_STA, OPCODE_END);

#if MIXED_SIM

		cout << "At time " << sc_time_stamp() << endl;
		cout << "RD " << RD << " WR " << WR << " ACT " << ACT << " AB_mode " << AB_mode << " pim_mode " << pim_mode << endl;
		cout << "PC " << pc_out << " Instruction " << OPCODE_STRING.at(OPCODE) << " " << instr << " Bank " << bank_addr << " Row " << row_addr << " Column " << col_addr << endl;
		cout << "External data DQ " << DQ << " data_to_RFs " << data_out << endl;
		cout << "CRF_wr_en " << crf_wr_en << " CRF_wr_addr " << crf_wr_addr << endl;
		cout << "SRF_rd_addr " << srf_rd_addr << " SRF_rd_a_nm " << srf_rd_a_nm << " SRF_wr_en " << srf_wr_en << " SRF_wr_addr " << srf_wr_addr << " SRF_wr_a_nm " << srf_wr_a_nm << " SRF_wr_from " << MUX_STORAGE_STRING.at(srf_wr_from->read().to_uint()) << endl;
		cout << "GRFA_rd_addr1 " << grfa_rd_addr1 << " GRFA_rd_addr2 " << grfa_rd_addr2 << " GRFA_wr_en " << grfa_wr_en << " GRFA_relu_en " << grfa_relu_en << " GRFA_wr_addr " << grfa_wr_addr << " GRFA_wr_from " << MUX_STORAGE_STRING.at(grfa_wr_from->read().to_uint()) << endl;
		cout << "GRFB_rd_addr1 " << grfb_rd_addr1 << " GRFB_rd_addr2 " << grfb_rd_addr2 << " GRFB_wr_en " << grfb_wr_en << " GRFB_relu_en " << grfb_relu_en << " GRFB_wr_addr " << grfb_wr_addr << " GRFB_wr_from " << MUX_STORAGE_STRING.at(grfb_wr_from->read().to_uint()) << endl;
		cout << "FPU_mult_en " << fpu_mult_en << " FPU_add_en " << fpu_add_en << " FPU_mult_in1_sel " << MUL1_SEL_STRING.at(fpu_mult_in1_sel->read().to_uint()) << " FPU_mult_in2_sel " << MUL2_SEL_STRING.at(fpu_mult_in2_sel->read().to_uint())
			 << " FPU_add_in1_sel " << ADD_SEL_STRING.at(fpu_add_in1_sel->read().to_uint()) << " FPU_add_in2_sel " << ADD_SEL_STRING.at(fpu_add_in2_sel->read().to_uint()) << " FPU_out_sel " << fpu_out_sel << endl << endl;

#else

        cout << "At time " << sc_time_stamp() << endl;
        cout << "RD " << RD << " WR " << WR << " ACT " << ACT << " AB_mode " << AB_mode << " pim_mode " << pim_mode << endl;
        cout << "PC " << unsigned(pc_out) << " Instruction " << OPCODE_STRING.at(OPCODE) << " " << std::bitset<32>(instr) << " Bank " << bank_addr << " Row " << row_addr << " Column " << col_addr << endl;
        cout << "External data DQ " << DQ << " data_to_RFs " << data_out  << endl;
        cout << "CRF_wr_en " << crf_wr_en << " CRF_wr_addr " << crf_wr_addr << endl;
        cout << "SRF_rd_addr " << srf_rd_addr << " SRF_rd_a_nm " << srf_rd_a_nm << " SRF_wr_en " << srf_wr_en << " SRF_wr_addr " << srf_wr_addr << " SRF_wr_a_nm " << srf_wr_a_nm << " SRF_wr_from " << MUX_STORAGE_STRING.at(srf_wr_from) << endl;
        cout << "GRFA_rd_addr1 " << grfa_rd_addr1 << " GRFA_rd_addr2 " << grfa_rd_addr2 << " GRFA_wr_en " << grfa_wr_en << " GRFA_relu_en " << grfa_relu_en << " GRFA_wr_addr " << grfa_wr_addr << " GRFA_wr_from " << MUX_STORAGE_STRING.at(grfa_wr_from) << endl;
        cout << "GRFB_rd_addr1 " << grfb_rd_addr1 << " GRFB_rd_addr2 " << grfb_rd_addr2 << " GRFB_wr_en " << grfb_wr_en << " GRFB_relu_en " << grfb_relu_en << " GRFB_wr_addr " << grfb_wr_addr << " GRFB_wr_from " << MUX_STORAGE_STRING.at(grfb_wr_from) << endl;
        cout << "FPU_mult_en " << fpu_mult_en << " FPU_add_en " << fpu_add_en << " FPU_mult_in1_sel " << MUL1_SEL_STRING.at(fpu_mult_in1_sel) << " FPU_mult_in2_sel " << MUL2_SEL_STRING.at(fpu_mult_in2_sel) << " FPU_add_in1_sel " << ADD_SEL_STRING.at(fpu_add_in1_sel)
                << " FPU_add_in2_sel " << ADD_SEL_STRING.at(fpu_add_in2_sel) << " FPU_out_sel " << fpu_out_sel << endl << endl;

#endif

        wait();
    }
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
