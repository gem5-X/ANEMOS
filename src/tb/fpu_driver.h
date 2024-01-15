#include "systemc.h"

#include "../cnm_base.h"
#include "../half.hpp"

using half_float::half;

SC_MODULE(fpu_driver) {

#if MIXED_SIM

	sc_out<sc_logic>    rst;
	sc_out<sc_logic>    mult_en;             	// Signals that a multiply computation step should be performed
	sc_out<sc_logic>	add_en;					// Signals that an add computation step should be performed
	sc_out<sc_lv<16> >  srf_in;                	// Scalar operand from SRF
	sc_out<sc_lv<16> >  grfa_in1[SIMD_WIDTH];   // Input 1 from GRF_A
	sc_out<sc_lv<16> >  grfa_in2[SIMD_WIDTH];   // Input 2 from GRF_A
	sc_out<sc_lv<16> >  grfb_in1[SIMD_WIDTH];   // Input 1 from GRF_B
	sc_out<sc_lv<16> >  grfb_in2[SIMD_WIDTH];   // Input 2 from GRF_B
	// sc_out<sc_lv<16> >   even_out[SIMD_WIDTH];    // Input from EVEN_BANK
	// sc_out<sc_lv<16> >   odd_out[SIMD_WIDTH];     // Input from ODD_BANK
	sc_out<sc_lv<8> >   mult_in1_sel;           // Selects input 1 for addition
	sc_out<sc_lv<8> >   mult_in2_sel;           // Selects input 2 for multiplication
	sc_out<sc_lv<8> >   add_in1_sel;            // Selects input 1 for addition
	sc_out<sc_lv<8> >   add_in2_sel;            // Selects input 2 for multiplication
	sc_out<sc_logic>	out_sel;				// Selects the output: 0 for adder output, 1 for multiplier output

#else

    sc_out<bool>    rst;
    sc_out<bool>    mult_en;                // Signals that a multiply computation step should be performed
    sc_out<bool>    add_en;                 // Signals that an add computation step should be performed
    sc_out<half>    srf_in;                	// Scalar operand from SRF
    sc_out<half>    grfa_in1[SIMD_WIDTH];   // Input 1 from GRF_A
    sc_out<half>    grfa_in2[SIMD_WIDTH];   // Input 2 from GRF_A
    sc_out<half>    grfb_in1[SIMD_WIDTH];   // Input 1 from GRF_B
    sc_out<half>    grfb_in2[SIMD_WIDTH];   // Input 2 from GRF_B
//    sc_out<half>    even_out[SIMD_WIDTH];   // Input from EVEN_BANK
//    sc_out<half>    odd_out[SIMD_WIDTH];    // Input from ODD_BANK
    sc_out<uint8_t> mult_in1_sel;           // Selects input 1 for addition
    sc_out<uint8_t> mult_in2_sel;           // Selects input 2 for multiplication
    sc_out<uint8_t> add_in1_sel;            // Selects input 1 for addition
    sc_out<uint8_t> add_in2_sel;            // Selects input 2 for multiplication
    sc_out<bool>    out_sel;                // Selects the output: 0 for adder output, 1 for multiplier output

#endif

    SC_CTOR(fpu_driver) {
        SC_THREAD(driver_thread);
    }

    void driver_thread();
};
