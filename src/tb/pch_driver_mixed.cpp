#include "../cnm_base.h"

#if MIXED_SIM	// Testbench for SystemC simulation

#include "pch_driver.h"

#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <deque>
#include <assert.h>

using namespace std;

void pch_driver::driver_thread() {

	uint clk_period = CLK_PERIOD;
	uint first_half_period = uint((CLK_PERIOD / 20) * 10);
    uint second_half_period = clk_period - first_half_period;
    int i, j, curCycle, DQCycle;
#if INSTR_CLK > 1
    int instrCycle;
#endif
    sc_biguint<GRF_WIDTH> bankAux;
    sc_uint<DQ_BITS> bank2out;
    sc_lv<GRF_WIDTH> allzs(SC_LOGIC_Z);
    sc_logic lastCmd, bankRead, bankWrite;

    sc_uint<ADDR_TOTAL_BITS> addrAux;

    // Values for reading from input
    string line;
    int readCycle = 0;
    unsigned long int readAddr;
    dq_type dataAux, data2DQ, data2bankAux;
    sc_biguint<GRF_WIDTH> data2bank;
    string readCmd;
    dq_type data2DQAux[DQ_CLK];
#if INSTR_CLK > 1
    dq_type instr2DQAux[INSTR_CLK];
#endif
    deque<dq_type> readData;
    deque<sc_biguint<GRF_WIDTH> > data2bankBuffer;

    // Mixed language auxiliars
	sc_uint<BA_STA-BA_END+1> bank_aux = 0;
	sc_uint<RO_STA-RO_END+1> row_aux = 0;
	sc_uint<CO_STA-CO_END+1> col_aux = 0;

    // Initial reset
    curCycle = 0;
    DQCycle = 0;
#if INSTR_CLK > 1
    instrCycle = 0;
#endif
    lastCmd = SC_LOGIC_0;
    bankRead = SC_LOGIC_0;
    bankWrite = SC_LOGIC_0;
    rst->write(SC_LOGIC_0);
    RD->write(SC_LOGIC_0);
    WR->write(SC_LOGIC_0);
    ACT->write(SC_LOGIC_0);
    AB_mode->write(SC_LOGIC_0);
    pim_mode->write(SC_LOGIC_0);
    bank_addr->write(0);
    row_addr->write(0);
    col_addr->write(0);
    DQ->write(0);
    for (i = 0; i < CORES_PER_PCH; i++) {
        even_in[i]->write(allzs);
        odd_in[i]->write(allzs);
    }

    wait(clk_period / 2, RESOLUTION);
    rst->write(SC_LOGIC_1);
    wait(0, RESOLUTION);

    wait(clk_period / 2 + clk_period / 10, RESOLUTION);
    curCycle++;

    // Open input file
    string fi = "../TB_FILES/input/" + filename + ".sci0";	// Input file name, located in pim-cores folder
    ifstream input;
    input.open(fi);
    if (!input.is_open())   {
        cout << "Error when opening input file" << endl;
        sc_stop();
        return;
    }

    // Open output file
    string fo = "../TB_FILES/output/" + filename + ".results";	// Output file name, located in pim-cores folder
    ofstream output;
    output.open(fo);
    if (!output.is_open())   {
        cout << "Error when opening output file" << endl;
        sc_stop();
        return;
    }

    // Read first line
    if (getline(input, line)) {

        // Read elements from a line in the input file
        istringstream iss(line);
        readData.clear();
        if (!(iss >> dec >> readCycle >> hex >> readAddr >> readCmd)) {
            cout << "Error when reading input" << endl;
            sc_stop();
            return;
        }
        while (iss >> hex >> dataAux) {
            readData.push_back(dataAux);
        }

    } else {
        cout << "No lines in the input file" << endl;
        sc_stop();
        return;
    }

    // Simulation loop
    while (1) {

//    	cout << "Cycle " << dec << curCycle << endl;

        // Default values
        RD->write(SC_LOGIC_0);
        WR->write(SC_LOGIC_0);
        ACT->write(SC_LOGIC_0);
        AB_mode->write(SC_LOGIC_1);
        pim_mode->write(SC_LOGIC_1);
        DQ->write(0);
        for (i = 0; i < CORES_PER_PCH; i++) {
            even_in[i]->write(allzs);
            odd_in[i]->write(allzs);
        }

        // Keep writing to DQ to finish GRF writing
        if (DQCycle) {
            DQ->write(data2DQAux[DQCycle]);
            DQCycle++;
        }
#if INSTR_CLK > 1
        // Keep writing to DQ to finish GRF writing
        if (instrCycle) {
            DQ->write(instr2DQAux[instrCycle]);
            instrCycle++;
        }
#endif

        // Fill the banks' sense amplifiers with the corresponding data
        if (bankRead == SC_LOGIC_1) {
            if (addrAux.range(BA_END, BA_END)) {
                for (i = 0; i < CORES_PER_PCH; i++) {
                    odd_in[i]->write(data2bankBuffer.front());
                    data2bankBuffer.pop_front();
                }
            } else {
                for (i = 0; i < CORES_PER_PCH; i++) {
                    even_in[i]->write(data2bankBuffer.front());
                    data2bankBuffer.pop_front();
                }
            }
            bankRead = SC_LOGIC_0;
        }

        // Execute necessary command at the right time and read next line
        // if current cycle caught up with the previous read one
        if (lastCmd == SC_LOGIC_0 && curCycle >= readCycle) {

            // Execute command at the corresponding cycle, assuming PIM mode

            assert(!DQCycle);// A write to a GRF shouldn't overlap with next command
#if INSTR_CLK > 1
            assert(!instrCycle);// A write to CRF shouldn't overlap with next command
#endif

            // Check first the MSB of the row address to see which mode we're in
            // (writing to RFs or PIM execution)
            addrAux = readAddr;
            if (addrAux.range(RO_STA, RO_STA)) {
                // Writing to the RFs

                // Check command
                if (!readCmd.compare("RD")) {

                    // If writing to RFs and RD, do nothing
                    cout << "Warning: RF writing mode but saw a RD command"
                            << endl;

                } else {

                    // If writing to RFs and WR, format data to DQ

#if INSTR_CLK > 1
                    if(addrAux.range(RO_STA - 1, RO_END) == RF_CRF) {
                        assert(readData.size() == INSTR_CLK);
                        for (i = 0; i < INSTR_CLK; i++) {
                            instr2DQAux[i] = readData.front();
                            readData.pop_front();
                        }
                        WR->write(SC_LOGIC_1);
                        bank_aux = addrAux.range(BA_STA, BA_END);
                        bank_addr->write(bank_aux);
                        row_aux = addrAux.range(RO_STA, RO_END);
						row_addr->write(row_aux);
						col_aux = addrAux.range(CO_STA, CO_END);
                        col_addr->write(col_aux);
                        DQ->write(instr2DQAux[instrCycle]);
                        instrCycle++;// Increase Instr cycle to know a write to  CRF is ongoing
                    } else
#endif
                    if (addrAux.range(RO_STA - 1, RO_END) < RF_GRF_A) {	// Writing to CRF or SRF, one cycle is enough

                        assert(readData.size() == 1);   // Check it is only one piece of data
                        data2DQ = readData.front();
//                        cout << "data pop " << showbase << hex << data2DQ << endl;
                        readData.pop_front();
                        WR->write(SC_LOGIC_1);
                        bank_aux = addrAux.range(BA_STA, BA_END);
                        bank_addr->write(bank_aux);
                        row_aux = addrAux.range(RO_STA, RO_END);
						row_addr->write(row_aux);
						col_aux = addrAux.range(CO_STA, CO_END);
                        col_addr->write(col_aux);
                        DQ->write(data2DQ);

                    } else {	// Writing to GRF, 4 cycles are needed

                        assert(readData.size() == DQ_CLK);   // Check it is only one piece of data, in 4 ints
                        for (i = 0; i < DQ_CLK; i++) {
                            data2DQAux[i] = readData.front();
//                            cout << "data pop " << showbase << hex << data2DQAux[i] << endl;
                            readData.pop_front();
                        }
                        WR->write(SC_LOGIC_1);
                        bank_aux = addrAux.range(BA_STA, BA_END);
                        bank_addr->write(bank_aux);
                        row_aux = addrAux.range(RO_STA, RO_END);
						row_addr->write(row_aux);
						col_aux = addrAux.range(CO_STA, CO_END);
                        col_addr->write(col_aux);
                        DQ->write(data2DQAux[DQCycle]);
                        DQCycle++;// Increase DQ cycle to know a write to GRF is ongoing

                    }
                }

            } else {
                // PIM execution

                // Check command
                if (!readCmd.compare("RD")) {

                    RD->write(SC_LOGIC_1);
                    bank_aux = addrAux.range(BA_STA, BA_END);
                    bank_addr->write(bank_aux);
                    row_aux = addrAux.range(RO_STA, RO_END);
					row_addr->write(row_aux);
					col_aux = addrAux.range(CO_STA, CO_END);
                    col_addr->write(col_aux);
                    // If PIM execution and RD with input data, send to the corresponding bank buses in the next cycle
                    if (!readData.empty()){
						assert(readData.size() == DQ_CLK*CORES_PER_PCH);// Check if there are enough pieces of data

						for (i = 0; i < CORES_PER_PCH; i++) {
							for (j = 0; j < DQ_CLK; j++){
								data2bankAux = readData.front();
//								cout << "data pop " << showbase << hex << data2bankAux << endl;
								readData.pop_front();
								data2bank.range(DQ_BITS*(j+1)-1,DQ_BITS*j) = data2bankAux;
							}
							data2bankBuffer.push_back(data2bank);
						}

						bankRead = SC_LOGIC_1;
                    }

                } else {

                    // If PIM execution and WR, we record bank buses next cycle
                    WR->write(SC_LOGIC_1);
                    bank_aux = addrAux.range(BA_STA, BA_END);
                    bank_addr->write(bank_aux);
                    row_aux = addrAux.range(RO_STA, RO_END);
					row_addr->write(row_aux);
					col_aux = addrAux.range(CO_STA, CO_END);
                    col_addr->write(col_aux);
                    bankWrite = SC_LOGIC_1;

                    // Write address here because it will be overwritten later with the next cmd
                    output << showbase << dec << curCycle << "\t" << hex << readAddr << "\t";

                }
            }

            // Read next line
            if (getline(input, line)) {

                // Read elements from a line in the input file
                istringstream iss(line);
                readData.clear();
                if (!(iss >> dec >> readCycle >> hex >> readAddr >> readCmd)) {
                    cout << "Error when reading input" << endl;
                    break;
                }
                while (iss >> hex >> dataAux) {
                    readData.push_back(dataAux);
//                    cout << "data push " << showbase << hex << dataAux << endl;
                }

            } else {// Wait for enough time for the last instruction to be completed
                input.close();
                lastCmd = SC_LOGIC_1;
                readCycle += (3 + MULT_STAGES + ADD_STAGES);
            }

        } else if (lastCmd == SC_LOGIC_1 && curCycle >= readCycle) {
            // End of simulation, last command was already read
            output.close();
            break;
        }

        // Reset DQCycle if writing to the GRF has ended
        if (DQCycle > DQ_CLK-1) {
            DQCycle = 0;
        }
#if INSTR_CLK > 1
        // Reset instrCycle if writing to the CRF has ended
        if (instrCycle > INSTR_CLK-1) {
            instrCycle = 0;
        }
#endif

        wait (first_half_period, RESOLUTION);
        if (bankWrite == SC_LOGIC_1) {	// TODO translate hex to half
            if (addrAux.range(BA_END, BA_END)) {
                for (i = 0; i < CORES_PER_PCH; i++) {
                    bankAux = odd_out[i]->read();
                    for (j = 0; j < DQ_CLK; j++) {
                        bank2out = bankAux.range(DQ_BITS*(j+1)-1,DQ_BITS*j);
                        output << showbase << hex << bank2out << "\t";
                    }
                }
            } else {
                for (i = 0; i < CORES_PER_PCH; i++) {
                    bankAux = even_out[i]->read();
                    for (j = 0; j < DQ_CLK; j++) {
                        bank2out = bankAux.range(DQ_BITS*(j+1)-1,DQ_BITS*j);
                        output << showbase << hex << bank2out << "\t";
                    }
                }
            }
            output << endl;
            bankWrite = SC_LOGIC_0;
        }

        wait(second_half_period, RESOLUTION);
        curCycle++;
    }

    cout << "Simulation finished at cycle " << dec << curCycle << endl;

    // Stop simulation
    sc_stop();

}

#endif
