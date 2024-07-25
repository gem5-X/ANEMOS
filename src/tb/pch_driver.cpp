#include "../cnm_base.h"

#if MIXED_SIM == 0	// Testbench for SystemC simulation
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


    int i, j, DQCycle;
    uint curCycle;
#if INSTR_CLK > 1
    int instrCycle;
#endif
    sc_biguint<GRF_WIDTH> bankAux;
    sc_uint<DQ_BITS> bank2out;
    sc_lv<GRF_WIDTH> allzs(SC_LOGIC_Z);
    bool lastCmd, bankRead, bankWrite;

    sc_uint<ADDR_TOTAL_BITS> addrAux;

    // Values for reading from input
    string line;
    uint64_t readCycle = 0;
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

    // Initial reset
    curCycle = 0;
    DQCycle = 0;
#if INSTR_CLK > 1
    instrCycle = 0;
#endif
    lastCmd = false;
    bankRead = false;
    bankWrite = false;
    rst->write(false);
    RD->write(false);
    WR->write(false);
    ACT->write(false);
    AB_mode->write(false);
    pim_mode->write(false);
    bank_addr->write(0);
    row_addr->write(0);
    col_addr->write(0);
    DQ->write(0);
    for (i = 0; i < CORES_PER_PCH; i++) {
        even_buses[i]->write(allzs);
        odd_buses[i]->write(allzs);
    }

    wait(CLK_PERIOD / 2, RESOLUTION);
    rst->write(1);
    wait(0, RESOLUTION);

    wait(CLK_PERIOD / 2 + 1, RESOLUTION);
    curCycle++;

    // Open input file
#ifdef MTI_SYSTEMC
    string fi = "../TB_FILES/input/" + filename + ".sci0";  // Input file name, located in pim-cores folder
#else
    string fi = "inputs/SystemC/" + filename + ".sci0";		// Input file name, located in pim-cores folder
#endif
    ifstream input;
    input.open(fi);
    if (!input.is_open())   {
        cout << "Error when opening input file " << endl;
        cout << filename << endl;
        sc_stop();
        return;
    }

    // Open output file
    string fo = "inputs/results/" + filename + ".results";	// Output file name, located in pim-cores folder
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

        // Default values
        RD->write(false);
        WR->write(false);
        ACT->write(false);
        AB_mode->write(true);
        pim_mode->write(true);
        DQ->write(0);
        for (i = 0; i < CORES_PER_PCH; i++) {
            even_buses[i]->write(allzs);
            odd_buses[i]->write(allzs);
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
        if (bankRead) {
            if (addrAux.range(BA_END, BA_END)) {
                for (i = 0; i < CORES_PER_PCH; i++) {
                    odd_buses[i]->write(data2bankBuffer.front());
                    data2bankBuffer.pop_front();
                }
            } else {
                for (i = 0; i < CORES_PER_PCH; i++) {
                    even_buses[i]->write(data2bankBuffer.front());
                    data2bankBuffer.pop_front();
                }
            }
            bankRead = false;
        }


        // Execute necessary command at the right time and read next line
        // if current cycle caught up with the previous read one
        if (!lastCmd && curCycle >= readCycle) {

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
                        WR->write(true);
                        bank_addr->write(addrAux.range(BA_STA, BA_END));
                        row_addr->write(addrAux.range(RO_STA, RO_END));
                        col_addr->write(addrAux.range(CO_STA, CO_END));
                        DQ->write(instr2DQAux[instrCycle]);
                        instrCycle++;// Increase Instr cycle to know a write to  CRF is ongoing
                    } else
#endif
                    if (addrAux.range(RO_STA - 1, RO_END) < RF_GRF_A) {	// Writing to CRF or SRF, one cycle is enough
                    	// removed this since I pass 4 datas but only the first one is valid so it should be okay for now
                        //assert(readData.size() == 1);   // Check it is only one piece of data
                        data2DQ = readData.front();
                        readData.pop_front();
                        WR->write(true);
                        bank_addr->write(addrAux.range(BA_STA, BA_END));
                        row_addr->write(addrAux.range(RO_STA, RO_END));
                        col_addr->write(addrAux.range(CO_STA, CO_END));
                        DQ->write(data2DQ);

                    } else {	// Writing to GRF, DQ_CLK cycles are needed

                        assert(readData.size() == DQ_CLK);   // Check it is only one piece of data, in 4 ints
                        for (i = 0; i < DQ_CLK; i++) {
                            data2DQAux[i] = readData.front();
                            readData.pop_front();
                        }
                        WR->write(true);
                        bank_addr->write(addrAux.range(BA_STA, BA_END));
                        row_addr->write(addrAux.range(RO_STA, RO_END));
                        col_addr->write(addrAux.range(CO_STA, CO_END));
                        DQ->write(data2DQAux[DQCycle]);
                        DQCycle++;// Increase DQ cycle to know a write to GRF is ongoing

                    }
                }

            } else {
                // PIM execution

                // Check command
                if (!readCmd.compare("RD")) {

                    RD->write(true);
                    bank_addr->write(addrAux.range(BA_STA, BA_END));
                    row_addr->write(addrAux.range(RO_STA, RO_END));
                    col_addr->write(addrAux.range(CO_STA, CO_END));

                    // If PIM execution and RD with input data, send to the corresponding bank buses in the next cycle
                    if (!readData.empty()){
						assert(readData.size() == DQ_CLK*CORES_PER_PCH);// Check if there are enough pieces of data

						for (i = 0; i < CORES_PER_PCH; i++) {
							for (j = 0; j < DQ_CLK; j++){
								data2bankAux = readData.front();
								readData.pop_front();
								data2bank.range(DQ_BITS*(j+1)-1,DQ_BITS*j) = data2bankAux;
							}
							data2bankBuffer.push_back(data2bank);
						}

						bankRead = true;
                    }

                } else {

                    // If PIM execution and WR, we record bank buses next cycle
                    WR->write(true);
                    bank_addr->write(addrAux.range(BA_STA, BA_END));
                    row_addr->write(addrAux.range(RO_STA, RO_END));
                    col_addr->write(addrAux.range(CO_STA, CO_END));
                    bankWrite = true;

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
                }

            } else {// Wait for enough time for the last instruction to be completed
                input.close();
                lastCmd = true;
                readCycle += (3 + MULT_STAGES + ADD_STAGES);
            }

        } else if (lastCmd && curCycle >= readCycle) {
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

        if (bankWrite) {	// TODO translate hex to half
            for (i = 0; i < 10; i++)    // More than one deltas are needed
                wait(0, RESOLUTION);    // We need to wait for a delta to solve the bank buses
            if (addrAux.range(BA_END, BA_END)) {
                for (i = 0; i < CORES_PER_PCH; i++) {
                    bankAux = odd_buses[i]->read();
                    for (j = 0; j < DQ_CLK; j++) {
                        bank2out = bankAux.range(DQ_BITS*(j+1)-1,DQ_BITS*j);
                        output << showbase << hex << bank2out << "\t";
                    }
                }
            } else {
                for (i = 0; i < CORES_PER_PCH; i++) {
                    bankAux = even_buses[i]->read();
                    for (j = 0; j < DQ_CLK; j++) {
                        bank2out = bankAux.range(DQ_BITS*(j+1)-1,DQ_BITS*j);
                        output << showbase << hex << bank2out << "\t";
                    }
                }
            }
            output << endl;

            bankWrite = false;
        }

        wait(CLK_PERIOD, RESOLUTION);
        curCycle++;
    }

    cout << "Simulation finished at cycle " << dec << curCycle << endl;

    // Stop simulation
    sc_stop();

}
#endif
