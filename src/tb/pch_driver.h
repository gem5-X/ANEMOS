#include "systemc.h"
#include "../cnm_base.h"

#if GEM5
	//Needed libraries for semaphores/shared memory (testbench stuff)
	#include <iostream>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/wait.h>
	#include <sstream>
	#include <sys/mman.h>
	#include <fcntl.h>
	#include <semaphore.h>
	#include <assert.h>
	#include <cstdio>
	#include <cstdlib>
	#include <stdint.h>
	#include <fstream>
    #define RF_START    (1UL << (GLOBAL_OFFSET + CHANNEL_BITS + COL_BITS + RANK_BITS + BG_BITS + BANK_BITS + ROW_BITS - 1))
#endif

class pch_driver: public sc_module {
public:

#if MIXED_SIM

    sc_out<sc_logic>            rst;
    sc_out<sc_logic>            RD;						// DRAM read command
    sc_out<sc_logic>            WR;						// DRAM write command
    sc_out<sc_logic>            ACT;					// DRAM activate command
//    sc_out<sc_logic>				RSTB;					//
    sc_out<sc_logic>            AB_mode;			    // Signals if the All-Banks mode is enabled
    sc_out<sc_logic>            pim_mode;				// Signals if the PIM mode is enabled
    sc_out<sc_lv<BANK_BITS> > 	bank_addr;				// Address of the bank
    sc_out<sc_lv<ROW_BITS> >  	row_addr;			    // Address of the bank row
    sc_out<sc_lv<COL_BITS> >  	col_addr;		        // Address of the bank column
    sc_out<sc_lv<DQ_BITS> >     DQ;                     // Data input from DRAM controller (output makes no sense)
    sc_out<sc_lv<GRF_WIDTH> >	even_in[CORES_PER_PCH];	// Direct data in/out to the even banks
    sc_out<sc_lv<GRF_WIDTH> >   odd_in[CORES_PER_PCH];  // Direct data in/out to the odd banks
    sc_in<sc_lv<GRF_WIDTH> >    even_out[CORES_PER_PCH];// Direct data in/out to the even banks
    sc_in<sc_lv<GRF_WIDTH> >    odd_out[CORES_PER_PCH]; // Direct data in/out to the odd banks

#else

    sc_out<bool>                rst;
    sc_out<bool>                RD;							// DRAM read command
    sc_out<bool>                WR;							// DRAM write command
    sc_out<bool>                ACT;						// DRAM activate command
//    sc_out<bool>				RSTB;						//
    sc_out<bool>                AB_mode;			        // Signals if the All-Banks mode is enabled
    sc_out<bool>                pim_mode;				    // Signals if the PIM mode is enabled
    sc_out<sc_uint<BANK_BITS> > bank_addr;				    // Address of the bank
    sc_out<sc_uint<ROW_BITS> >  row_addr;			        // Address of the bank row
    sc_out<sc_uint<COL_BITS> >  col_addr;		            // Address of the bank column
    sc_out<sc_uint<DQ_BITS> >   DQ;                         // Data input from DRAM controller (output makes no sense)
    sc_inout_rv<GRF_WIDTH>      even_buses[CORES_PER_PCH];  // Direct data in/out to the even banks
    sc_inout_rv<GRF_WIDTH>      odd_buses[CORES_PER_PCH];   // Direct data in/out to the odd banks

#if GEM5
    //Communicating struct with gem5
    typedef struct fileLine{
        uint64_t address;
        uint64_t dataArray[4*CORES_PER_PCH];
        uint64_t issuedTick;
        uint8_t MemTraceOn;
        uint8_t lastCmd;
        uint8_t RDcmd;
    } fileLine;

    void copyFileLine(fileLine* dest, fileLine* src);
	void printFileLine(fileLine fl);

    //Shared memory and semaphores
    const char* semName1 = "/mySemaphore1";
    const char* semName2 = "/mySemaphore2";
    const char* shmName = "/sharedMemory";

#endif

#endif

    std::string filename;

    SC_HAS_PROCESS(pch_driver);
    pch_driver(sc_module_name name_, std::string filename_) : sc_module(name_), filename(filename_) {
        SC_THREAD(driver_thread);
    }

    void driver_thread();
};
