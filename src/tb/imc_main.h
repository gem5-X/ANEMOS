#ifndef IMC_MAIN_H_
#define IMC_MAIN_H_

#include "imc_driver.h"
#include "imc_monitor.h"
#include <string>

#if MIXED_SIM
#include "../imc_wrapped.h"
#else
#include "../imc_core.h"
#endif

#ifdef MTI_SYSTEMC

SC_MODULE(top) {

#if MIXED_SIM

	sc_clock						clk_g;
	sc_signal<sc_logic>				clk;
	sc_signal<sc_logic>     		rst;
	sc_signal<sc_logic>				RD;			// DRAM read command
	sc_signal<sc_logic>				WR;			// DRAM write command
	sc_signal<sc_logic>				ACT;		// DRAM activate command
//    sc_signal<sc_logic>				RSTB;		//
	sc_signal<sc_logic>				AB_mode;	// Signals if the All-Banks mode is enabled
	sc_signal<sc_logic>				pim_mode;	// Signals if the PIM mode is enabled
	sc_signal<sc_lv<BANK_BITS> >	bank_addr;	// Address of the bank
	sc_signal<sc_lv<ROW_BITS> >		row_addr;	// Address of the bank row
	sc_signal<sc_lv<COL_BITS> >		col_addr;	// Address of the bank column
	sc_signal<sc_lv<64> >			DQ;			// Data input from DRAM controller (output makes no sense
	sc_signal<sc_lv<GRF_WIDTH> >	even_in;	// Direct data in/out to the even bank
	sc_signal<sc_lv<GRF_WIDTH> >	odd_in;		// Direct data in/out to the odd bank
	sc_signal<sc_lv<GRF_WIDTH> >	even_out;	// Direct data in/out to the even bank
	sc_signal<sc_lv<GRF_WIDTH> >	odd_out;	// Direct data in/out to the odd bank

	imc_core dut;
	imc_driver driver;
	imc_monitor monitor;

	SC_CTOR(top) : clk_g("clk", CLK_PERIOD, RESOLUTION), dut("IMCCoreUnderTest", "imc_core"),
					driver("Driver"), monitor("Monitor")
	{
		SC_METHOD(clock_method);
		sensitive << clk_g;

		dut.clk(clk);
		dut.rst(rst);
		dut.RD(RD);
		dut.WR(WR);
		dut.ACT(ACT);
	//    dut.RSTB(RSTB);
		dut.AB_mode(AB_mode);
		dut.pim_mode(pim_mode);
		dut.bank_addr(bank_addr);
		dut.row_addr(row_addr);
		dut.col_addr(col_addr);
		dut.DQ(DQ);
	    dut.even_in(even_in);
	    dut.odd_in(odd_in);
	    dut.even_out(even_out);
	    dut.odd_out(odd_out);

		driver.rst(rst);
		driver.RD(RD);
		driver.WR(WR);
		driver.ACT(ACT);
	//    driver.RSTB(RSTB);
		driver.AB_mode(AB_mode);
		driver.pim_mode(pim_mode);
		driver.bank_addr(bank_addr);
		driver.row_addr(row_addr);
		driver.col_addr(col_addr);
		driver.DQ(DQ);
		driver.even_in(even_in);
		driver.odd_in(odd_in);

		monitor.clk(clk);
		monitor.rst(rst);
		monitor.RD(RD);
		monitor.WR(WR);
		monitor.ACT(ACT);
	//    monitor.RSTB(RSTB);
		monitor.AB_mode(AB_mode);
		monitor.pim_mode(pim_mode);
		monitor.bank_addr(bank_addr);
		monitor.row_addr(row_addr);
		monitor.col_addr(col_addr);
		monitor.DQ(DQ);
		monitor.even_in(even_in);
		monitor.odd_in(odd_in);
		monitor.even_out(even_out);
		monitor.odd_out(odd_out);

	}

	void clock_method();	// Turns bool clk_g into sc_logic clk

#else

	sc_clock       					clk;
	sc_signal<bool>     			rst;
	sc_signal<bool>					RD;			// DRAM read command
	sc_signal<bool>					WR;			// DRAM write command
	sc_signal<bool>					ACT;		// DRAM activate command
//    sc_signal<bool>					RSTB;		//
	sc_signal<bool>					AB_mode;	// Signals if the All-Banks mode is enabled
	sc_signal<bool>					pim_mode;	// Signals if the PIM mode is enabled
	sc_signal<sc_uint<BANK_BITS> >	bank_addr;	// Address of the bank
	sc_signal<sc_uint<ROW_BITS> >	row_addr;	// Address of the bank row
	sc_signal<sc_uint<COL_BITS> >	col_addr;	// Address of the bank column
	sc_signal<uint64_t>				DQ;			// Data input from DRAM controller (output makes no sense
	sc_signal_rv<GRF_WIDTH>			even_bus;	// Direct data in/out to the even bank
	sc_signal_rv<GRF_WIDTH>			odd_bus;	// Direct data in/out to the odd bank

	imc_core dut;
	imc_driver driver;
	imc_monitor monitor;

	SC_CTOR(top) : clk("clk", CLK_PERIOD, RESOLUTION), dut("IMCCoreUnderTest", 1),
					driver("Driver"), monitor("Monitor")
	{
		dut.clk(clk);
		dut.rst(rst);
		dut.RD(RD);
		dut.WR(WR);
		dut.ACT(ACT);
	//    dut.RSTB(RSTB);
		dut.AB_mode(AB_mode);
		dut.pim_mode(pim_mode);
		dut.bank_addr(bank_addr);
		dut.row_addr(row_addr);
		dut.col_addr(col_addr);
		dut.DQ(DQ);
	    dut.even_bus(even_bus);
	    dut.odd_bus(odd_bus);

		driver.rst(rst);
		driver.RD(RD);
		driver.WR(WR);
		driver.ACT(ACT);
	//    driver.RSTB(RSTB);
		driver.AB_mode(AB_mode);
		driver.pim_mode(pim_mode);
		driver.bank_addr(bank_addr);
		driver.row_addr(row_addr);
		driver.col_addr(col_addr);
		driver.DQ(DQ);
	    driver.even_bus(even_bus);
	    driver.odd_bus(odd_bus);

		monitor.clk(clk);
		monitor.rst(rst);
		monitor.RD(RD);
		monitor.WR(WR);
		monitor.ACT(ACT);
	//    monitor.RSTB(RSTB);
		monitor.AB_mode(AB_mode);
		monitor.pim_mode(pim_mode);
		monitor.bank_addr(bank_addr);
		monitor.row_addr(row_addr);
		monitor.col_addr(col_addr);
		monitor.DQ(DQ);
	    monitor.even_bus(even_bus);
	    monitor.odd_bus(odd_bus);
	}

#endif

};

#endif

#endif /* IMC_MAIN_H_ */
