#ifndef MAP_CONV_H
#define MAP_CONV_H

#include "half.hpp"
#include "datatypes.h"
#include "../../src/defs.h"
#include "../../src/opcodes.h"
#include "utils.h"

using namespace std;

// Channel-wise mapping of convolution with weight reuse, R-limited
void mapConvCWWRRLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t *act, cnm_t *weight, cnm_t *bias,
                                int ci, int wi, int hi, int k, int co, int wo, int ho, int stride);

// Channel-wise mapping of convolution with weight reuse, C-limited
void mapConvCWWRCLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t *act, cnm_t *weight, cnm_t *bias,
                                int ci, int wi, int hi, int k, int co, int wo, int ho, int stride); 

#if (HALF_FLOAT)

// Function for computing partial convolution resulting in a pixel
float partPixConv(float *act, float *weight, float *bias, int stride, int k,
                    int ci, int hi, int wi, int ck, int hk, int wk, int co, int ho, int wo);

#else

// Function for computing partial convolution resulting in a pixel
cnm_t partPixConv(cnm_t *act, cnm_t *weight, cnm_t *bias, int stride, int k,
                    int ci, int hi, int wi, int ck, int hk, int wk, int co, int ho, int wo);

#endif  // HALF_FLOAT

#endif  // MAP_CONV_H
