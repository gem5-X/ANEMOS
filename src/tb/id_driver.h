#include "systemc.h"

#include "../cnm_base.h"

SC_MODULE(id_driver) {
    sc_out<bool>                rst;
    sc_out<bool>                rf_access;	// Enables rd/wr to the RFs while not in PIM mode
//	sc_out<bool>							rf_wr_nrd;	// Signals if reading (low) or writing (high)
    sc_out<bool>                decode_en;	// Enables decoding of the next CRF instruction
    sc_out<uint8_t>             pc_in;		// Program Counter
    sc_out<uint32_t>            instr;		// Instruction input from CRF
    sc_out<sc_uint<ROW_BITS> >  row_addr;	// Address of the bank row
    sc_out<sc_uint<COL_BITS> >  col_addr;	// Address of the bank column

    SC_CTOR(id_driver) {
        SC_THREAD(driver_thread);
    }

    void driver_thread();
};
