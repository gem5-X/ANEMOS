/*
 * Copyright EPFL 2021
 * Rafael Medina Morillas
 *
 * Description of a pseudo-Channel that contains several IMC cores
 *
 */

#ifndef IMC_PCH_H_
#define IMC_PCH_H_

#include "imc_core.h"

class imc_pch: public sc_module {
public:

#ifdef __SYNTHESIS__

    sc_in_clk						clk;
    sc_in<bool>                 	rst;
    sc_in<bool>                 	RD;							// DRAM read command
    sc_in<bool>                 	WR;							// DRAM write command
    sc_in<bool>                 	ACT;						// DRAM activate command
//    sc_in<bool>						RSTB;						//
    sc_in<bool>                 	AB_mode;			        // Signals if the All-Banks mode is enabled
    sc_in<bool>                 	pim_mode;				    // Signals if the PIM mode is enabled
    sc_in<sc_uint<BANK_BITS> >  	bank_addr;				    // Address of the bank
    sc_in<sc_uint<ROW_BITS> >   	row_addr;				    // Address of the bank row
    sc_in<sc_uint<COL_BITS> >   	col_addr;			        // Address of the bank column
    sc_in<sc_uint<DQ_BITS> >        DQ;	                        // Data input from DRAM controller (output makes no sense)
    sc_in<sc_biguint<GRF_WIDTH> >	even_in[CORES_PER_PCH];		// Direct data in/out to the even bank
	sc_in<sc_biguint<GRF_WIDTH> >	odd_in[CORES_PER_PCH];		// Direct data in/out to the odd bank
	sc_out<sc_biguint<GRF_WIDTH> >	even_out[CORES_PER_PCH];	// Direct data in/out to the even bank
	sc_out<sc_biguint<GRF_WIDTH> >	odd_out[CORES_PER_PCH];		// Direct data in/out to the odd bank


    // ** INTERNAL SIGNALS AND VARIABLES **

    // Auxiliar signals

    // Internal modules

    SC_CTOR(imc_pch) {

        uint i;

        imc_core *imc_cores[CORES_PER_PCH];	// Vector of IMC cores

        for (i = 0; i < CORES_PER_PCH; i++) {
            imc_cores[i] = new imc_core(sc_gen_unique_name("imc_core"));
            imc_cores[i]->clk(clk);
            imc_cores[i]->rst(rst);
            imc_cores[i]->RD(RD);
            imc_cores[i]->WR(WR);
            imc_cores[i]->ACT(ACT);
//			imc_cores[i]->RSTB(RSTB);
            imc_cores[i]->AB_mode(AB_mode);
            imc_cores[i]->pim_mode(pim_mode);
            imc_cores[i]->bank_addr(bank_addr);
            imc_cores[i]->row_addr(row_addr);
            imc_cores[i]->col_addr(col_addr);
            imc_cores[i]->DQ(DQ);
            imc_cores[i]->even_in(even_in[i]);
            imc_cores[i]->odd_in(odd_in[i]);
            imc_cores[i]->even_out(even_out[i]);
            imc_cores[i]->odd_out(odd_out[i]);
        }

    }

#else

    sc_in_clk					clk;
    sc_in<bool>                 rst;
    sc_in<bool>                 RD;							// DRAM read command
    sc_in<bool>                 WR;							// DRAM write command
    sc_in<bool>                 ACT;						// DRAM activate command
//    sc_in<bool>					RSTB;						//
    sc_in<bool>                 AB_mode;			        // Signals if the All-Banks mode is enabled
    sc_in<bool>                 pim_mode;				    // Signals if the PIM mode is enabled
    sc_in<sc_uint<BANK_BITS> >  bank_addr;				    // Address of the bank
    sc_in<sc_uint<ROW_BITS> >   row_addr;				    // Address of the bank row
    sc_in<sc_uint<COL_BITS> >   col_addr;			        // Address of the bank column
    sc_in<sc_uint<DQ_BITS> >    DQ;	                        // Data input from DRAM controller (output makes no sense)
    sc_inout_rv<GRF_WIDTH>      even_buses[CORES_PER_PCH];  // Direct data in/out to the even banks
    sc_inout_rv<GRF_WIDTH>      odd_buses[CORES_PER_PCH];   // Direct data in/out to the odd banks

    // ** INTERNAL SIGNALS AND VARIABLES **

    // Auxiliar signals

    // Internal modules
    imc_core *imc_cores[CORES_PER_PCH];	// Vector of IMC cores

    SC_CTOR(imc_pch) {

        uint i;

        for (i = 0; i < CORES_PER_PCH; i++) {
            imc_cores[i] = new imc_core(sc_gen_unique_name("imc_core"));
            imc_cores[i]->clk(clk);
            imc_cores[i]->rst(rst);
            imc_cores[i]->RD(RD);
            imc_cores[i]->WR(WR);
            imc_cores[i]->ACT(ACT);
//			imc_cores[i]->RSTB(RSTB);
            imc_cores[i]->AB_mode(AB_mode);
            imc_cores[i]->pim_mode(pim_mode);
            imc_cores[i]->bank_addr(bank_addr);
            imc_cores[i]->row_addr(row_addr);
            imc_cores[i]->col_addr(col_addr);
            imc_cores[i]->DQ(DQ);
            imc_cores[i]->even_bus(even_buses[i]);
            imc_cores[i]->odd_bus(odd_buses[i]);
        }

    }

#endif
#if __SYNTHESIS__

#endif

};

#endif /* IMC_PCH_H_ */
