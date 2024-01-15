 /* 
 * Copyright EPFL 2023
 * Rafael Medina Morillas
 * Riselda Kodra
 * 
 * Basic include for the CnM design.
 *
 */

#ifndef CNM_BASE_H
#define CNM_BASE_H

#include <map>
#include <stdlib.h>
#include <string>
#include "systemc.h"
#include "defs.h"
#include "opcodes.h"
#include "datatypes.h"

// RoBaBgRaCoCh mapping
#define CH_END          GLOBAL_OFFSET
#define CH_STA          GLOBAL_OFFSET + CHANNEL_BITS - 1
#define CO_END          GLOBAL_OFFSET + CHANNEL_BITS
#define CO_STA          GLOBAL_OFFSET + CHANNEL_BITS + COL_BITS - 1
#define RA_END          GLOBAL_OFFSET + CHANNEL_BITS + COL_BITS
#define RA_STA          GLOBAL_OFFSET + CHANNEL_BITS + COL_BITS + RANK_BITS - 1
#define BG_END          GLOBAL_OFFSET + CHANNEL_BITS + COL_BITS + RANK_BITS
#define BG_STA          GLOBAL_OFFSET + CHANNEL_BITS + COL_BITS + RANK_BITS + BG_BITS - 1
#define BA_END          GLOBAL_OFFSET + CHANNEL_BITS + COL_BITS + RANK_BITS + BG_BITS
#define BA_STA          GLOBAL_OFFSET + CHANNEL_BITS + COL_BITS + RANK_BITS + BG_BITS + BANK_BITS - 1
#define RO_END          GLOBAL_OFFSET + CHANNEL_BITS + COL_BITS + RANK_BITS + BG_BITS + BANK_BITS
#define RO_STA          GLOBAL_OFFSET + CHANNEL_BITS + COL_BITS + RANK_BITS + BG_BITS + BANK_BITS + ROW_BITS - 1
#define ADDR_TOTAL_BITS GLOBAL_OFFSET + CHANNEL_BITS + COL_BITS + RANK_BITS + BG_BITS + BANK_BITS + ROW_BITS

// DQ constants
#ifndef __SYNTHESIS__
#define VAR_DQ_CLK      (DQ_CLK > 1)    // True if more than one clk is needed to write the GRFs
#define DQ_CLK_GT_2     (DQ_CLK > 2)
#define DQ_CLK_GT_4     (DQ_CLK > 4)
#define DQ_CLK_GT_8     (DQ_CLK > 8)
#else
#define VAR_DQ_CLK      1   // True if more than one clk is needed to write the GRFs
#define DQ_CLK_GT_2     0
#define DQ_CLK_GT_4     0
#define DQ_CLK_GT_8     0
#endif

#endif
