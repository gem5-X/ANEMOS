#ifndef MAP_MM_H
#define MAP_MM_H

#include "half.hpp"
#include "datatypes.h"
#include "../../src/defs.h"
#include "../../src/opcodes.h"
#include "utils.h"

using namespace std;

// Mapping of matrix multriplication using the SRF, R-limited
void mapMatrixMultSrfRLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t *op1, cnm_t *op2, int m, int n, int q);

// Mapping of matrix multriplication using the SRF, C-limited
void mapMatrixMultSrfCLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t *op1, cnm_t *op2, int m, int n, int q);

#if (HALF_FLOAT)

// Function for computing a partial dot product within a matrix multiplication
float partMatMultDP(float *op1, float *op2, int m, int n, int q, int rowOp1, int colOp2, int len);

#else

// Function for computing a partial dot product within a matrix multiplication
cnm_t partMatMultDP(cnm_t *op1, cnm_t *op2, int m, int n, int q, int rowOp1, int colOp2, int len);

#endif  // HALF_FLOAT

#endif  // MAP_MM_H
