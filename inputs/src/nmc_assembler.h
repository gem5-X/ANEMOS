#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <array>
#include <map>
#include <random>

#include "half.hpp"
#include "datatypes.h"
#include "../../src/defs.h"
#include "../../src/opcodes.h"

#if WORD_BITS != 64
    #define MASK ((1ul << WORD_BITS) - 1)
#else 
    #define MASK 0xFFFFFFFFFFFFFFFF
#endif
#define WORDS_PER_BURST (DQ_BITS/WORD_BITS)

#define DATAFILE    "DataFile"
#define ADDRFILE    "AddrFile"

using namespace std;

// Format of assembly input:    Inst        [Operands]
// Format of raw traces:        Address     R/W         Data

enum INSTR {
    WRF,
    NOP, JUMP, EXIT,
    MOV, RELU, FILL,
    ADD, MUL, MAD, MAC,
    ADDa, MULa, MADa, MACa,
    EXEC
};

const std::map<uint8_t, std::string> INSTR2STRING = {
    { WRF, "WRF" },
    { NOP, "NOP" }, { JUMP, "JUMP" }, { EXIT, "EXIT" },
    { MOV, "MOV" }, { RELU, "RELU" }, { FILL, "FILL" },
    { ADD, "ADD" }, { MUL, "MUL" }, { MAD, "MAD" }, { MAC, "MAC" },
    { ADDa, "ADDa" }, { MULa, "MULa" }, { MADa, "MADa" }, { MACa, "MACa" },
    { EXEC, "EXEC" },
};

const std::map<std::string, uint8_t> STRING2INSTR = {
    { "WRF", WRF },
    { "NOP", NOP }, { "JUMP", JUMP }, { "EXIT", EXIT },
    { "MOV", MOV }, { "RELU", RELU }, { "FILL", FILL },
    { "ADD", ADD }, { "MUL", MUL }, { "MAD", MAD }, { "MAC", MAC },
    { "ADDa", ADDa }, { "MULa", MULa }, { "MADa", MADa }, { "MACa", MACa },
    { "EXEC", EXEC },
};

// // OPCODES operation identifiers
// enum OPCODE_VALUES {
//     OP_NOP = 0,
//     OP_JUMP = 1,
//     OP_EXIT = 2,
//     OP_MOV = 4,
//     OP_FILL = 5,
//     OP_ADD = 8,
//     OP_MUL = 9,
//     OP_MAD = 10,
//     OP_MAC = 11
// };

const std::map<uint8_t, uint8_t> INSTR2OPCODE = {
    { NOP, OP_NOP },
    { JUMP, OP_JUMP },
    { EXIT, OP_EXIT },
    { ADD, OP_ADD },
    { MUL, OP_MUL },
    { MAD, OP_MAD },
    { MAC, OP_MAC },
    { ADDa, OP_ADD },
    { MULa, OP_MUL },
    { MADa, OP_MAD },
    { MACa, OP_MAC },
    { MOV, OP_MOV },
    { RELU, OP_MOV },
    { FILL, OP_FILL },
};

enum STORE {
    CRF = 0,
    SRFM = 1, SRFA = 2,
    GRFA = 3, GRFB = 4,
    EVEN = 5, ODD = 6
};

const std::map<uint8_t, std::string> STORE2STRING = {
    { CRF, "CRF" },
    { GRFA, "GRFA" }, { GRFB, "GRFB" },
    { SRFM, "SRFM" }, { SRFA, "SRFA" },
    { EVEN, "EVEN_BANK" }, { ODD, "ODD_BANK" }
};

const std::map<std::string, uint8_t> STRING2STORE = {
    { "CRF", CRF },
    { "GRFA", GRFA }, { "GRFB", GRFB },
    { "SRFM", SRFM }, { "SRFA", SRFA },
    { "EVEN_BANK", EVEN }, { "ODD_BANK", ODD  }
};

// // OPCODES storage identifiers
// enum OPC_STORAGE {
//     OPC_GRF_A = 0,
//     OPC_GRF_B = 1,
//     OPC_SRF_M = 2,
//     OPC_SRF_A = 3,
//     OPC_EVEN_BANK = 4,
//     OPC_ODD_BANK = 5
// };

const std::map<string, uint8_t> STRING2OPCSTORAGE = {
    { "GRFA", OPC_GRF_A },
    { "GRFB", OPC_GRF_B },
    { "SRFM", OPC_SRF_M },
    { "SRFA", OPC_SRF_A },
    { "EVEN_BANK", OPC_EVEN_BANK },
    { "ODD_BANK", OPC_ODD_BANK }
};

enum CMD {
    RD, WR,
    DC
};

const std::map<uint8_t, std::string> CMD_STRING = {
    { RD, "RD" }, { WR, "WR" },
    { DC, "DC" },
};

class nmcInst {
  public:
    uint8_t opCode;
    uint16_t imm0;
    uint16_t imm1;
    uint8_t dst;
    uint8_t src0;
    uint8_t src1;
    uint8_t src2;
    uint64_t idxDst;
    uint64_t idxSrc0;
    uint64_t idxSrc1;
    uint64_t idxSrc2;
    bool dstAddrFile;
    bool src0AddrFile;
    bool src1AddrFile;
    bool src2AddrFile;
    bool relu;
    bool aam;
    bool dataFile;
    deque<dq_type> data;
    nmcInst()
        : opCode(-1), imm0(0), imm1(0), dst(0), src0(0), src1(0), src2(0),
          idxDst(0), idxSrc0(0), idxSrc1(0), dstAddrFile(false), src0AddrFile(false),
          src1AddrFile(false), relu(false), aam(false), dataFile(false)
    {}
};

// Definition of the address mapping
enum class Level : int {Channel, Rank, BankGroup, Bank, Row, Column, MAX};
string level_str[int(Level::MAX)] = {"Ch", "Ra", "Bg", "Ba", "Ro", "Co"};
int addr_bits[int(Level::MAX)] = {CHANNEL_BITS, RANK_BITS, BG_BITS, BANK_BITS, ROW_BITS, COL_BITS};
Level addr_map[int(Level::MAX)] = {Level::Channel, Level::Column, Level::Rank,
                        Level::BankGroup, Level::Bank, Level::Row};
int global_offset = GLOBAL_OFFSET;

// Function for building the address using the indices of the different levels
uint64_t build_addr(vector<uint64_t> addr_vec, bool rf_write);

// Function for spliting the store type and the index
bool splitStoreIndex(string* storeTypeString, uint64_t *idx);

// Function for getting the bank index from an address
uint8_t get_bank(uint64_t addr);

// Function for getting the row index from an address
uint16_t get_row(uint64_t addr);

// Function for getting the column index from an address
uint16_t get_col(uint64_t addr);

// Function for building the binary instruction word
uint64_t build_instr(nmcInst instrData);

// Function from getting the data for the NMC instruction, either from
// the assembly file, or signal to be read when the loop is executed
bool getInstData(istringstream &aistream, deque<rfBin_t> &rfBin, nmcInst *currInstr);

// Function to get data from file at execution time, when the loop is executed
bool getDataFromFile (ifstream &dataFile, deque<rfBin_t> &rfBin, nmcInst *currInstr);

// Function to get address from file at execution time, when the loop is executed
bool getAddrFromFile (ifstream &addrFile, uint64_t *addr);