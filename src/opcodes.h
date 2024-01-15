 /*
 * Copyright EPFL 2024
 * Rafael Medina Morillas
 *
 * Definition of the CnM opcodes and instruction formats
 *
 */

#ifndef SRC_OPCODES_H_
#define SRC_OPCODES_H_

#include <map>
#include <string>

// Instructions format
#define OPCODE_STA  31
#define OPCODE_END  28
#define IMM0_STA    18
#define IMM0_END    11
#define IMM1_STA    10
#define IMM1_END    0
#define DST_STA     27
#define DST_END     25
#define SRC0_STA    24
#define SRC0_END    22
#define SRC1_STA    21
#define SRC1_END    19
#define SRC2_STA    18
#define SRC2_END    16
#define RELU_BIT    15
#define AAM_BIT     15
#define DST_N_STA   14
#define DST_N_END   10
#define SRC0_N_STA  9
#define SRC0_N_END  5
#define SRC1_N_STA  4
#define SRC1_N_END  0

// OPCODES operation identifiers
enum OPCODE_VALUES {
    OP_NOP = 0,
    OP_JUMP = 1,
    OP_EXIT = 2,
    OP_MOV = 4,
    OP_FILL = 5,
    OP_ADD = 8,
    OP_MUL = 9,
    OP_MAD = 10,
    OP_MAC = 11
};

const std::map<uint8_t, std::string> OPCODE_STRING = {
    { OP_NOP, "OP_NOP" },
    { OP_JUMP, "OP_JUMP" },
    { OP_EXIT, "OP_EXIT" },
    { OP_ADD, "OP_ADD" },
    { OP_MUL, "OP_MUL" },
    { OP_MAD, "OP_MAD" },
    { OP_MAC, "OP_MAC" },
    { OP_MOV, "OP_MOV" },
    { OP_FILL, "OP_FILL" },
};

// OPCODES storage identifiers
enum OPC_STORAGE {
    OPC_GRF_A = 0,
    OPC_GRF_B = 1,
    OPC_SRF_M = 2,
    OPC_SRF_A = 3,
    OPC_EVEN_BANK = 4,
    OPC_ODD_BANK = 5
};

const std::map<uint8_t, std::string> OPC_STORAGE_STRING = {
    { OPC_GRF_A, "GRF_A" },
    { OPC_GRF_B, "GRF_B" },
    { OPC_SRF_M, "SRF_M" },
    { OPC_SRF_A, "SRF_A" },
    { OPC_EVEN_BANK, "EVEN_BANK" },
    { OPC_ODD_BANK, "ODD_BANK" },
};

// MUX select signals
enum MUX_STORAGE {
    MUX_EXT = 0,
    MUX_FPU = 1,
    MUX_GRF_A = 2,
    MUX_GRF_B = 3,
    MUX_SRF = 4,
    MUX_EVEN_BANK = 5,
    MUX_ODD_BANK = 6
};

const std::map<uint8_t, std::string> MUX_STORAGE_STRING = {
    { MUX_EXT, "MUX_EXT" },
    { MUX_FPU, "MUX_FPU" },
    { MUX_GRF_A, "GRF_A" },
    { MUX_GRF_B, "GRF_B" },
    { MUX_SRF, "SRF" },
    { MUX_EVEN_BANK, "EVEN_BANK" },
    { MUX_ODD_BANK, "ODD_BANK" },
};

enum MUL1_SEL {
    M1_GRF_A1 = 0,
    M1_GRF_A2 = 1,
    M1_GRF_B1 = 2,
    M1_GRF_B2 = 3,
    M1_EVEN_BANK = 4,
    M1_ODD_BANK = 5
};

const std::map<uint8_t, std::string> MUL1_SEL_STRING = {
    { M1_GRF_A1, "GRF_A1" },
    { M1_GRF_A2, "GRF_A2" },
    { M1_GRF_B1, "GRF_B1" },
    { M1_GRF_B2, "GRF_B2" },
    { M1_EVEN_BANK, "EVEN_BANK" },
    { M1_ODD_BANK, "ODD_BANK" },
};

enum MUL2_SEL {
    M2_SRF = 0,
    M2_GRF_A1 = 1,
    M2_GRF_A2 = 2,
    M2_GRF_B1 = 3,
    M2_GRF_B2 = 4,
    M2_EVEN_BANK = 5,
    M2_ODD_BANK = 6
};

const std::map<uint8_t, std::string> MUL2_SEL_STRING = {
    { M2_SRF, "SRF" },
    { M2_GRF_A1, "GRF_A1" },
    { M2_GRF_A2, "GRF_A2" },
    { M2_GRF_B1, "GRF_B1" },
    { M2_GRF_B2, "GRF_B2" },
    { M2_EVEN_BANK, "EVEN_BANK" },
    { M2_ODD_BANK, "ODD_BANK" },
};

enum ADD_SEL {
    A_MULT_OUT = 0,
    A_SRF = 1,
    A_GRF_A1 = 2,
    A_GRF_A2 = 3,
    A_GRF_B1 = 4,
    A_GRF_B2 = 5,
    A_EVEN_BANK = 6,
    A_ODD_BANK = 7
};

const std::map<uint8_t, std::string> ADD_SEL_STRING = {
    { A_MULT_OUT, "MULT_OUT" },
    { A_SRF, "SRF" },
    { A_GRF_A1, "GRF_A1" },
    { A_GRF_A2, "GRF_A2" },
    { A_GRF_B1, "GRF_B1" },
    { A_GRF_B2, "GRF_B2" },
    { A_EVEN_BANK, "EVEN_BANK" },
    { A_ODD_BANK, "ODD_BANK" }
};

enum RF_SEL {
    RF_CRF = 0,
    RF_SRF_M = 1,
    RF_SRF_A = 2,
    RF_GRF_A = 3,
    RF_GRF_B = 4
};

const std::map<uint8_t, std::string> RF_SEL_STRING = {
    { RF_CRF, "RF_CRF" },
    { RF_SRF_M, "RF_SRF_M" },
    { RF_SRF_A, "RF_SRF_A" },
    { RF_GRF_A, "RF_GRF_A" },
    { RF_GRF_B, "RF_GRF_B" },
};

#endif /* SRC_OPCODES_H_ */
