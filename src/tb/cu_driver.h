#include "systemc.h"

#include "../cnm_base.h"

SC_MODULE(cu_driver) {

#if MIXED_SIM

	sc_out<sc_logic>     			rst;
	sc_out<sc_logic>				RD;			// DRAM read command
	sc_out<sc_logic>				WR;			// DRAM write command
	sc_out<sc_logic>				ACT;		// DRAM activate command
//    sc_out<sc_logic>					RSTB;		//
	sc_out<sc_logic>				AB_mode;	// Signals if the All-Banks mode is enabled
	sc_out<sc_logic>				pim_mode;	// Signals if the PIM mode is enabled
	sc_out<sc_lv<BANK_BITS> >		bank_addr;	// Address of the bank
	sc_out<sc_lv<ROW_BITS> >		row_addr;	// Address of the bank row
	sc_out<sc_lv<COL_BITS> >		col_addr;	// Address of the bank column
	sc_out<sc_lv<64> >				DQ;			// Data input from DRAM controller (output makes no sense
	sc_out<sc_lv<32> >				instr;		// Instruction input from CRF

#else

    sc_out<bool>                rst;
    sc_out<bool>                RD;			// DRAM read command
    sc_out<bool>                WR;			// DRAM write command
    sc_out<bool>                ACT;		// DRAM activate command
//    sc_out<bool>						RSTB;		//
    sc_out<bool>                AB_mode;	// Signals if the All-Banks mode is enabled
    sc_out<bool>                pim_mode;	// Signals if the PIM mode is enabled
    sc_out<sc_uint<BANK_BITS> > bank_addr;	// Address of the bank
    sc_out<sc_uint<ROW_BITS> >  row_addr;	// Address of the bank row
    sc_out<sc_uint<COL_BITS> >  col_addr;	// Address of the bank column
    sc_out<uint64_t>            DQ;         // Data input from DRAM controller (output makes no sense
    sc_out<uint32_t>            instr;		// Instruction input from CRF

#endif

    SC_CTOR(cu_driver) {
        SC_THREAD(driver_thread);
    }

    void driver_thread();
};

