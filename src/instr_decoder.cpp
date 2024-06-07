/*
 * Copyright EPFL 2021
 * Rafael Medina Morillas
 *
 * Implementation of the Instruction Decoder Unit.
 *
 */

#include "instr_decoder.h"

void instr_decoder::clk_thread() {
    int i;
    // Auxiliar pipelines for the GRF "from" signals
    bool wr_en_pipe[1 + MULT_STAGES + ADD_STAGES];
    uint wr_addr_pipe[1 + MULT_STAGES + ADD_STAGES];
    uint8_t wr_from_pipe[1 + MULT_STAGES + ADD_STAGES];

    // Reset all registers and pipelines
    nop_cnt_reg = false;

    jmp_act_reg = false;
    jmp_cnt_reg = false;

    grfa_rd_addr2_reg = 0;
    grfa_relu_en_reg = 0;

    grfb_rd_addr2_reg = 0;
    grfb_relu_en_reg = 0;

    srf_wr_en_reg = false;
    srf_wr_from_reg = 0;
    srf_wr_addr_reg = 0;
    srf_wr_a_nm_reg = false;

    fpu_mult_in1_sel_reg = 0;
    fpu_mult_in2_sel_reg = 0;

    for (i = 0; i < MULT_STAGES; i++) {
        mul_en_pipe[i] = false;
    }

    for (i = 0; i < (1 + MULT_STAGES); i++) {
        grfa_rd_addr1_pipe[i] = 0;
        grfb_rd_addr1_pipe[i] = 0;

        srf_rd_addr_pipe[i] = 0;
        srf_rd_a_nm_pipe[i] = false;

        fpu_add_in1_sel_pipe[i] = 0;
        fpu_add_in2_sel_pipe[i] = 0;
        fpu_out_sel_pipe[i] = false;
    }

    for (i = 0; i < (MULT_STAGES + ADD_STAGES); i++) {
        add_en_pipe[i] = false;
    }

    for (i = 0; i < (1 + MULT_STAGES + ADD_STAGES); i++) {
        grfa_wr_en_pipe[i] = false;
        grfa_wr_addr_pipe[i] = 0;
        grfa_wr_from_pipe[i] = 0;

        grfb_wr_en_pipe[i] = false;
        grfb_wr_addr_pipe[i] = 0;
        grfb_wr_from_pipe[i] = 0;
    }

    wait();

    // Update registers and advance pipelines
    while (1) {

        // NOP
        nop_cnt_reg = nop_cnt_nxt;

        // JUMP
        jmp_act_reg = jmp_act_nxt;
        jmp_cnt_reg = jmp_cnt_nxt;

        // GRF_A
        grfa_rd_addr1_pipe[0] = grfa_rd_addr1_toAddAftLoad;
#if MULT_STAGES > 1
		grfa_rd_addr1_pipe[1] = grfa_rd_addr1_pipe[0] | grfa_rd_addr1_toAdd;
#if MULT_STAGES > 2
		for (i=2; i<MULT_STAGES; i++) {
			grfa_rd_addr1_pipe[i] = grfa_rd_addr1_pipe[i-1];
		}
#endif
		grfa_rd_addr1_pipe[MULT_STAGES] = grfa_rd_addr1_pipe[MULT_STAGES-1] | grfa_rd_addr1_toMAWAftLoad;
#else
        grfa_rd_addr1_pipe[1] = grfa_rd_addr1_pipe[0] | grfa_rd_addr1_toAdd
                | grfa_rd_addr1_toMAWAftLoad;
#endif

        grfa_rd_addr2_reg = grfa_rd_addr2_nxt;

        for (i = 0; i < 1 + MULT_STAGES + ADD_STAGES; i++) {
            wr_en_pipe[i] = false;
            wr_addr_pipe[i] = 0;
            wr_from_pipe[i] = 0;
        }
        wr_en_pipe[0] |= grfa_wr_en_fromMaAAftLoad;
        wr_en_pipe[1] |= grfa_wr_en_fromMaA;
        wr_en_pipe[MULT_STAGES] |= grfa_wr_en_fromAddAftLoad;
        wr_en_pipe[MULT_STAGES + 1] |= grfa_wr_en_fromAdd;
        wr_en_pipe[ADD_STAGES] |= grfa_wr_en_fromMulAftLoad;
        wr_en_pipe[ADD_STAGES + 1] |= grfa_wr_en_fromMul;
        wr_en_pipe[MULT_STAGES + ADD_STAGES] |= grfa_wr_en_fromLoad;
        wr_addr_pipe[0] |= grfa_wr_addr_fromMaAAftLoad;
        wr_addr_pipe[1] |= grfa_wr_addr_fromMaA;
        wr_addr_pipe[MULT_STAGES] |= grfa_wr_addr_fromAddAftLoad;
        wr_addr_pipe[MULT_STAGES + 1] |= grfa_wr_addr_fromAdd;
        wr_addr_pipe[ADD_STAGES] |= grfa_wr_addr_fromMulAftLoad;
        wr_addr_pipe[ADD_STAGES + 1] |= grfa_wr_addr_fromMul;
        wr_addr_pipe[MULT_STAGES + ADD_STAGES] |= grfa_wr_addr_fromLoad;
        wr_from_pipe[0] |= grfa_wr_from_fromMaAAftLoad;
        wr_from_pipe[1] |= grfa_wr_from_fromMaA;
        wr_from_pipe[MULT_STAGES] |= grfa_wr_from_fromAddAftLoad;
        wr_from_pipe[MULT_STAGES + 1] |= grfa_wr_from_fromAdd;
        wr_from_pipe[ADD_STAGES] |= grfa_wr_from_fromMulAftLoad;
        wr_from_pipe[ADD_STAGES + 1] |= grfa_wr_from_fromMul;
        wr_from_pipe[MULT_STAGES + ADD_STAGES] |= grfa_wr_from_fromLoad;
        grfa_wr_en_pipe[0] = wr_en_pipe[0];
        grfa_wr_addr_pipe[0] = wr_addr_pipe[0];
        grfa_wr_from_pipe[0] = wr_from_pipe[0];
        for (i = 1; i < 1 + MULT_STAGES + ADD_STAGES; i++) {
            grfa_wr_en_pipe[i] = grfa_wr_en_pipe[i - 1] | wr_en_pipe[i];
            grfa_wr_addr_pipe[i] = grfa_wr_addr_pipe[i - 1] | wr_addr_pipe[i];
            grfa_wr_from_pipe[i] = grfa_wr_from_pipe[i - 1] | wr_from_pipe[i];
        }

        grfa_relu_en_reg = grfa_relu_en_nxt;

        // GRF_B
        grfb_rd_addr1_pipe[0] = grfb_rd_addr1_toAddAftLoad;
#if MULT_STAGES > 1
		grfb_rd_addr1_pipe[1] = grfb_rd_addr1_pipe[0] | grfb_rd_addr1_toAdd;
#if MULT_STAGES > 2
		for (i=2; i<MULT_STAGES; i++) {
			grfb_rd_addr1_pipe[i] = grfb_rd_addr1_pipe[i-1];
		}
#endif
		grfb_rd_addr1_pipe[MULT_STAGES] = grfb_rd_addr1_pipe[MULT_STAGES-1] | grfb_rd_addr1_toMAWAftLoad;
#else
        grfb_rd_addr1_pipe[1] = grfb_rd_addr1_pipe[0] | grfb_rd_addr1_toAdd
                | grfb_rd_addr1_toMAWAftLoad;
#endif

        grfb_rd_addr2_reg = grfb_rd_addr2_nxt;

        for (i = 0; i < 1 + MULT_STAGES + ADD_STAGES; i++) {
            wr_en_pipe[i] = false;
            wr_addr_pipe[i] = 0;
            wr_from_pipe[i] = 0;
        }
        wr_en_pipe[0] |= grfb_wr_en_fromMaAAftLoad;
        wr_en_pipe[1] |= grfb_wr_en_fromMaA;
        wr_en_pipe[MULT_STAGES] |= grfb_wr_en_fromAddAftLoad;
        wr_en_pipe[MULT_STAGES + 1] |= grfb_wr_en_fromAdd;
        wr_en_pipe[ADD_STAGES] |= grfb_wr_en_fromMulAftLoad;
        wr_en_pipe[ADD_STAGES + 1] |= grfb_wr_en_fromMul;
        wr_en_pipe[MULT_STAGES + ADD_STAGES] |= grfb_wr_en_fromLoad;
        wr_addr_pipe[0] |= grfb_wr_addr_fromMaAAftLoad;
        wr_addr_pipe[1] |= grfb_wr_addr_fromMaA;
        wr_addr_pipe[MULT_STAGES] |= grfb_wr_addr_fromAddAftLoad;
        wr_addr_pipe[MULT_STAGES + 1] |= grfb_wr_addr_fromAdd;
        wr_addr_pipe[ADD_STAGES] |= grfb_wr_addr_fromMulAftLoad;
        wr_addr_pipe[ADD_STAGES + 1] |= grfb_wr_addr_fromMul;
        wr_addr_pipe[MULT_STAGES + ADD_STAGES] |= grfb_wr_addr_fromLoad;
        wr_from_pipe[0] |= grfb_wr_from_fromMaAAftLoad;
        wr_from_pipe[1] |= grfb_wr_from_fromMaA;
        wr_from_pipe[MULT_STAGES] |= grfb_wr_from_fromAddAftLoad;
        wr_from_pipe[MULT_STAGES + 1] |= grfb_wr_from_fromAdd;
        wr_from_pipe[ADD_STAGES] |= grfb_wr_from_fromMulAftLoad;
        wr_from_pipe[ADD_STAGES + 1] |= grfb_wr_from_fromMul;
        wr_from_pipe[MULT_STAGES + ADD_STAGES] |= grfb_wr_from_fromLoad;
        grfb_wr_en_pipe[0] = wr_en_pipe[0];
        grfb_wr_addr_pipe[0] = wr_addr_pipe[0];
        grfb_wr_from_pipe[0] = wr_from_pipe[0];
        for (i = 1; i < 1 + MULT_STAGES + ADD_STAGES; i++) {
            grfb_wr_en_pipe[i] = grfb_wr_en_pipe[i - 1] | wr_en_pipe[i];
            grfb_wr_addr_pipe[i] = grfb_wr_addr_pipe[i - 1] | wr_addr_pipe[i];
            grfb_wr_from_pipe[i] = grfb_wr_from_pipe[i - 1] | wr_from_pipe[i];
        }

        grfb_relu_en_reg = grfb_relu_en_nxt;

        // SRF
        srf_rd_addr_pipe[0] = srf_rd_addr_toAddAftLoad;
        srf_rd_a_nm_pipe[0] = srf_rd_a_nm_toAddAftLoad;
#if MULT_STAGES > 1
		srf_rd_addr_pipe[1] = srf_rd_addr_pipe[0] | srf_rd_addr_toAdd;
		srf_rd_a_nm_pipe[1] = srf_rd_a_nm_pipe[0] | srf_rd_a_nm_toAdd;
#if MULT_STAGES > 2
		for (i=2; i<MULT_STAGES; i++) {
			srf_rd_addr_pipe[i] = srf_rd_addr_pipe[i-1];
			srf_rd_a_nm_pipe[i] = srf_rd_a_nm_pipe[i-1];
		}
#endif
		srf_rd_addr_pipe[MULT_STAGES] = srf_rd_addr_pipe[MULT_STAGES-1] | srf_rd_addr_toMAWAftLoad;
		srf_rd_a_nm_pipe[MULT_STAGES] = srf_rd_a_nm_pipe[MULT_STAGES-1] | srf_rd_a_nm_toMAWAftLoad;
#else
        srf_rd_addr_pipe[1] = srf_rd_addr_pipe[0] | srf_rd_addr_toAdd
                | srf_rd_addr_toMAWAftLoad;
        srf_rd_a_nm_pipe[1] = srf_rd_a_nm_pipe[0] | srf_rd_a_nm_toAdd
                | srf_rd_a_nm_toMAWAftLoad;
#endif

        srf_wr_en_reg = srf_wr_en_nxt;
        srf_wr_from_reg = srf_wr_from_nxt;
        srf_wr_addr_reg = srf_wr_addr_nxt;
        srf_wr_a_nm_reg = srf_wr_a_nm_nxt;

        // FPU
        fpu_add_in1_sel_pipe[0] = fpu_add_in1_sel_toAddAftLoad;
        fpu_add_in2_sel_pipe[0] = fpu_add_in2_sel_toAddAftLoad;
#if MULT_STAGES > 1
		fpu_add_in1_sel_pipe[1] = fpu_add_in1_sel_pipe[0] | fpu_add_in1_sel_toAdd;
		fpu_add_in2_sel_pipe[1] = fpu_add_in2_sel_pipe[0] | fpu_add_in2_sel_toAdd;
#if MULT_STAGES > 2
		for (i=2; i<MULT_STAGES; i++) {
			fpu_add_in1_sel_pipe[i] = fpu_add_in1_sel_pipe[i-1];
			fpu_add_in2_sel_pipe[i] = fpu_add_in2_sel_pipe[i-1];
		}
#endif
		fpu_add_in1_sel_pipe[MULT_STAGES] = fpu_add_in1_sel_pipe[MULT_STAGES-1] | fpu_add_in1_sel_toMoAAftLoad;
		fpu_add_in2_sel_pipe[MULT_STAGES] = fpu_add_in2_sel_pipe[MULT_STAGES-1] | fpu_add_in2_sel_toMoAAftLoad;
#else
        fpu_add_in1_sel_pipe[1] = fpu_add_in1_sel_pipe[0]
                | fpu_add_in1_sel_toAdd | fpu_add_in1_sel_toMoAAftLoad;
        fpu_add_in2_sel_pipe[1] = fpu_add_in2_sel_pipe[0]
                | fpu_add_in2_sel_toAdd | fpu_add_in2_sel_toMoAAftLoad;
#endif

        fpu_mult_in1_sel_reg = fpu_mult_in1_sel_nxt;
        fpu_mult_in2_sel_reg = fpu_mult_in2_sel_nxt;

        add_en_pipe[0] = add_en_toAddAftLoad;
#if MULT_STAGES > 1
		add_en_pipe[1] = add_en_pipe[0] | add_en_toAdd;
#if MULT_STAGES > 2
		for (i=2; i<MULT_STAGES; i++) {
			add_en_pipe[i] = add_en_pipe[i-1];
		}
#endif
		add_en_pipe[MULT_STAGES] = add_en_pipe[MULT_STAGES-1] | add_en_toMoAAftLoad;
#else
        add_en_pipe[1] = add_en_pipe[0] | add_en_toAdd | add_en_toMoAAftLoad;
#endif
#if ADD_STAGES > 1
        add_en_pipe[MULT_STAGES + 1] = add_en_pipe[MULT_STAGES] | add_en_toMoA;
#if ADD_STAGES > 2
		for (i=MULT_STAGES+2; i<(MULT_STAGES+ADD_STAGES); i++) {
			add_en_pipe[i] = add_en_pipe[i-1];
		}
#endif
#endif

        mul_en_pipe[0] = mul_en_toMoAAftLoad;
#if MULT_STAGES > 1
		mul_en_pipe[1] = mul_en_pipe[0] | mul_en_toMoA;
#if MULT_STAGES > 2
		for (i=2; i<MULT_STAGES; i++) {
			mul_en_pipe[i] = mul_en_pipe[i-1];
		}
#endif
#endif

        fpu_out_sel_pipe[0] = fpu_out_sel_fromMulAftLoad;
        fpu_out_sel_pipe[1] = fpu_out_sel_pipe[0] | fpu_out_sel_fromMul;
#if MULT_STAGES > 1
		for (i=2; i<(1+MULT_STAGES); i++) {
			fpu_out_sel_pipe[i] = fpu_out_sel_pipe[i-1];
		}
#endif

        wait();
    }
}

