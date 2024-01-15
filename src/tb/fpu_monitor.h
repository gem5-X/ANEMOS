#include "systemc.h"

#include "../cnm_base.h"
#include "../half.hpp"

using half_float::half;

SC_MODULE(fpu_monitor) {

#if MIXED_SIM

	sc_in<sc_logic>     clk;
	sc_in<sc_logic>     rst;
	sc_in<sc_logic>     mult_en;             	// Signals that a multiply computation step should be performed
	sc_in<sc_logic>		add_en;					// Signals that an add computation step should be performed
	sc_in<sc_lv<16> >   srf_in;                	// Scalar operand from SRF
	sc_in<sc_lv<16> >   grfa_in1[SIMD_WIDTH];   // Input 1 from GRF_A
	sc_in<sc_lv<16> >   grfa_in2[SIMD_WIDTH];   // Input 2 from GRF_A
	sc_in<sc_lv<16> >   grfb_in1[SIMD_WIDTH];   // Input 1 from GRF_B
	sc_in<sc_lv<16> >   grfb_in2[SIMD_WIDTH];   // Input 2 from GRF_B
	// sc_in<sc_lv<16> >    even_in[SIMD_WIDTH];    // Input from EVEN_BANK
	// sc_in<sc_lv<16> >    odd_in[SIMD_WIDTH];     // Input from ODD_BANK
	sc_in<sc_lv<8> >	mult_in1_sel;           // Selects input 1 for addition
	sc_in<sc_lv<8> >    mult_in2_sel;           // Selects input 2 for multiplication
	sc_in<sc_lv<8> >    add_in1_sel;            // Selects input 1 for addition
	sc_in<sc_lv<8> >    add_in2_sel;            // Selects input 2 for multiplication
	sc_in<sc_logic>		out_sel;				// Selects the output: 0 for adder output, 1 for multiplier output
	sc_in<sc_lv<16> >   output[SIMD_WIDTH];     // Output of the Floating Point Unit

#else

    sc_in_clk       clk;
    sc_in<bool>     rst;
    sc_in<bool>     mult_en;                // Signals that a multiply computation step should be performed
    sc_in<bool>     add_en;	                // Signals that an add computation step should be performed
    sc_in<half>     srf_in;                	// Scalar operand from SRF
    sc_in<half>     grfa_in1[SIMD_WIDTH];   // Input 1 from GRF_A
    sc_in<half>     grfa_in2[SIMD_WIDTH];   // Input 2 from GRF_A
    sc_in<half>     grfb_in1[SIMD_WIDTH];   // Input 1 from GRF_B
    sc_in<half>     grfb_in2[SIMD_WIDTH];   // Input 2 from GRF_B
//    sc_in<half>     even_in[SIMD_WIDTH];    // Input from EVEN_BANK
//    sc_in<half>     odd_in[SIMD_WIDTH];     // Input from ODD_BANK
    sc_in<uint8_t>  mult_in1_sel;           // Selects input 1 for addition
    sc_in<uint8_t>  mult_in2_sel;           // Selects input 2 for multiplication
    sc_in<uint8_t>  add_in1_sel;            // Selects input 1 for addition
    sc_in<uint8_t>  add_in2_sel;            // Selects input 2 for multiplication
    sc_in<bool>     out_sel;                // Selects the output: 0 for adder output, 1 for multiplier output
    sc_in<half>     output[SIMD_WIDTH];     // Output of the Floating Point Unit

#endif

    // Internal events
    sc_event_or_list output_change_evnt; // Event signaling that any of the outputs has changed

    // Internal variables and signals for checking
    half **mpipe;
    half **apipe;
    half *m1_in;
    half *m2_in;
    half *a1_in;
    half *a2_in;
    half *pred_out;
    bool error;

    SC_CTOR(fpu_monitor) {
        SC_THREAD(monitor_thread);
        sensitive << clk.pos();

        SC_THREAD(mirror_thread);
        sensitive << rst.neg() << clk.pos();

        SC_METHOD(out_mirror_thread);

        int i, j;
        mpipe = new half*[MULT_STAGES];
        for (i = 0; i < MULT_STAGES; i++) {
            mpipe[i] = new half[SIMD_WIDTH];
            for (j = 0; j < SIMD_WIDTH; j++)
                mpipe[i][j] = half_float::half_cast<half>(0.0);
        }
        apipe = new half*[ADD_STAGES];
        for (i = 0; i < ADD_STAGES; i++) {
            apipe[i] = new half[SIMD_WIDTH];
            for (j = 0; j < SIMD_WIDTH; j++)
                apipe[i][j] = half_float::half_cast<half>(0.0);
        }

        m1_in = new half[SIMD_WIDTH];
        m2_in = new half[SIMD_WIDTH];
        a1_in = new half[SIMD_WIDTH];
        a2_in = new half[SIMD_WIDTH];
        pred_out = new half[SIMD_WIDTH];
        for (i = 0; i < SIMD_WIDTH; i++) {
            m1_in[i] = half_float::half_cast<half>(0.0);
            m2_in[i] = half_float::half_cast<half>(0.0);
            a1_in[i] = half_float::half_cast<half>(0.0);
            a2_in[i] = half_float::half_cast<half>(0.0);
            pred_out[i] = half_float::half_cast<half>(0.0);
        }

        error = false;
    }

    void end_of_elaboration() {
        // creates output_change_evnt once all ports are bounded
        for (int i = 0; i < SIMD_WIDTH; i++) {
            output_change_evnt |= output[i]->value_changed_event();
        }
    }

    void monitor_thread(); // Outputs the behavior for the first SIMD rail, and automatically checks the functionality
    void mirror_thread(); // Mirror FPU behavior for checking its functionality
    void out_mirror_thread();	// Mirror FPU output for checking functionality
};
