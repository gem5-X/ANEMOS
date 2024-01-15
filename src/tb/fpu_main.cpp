#include "fpu_main.h"

#ifdef MTI_SYSTEMC

	SC_MODULE_EXPORT(top);

#if MIXED_SIM

	void top::clock_method() {
		clk = static_cast<sc_logic>(clk_g);
	}

#endif

#else

using half_float::half;

int sc_main(int argc, char *argv[]) {
    sc_clock            clk("clk", 10, SC_NS);
    sc_signal<bool>     rst;
    sc_signal<bool>     mult_en;                // Signals that a multiply computation step should be performed
    sc_signal<bool>     add_en;	                // Signals that an add computation step should be performed
    sc_signal<half>     srf_in;					// Scalar operand from SRF
    sc_signal<half>     grfa_in1[SIMD_WIDTH];   // Input 1 from GRF_A
    sc_signal<half>     grfa_in2[SIMD_WIDTH];   // Input 2 from GRF_A
    sc_signal<half>     grfb_in1[SIMD_WIDTH];   // Input 1 from GRF_B
    sc_signal<half>     grfb_in2[SIMD_WIDTH];   // Input 2 from GRF_B
//    sc_signal<half>     even_in[SIMD_WIDTH];    // Input from EVEN_BANK
//    sc_signal<half>     odd_in[SIMD_WIDTH];     // Input from ODD_BANK
    sc_signal<uint8_t>  mult_in1_sel;           // Selects input 1 for addition
    sc_signal<uint8_t>  mult_in2_sel;           // Selects input 2 for multiplication
    sc_signal<uint8_t>  add_in1_sel;            // Selects input 1 for addition
    sc_signal<uint8_t>  add_in2_sel;            // Selects input 2 for multiplication
    sc_signal<bool>     out_sel;                // Selects the output: 0 for adder output, 1 for multiplier output
    sc_signal<half>     output[SIMD_WIDTH];     // Output of the Floating Point Unit

    fpu dtu("FPUnitUnderTest");
    dtu.clk(clk);
    dtu.rst(rst);
    dtu.mult_en(mult_en);
    dtu.add_en(add_en);
    dtu.srf_in(srf_in);
    dtu.mult_in1_sel(mult_in1_sel);
    dtu.mult_in2_sel(mult_in2_sel);
    dtu.add_in1_sel(add_in1_sel);
    dtu.add_in2_sel(add_in2_sel);
    dtu.out_sel(out_sel);
    for (int i = 0; i < SIMD_WIDTH; i++) {
        dtu.grfa_in1[i](grfa_in1[i]);
        dtu.grfa_in2[i](grfa_in2[i]);
        dtu.grfb_in1[i](grfb_in1[i]);
        dtu.grfb_in2[i](grfb_in2[i]);
        // dtu.even_in[i](even_in[i]);
        // dtu.odd_in[i](odd_in[i]);
        dtu.output[i](output[i]);
    }

    fpu_driver driver("Driver");
    driver.rst(rst);
    driver.mult_en(mult_en);
    driver.add_en(add_en);
    driver.srf_in(srf_in);
    driver.mult_in1_sel(mult_in1_sel);
    driver.mult_in2_sel(mult_in2_sel);
    driver.add_in1_sel(add_in1_sel);
    driver.add_in2_sel(add_in2_sel);
    driver.out_sel(out_sel);
    for (int i = 0; i < SIMD_WIDTH; i++) {
        driver.grfa_in1[i](grfa_in1[i]);
        driver.grfa_in2[i](grfa_in2[i]);
        driver.grfb_in1[i](grfb_in1[i]);
        driver.grfb_in2[i](grfb_in2[i]);
        // driver.even_in[i](even_in[i]);
        // driver.odd_in[i](odd_in[i]);
    }

    fpu_monitor monitor("Monitor");
    monitor.clk(clk);
    monitor.rst(rst);
    monitor.mult_en(mult_en);
    monitor.add_en(add_en);
    monitor.srf_in(srf_in);
    monitor.mult_in1_sel(mult_in1_sel);
    monitor.mult_in2_sel(mult_in2_sel);
    monitor.add_in1_sel(add_in1_sel);
    monitor.add_in2_sel(add_in2_sel);
    monitor.out_sel(out_sel);
    for (int i = 0; i < SIMD_WIDTH; i++) {
        monitor.grfa_in1[i](grfa_in1[i]);
        monitor.grfa_in2[i](grfa_in2[i]);
        monitor.grfb_in1[i](grfb_in1[i]);
        monitor.grfb_in2[i](grfb_in2[i]);
        // monitor.even_in[i](even_in[i]);
        // monitor.odd_in[i](odd_in[i]);
        monitor.output[i](output[i]);
    }

    sc_trace_file *tracefile;
    tracefile = sc_create_vcd_trace_file("waveforms/fpu_wave");

    sc_trace(tracefile, clk, "clk");
    sc_trace(tracefile, rst, "rst");
    sc_trace(tracefile, mult_en, "mult_en");
    sc_trace(tracefile, add_en, "add_en");
    sc_trace(tracefile, srf_in, "srf_in");
    sc_trace(tracefile, grfa_in1[0], "grfa_in1[0]");
    sc_trace(tracefile, grfa_in2[0], "grfa_in2[0]");
    sc_trace(tracefile, grfb_in1[0], "grfb_in1[0]");
    sc_trace(tracefile, grfb_in2[0], "grfb_in2[0]");
    sc_trace(tracefile, mult_in1_sel, "mult_in1_sel");
    sc_trace(tracefile, mult_in2_sel, "mult_in2_sel");
    sc_trace(tracefile, add_in1_sel, "add_in1_sel");
    sc_trace(tracefile, add_in2_sel, "add_in2_sel");
    sc_trace(tracefile, out_sel, "out_sel");
    sc_trace(tracefile, dtu.multipliers[0].pipeline[0], "mpipeline[0]");
    sc_trace(tracefile, dtu.multipliers[0].pipeline[1], "mpipeline[1]");
    sc_trace(tracefile, dtu.multipliers[0].pipeline[2], "mpipeline[2]");
    sc_trace(tracefile, dtu.multipliers[0].pipeline[3], "mpipeline[3]");
    sc_trace(tracefile, dtu.multipliers[0].pipeline[4], "mpipeline[4]");
    sc_trace(tracefile, dtu.adders[0].pipeline[0], "apipeline[0]");
    sc_trace(tracefile, dtu.adders[0].pipeline[1], "apipeline[1]");
    sc_trace(tracefile, dtu.adders[0].pipeline[2], "apipeline[2]");
    sc_trace(tracefile, dtu.adders[0].pipeline[3], "apipeline[3]");
    sc_trace(tracefile, dtu.adders[0].pipeline[4], "apipeline[4]");
    sc_trace(tracefile, output[0], "output[0]");

    sc_start(1000, SC_NS);

    sc_close_vcd_trace_file(tracefile);

    return 0;
}

#endif
