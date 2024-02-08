#include "half.hpp"
#include "datatypes.h"
#include "../../src/defs.h"
#include "../../src/opcodes.h"
#include "map_va.h"
#include "map_dp.h"
#include "map_mm.h"
#include "map_conv.h"

using namespace std;

enum KERNEL_VALUES {
    EWARW,  // Element-Wise Addition Row-Wise
    EWACW,  // Element-Wise Addition Column-Wise
    EWMRW,  // Element-Wise Multiplication Row-Wise
    EWMCW,  // Element-Wise Multiplication Column-Wise
    DP,     // Dot Product
    MMS,    // Matrix Multiplication using SRF
    CCWWR,  // Convolution Channel-Wise wieht Weight Reuse
};

const std::map<std::string, uint8_t> KERNEL = {
    { "EWARW", EWARW },
    { "EWACW", EWACW },
    { "EWMRW", EWMRW },
    { "EWMCW", EWMCW },
    { "DP", DP },
    { "MMS", MMS },
    { "CCWWR", CCWWR },
};