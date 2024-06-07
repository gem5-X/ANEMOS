 /*
 * Copyright EPFL 2024
 * Rafael Medina Morillas
 *
 * Definition of the CnM datatype
 *
 */


#ifndef SRC_DATATYPES_H_
#define SRC_DATATYPES_H_


#include <cstdint>
#include "defs.h"

#ifdef __SYNTHESIS__
#include "systemc.h"
#include <ac_std_float.h>
#include <ac_sc.h>
#else
#include "half.hpp"
using half_float::half;
#endif

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
=========Not Synthesis=========
half16                 =>   half
integer data types     =>   sc_int<WORD_BITS>
brain float 16         =>   only synthesis simulation
floating point 32      =>   float
floating point 64      =>   double
===========Synthesis===========
half16                 =>   ac_ieee_float16
integer data types     => intX_t X = number of bits
brain float 16         => ac:: bfloat16
floating point 32  => ac_ieee_float32
floating point 64  => ac_ieee_float64
*/

#if (DATA_TYPE == 0)    // half
    #define WORD_BITS   16
    #ifndef __SYNTHESIS__
        typedef half        cnm_t;
        typedef uint16_t    rfBin_t;
    #else
        typedef ac_ieee_float16     cnm_synth;
    #endif
#elif (DATA_TYPE == 1)  // float
    #define WORD_BITS 32
    #ifndef __SYNTHESIS__
        typedef float   cnm_t;
        typedef union
        {
            cnm_t       data;
            uint32_t    bin;
        } cnm_union;
    #else
        typedef ac_ieee_float32    cnm_synth;
    #endif
#elif (DATA_TYPE == 2)  // double
    #define WORD_BITS 64
    #ifndef __SYNTHESIS__
        typedef double  cnm_t;
        typedef union
        {
            cnm_t       data;
            uint64_t    bin;
        } cnm_union;
    #else
        typedef ac_ieee_float64    cnm_synth;
    #endif
#elif (DATA_TYPE == 3)  // bfloat
    #define WORD_BITS   16
    #ifndef __SYNTHESIS__
        typedef half        cnm_t;
        typedef uint16_t    rfBin_t;
    #else
        typedef ac_std_float<16,8>  cnm_synth;
    #endif
#elif (DATA_TYPE == 4)  // int8
    #define WORD_BITS 8
    #ifndef __SYNTHESIS__
        typedef sc_int<WORD_BITS>   cnm_t;
    #else
        typedef int8_t              cnm_synth;
    #endif
#elif (DATA_TYPE == 5)  // int16
    #define WORD_BITS 16
    #ifndef __SYNTHESIS__
        typedef sc_int<WORD_BITS>   cnm_t;
    #else
        typedef int16_t             cnm_synth;
    #endif
#elif (DATA_TYPE == 6)  // int32
    #define WORD_BITS 32
    #ifndef __SYNTHESIS__
        typedef sc_int<WORD_BITS>   cnm_t;
    #else
        typedef int32_t             cnm_synth;
    #endif
#elif (DATA_TYPE == 7)  // int64
    #define WORD_BITS 64
    #ifndef __SYNTHESIS__
        typedef sc_int<WORD_BITS>   cnm_t;
    #else
        typedef int64_t             cnm_synth;
    #endif
#endif

#if DQ_BITS == 16
typedef uint16_t dq_type;
#elif DQ_BITS == 32
typedef uint32_t dq_type;
#elif DQ_BITS == 64
typedef uint64_t dq_type;
#endif

#endif /* SRC_DATATYPES_H_ */