void instr_decoder::comb_method() {

    // Break the instruction word into the different fields
    sc_uint<32> instruction = instr->read();
    sc_uint<OPCODE_STA - OPCODE_END + 1> OPCODE = instruction.range(OPCODE_STA,
    OPCODE_END);
    sc_uint<IMM0_STA - IMM0_END + 1> IMM0 = instruction.range(IMM0_STA,
    IMM0_END);
    sc_uint<IMM1_STA - IMM1_END + 1> IMM1 = instruction.range(IMM1_STA,
    IMM1_END);
    sc_uint<DST_STA - DST_END + 1> DST = instruction.range(DST_STA, DST_END);
    sc_uint<SRC0_STA - SRC0_END + 1> SRC0 = instruction.range(SRC0_STA,
    SRC0_END);
    sc_uint<SRC1_STA - SRC1_END + 1> SRC1 = instruction.range(SRC1_STA,
    SRC1_END);
    sc_uint<SRC2_STA - SRC2_END + 1> SRC2 = instruction.range(SRC2_STA,
    SRC2_END);
    bool RELU = instruction.range(RELU_BIT, RELU_BIT);
    bool AAM = instruction.range(AAM_BIT, AAM_BIT);
    sc_uint<DST_N_STA - DST_N_END + 1> DST_N = instruction.range(DST_N_STA,
    DST_N_END);
    sc_uint<SRC0_N_STA - SRC0_N_END + 1> SRC0_N = instruction.range(SRC0_N_STA,
    SRC0_N_END);
    sc_uint<SRC1_N_STA - SRC1_N_END + 1> SRC1_N = instruction.range(SRC1_N_STA,
    SRC1_N_END);

    // ** DEFAULT VALUES FOR SIGNALS **

    // NOP counter behavior
    if (nop_cnt_reg == 0)
        nop_cnt_nxt = 0;
    else
        nop_cnt_nxt = nop_cnt_reg - 1;

    // Jump signals
    jmp_act_nxt = jmp_act_reg;
    jmp_cnt_nxt = jmp_cnt_reg;
    jump_en->write(false);
    jump_num->write(0);

    // GRF_A signals and pipelines
    grfa_rd_addr1_comb = 0;
    grfa_rd_addr1_toAddAftLoad = 0;
    grfa_rd_addr1_toAdd = 0;
    grfa_rd_addr1_toMAWAftLoad = 0;
    grfa_rd_addr2_comb = 0;
    grfa_rd_addr2_nxt = 0;
    grfa_wr_en_comb = 0;
    grfa_wr_en_fromMaAAftLoad = 0;
    grfa_wr_en_fromMaA = 0;
    grfa_wr_en_fromMulAftLoad = 0;
    grfa_wr_en_fromMul = 0;
    grfa_wr_en_fromAddAftLoad = 0;
    grfa_wr_en_fromAdd = 0;
    grfa_wr_en_fromLoad = 0;
    grfa_wr_addr_comb = 0;
    grfa_wr_addr_fromMaAAftLoad = 0;
    grfa_wr_addr_fromMaA = 0;
    grfa_wr_addr_fromMulAftLoad = 0;
    grfa_wr_addr_fromMul = 0;
    grfa_wr_addr_fromAddAftLoad = 0;
    grfa_wr_addr_fromAdd = 0;
    grfa_wr_addr_fromLoad = 0;
    grfa_wr_from_comb = 0;
    grfa_wr_from_fromMaAAftLoad = 0;
    grfa_wr_from_fromMaA = 0;
    grfa_wr_from_fromMulAftLoad = 0;
    grfa_wr_from_fromMul = 0;
    grfa_wr_from_fromAddAftLoad = 0;
    grfa_wr_from_fromAdd = 0;
    grfa_wr_from_fromLoad = 0;
    grfa_relu_en_comb = false;
    grfa_relu_en_nxt = false;

    // GRF_B signals and pipelines
    grfb_rd_addr1_comb = 0;
    grfb_rd_addr1_toAddAftLoad = 0;
    grfb_rd_addr1_toAdd = 0;
    grfb_rd_addr1_toMAWAftLoad = 0;
    grfb_rd_addr2_comb = 0;
    grfb_rd_addr2_nxt = 0;
    grfb_wr_en_comb = 0;
    grfb_wr_en_fromMaAAftLoad = 0;
    grfb_wr_en_fromMaA = 0;
    grfb_wr_en_fromMulAftLoad = 0;
    grfb_wr_en_fromMul = 0;
    grfb_wr_en_fromAddAftLoad = 0;
    grfb_wr_en_fromAdd = 0;
    grfb_wr_en_fromLoad = 0;
    grfb_wr_addr_comb = 0;
    grfb_wr_addr_fromMaAAftLoad = 0;
    grfb_wr_addr_fromMaA = 0;
    grfb_wr_addr_fromMulAftLoad = 0;
    grfb_wr_addr_fromMul = 0;
    grfb_wr_addr_fromAddAftLoad = 0;
    grfb_wr_addr_fromAdd = 0;
    grfb_wr_addr_fromLoad = 0;
    grfb_wr_from_comb = 0;
    grfb_wr_from_fromMaAAftLoad = 0;
    grfb_wr_from_fromMaA = 0;
    grfb_wr_from_fromMulAftLoad = 0;
    grfb_wr_from_fromMul = 0;
    grfb_wr_from_fromAddAftLoad = 0;
    grfb_wr_from_fromAdd = 0;
    grfb_wr_from_fromLoad = 0;
    grfb_relu_en_comb = false;
    grfb_relu_en_nxt = false;

    // SRF signals and pipelines

    srf_rd_addr_comb = 0;
    srf_rd_addr_toAddAftLoad = 0;
    srf_rd_addr_toAdd = 0;
    srf_rd_addr_toMAWAftLoad = 0;
    srf_rd_a_nm_comb = false;
    srf_rd_a_nm_toAddAftLoad = false;
    srf_rd_a_nm_toAdd = false;
    srf_rd_a_nm_toMAWAftLoad = false;
    srf_wr_en_comb = false;
    srf_wr_en_nxt = false;
    srf_wr_from_comb = 0;
    srf_wr_from_nxt = 0;
    srf_wr_addr_comb = 0;
    srf_wr_addr_nxt = 0;
    srf_wr_a_nm_comb = false;
    srf_wr_a_nm_nxt = false;

    // FPU signals and pipeline
    fpu_add_in1_sel_comb = 0;
    fpu_add_in1_sel_toAddAftLoad = 0;
    fpu_add_in1_sel_toAdd = 0;
    fpu_add_in1_sel_toMoAAftLoad = 0;
    fpu_add_in2_sel_comb = 0;
    fpu_add_in2_sel_toAddAftLoad = 0;
    fpu_add_in2_sel_toAdd = 0;
    fpu_add_in2_sel_toMoAAftLoad = 0;
    fpu_mult_in1_sel_comb = 0;
    fpu_mult_in1_sel_nxt = 0;
    fpu_mult_in2_sel_comb = 0;
    fpu_mult_in2_sel_nxt = 0;
    add_en_toAddAftLoad = false;
    add_en_toAdd = false;
    add_en_toMoAAftLoad = false;
    add_en_toMoA = false;
    mul_en_toMoAAftLoad = false;
    mul_en_toMoA = false;
    fpu_out_sel_fromMulAftLoad = false;
    fpu_out_sel_fromMul = false;

    // Ports
    pc_rst->write(false);
    count_en->write(false);
    jump_en->write(false);
    jump_num->write(0);
    crf_wr_en->write(false);
    crf_wr_addr->write(0);
    even_out_en->write(false);
    odd_out_en->write(false);

    // Intermediate variables
    uint8_t rd_from_mux = 0;
    sc_uint<ROW_BITS> row = row_addr->read(); // Separate row address in MSB and rest
    sc_uint<ROW_BITS - 1> rlsb = row.range(ROW_BITS - 2, 0);
    sc_uint<ROW_BITS - 1 + COL_BITS> rowcol_addr;
    rowcol_addr.range(ROW_BITS - 1 + COL_BITS - 1, COL_BITS) = rlsb;
    rowcol_addr.range(COL_BITS - 1, 0) = col_addr;
    sc_uint<RF_SEL_BITS> rf_sel = rowcol_addr.range(RF_SEL_BITS + RF_ADDR_BITS - 1, RF_ADDR_BITS);
    sc_uint<RF_ADDR_BITS> rf_addr = rowcol_addr.range(RF_ADDR_BITS - 1, 0);
#if CRF_BANK_ADDR
    // If not enough column bits to address CRF, using also bank bits
    sc_uint<BANK_BITS + RF_ADDR_BITS> crf_addr;
    crf_addr.range(RF_ADDR_BITS-1, 0) = rf_addr;
    crf_addr.range(BANK_BITS+RF_ADDR_BITS-1, RF_ADDR_BITS) = bank_addr;
#endif
    // TODO check this assumption (aam signals grfa and grfb idx vs signals src0-1 and src2-dst idx)
//	sc_uint<AAM_ADDR_BITS> aam_grfa_addr = rowcol_addr.range(AAM_ADDR_BITS-1,0);
//	sc_uint<AAM_ADDR_BITS> aam_grfb_addr = rowcol_addr.range(2*AAM_ADDR_BITS-1,AAM_ADDR_BITS);
    sc_uint<AAM_ADDR_BITS> aam_src_addr = rowcol_addr.range(AAM_ADDR_BITS - 1, 0);
    sc_uint<AAM_ADDR_BITS> aam_dst_addr = rowcol_addr.range(2 * AAM_ADDR_BITS - 1, AAM_ADDR_BITS);

    // Write to RFs when not in PIM mode (I don't see the need for read)
    // Signal width adaptation in the interface unit
    if (rf_access->read()) {
        switch (rf_sel) {
            case RF_CRF:
    //				if (rf_wr_nrd) {
                crf_wr_en->write(true);
#if CRF_BANK_ADDR
                crf_wr_addr->write(crf_addr.to_uint());
#else
                crf_wr_addr->write(rf_addr.to_uint());
#endif
//				}
            break;
            case RF_SRF_M:
    //				if (!rf_wr_nrd) {
    //					srf_rd_addr_comb = rf_addr.to_uint();
    //					srf_rd_a_nm_comb = false;
    //				} else {
                srf_wr_en_comb = true;
                srf_wr_addr_comb = rf_addr.to_uint();
                srf_wr_a_nm_comb = false;
                srf_wr_from_comb = MUX_EXT;
    //				}
            break;
            case RF_SRF_A:
    //				if (!rf_wr_nrd) {
    //					srf_rd_addr_comb = rf_addr.to_uint();
    //					srf_rd_a_nm_comb = true;
    //				} else {
                srf_wr_en_comb = true;
                srf_wr_addr_comb = rf_addr.to_uint();
                srf_wr_a_nm_comb = true;
                srf_wr_from_comb = MUX_EXT;
    //				}
            break;
            case RF_GRF_A:
    //				if (!rf_wr_nrd) {
    //					grfa_rd_addr1_comb = rf_addr.to_uint();
    //				} else {
                grfa_wr_en_comb = true;
                grfa_wr_addr_comb = rf_addr.to_uint();
                grfa_wr_from_comb = MUX_EXT;
    //				}
            break;
            case RF_GRF_B:
    //				if (!rf_wr_nrd) {
    //					grfb_rd_addr1_comb = rf_addr.to_uint();
    //				} else {
                grfb_wr_en_comb = true;
                grfb_wr_addr_comb = rf_addr.to_uint();
                grfb_wr_from_comb = MUX_EXT;
    //				}
            break;
        }
    }

    // Decode new instruction when signaled, if there is no current NOP
    if (decode_en->read() && !nop_cnt_reg) {
        // Signal a increase in the PC for the next cycle
        count_en->write(true);

        // Decode the instruction and generate the control signals
        switch (OPCODE) {

            // IMM0 cycles NOP
            case OP_NOP:
                nop_cnt_nxt = IMM0 - 1;	// The FETCH already counts as a NOP cycle
            break;

            // JUMP with zero-cycles execution stage and only one register for number of loops
            case OP_JUMP:
                if (!jmp_act_reg) {			// First sight of this jump
                    count_en->write(false);
                    jmp_act_nxt = true;
                    jmp_cnt_nxt = IMM1 - 1;
                    jump_en->write(true);
                    jump_num->write(IMM0);
                } else if (jmp_cnt_reg) {	// Still more jumps to make
                    count_en->write(false);
                    jmp_act_nxt = true;
                    jmp_cnt_nxt = jmp_cnt_reg - 1;
                    jump_en->write(true);
                    jump_num->write(IMM0);
                } else {					// Last jump made
                    jmp_act_nxt = false;
                }
            break;

            // Reset PC and control variables
            case OP_EXIT:
                // Only reset PC, we allow for further instructions if triggered by DRAM commands
                pc_rst->write(true);
            break;

            // DST(DST_N) <- SRC0(SRC0_N), with optional ReLU
            case OP_MOV:
                if (SRC0 == OPC_EVEN_BANK) {	// Loading from even bank
                    rd_from_mux = MUX_EVEN_BANK;
                    switch (DST) {	// Only even_bank to GRF_A allowed
                        case OPC_GRF_A:
                            grfa_wr_en_fromLoad = true;
                            grfa_wr_from_fromLoad = rd_from_mux;
                            grfa_wr_addr_fromLoad = DST_N.to_uint();
                            if (RELU)
                                grfa_relu_en_nxt = true;
                        break;
                        default:
                        break;
                    }
                } else if (SRC0 == OPC_ODD_BANK) {
                    rd_from_mux = MUX_ODD_BANK;
                    switch (DST) {	// Only odd_bank to GRF_B allowed
                        case OPC_GRF_B:
                            grfb_wr_en_fromLoad = true;
                            grfb_wr_from_fromLoad = rd_from_mux;
                            grfb_wr_addr_fromLoad = DST_N.to_uint();
                            if (RELU)
                                grfb_relu_en_nxt = true;
                        break;
                        default:
                        break;
                    }
                } else {	// Reading from RFs
                    switch (SRC0) {
                        case OPC_GRF_A:
                            grfa_rd_addr1_comb = SRC0_N.to_uint();
                            rd_from_mux = MUX_GRF_A;
                        break;
                        case OPC_GRF_B:
                            grfb_rd_addr1_comb = SRC0_N.to_uint();
                            rd_from_mux = MUX_GRF_B;
                        break;
                        case OPC_SRF_M:
                            srf_rd_addr_comb = SRC0_N.to_uint();
                            srf_rd_a_nm_comb = false;
                            rd_from_mux = MUX_SRF;
                        break;
                        case OPC_SRF_A:
                            srf_rd_addr_comb = SRC0_N.to_uint();
                            srf_rd_a_nm_comb = true;
                            rd_from_mux = MUX_SRF;
                        break;
                        default:
                        break;
                    }
                    switch (DST) {	// Store in intended RF/BANK
                        case OPC_GRF_A:
                            grfa_wr_en_comb = true;
                            grfa_wr_from_comb = rd_from_mux;
                            grfa_wr_addr_comb = DST_N.to_uint();
                            if (RELU)
                                grfa_relu_en_comb = true;
                        break;
                        case OPC_GRF_B:
                            grfb_wr_en_comb = true;
                            grfb_wr_from_comb = rd_from_mux;
                            grfb_wr_addr_comb = DST_N.to_uint();
                            if (RELU)
                                grfb_relu_en_comb = true;
                        break;
                        case OPC_SRF_M:
                            srf_wr_en_comb = true;
                            srf_wr_from_comb = rd_from_mux;
                            srf_wr_addr_comb = DST_N.to_uint();
                            srf_wr_a_nm_comb = false;
                        break;
                        case OPC_SRF_A:
                            srf_wr_en_comb = true;
                            srf_wr_from_comb = rd_from_mux;
                            srf_wr_addr_comb = DST_N.to_uint();
                            srf_wr_a_nm_comb = true;
                        break;
                        case OPC_EVEN_BANK:
                            if (rd_from_mux == MUX_GRF_A)	// Only GRF_A to even_bank
                                even_out_en->write(true);
                        break;
                        case OPC_ODD_BANK:
                            if (rd_from_mux == MUX_GRF_B)	// Only GRF_B to odd_bank
                                odd_out_en->write(true);
                        break;
                        default:
                        break;
                    }
                }
            break;

            // DST(DST_N) <- SRC0(SRC0_N) ?
            case OP_FILL:
                // TODO left unimplemented, then we'll see
            break;

            // DST(DST_N) = SRC0(SRC0_N) + SRC1(SRC1_N)
            case OP_ADD:
                if (SRC0 == OPC_EVEN_BANK || SRC0 == OPC_ODD_BANK
                        || SRC1 == OPC_EVEN_BANK || SRC1 == OPC_ODD_BANK) {	// Loading from bank
                    switch (SRC0) {	// Select first input to adder
                        case OPC_GRF_A:
        //							grfa_rd_addr1_toMAWAftLoad = SRC0_N.to_uint();
                            grfa_rd_addr1_toMAWAftLoad =
                                    AAM ? aam_src_addr.to_uint() : SRC0_N.to_uint();
                            fpu_add_in1_sel_toMoAAftLoad = A_GRF_A1;
                        break;
                        case OPC_GRF_B:
        //							grfb_rd_addr1_toMAWAftLoad = SRC0_N.to_uint();
                            grfb_rd_addr1_toMAWAftLoad =
                                    AAM ? aam_src_addr.to_uint() : SRC0_N.to_uint();
                            fpu_add_in1_sel_toMoAAftLoad = A_GRF_B1;
                        break;
                        case OPC_SRF_A:
        //							srf_rd_addr_toMAWAftLoad = SRC0_N.to_uint();
                            srf_rd_addr_toMAWAftLoad =
                                    AAM ? aam_src_addr.to_uint() : SRC0_N.to_uint();
                            srf_rd_a_nm_toMAWAftLoad = true;
                            fpu_add_in1_sel_toMoAAftLoad = A_SRF;
                        break;
                        case OPC_EVEN_BANK:
                            fpu_add_in1_sel_toMoAAftLoad = A_EVEN_BANK;
                        break;
                        case OPC_ODD_BANK:
                            fpu_add_in1_sel_toMoAAftLoad = A_ODD_BANK;
                        break;
                        default:
                        break;
                    }
                    switch (SRC1) {	// Select second input to adder
                        case OPC_GRF_A:
        //							grfa_rd_addr2_nxt = SRC1_N.to_uint();
                            grfa_rd_addr2_nxt =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            fpu_add_in2_sel_toMoAAftLoad = A_GRF_A2;
                        break;
                        case OPC_GRF_B:
        //							grfb_rd_addr2_nxt = SRC1_N.to_uint();
                            grfb_rd_addr2_nxt =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            fpu_add_in2_sel_toMoAAftLoad = A_GRF_B2;
                        break;
                        case OPC_SRF_A:
        //							srf_rd_addr_toMAWAftLoad = SRC1_N.to_uint();
                            srf_rd_addr_toMAWAftLoad =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            srf_rd_a_nm_toMAWAftLoad = true;
                            fpu_add_in2_sel_toMoAAftLoad = A_SRF;
                        break;
                        case OPC_EVEN_BANK:
                            fpu_add_in2_sel_toMoAAftLoad = A_EVEN_BANK;
                        break;
                        case OPC_ODD_BANK:
                            fpu_add_in2_sel_toMoAAftLoad = A_ODD_BANK;
                        break;
                        default:
                        break;
                    }

                    add_en_toMoAAftLoad = true;	// Enable addition for 5 cycles after next one

                    switch (DST) {// Enable writing of addition result to destination
                        case OPC_GRF_A:
                            grfa_wr_en_fromAddAftLoad = true;
                            grfa_wr_from_fromAddAftLoad = MUX_FPU;
        //							grfa_wr_addr_fromAddAftLoad = DST_N.to_uint();
                            grfa_wr_addr_fromAddAftLoad =
                                    AAM ? aam_dst_addr.to_uint() : DST_N.to_uint();
                        break;
                        case OPC_GRF_B:
                            grfb_wr_en_fromAddAftLoad = true;
                            grfb_wr_from_fromAddAftLoad = MUX_FPU;
        //							grfb_wr_addr_fromAddAftLoad = DST_N.to_uint();
                            grfb_wr_addr_fromAddAftLoad =
                                    AAM ? aam_dst_addr.to_uint() : DST_N.to_uint();
                        break;
                    }
                } else {	// Loading from RFs only
                    switch (SRC0) {	// Select first input to adder
                        case OPC_GRF_A:
        //							grfa_rd_addr1_comb = SRC0_N.to_uint();
                            grfa_rd_addr1_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC0_N.to_uint();
                            fpu_add_in1_sel_comb = A_GRF_A1;
                        break;
                        case OPC_GRF_B:
        //							grfb_rd_addr1_comb = SRC0_N.to_uint();
                            grfb_rd_addr1_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC0_N.to_uint();
                            fpu_add_in1_sel_comb = A_GRF_B1;
                        break;
                        case OPC_SRF_A:
        //							srf_rd_addr_comb = SRC0_N.to_uint();
                            srf_rd_addr_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC0_N.to_uint();
                            srf_rd_a_nm_comb = true;
                            fpu_add_in1_sel_comb = A_SRF;
                        break;
                        default:
                        break;
                    }
                    switch (SRC1) {	// Select second input to adder
                        case OPC_GRF_A:
        //							grfa_rd_addr2_comb = SRC1_N.to_uint();
                            grfa_rd_addr2_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            fpu_add_in2_sel_comb = A_GRF_A2;
                        break;
                        case OPC_GRF_B:
        //							grfb_rd_addr2_comb = SRC1_N.to_uint();
                            grfb_rd_addr2_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            fpu_add_in2_sel_comb = A_GRF_B2;
                        break;
                        case OPC_SRF_A:
        //							srf_rd_addr_comb = SRC1_N.to_uint();
                            srf_rd_addr_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            srf_rd_a_nm_comb = true;
                            fpu_add_in2_sel_comb = A_SRF;
                        break;
                        default:
                        break;
                    }

                    add_en_toMoA = true; // Enable addition for the next 5 cycles

                    switch (DST) { // Enable writing of addition result to destination
                        case OPC_GRF_A:
                            grfa_wr_en_fromAdd = true;
                            grfa_wr_from_fromAdd = MUX_FPU;
        //							grfa_wr_addr_fromAdd = DST_N.to_uint();
                            grfa_wr_addr_fromAdd =
                                    AAM ? aam_dst_addr.to_uint() : DST_N.to_uint();
                        break;
                        case OPC_GRF_B:
                            grfb_wr_en_fromAdd = true;
                            grfb_wr_from_fromAdd = MUX_FPU;
        //							grfb_wr_addr_fromAdd = DST_N.to_uint();
                            grfb_wr_addr_fromAdd =
                                    AAM ? aam_dst_addr.to_uint() : DST_N.to_uint();
                        break;
                    }
                }
            break;

            // DST(DST_N) = SRC0(SRC0_N) * SRC1(SRC1_N)
            case OP_MUL:
                if (SRC0 == OPC_EVEN_BANK || SRC0 == OPC_ODD_BANK
                        || SRC1 == OPC_EVEN_BANK || SRC1 == OPC_ODD_BANK) {	// Loading from bank
                    switch (SRC0) {	// Select first input to multiplier
                        case OPC_GRF_A:
        //							grfa_rd_addr1_toMAWAftLoad = SRC0_N.to_uint();
                            grfa_rd_addr1_toMAWAftLoad =
                                    AAM ? aam_src_addr.to_uint() : SRC0_N.to_uint();
                            fpu_mult_in1_sel_nxt = M1_GRF_A1;
                        break;
                        case OPC_GRF_B:
        //							grfb_rd_addr1_toMAWAftLoad = SRC0_N.to_uint();
                            grfb_rd_addr1_toMAWAftLoad =
                                    AAM ? aam_src_addr.to_uint() : SRC0_N.to_uint();
                            fpu_mult_in1_sel_nxt = M1_GRF_B1;
                        break;
                        case OPC_EVEN_BANK:
                            fpu_mult_in1_sel_nxt = M1_EVEN_BANK;
                        break;
                        case OPC_ODD_BANK:
                            fpu_mult_in1_sel_nxt = M1_ODD_BANK;
                        break;
                        default:
                        break;
                    }
                    switch (SRC1) {	// Select second input to multiplier
                        case OPC_GRF_A:
        //							grfa_rd_addr2_nxt = SRC1_N.to_uint();
                            grfa_rd_addr2_nxt =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            fpu_mult_in2_sel_nxt = M2_GRF_A2;
                        break;
                        case OPC_GRF_B:
        //							grfb_rd_addr2_nxt = SRC1_N.to_uint();
                            grfb_rd_addr2_nxt =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            fpu_mult_in2_sel_nxt = M2_GRF_B2;
                        break;
                        case OPC_SRF_M:
        //							srf_rd_addr_toMAWAftLoad = SRC1_N.to_uint();
                            srf_rd_addr_toMAWAftLoad =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            srf_rd_a_nm_toMAWAftLoad = false;
                            fpu_mult_in2_sel_nxt = M2_SRF;
                        break;
                        case OPC_EVEN_BANK:
                            fpu_mult_in2_sel_nxt = M2_EVEN_BANK;
                        break;
                        case OPC_ODD_BANK:
                            fpu_mult_in2_sel_nxt = M2_ODD_BANK;
                        break;
                        default:
                        break;
                    }

                    mul_en_toMoAAftLoad = true; // Enable multiplication for 5 cycles after next one
                    fpu_out_sel_fromMulAftLoad = true; // Select multiplier output when necessary

                    switch (DST) { // Enable writing of multiplication result to destination
                        case OPC_GRF_A:
                            grfa_wr_en_fromMulAftLoad = true;
                            grfa_wr_from_fromMulAftLoad = MUX_FPU;
        //							grfa_wr_addr_fromMulAftLoad = DST_N.to_uint();
                            grfa_wr_addr_fromMulAftLoad =
                                    AAM ? aam_dst_addr.to_uint() : DST_N.to_uint();
                        break;
                        case OPC_GRF_B:
                            grfb_wr_en_fromMulAftLoad = true;
                            grfb_wr_from_fromMulAftLoad = MUX_FPU;
        //							grfb_wr_addr_fromMulAftLoad = DST_N.to_uint();
                            grfb_wr_addr_fromMulAftLoad =
                                    AAM ? aam_dst_addr.to_uint() : DST_N.to_uint();
                        break;
                    }
                } else {	// Loading from RFs only
                    switch (SRC0) {	// Select first input to multiplier
                        case OPC_GRF_A:
        //							grfa_rd_addr1_comb = SRC0_N.to_uint();
                            grfa_rd_addr1_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC0_N.to_uint();
                            fpu_mult_in1_sel_comb = M1_GRF_A1;
                        break;
                        case OPC_GRF_B:
        //							grfb_rd_addr1_comb = SRC0_N.to_uint();
                            grfb_rd_addr1_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC0_N.to_uint();
                            fpu_mult_in1_sel_comb = M1_GRF_B1;
                        break;
                        default:
                        break;
                    }
                    switch (SRC1) {	// Select second input to multiplier
                        case OPC_GRF_A:
        //							grfa_rd_addr2_comb = SRC1_N.to_uint();
                            grfa_rd_addr2_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            fpu_mult_in2_sel_comb = M2_GRF_A2;
                        break;
                        case OPC_GRF_B:
        //							grfb_rd_addr2_comb = SRC1_N.to_uint();
                            grfb_rd_addr2_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            fpu_mult_in2_sel_comb = M2_GRF_B2;
                        break;
                        case OPC_SRF_M:
        //							srf_rd_addr_comb = SRC1_N.to_uint();
                            srf_rd_addr_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            srf_rd_a_nm_comb = false;
                            fpu_mult_in2_sel_comb = M2_SRF;
                        break;
                        default:
                        break;
                    }

                    mul_en_toMoA = true; // Enable multiplication for the next 5 cycles
                    fpu_out_sel_fromMul = true;	// Select multiplier output when necessary

                    switch (DST) {// Enable writing of multiplication result to destination
                        case OPC_GRF_A:
                            grfa_wr_en_fromMul = true;
                            grfa_wr_from_fromMul = MUX_FPU;
        //							grfa_wr_addr_fromMul = DST_N.to_uint();
                            grfa_wr_addr_fromMul =
                                    AAM ? aam_dst_addr.to_uint() : DST_N.to_uint();
                        break;
                        case OPC_GRF_B:
                            grfb_wr_en_fromMul = true;
                            grfb_wr_from_fromMul = MUX_FPU;
        //							grfb_wr_addr_fromMul = DST_N.to_uint();
                            grfb_wr_addr_fromMul =
                                    AAM ? aam_dst_addr.to_uint() : DST_N.to_uint();
                        break;
                    }
                }
            break;

            // DST(DST_N) = SRC0(SRC0_N) * SRC1(SRC1_N) + SRC2(SRC1_N)
            case OP_MAD:
                if (SRC0 == OPC_EVEN_BANK || SRC0 == OPC_ODD_BANK
                        || SRC1 == OPC_EVEN_BANK || SRC1 == OPC_ODD_BANK) {	// Loading from bank
                    switch (SRC0) {	// Select first input to multiplier
                        case OPC_GRF_A:
        //							grfa_rd_addr1_toMAWAftLoad = SRC0_N.to_uint();
                            grfa_rd_addr1_toMAWAftLoad =
                                    AAM ? aam_src_addr.to_uint() : SRC0_N.to_uint();
                            fpu_mult_in1_sel_nxt = M1_GRF_A1;
                        break;
                        case OPC_GRF_B:
        //							grfb_rd_addr1_toMAWAftLoad = SRC0_N.to_uint();
                            grfb_rd_addr1_toMAWAftLoad =
                                    AAM ? aam_src_addr.to_uint() : SRC0_N.to_uint();
                            fpu_mult_in1_sel_nxt = M1_GRF_B1;
                        break;
                        case OPC_EVEN_BANK:
                            fpu_mult_in1_sel_nxt = M1_EVEN_BANK;
                        break;
                        case OPC_ODD_BANK:
                            fpu_mult_in1_sel_nxt = M1_ODD_BANK;
                        break;
                        default:
                        break;
                    }
                    switch (SRC1) {	// Select second input to multiplier
                        case OPC_GRF_A:
        //							grfa_rd_addr2_nxt = SRC1_N.to_uint();
                            grfa_rd_addr2_nxt =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            fpu_mult_in2_sel_nxt = M2_GRF_A2;
                        break;
                        case OPC_GRF_B:
        //							grfb_rd_addr2_nxt = SRC1_N.to_uint();
                            grfa_rd_addr2_nxt =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            fpu_mult_in2_sel_nxt = M2_GRF_B2;
                        break;
                        case OPC_SRF_M:
        //							srf_rd_addr_toMAWAftLoad = SRC1_N.to_uint();
                            srf_rd_addr_toMAWAftLoad =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            srf_rd_a_nm_toMAWAftLoad = false;
                            fpu_mult_in2_sel_nxt = M2_SRF;
                        break;
                        case OPC_EVEN_BANK:
                            fpu_mult_in2_sel_nxt = M2_EVEN_BANK;
                        break;
                        case OPC_ODD_BANK:
                            fpu_mult_in2_sel_nxt = M2_ODD_BANK;
                        break;
                        default:
                        break;
                    }
                    switch (SRC2) {	// Select first input to adder
                        case OPC_GRF_A:
        //							grfa_rd_addr1_toAddAftLoad = SRC1_N.to_uint();
                            grfa_rd_addr1_toAddAftLoad =
                                    AAM ? aam_dst_addr.to_uint() : SRC1_N.to_uint();
                            fpu_add_in1_sel_toAddAftLoad = A_GRF_A1;
                        break;
                        case OPC_GRF_B:
        //							grfb_rd_addr1_toAddAftLoad = SRC1_N.to_uint();
                            grfb_rd_addr1_toAddAftLoad =
                                    AAM ? aam_dst_addr.to_uint() : SRC1_N.to_uint();
                            fpu_add_in1_sel_toAddAftLoad = A_GRF_B1;
                        break;
                        case OPC_SRF_A:
        //							srf_rd_addr_toAddAftLoad = SRC1_N.to_uint();
                            srf_rd_addr_toAddAftLoad =
                                    AAM ? aam_dst_addr.to_uint() : SRC1_N.to_uint();
                            srf_rd_a_nm_toAddAftLoad = true;
                            fpu_add_in1_sel_toAddAftLoad = A_SRF;
                        break;
                        case OPC_EVEN_BANK:
                            fpu_add_in1_sel_toAddAftLoad = A_EVEN_BANK;
                        break;
                        case OPC_ODD_BANK:
                            fpu_add_in1_sel_toAddAftLoad = A_ODD_BANK;
                        break;
                        default:
                        break;
                    }
                    fpu_add_in2_sel_toAddAftLoad = A_MULT_OUT;// Select second input as mult result when necessary

                    mul_en_toMoAAftLoad = true; // Enable multiplication for 5 cycles after next one
                    add_en_toAddAftLoad = true;	// Enable addition for the 5 cycles after multiplication

                    switch (DST) {	// Enable writing of MAD result to destination
                        case OPC_GRF_A:
                            grfa_wr_en_fromMaAAftLoad = true;
                            grfa_wr_from_fromMaAAftLoad = MUX_FPU;
        //							grfa_wr_addr_fromMaAAftLoad = DST_N.to_uint();
                            grfa_wr_addr_fromMaAAftLoad =
                                    AAM ? aam_dst_addr.to_uint() : DST_N.to_uint();
                        break;
                        case OPC_GRF_B:
                            grfb_wr_en_fromMaAAftLoad = true;
                            grfb_wr_from_fromMaAAftLoad = MUX_FPU;
        //							grfb_wr_addr_fromMaAAftLoad = DST_N.to_uint();
                            grfb_wr_addr_fromMaAAftLoad =
                                    AAM ? aam_dst_addr.to_uint() : DST_N.to_uint();
                        break;
                    }
                } else {	// Loading from RFs only
                    switch (SRC0) {	// Select first input to multiplier
                        case OPC_GRF_A:
        //							grfa_rd_addr1_comb = SRC0_N.to_uint();
                            grfa_rd_addr1_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC0_N.to_uint();
                            fpu_mult_in1_sel_comb = M1_GRF_A1;
                        break;
                        case OPC_GRF_B:
        //							grfb_rd_addr1_comb = SRC0_N.to_uint();
                            grfb_rd_addr1_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC0_N.to_uint();
                            fpu_mult_in1_sel_comb = M1_GRF_B1;
                        break;
                        default:
                        break;
                    }
                    switch (SRC1) {	// Select second input to multiplier
                        case OPC_GRF_A:
        //							grfa_rd_addr2_comb = SRC1_N.to_uint();
                            grfa_rd_addr2_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            fpu_mult_in2_sel_comb = M2_GRF_A2;
                        break;
                        case OPC_GRF_B:
        //							grfb_rd_addr2_comb = SRC1_N.to_uint();
                            grfb_rd_addr2_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            fpu_mult_in2_sel_comb = M2_GRF_B2;
                        break;
                        case OPC_SRF_M:
        //							srf_rd_addr_comb = SRC1_N.to_uint();
                            srf_rd_addr_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            srf_rd_a_nm_comb = false;
                            fpu_mult_in2_sel_comb = M2_SRF;
                        break;
                        default:
                        break;
                    }
                    switch (SRC2) {	// Select first input to adder
                        case OPC_GRF_A:
        //							grfa_rd_addr1_toAdd = SRC1_N.to_uint();
                            grfa_rd_addr1_toAdd =
                                    AAM ? aam_dst_addr.to_uint() : SRC1_N.to_uint();
                            fpu_add_in1_sel_toAdd = A_GRF_A1;
                        break;
                        case OPC_GRF_B:
        //							grfb_rd_addr1_toAdd = SRC1_N.to_uint();
                            grfb_rd_addr1_toAdd =
                                    AAM ? aam_dst_addr.to_uint() : SRC1_N.to_uint();
                            fpu_add_in1_sel_toAdd = A_GRF_B1;
                        break;
                        case OPC_SRF_A:
        //							srf_rd_addr_toAdd = SRC1_N.to_uint();
                            srf_rd_addr_toAdd =
                                    AAM ? aam_dst_addr.to_uint() : SRC1_N.to_uint();
                            srf_rd_a_nm_toAdd = true;
                            fpu_add_in1_sel_toAdd = A_SRF;
                        break;
                        case OPC_EVEN_BANK:
                            fpu_add_in1_sel_toAdd = A_EVEN_BANK;
                        break;
                        case OPC_ODD_BANK:
                            fpu_add_in1_sel_toAdd = A_ODD_BANK;
                        break;
                        default:
                        break;
                    }
                    fpu_add_in2_sel_toAdd = A_MULT_OUT;	// Select second input as mult result when necessary

                    mul_en_toMoA = true; // Enable multiplication for the next 5 cycles
                    add_en_toAdd = true; // Enable addition for the 5 cycles after multiplication

                    switch (DST) {	// Enable writing of MAD result to destination
                        case OPC_GRF_A:
                            grfa_wr_en_fromMaA = true;
                            grfa_wr_from_fromMaA = MUX_FPU;
        //							grfa_wr_addr_fromMaA = DST_N.to_uint();
                            grfa_wr_addr_fromMaA =
                                    AAM ? aam_dst_addr.to_uint() : DST_N.to_uint();
                        break;
                        case OPC_GRF_B:
                            grfb_wr_en_fromMaA = true;
                            grfb_wr_from_fromMaA = MUX_FPU;
        //							grfb_wr_addr_fromMaA = DST_N.to_uint();
                            grfb_wr_addr_fromMaA =
                                    AAM ? aam_dst_addr.to_uint() : DST_N.to_uint();
                        break;
                    }
                }
            break;

            // DST(DST_N) = SRC0(SRC0_N) * SRC1(SRC1_N) + DST(DST_N)
            case OP_MAC:
                if (SRC0 == OPC_EVEN_BANK || SRC0 == OPC_ODD_BANK
                        || SRC1 == OPC_EVEN_BANK || SRC1 == OPC_ODD_BANK) {	// Loading from bank
                    switch (SRC0) {	// Select first input to multiplier
                        case OPC_GRF_A:
        //							grfa_rd_addr1_toMAWAftLoad = AAM ? aam_grfa_addr.to_uint() : SRC0_N.to_uint();
                            grfa_rd_addr1_toMAWAftLoad =
                                    AAM ? aam_src_addr.to_uint() : SRC0_N.to_uint();
                            fpu_mult_in1_sel_nxt = M1_GRF_A1;
                        break;
                        case OPC_GRF_B:
        //							grfb_rd_addr1_toMAWAftLoad = AAM ? aam_grfb_addr.to_uint() : SRC0_N.to_uint();
                            grfb_rd_addr1_toMAWAftLoad =
                                    AAM ? aam_src_addr.to_uint() : SRC0_N.to_uint();
                            fpu_mult_in1_sel_nxt = M1_GRF_B1;
                        break;
                        case OPC_EVEN_BANK:
                            fpu_mult_in1_sel_nxt = M1_EVEN_BANK;
                        break;
                        case OPC_ODD_BANK:
                            fpu_mult_in1_sel_nxt = M1_ODD_BANK;
                        break;
                        default:
                        break;
                    }
                    switch (SRC1) {	// Select second input to multiplier
                        case OPC_GRF_A:
        //							grfa_rd_addr2_nxt = AAM ? aam_grfa_addr.to_uint() : SRC1_N.to_uint();
                            grfa_rd_addr2_nxt =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            fpu_mult_in2_sel_nxt = M2_GRF_A2;
                        break;
                        case OPC_GRF_B:
        //							grfb_rd_addr2_nxt = AAM ? aam_grfb_addr.to_uint() : SRC1_N.to_uint();
                            grfb_rd_addr2_nxt =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            fpu_mult_in2_sel_nxt = M2_GRF_B2;
                        break;
                        case OPC_SRF_M:
        //							srf_rd_addr_toMAWAftLoad = SRC1_N.to_uint();
                            srf_rd_addr_toMAWAftLoad =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            srf_rd_a_nm_toMAWAftLoad = false;
                            fpu_mult_in2_sel_nxt = M2_SRF;
                        break;
                        case OPC_EVEN_BANK:
                            fpu_mult_in2_sel_nxt = M2_EVEN_BANK;
                        break;
                        case OPC_ODD_BANK:
                            fpu_mult_in2_sel_nxt = M2_ODD_BANK;
                        break;
                        default:
                        break;
                    }
                    switch (DST) {	// Select first input to adder
                        case OPC_GRF_B:
        //							grfb_rd_addr1_toAddAftLoad = AAM ? aam_grfb_addr.to_uint() : DST_N.to_uint();
                            grfb_rd_addr1_toAddAftLoad =
                                    AAM ? aam_dst_addr.to_uint() : DST_N.to_uint();
                            fpu_add_in1_sel_toAddAftLoad = A_GRF_B1;
                        break;
                        default:
                        break;
                    }
                    fpu_add_in2_sel_toAddAftLoad = A_MULT_OUT;// Select second input as mult result when necessary

                    mul_en_toMoAAftLoad = true; // Enable multiplication for 5 cycles after next one
                    add_en_toAddAftLoad = true;	// Enable addition for the 5 cycles after multiplication

                    switch (DST) {	// Enable writing of MAC result to destination
                        case OPC_GRF_B:
                            grfb_wr_en_fromMaAAftLoad = true;
                            grfb_wr_from_fromMaAAftLoad = MUX_FPU;
        //							grfb_wr_addr_fromMaAAftLoad = AAM ? aam_grfb_addr.to_uint() : DST_N.to_uint();
                            grfb_wr_addr_fromMaAAftLoad =
                                    AAM ? aam_dst_addr.to_uint() : DST_N.to_uint();
                        break;
                    }
                } else {	// Loading from RFs only
                    switch (SRC0) {	// Select first input to multiplier
                        case OPC_GRF_A:
        //							grfa_rd_addr1_comb = SRC0_N.to_uint();
                            grfa_rd_addr1_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC0_N.to_uint();
                            fpu_mult_in1_sel_comb = M1_GRF_A1;
                        break;
                        case OPC_GRF_B:
        //							grfb_rd_addr1_comb = SRC0_N.to_uint();
                            grfb_rd_addr1_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC0_N.to_uint();
                            fpu_mult_in1_sel_comb = M1_GRF_B1;
                        break;
                        default:
                        break;
                    }
                    switch (SRC1) {	// Select second input to multiplier
                        case OPC_GRF_A:
        //							grfa_rd_addr2_comb = SRC1_N.to_uint();
                            grfa_rd_addr2_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            fpu_mult_in2_sel_comb = M2_GRF_A2;
                        break;
                        case OPC_GRF_B:
        //							grfb_rd_addr2_comb = SRC1_N.to_uint();
                            grfb_rd_addr2_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            fpu_mult_in2_sel_comb = M2_GRF_B2;
                        break;
                        case OPC_SRF_M:
        //							srf_rd_addr_comb = SRC1_N.to_uint();
                            srf_rd_addr_comb =
                                    AAM ? aam_src_addr.to_uint() : SRC1_N.to_uint();
                            srf_rd_a_nm_comb = false;
                            fpu_mult_in2_sel_comb = M2_SRF;
                        break;
                        default:
                        break;
                    }
                    switch (DST) {	// Select first input to adder
                        case OPC_GRF_B:
        //							grfb_rd_addr1_toAdd = DST_N.to_uint();
                            grfb_rd_addr1_toAdd =
                                    AAM ? aam_dst_addr.to_uint() : DST_N.to_uint();
                            fpu_add_in1_sel_toAdd = A_GRF_B1;
                        break;
                        default:
                        break;
                    }
                    fpu_add_in2_sel_toAdd = A_MULT_OUT;	// Select second input as mult result when necessary

                    mul_en_toMoA = true; // Enable multiplication for the next 5 cycles
                    add_en_toAdd = true; // Enable addition for the 5 cycles after multiplication

                    switch (DST) {	// Enable writing of MAC result to destination
                        case OPC_GRF_B:
                            grfb_wr_en_fromMaA = true;
                            grfb_wr_from_fromMaA = MUX_FPU;
        //							grfb_wr_addr_fromMaA = DST_N.to_uint();
                            grfb_wr_addr_fromMaA =
                                    AAM ? aam_dst_addr.to_uint() : DST_N.to_uint();
                        break;
                    }
                }
            break;
        }
    }

}

