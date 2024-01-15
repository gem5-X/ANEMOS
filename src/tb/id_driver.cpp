#include "id_driver.h"
#include <random>

void id_driver::driver_thread() {
    int i, j, k, n;

    // Randomization tools
    std::mt19937 gen(11111);    // Standard mersenne_twister_engine seeded
    std::uniform_int_distribution<> dis_mul1_sel(M1_GRF_A1, M1_GRF_B2);
    std::uniform_int_distribution<> dis_bool(0, 1);

    sc_uint<ROW_BITS> row_aux = 0;

    // Instruction word
    sc_uint<32> instruction;
    sc_uint<OPCODE_STA - OPCODE_END + 1> OPCODE;
    sc_uint<IMM0_STA - IMM0_END + 1> IMM0;
    sc_uint<IMM1_STA - IMM1_END + 1> IMM1;
    sc_uint<DST_STA - DST_END + 1> DST;
    sc_uint<SRC0_STA - SRC0_END + 1> SRC0;
    sc_uint<SRC1_STA - SRC1_END + 1> SRC1;
    sc_uint<SRC2_STA - SRC2_END + 1> SRC2;
    bool RELU;
//	bool AAM;
    sc_uint<DST_N_STA - DST_N_END + 1> DST_N;
    sc_uint<SRC0_N_STA - SRC0_N_END + 1> SRC0_N;
    sc_uint<SRC1_N_STA - SRC1_N_END + 1> SRC1_N;

    // Initialization
    rst->write(false);
    rf_access->write(false);
//    rf_wr_nrd->write(false);
    decode_en->write(false);
    pc_in->write(0);
    instr->write(0);
    row_aux.range(RF_SEL_BITS - 1, 0) = 0;
    row_addr->write(row_aux);
    col_addr->write(0);

    wait(5, SC_NS);
    rst->write(1);
    wait(0, SC_NS);

    wait(5, SC_NS);

    // Test moving of data when not computing	TODO can be randomized for extensive testing
    wait(0, SC_NS);
    rf_access->write(true);
    decode_en->write(false);
    row_aux.range(RF_SEL_BITS - 1, 0) = RF_CRF;
    row_addr->write(row_aux);
    col_addr->write(0);

    wait(10, SC_NS);
    wait(0, SC_NS);
    row_aux.range(RF_SEL_BITS - 1, 0) = RF_SRF_M;
    row_addr->write(row_aux);
    col_addr->write(2);

    wait(10, SC_NS);
    wait(0, SC_NS);
    row_aux.range(RF_SEL_BITS - 1, 0) = RF_SRF_A;
    row_addr->write(row_aux);
    col_addr->write(8);

    wait(10, SC_NS);
    wait(0, SC_NS);
    row_aux.range(RF_SEL_BITS - 1, 0) = RF_GRF_A;
    row_addr->write(row_aux);
    col_addr->write(5);

    wait(10, SC_NS);
    wait(0, SC_NS);
    row_aux.range(RF_SEL_BITS - 1, 0) = RF_GRF_B;
    row_addr->write(row_aux);
    col_addr->write(1);

    wait(10, SC_NS);
    wait(0, SC_NS);
    rf_access->write(true);
    decode_en->write(false);
    row_aux.range(RF_SEL_BITS - 1, 0) = RF_CRF;
    row_addr->write(row_aux);
    col_addr->write(6);

    wait(10, SC_NS);
    wait(0, SC_NS);
    row_aux.range(RF_SEL_BITS - 1, 0) = RF_SRF_M;
    row_addr->write(row_aux);
    col_addr->write(7);

    wait(10, SC_NS);
    wait(0, SC_NS);
    row_aux.range(RF_SEL_BITS - 1, 0) = RF_SRF_A;
    row_addr->write(row_aux);
    col_addr->write(3);

    wait(10, SC_NS);
    wait(0, SC_NS);
    row_aux.range(RF_SEL_BITS - 1, 0) = RF_GRF_A;
    row_addr->write(row_aux);
    col_addr->write(10);

    wait(10, SC_NS);
    wait(0, SC_NS);
    row_aux.range(RF_SEL_BITS - 1, 0) = RF_GRF_B;
    row_addr->write(row_aux);
    col_addr->write(5);

    // Testing control instructions
    wait(10, SC_NS);
    wait(0, SC_NS);
    OPCODE = OP_NOP;
    IMM0 = 4;
    IMM1 = 45;
    instruction.range(OPCODE_STA, OPCODE_END) = OPCODE;
    instruction.range(IMM0_STA, IMM0_END) = IMM0;
    instruction.range(IMM1_STA, IMM1_END) = IMM1;
    instr->write(instruction.to_int());
    rf_access->write(false);
    decode_en->write(true);

    wait(10, SC_NS);
    wait(0, SC_NS);
    decode_en->write(true);	// Check if it ignores the second NOP

    wait(10, SC_NS);
    wait(0, SC_NS);
    OPCODE = OP_JUMP;
    IMM0 = 1;
    IMM1 = 2;
    instruction.range(OPCODE_STA, OPCODE_END) = OPCODE;
    instruction.range(IMM0_STA, IMM0_END) = IMM0;
    instruction.range(IMM1_STA, IMM1_END) = IMM1;
    instr->write(instruction.to_int());
    decode_en->write(true);

    wait(50, SC_NS);	// Automatically does the 2 loops when noop ends
    wait(0, SC_NS);
    OPCODE = OP_EXIT;
    instruction.range(OPCODE_STA, OPCODE_END) = OPCODE;
    instruction.range(IMM0_STA, IMM0_END) = IMM0;
    instruction.range(IMM1_STA, IMM1_END) = IMM1;
    instr->write(instruction.to_int());
    decode_en->write(true);

    // Testing data movement instructions
    for (i = 0; i <= OPC_SRF_A; i++) {
        for (j = 0; j <= OPC_SRF_A; j++) {
            for (k = 0; k < 2; k++) {
                wait(10, SC_NS);
                wait(0, SC_NS);
                OPCODE = OP_MOV;
                SRC0 = i;
                DST = j;
                RELU = k;
                SRC0_N = (i + j) % 8;
                DST_N = (i + j + 1) % 8;
                instruction.range(OPCODE_STA, OPCODE_END) = OPCODE;
                instruction.range(SRC0_STA, SRC0_END) = SRC0;
                instruction.range(DST_STA, DST_END) = DST;
                instruction.range(RELU_BIT, RELU_BIT) = RELU;
                instruction.range(SRC0_N_STA, SRC0_N_END) = SRC0_N;
                instruction.range(DST_N_STA, DST_N_END) = DST_N;
                instr->write(instruction.to_int());
                decode_en->write(true);
            }
        }
    }

    // Testing ALU instructions
    for (i = 0; i <= OPC_SRF_A; i++) {
        for (j = 0; j <= OPC_SRF_A; j++) {
            for (k = 0; k <= OPC_GRF_B; k++) {
                wait(10, SC_NS);
                wait(0, SC_NS);
                OPCODE = OP_MUL;
                SRC0 = i;
                SRC1 = j;
                DST = k;
                SRC0_N = (i + j + k) % 8;
                SRC1_N = (i + j + k + 1) % 8;
                DST_N = (i + j + k + 2) % 8;
                instruction.range(OPCODE_STA, OPCODE_END) = OPCODE;
                instruction.range(SRC0_STA, SRC0_END) = SRC0;
                instruction.range(SRC1_STA, SRC1_END) = SRC1;
                instruction.range(DST_STA, DST_END) = DST;
                instruction.range(SRC0_N_STA, SRC0_N_END) = SRC0_N;
                instruction.range(SRC1_N_STA, SRC1_N_END) = SRC1_N;
                instruction.range(DST_N_STA, DST_N_END) = DST_N;
                instr->write(instruction.to_int());
                decode_en->write(true);
            }
        }
    }

    wait(10, SC_NS);
    wait(0, SC_NS);
    decode_en->write(false);
    wait(50, SC_NS);

    for (i = 0; i <= OPC_SRF_A; i++) {
        for (j = 0; j <= OPC_SRF_A; j++) {
            for (k = 0; k <= OPC_GRF_B; k++) {
                wait(10, SC_NS);
                wait(0, SC_NS);
                OPCODE = OP_ADD;
                SRC0 = i;
                SRC1 = j;
                DST = k;
                SRC0_N = (i + j + k) % 8;
                SRC1_N = (i + j + k + 1) % 8;
                DST_N = (i + j + k + 2) % 8;
                instruction.range(OPCODE_STA, OPCODE_END) = OPCODE;
                instruction.range(SRC0_STA, SRC0_END) = SRC0;
                instruction.range(SRC1_STA, SRC1_END) = SRC1;
                instruction.range(DST_STA, DST_END) = DST;
                instruction.range(SRC0_N_STA, SRC0_N_END) = SRC0_N;
                instruction.range(SRC1_N_STA, SRC1_N_END) = SRC1_N;
                instruction.range(DST_N_STA, DST_N_END) = DST_N;
                instr->write(instruction.to_int());
                decode_en->write(true);
            }
        }
    }

    for (i = 0; i <= OPC_SRF_A; i++) {
        for (j = 0; j <= OPC_SRF_A; j++) {
            for (k = 0; k <= OPC_SRF_A; k++) {
                for (n = 0; n <= OPC_GRF_B; n++) {
                    wait(10, SC_NS);
                    wait(0, SC_NS);
                    OPCODE = OP_MAD;
                    SRC0 = i;
                    SRC1 = j;
                    SRC2 = k;
                    DST = n;
                    SRC0_N = (i + j + k + n) % 8;
                    SRC1_N = (i + j + k + n + 1) % 8;
                    DST_N = (i + j + k + n + 2) % 8;
                    instruction.range(OPCODE_STA, OPCODE_END) = OPCODE;
                    instruction.range(SRC0_STA, SRC0_END) = SRC0;
                    instruction.range(SRC1_STA, SRC1_END) = SRC1;
                    instruction.range(SRC2_STA, SRC2_END) = SRC2;
                    instruction.range(DST_STA, DST_END) = DST;
                    instruction.range(SRC0_N_STA, SRC0_N_END) = SRC0_N;
                    instruction.range(SRC1_N_STA, SRC1_N_END) = SRC1_N;
                    instruction.range(DST_N_STA, DST_N_END) = DST_N;
                    instr->write(instruction.to_int());
                    decode_en->write(true);
                }
            }
        }
    }

    for (i = 0; i <= OPC_SRF_A; i++) {
        for (j = 0; j <= OPC_SRF_A; j++) {
            for (k = 0; k <= OPC_GRF_B; k++) {
                wait(10, SC_NS);
                wait(0, SC_NS);
                OPCODE = OP_MAC;
                SRC0 = i;
                SRC1 = j;
                DST = k;
                SRC0_N = (i + j + k) % 8;
                SRC1_N = (i + j + k + 1) % 8;
                DST_N = (i + j + k + 2) % 8;
                instruction.range(OPCODE_STA, OPCODE_END) = OPCODE;
                instruction.range(SRC0_STA, SRC0_END) = SRC0;
                instruction.range(SRC1_STA, SRC1_END) = SRC1;
                instruction.range(DST_STA, DST_END) = DST;
                instruction.range(SRC0_N_STA, SRC0_N_END) = SRC0_N;
                instruction.range(SRC1_N_STA, SRC1_N_END) = SRC1_N;
                instruction.range(DST_N_STA, DST_N_END) = DST_N;
                instr->write(instruction.to_int());
                decode_en->write(true);
            }
        }
    }

    wait(10, SC_NS);
    wait(0, SC_NS);
    decode_en->write(false);

}
