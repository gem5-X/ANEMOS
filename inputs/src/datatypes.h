#ifndef SRC_DATATYPES_H_
#define SRC_DATATYPES_H_

#include <stdint.h>
#include "../../src/defs.h"
#include "half.hpp"
using half_float::half;

#if (DATA_TYPE > 3)
#define INT_TYPE    1       // 1 if Integer data type, 0 otherwise
#else
#define INT_TYPE    0       // 1 if Integer data type, 0 otherwise
#endif

#if (DATA_TYPE == 0 || DATA_TYPE == 3) // TODO until bfloat is implemented
#define HALF_FLOAT  1       // 1 if Half Precision Floating Point data type, 0 otherwise
#else
#define HALF_FLOAT  0
#endif

/*
Data Formats for All Data Types:
half16                 =>   half
integer data types     =>   uintX_t
floating point 32      =>   float
floating point 64      =>   double
*/

#if (DATA_TYPE == 0)    // half
    #define WORD_BITS   16
    typedef float       cnm_t;
    typedef uint16_t    rfBin_t;
#elif (DATA_TYPE == 1)  // float
    #define WORD_BITS   32
    typedef float       cnm_t;
    typedef uint32_t    rfBin_t;
#elif (DATA_TYPE == 2)  // double
    #define WORD_BITS   64
    typedef double      cnm_t;
    typedef uint64_t    rfBin_t;
#elif (DATA_TYPE == 3)  // bfloat

#elif (DATA_TYPE == 4)  // int8
    #define WORD_BITS   8
    typedef int8_t      cnm_t;
    typedef uint32_t    rfBin_t;
#elif (DATA_TYPE == 5)  // int16
    #define WORD_BITS   16
    typedef int16_t     cnm_t;
    typedef uint32_t    rfBin_t;
#elif (DATA_TYPE == 6)  // int32
    #define WORD_BITS   32
    typedef int32_t     cnm_t;
    typedef uint32_t    rfBin_t;
#elif (DATA_TYPE == 7)  // int64
    #define WORD_BITS   64
    typedef int64_t     cnm_t;
    typedef uint64_t    rfBin_t;
#endif

#if (!HALF_FLOAT)
    typedef union
    {
        cnm_t       data;
        rfBin_t     bin;
    } cnm_union;
#endif

#if DQ_BITS == 16
typedef uint16_t dq_type;
#elif DQ_BITS == 32
typedef uint32_t dq_type;
#elif DQ_BITS == 64
typedef uint64_t dq_type;
#endif

#endif /* SRC_DATATYPES_H_ */
