#ifndef MAP_VA_H
#define MAP_VA_H

#include "half.hpp"
#include "datatypes.h"
#include "../../src/defs.h"
#include "../../src/opcodes.h"
#include "utils.h"

// Row-wise mapping of element-wise vector addition, R-limited
void mapEWAdditionRowWiseRLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t **op1, cnm_t **op2, int V, int n);

// Row-wise mapping of element-wise vector addition, C-limited
void mapEWAdditionRowWiseCLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t **op1, cnm_t **op2, int V, int n);

// Column-wise mapping of element-wise vector addition, R-limited
void mapEWAdditionColWiseRLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t **op1, cnm_t **op2, int V, int n);

// Column-wise mapping of element-wise vector addition, C-limited
void mapEWAdditionColWiseCLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t **op1, cnm_t **op2, int V, int n);

#endif  // MAP_VA_H