void instr_decoder::out_method() {

    uint i = 0;
    add_en_or = false;
    mul_en_or = false;

    // ** Resolve all outputs not yet written **

    // SRF control
    srf_rd_addr->write(srf_rd_addr_comb | srf_rd_addr_pipe[MULT_STAGES]);
    srf_rd_a_nm->write(srf_rd_a_nm_comb | srf_rd_a_nm_pipe[MULT_STAGES]);
    srf_wr_en->write(srf_wr_en_comb | srf_wr_en_reg);
    srf_wr_addr->write(srf_wr_addr_comb | srf_wr_addr_reg);
    srf_wr_a_nm->write(srf_wr_a_nm_comb | srf_wr_a_nm_reg);
    srf_wr_from->write(srf_wr_from_comb | srf_wr_from_reg);

    // GRF_A control
    grfa_rd_addr1->write(grfa_rd_addr1_comb | grfa_rd_addr1_pipe[MULT_STAGES]);
    grfa_rd_addr2->write(grfa_rd_addr2_comb | grfa_rd_addr2_reg);
    grfa_wr_en->write(
            grfa_wr_en_comb | grfa_wr_en_pipe[MULT_STAGES + ADD_STAGES]);
    grfa_relu_en->write(grfa_relu_en_comb | grfa_relu_en_reg);
    grfa_wr_addr->write(
            grfa_wr_addr_comb | grfa_wr_addr_pipe[MULT_STAGES + ADD_STAGES]);
    grfa_wr_from->write(
            grfa_wr_from_comb | grfa_wr_from_pipe[MULT_STAGES + ADD_STAGES]);

    // GRF_B control
    grfb_rd_addr1->write(grfb_rd_addr1_comb | grfb_rd_addr1_pipe[MULT_STAGES]);
    grfb_rd_addr2->write(grfb_rd_addr2_comb | grfb_rd_addr2_reg);
    grfb_wr_en->write(
            grfb_wr_en_comb | grfb_wr_en_pipe[MULT_STAGES + ADD_STAGES]);
    grfb_relu_en->write(grfb_relu_en_comb | grfb_relu_en_reg);
    grfb_wr_addr->write(
            grfb_wr_addr_comb | grfb_wr_addr_pipe[MULT_STAGES + ADD_STAGES]);
    grfb_wr_from->write(
            grfb_wr_from_comb | grfb_wr_from_pipe[MULT_STAGES + ADD_STAGES]);

    // FPU control
    for (i = 0; i < MULT_STAGES; i++) {
        mul_en_or |= mul_en_pipe[i];
    }
    fpu_mult_en->write(mul_en_toMoA | mul_en_or);
    for (i = MULT_STAGES; i < MULT_STAGES + ADD_STAGES; i++) {
        add_en_or |= add_en_pipe[i];
    }
    fpu_add_en->write(add_en_toMoA | add_en_or);
    fpu_mult_in1_sel->write(fpu_mult_in1_sel_comb | fpu_mult_in1_sel_reg);
    fpu_mult_in2_sel->write(fpu_mult_in2_sel_comb | fpu_mult_in2_sel_reg);
    fpu_add_in1_sel->write(
            fpu_add_in1_sel_comb | fpu_add_in1_sel_pipe[MULT_STAGES]);
    fpu_add_in2_sel->write(
            fpu_add_in2_sel_comb | fpu_add_in2_sel_pipe[MULT_STAGES]);
    fpu_out_sel->write(fpu_out_sel_pipe[MULT_STAGES]);
}
