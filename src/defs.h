 /*
 * Copyright EPFL 2023
 * Rafael Medina Morillas
 * Riselda Kodra
 *
 * Defines for configuring the design.
 *
 */

#ifndef SRC_DEFS_H_
#define SRC_DEFS_H_

#define MIXED_SIM 0     // 0 if SystemC-only simulation, 1 if mixed SystemC + RTL

#define CLK_PERIOD 3333
#define RESOLUTION SC_PS

// 0: half
// 1: float
// 2: double
// 3: bfloat (not implemented)
// 4: int8
// 5: int16
// 6: int32
// 7: int64
#define DATA_TYPE   0

// Address mapping constants
#define CHANNEL_BITS    4
#define RANK_BITS       0
#define BG_BITS         2
#define BANK_BITS       2
#define ROW_BITS        14
#define COL_BITS        5
#define GLOBAL_OFFSET   6

// Sizing constants
#define CORES_PER_PCH   1
#define SIMD_WIDTH      (256 / WORD_BITS)   // Compatible with HBM interface
#define CRF_ENTRIES     32
#define SRF_A_ENTRIES   8
#define SRF_M_ENTRIES   8
#define GRF_ENTRIES     8
#define ADD_STAGES      1
#define MULT_STAGES     1
#define RF_SEL_BITS     ROW_BITS-1
#define RF_ADDR_BITS    COL_BITS
#define AAM_ADDR_BITS   3
#define INSTR_BITS      32
//#define WORD_BITS       16
#define GRF_WIDTH       (WORD_BITS*SIMD_WIDTH)
#define DQ_BITS         64
#define DQ_CLK          (GRF_WIDTH/DQ_BITS)
#define INSTR_CLK       (INSTR_BITS/DQ_BITS)

// If not enough column bits to address CRF, using also bank bits
#define CRF_BANK_ADDR   ((1 << COL_BITS) < CRF_ENTRIES)

#endif /* SRC_DEFS_H_ */
