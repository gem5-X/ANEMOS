#ifndef MAP_DP_H
#define MAP_DP_H

#include "half.hpp"
#include "datatypes.h"
#include "../../src/defs.h"
#include "../../src/opcodes.h"
#include "utils.h"

using namespace std;

// Mapping of dot product, R-limited
void mapDotProductRLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t **op1, cnm_t **op2, int V, int n);

// Mapping of dot product, C-limited
void mapDotProductCLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                   cnm_t **op1, cnm_t **op2, int V, int n);

#if (HALF_FLOAT)

// Function for computing partial dot products
float partDotProduct(float *op1, float *op2, int n);

#else

// Function for computing partial dot products
cnm_t partDotProduct(cnm_t *op1, cnm_t *op2, int n);

#endif  // HALF_FLOAT

#endif  // MAP_DP_H
