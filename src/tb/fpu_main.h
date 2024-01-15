#ifndef FPU_MAIN_H_
#define FPU_MAIN_H_

#include "fpu_driver.h"
#include "fpu_monitor.h"
#include "../half.hpp"

#if MIXED_SIM
#include "../fpu_wrapped.h"
#else
#include "../fpu.h"
#endif

#ifdef MTI_SYSTEMC

using half_float::half;

SC_MODULE(top) {

#if MIXED_SIM

	sc_clock				clk_g;
	sc_signal<sc_logic>		clk;
	sc_signal<sc_logic>     rst;
	sc_buffer<sc_logic>     mult_en;             	// Signals that a multiply computation step should be performed
	sc_signal<sc_logic>		add_en;					// Signals that an add computation step should be performed
	sc_signal<sc_lv<16> >   srf_in;					// Scalar operand from SRF
	sc_signal<sc_lv<16> >   grfa_in1[SIMD_WIDTH];   // Input 1 from GRF_A
	sc_signal<sc_lv<16> >   grfa_in2[SIMD_WIDTH];   // Input 2 from GRF_A
	sc_signal<sc_lv<16> >   grfb_in1[SIMD_WIDTH];   // Input 1 from GRF_B
	sc_signal<sc_lv<16> >   grfb_in2[SIMD_WIDTH];   // Input 2 from GRF_B
	// sc_signal<sc_lv<16> >    even_in[SIMD_WIDTH];    // Input from EVEN_BANK
	// sc_signal<sc_lv<16> >    odd_in[SIMD_WIDTH];     // Input from ODD_BANK
	sc_signal<sc_lv<8> >    mult_in1_sel;           // Selects input 1 for addition
	sc_signal<sc_lv<8> >    mult_in2_sel;           // Selects input 2 for multiplication
	sc_signal<sc_lv<8> >    add_in1_sel;            // Selects input 1 for addition
	sc_signal<sc_lv<8> >    add_in2_sel;            // Selects input 2 for multiplication
	sc_signal<sc_logic>		out_sel;				// Selects the output: 0 for adder output, 1 for multiplier output
	sc_signal<sc_lv<16> >   output[SIMD_WIDTH];     // Output of the Floating Point Unit

	fpu dut;
	fpu_driver driver;
	fpu_monitor monitor;

	SC_CTOR(top) : clk_g("clk", 10, SC_NS), dut("FPUnitUnderTest", "fpu"),
					driver("Driver"), monitor("Monitor")
	{
		SC_METHOD(clock_method);
		sensitive << clk_g;

		dut.clk(clk);
		dut.rst(rst);
		dut.mult_en(mult_en);
		dut.add_en(add_en);
		dut.srf_in(srf_in);
		dut.mult_in1_sel(mult_in1_sel);
		dut.mult_in2_sel(mult_in2_sel);
		dut.add_in1_sel(add_in1_sel);
		dut.add_in2_sel(add_in2_sel);
		dut.out_sel(out_sel);
		dut.grfa_in1_0(grfa_in1[0]);
		dut.grfa_in1_1(grfa_in1[1]);
		dut.grfa_in1_2(grfa_in1[2]);
		dut.grfa_in1_3(grfa_in1[3]);
		dut.grfa_in1_4(grfa_in1[4]);
		dut.grfa_in1_5(grfa_in1[5]);
		dut.grfa_in1_6(grfa_in1[6]);
		dut.grfa_in1_7(grfa_in1[7]);
		dut.grfa_in1_8(grfa_in1[8]);
		dut.grfa_in1_9(grfa_in1[9]);
		dut.grfa_in1_10(grfa_in1[10]);
		dut.grfa_in1_11(grfa_in1[11]);
		dut.grfa_in1_12(grfa_in1[12]);
		dut.grfa_in1_13(grfa_in1[13]);
		dut.grfa_in1_14(grfa_in1[14]);
		dut.grfa_in1_15(grfa_in1[15]);
		dut.grfa_in2_0(grfa_in2[0]);
		dut.grfa_in2_1(grfa_in2[1]);
		dut.grfa_in2_2(grfa_in2[2]);
		dut.grfa_in2_3(grfa_in2[3]);
		dut.grfa_in2_4(grfa_in2[4]);
		dut.grfa_in2_5(grfa_in2[5]);
		dut.grfa_in2_6(grfa_in2[6]);
		dut.grfa_in2_7(grfa_in2[7]);
		dut.grfa_in2_8(grfa_in2[8]);
		dut.grfa_in2_9(grfa_in2[9]);
		dut.grfa_in2_10(grfa_in2[10]);
		dut.grfa_in2_11(grfa_in2[11]);
		dut.grfa_in2_12(grfa_in2[12]);
		dut.grfa_in2_13(grfa_in2[13]);
		dut.grfa_in2_14(grfa_in2[14]);
		dut.grfa_in2_15(grfa_in2[15]);
		dut.grfb_in1_0(grfb_in1[0]);
		dut.grfb_in1_1(grfb_in1[1]);
		dut.grfb_in1_2(grfb_in1[2]);
		dut.grfb_in1_3(grfb_in1[3]);
		dut.grfb_in1_4(grfb_in1[4]);
		dut.grfb_in1_5(grfb_in1[5]);
		dut.grfb_in1_6(grfb_in1[6]);
		dut.grfb_in1_7(grfb_in1[7]);
		dut.grfb_in1_8(grfb_in1[8]);
		dut.grfb_in1_9(grfb_in1[9]);
		dut.grfb_in1_10(grfb_in1[10]);
		dut.grfb_in1_11(grfb_in1[11]);
		dut.grfb_in1_12(grfb_in1[12]);
		dut.grfb_in1_13(grfb_in1[13]);
		dut.grfb_in1_14(grfb_in1[14]);
		dut.grfb_in1_15(grfb_in1[15]);
		dut.grfb_in2_0(grfb_in2[0]);
		dut.grfb_in2_1(grfb_in2[1]);
		dut.grfb_in2_2(grfb_in2[2]);
		dut.grfb_in2_3(grfb_in2[3]);
		dut.grfb_in2_4(grfb_in2[4]);
		dut.grfb_in2_5(grfb_in2[5]);
		dut.grfb_in2_6(grfb_in2[6]);
		dut.grfb_in2_7(grfb_in2[7]);
		dut.grfb_in2_8(grfb_in2[8]);
		dut.grfb_in2_9(grfb_in2[9]);
		dut.grfb_in2_10(grfb_in2[10]);
		dut.grfb_in2_11(grfb_in2[11]);
		dut.grfb_in2_12(grfb_in2[12]);
		dut.grfb_in2_13(grfb_in2[13]);
		dut.grfb_in2_14(grfb_in2[14]);
		dut.grfb_in2_15(grfb_in2[15]);
//		dut.even_in_0(even_in[0]);
//		dut.even_in_1(even_in[1]);
//		dut.even_in_2(even_in[2]);
//		dut.even_in_3(even_in[3]);
//		dut.even_in_4(even_in[4]);
//		dut.even_in_5(even_in[5]);
//		dut.even_in_6(even_in[6]);
//		dut.even_in_7(even_in[7]);
//		dut.even_in_8(even_in[8]);
//		dut.even_in_9(even_in[9]);
//		dut.even_in_10(even_in[10]);
//		dut.even_in_11(even_in[11]);
//		dut.even_in_12(even_in[12]);
//		dut.even_in_13(even_in[13]);
//		dut.even_in_14(even_in[14]);
//		dut.even_in_15(even_in[15]);
//		dut.odd_in_0(odd_in[0]);
//		dut.odd_in_1(odd_in[1]);
//		dut.odd_in_2(odd_in[2]);
//		dut.odd_in_3(odd_in[3]);
//		dut.odd_in_4(odd_in[4]);
//		dut.odd_in_5(odd_in[5]);
//		dut.odd_in_6(odd_in[6]);
//		dut.odd_in_7(odd_in[7]);
//		dut.odd_in_8(odd_in[8]);
//		dut.odd_in_9(odd_in[9]);
//		dut.odd_in_10(odd_in[10]);
//		dut.odd_in_11(odd_in[11]);
//		dut.odd_in_12(odd_in[12]);
//		dut.odd_in_13(odd_in[13]);
//		dut.odd_in_14(odd_in[14]);
//		dut.odd_in_15(odd_in[15]);
		dut.output_0(output[0]);
		dut.output_1(output[1]);
		dut.output_2(output[2]);
		dut.output_3(output[3]);
		dut.output_4(output[4]);
		dut.output_5(output[5]);
		dut.output_6(output[6]);
		dut.output_7(output[7]);
		dut.output_8(output[8]);
		dut.output_9(output[9]);
		dut.output_10(output[10]);
		dut.output_11(output[11]);
		dut.output_12(output[12]);
		dut.output_13(output[13]);
		dut.output_14(output[14]);
		dut.output_15(output[15]);

#else

	sc_clock				clk;
	sc_signal<bool>         rst;
	sc_buffer<bool>         mult_en;             	// Signals that a multiply computation step should be performed
	sc_signal<bool>			add_en;					// Signals that an add computation step should be performed
	sc_signal<half>        	srf_in;					// Scalar operand from SRF
	sc_signal<half>        	grfa_in1[SIMD_WIDTH];   // Input 1 from GRF_A
	sc_signal<half>        	grfa_in2[SIMD_WIDTH];   // Input 2 from GRF_A
	sc_signal<half>        	grfb_in1[SIMD_WIDTH];   // Input 1 from GRF_B
	sc_signal<half>        	grfb_in2[SIMD_WIDTH];   // Input 2 from GRF_B
	// sc_signal<half>        even_in[SIMD_WIDTH];    // Input from EVEN_BANK
	// sc_signal<half>        odd_in[SIMD_WIDTH];     // Input from ODD_BANK
	sc_signal<uint8_t>    	mult_in1_sel;           // Selects input 1 for addition
	sc_signal<uint8_t>    	mult_in2_sel;           // Selects input 2 for multiplication
	sc_signal<uint8_t>    	add_in1_sel;            // Selects input 1 for addition
	sc_signal<uint8_t>    	add_in2_sel;            // Selects input 2 for multiplication
	sc_signal<bool>			out_sel;				// Selects the output: 0 for adder output, 1 for multiplier output
	sc_signal<half>        	output[SIMD_WIDTH];     // Output of the Floating Point Unit

	fpu dut;
	fpu_driver driver;
	fpu_monitor monitor;

	SC_CTOR(top) : clk("clk", 10, SC_NS), dut("FPUnitUnderTest"),
					driver("Driver"), monitor("Monitor")
	{

		dut.clk(clk);
		dut.rst(rst);
		dut.mult_en(mult_en);
		dut.add_en(add_en);
		dut.srf_in(srf_in);
		dut.mult_in1_sel(mult_in1_sel);
		dut.mult_in2_sel(mult_in2_sel);
		dut.add_in1_sel(add_in1_sel);
		dut.add_in2_sel(add_in2_sel);
		dut.out_sel(out_sel);
		for (int i=0; i<SIMD_WIDTH; i++) {
			dut.grfa_in1[i](grfa_in1[i]);
			dut.grfa_in2[i](grfa_in2[i]);
			dut.grfb_in1[i](grfb_in1[i]);
			dut.grfb_in2[i](grfb_in2[i]);
			// dut.even_in[i](even_in[i]);
			// dut.odd_in[i](odd_in[i]);
			dut.output[i](output[i]);
		}

#endif

		driver.rst(rst);
		driver.mult_en(mult_en);
		driver.add_en(add_en);
		driver.srf_in(srf_in);
		driver.mult_in1_sel(mult_in1_sel);
		driver.mult_in2_sel(mult_in2_sel);
		driver.add_in1_sel(add_in1_sel);
		driver.add_in2_sel(add_in2_sel);
		driver.out_sel(out_sel);
		for (int i=0; i<SIMD_WIDTH; i++) {
			driver.grfa_in1[i](grfa_in1[i]);
			driver.grfa_in2[i](grfa_in2[i]);
			driver.grfb_in1[i](grfb_in1[i]);
			driver.grfb_in2[i](grfb_in2[i]);
			// driver.even_in[i](even_in[i]);
			// driver.odd_in[i](odd_in[i]);
		}

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
		for (int i=0; i<SIMD_WIDTH; i++) {
			monitor.grfa_in1[i](grfa_in1[i]);
			monitor.grfa_in2[i](grfa_in2[i]);
			monitor.grfb_in1[i](grfb_in1[i]);
			monitor.grfb_in2[i](grfb_in2[i]);
			// monitor.even_in[i](even_in[i]);
			// monitor.odd_in[i](odd_in[i]);
			monitor.output[i](output[i]);
		}
	}

#if MIXED_SIM
	void clock_method();	// Turns bool clk_g into sc_logic clk
#endif

};

#endif

#endif /* FPU_MAIN_H_ */
