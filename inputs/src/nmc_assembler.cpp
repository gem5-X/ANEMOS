#include "nmc_assembler.h"

// Format of assembly input:    Inst        [Operands]
// Format of raw traces:        Address     R/W         Data

enum class DefaultCmd : int {defaultRD, defaultWR, defaultInherit};

int main(int argc, const char *argv[])
{ 
    if (argc < 3 || argc > 5) {
        cout << "Usage: " << argv[0] << " <assembly-input> <raw-output> [<data-input> <address-input]" << endl;
        return 0;
    }

    string ai = argv[1];        // Input assembly file name
    string ro = argv[2];        // Output raw hexadecimal code
    string ailine;

    // Open input and output files
    ifstream assembly;
    ifstream dataFile;
    ifstream addrFile;
    ofstream rawSeq;
    assembly.open(ai);
    rawSeq.open(ro);

    // Prepare data input if needed
    if (argc == 5) {
        string di = argv[3];    // Input data file name
        string ad = argv[4];    // Input address file name
        dataFile.open(di);
        addrFile.open(ad);
    }

    bool error = false;
    DefaultCmd defaultCmd = DefaultCmd::defaultRD;  // Change here for the defaulr cmd policy

    // Variables for assembly decoding
    uint8_t instrType, storeType;
    string instrTypeString, storeTypeString;
    uint64_t idx;
    string dataString, diline;
    float dataFloat;
#if HALF_FLOAT
    half_float::half dataHalf;
#else
    cnm_union dataVal;
#endif
    uint16_t dataBin;
    deque<rfBin_t> rfBin;
    dq_type dataAux;
    deque<dq_type> rfData;

    // Variables for writing to CRF
    std::array<nmcInst, CRF_ENTRIES> crfInstr;
    uint8_t crfIdx;
    bool crfWrMode = false;
    string dstAux, src0Aux, src1Aux, src2Aux;

    // Variables for writing EXEC triggers
    uint8_t execIdx = 0;
    bool execStop;
    string memCmd;
    uint16_t lastRow, lastCol;
    string lastMemCmd;
    bool jmpAct;
    uint16_t jmpNum;

    // Variables to write the raw sequence
    uint64_t addr;

    // Run through assembly input
    while (getline(assembly, ailine) && !error) {

        rfBin.clear();
        rfData.clear();

        // Don't process comment lines or emptly lines
        if(ailine.length() != 0 && !(ailine.at(0) == ';')){
            
            istringstream aistream(ailine);

            // Check instruction type
            if (!(aistream >> instrTypeString)) {
                cout << "Error when reading instruction type" << endl;
                error = true;
                break;
            }
            try{ instrType = STRING2INSTR.at(instrTypeString); }
            catch (const std::out_of_range& oor) {
                cout << "Error when reading instruction type" << endl;
                error = true;
                break;
            }

            switch (instrType) {

                // If Write to RF (WRF), check target RF
                case (INSTR::WRF):

                    // Split storage ID and index
                    if (!(aistream >> storeTypeString)) {
                        cout << "Error when reading which Register File to write to" << endl;
                        error = true;
                        break;
                    }
                    splitStoreIndex(&storeTypeString, &idx);

                    try{ storeType = STRING2STORE.at(storeTypeString); }
                    catch (const std::out_of_range& oor) {
                        cout << "Error when reading which Register File to write to" << endl;
                        error = true;
                        break;
                    }

                    // If WRF to CRF, start CRF writing mode starting at index n, checking if it's valid
                    if (storeType == STORE::CRF) {
                        if (idx < CRF_ENTRIES) {
                            crfWrMode = true;
                            crfIdx = idx;
                        } else {
                            cout << "Error, starting to write out of CRF range" << endl;
                            error = true;
                            break;
                        }

                    // Else, write to output the correct {A,R/W,D}
                    } else {
                        
                        // Obtain the data to write to the RF
                        if (aistream >> dataString) {
                            // Read from data file
                            if (!dataString.compare(DATAFILE)){
                                do {
                                    if (!getline(dataFile, diline)) {
                                        cout << "Error, data file cannot be read" << endl;
                                        error = true;   // Signal error
                                        break;
                                    }
                                } while (diline.length() == 0  || diline.at(0) == '#');
                                istringstream distream(diline);
#if (HALF_FLOAT)
                                while (distream >> dataFloat) {  
                                    dataHalf = dataFloat;
                                    dataBin = dataHalf.bin_word();
                                    rfBin.push_back(dataBin);
                                }
#else 
#if DATA_TYPE == 4
                                int int8Aux;
                                while(distream >> int8Aux){
                                    dataVal.data = int8Aux;
                                    rfBin.push_back(dataVal.bin);
                                }
#else
                                while(distream >> dataVal.data){   
                                    rfBin.push_back(dataVal.bin);
                                }
#endif  // DATA_TYPE == 4
#endif  // HALF_FLOAT
                            // Read from assembly file
                            } else {
#if (HALF_FLOAT)
                                dataFloat = stof(dataString);
                                dataHalf = dataFloat;
                                dataBin = dataHalf.bin_word();
                                rfBin.push_back(dataBin);
                                while (aistream >> dataFloat) {    
                                    dataHalf = dataFloat;                             
                                    dataBin = dataHalf.bin_word();
                                    rfBin.push_back(dataBin);
                                }
#else
                                dataVal.data = stof(dataString);
                                rfBin.push_back(dataVal.bin);
                                while (aistream >> dataVal.data) {
                                    rfBin.push_back((dataVal.bin));
                                }
#endif
                            }
                        }
                                                    
                        switch (storeType) {

                            case (STORE::GRFA):
                                if (rfBin.size() != SIMD_WIDTH || idx > GRF_ENTRIES) {
                                    cout << "Error trying to write to GRFA" << endl;
                                    error = true;
                                    break;
                                }

                                // Parse data to compatible format
                                for (int i = 0; i < (SIMD_WIDTH)/(WORDS_PER_BURST); i++) {
                                    dataAux = 0;
                                    for (int j = 0; j < WORDS_PER_BURST; j++) {
                                        dataAux |= (MASK & (dq_type) rfBin.front()) << j*WORD_BITS;
                                        rfBin.pop_front();
                                    }
                                    rfData.push_back(dataAux);
                                }
                            break;
                            case (STORE::GRFB):
                                if (rfBin.size() != SIMD_WIDTH || idx > GRF_ENTRIES) {
                                    cout << "Error trying to write to GRFB" << endl;
                                    error = true;
                                    break;
                                }

                                // Parse data to compatible format
                                for (int i = 0; i < (SIMD_WIDTH)/(WORDS_PER_BURST); i++) {
                                    dataAux = 0;
                                    for (int j = 0; j < WORDS_PER_BURST; j++) {
                                        dataAux |= (MASK & (dq_type) rfBin.front()) << j*WORD_BITS;
                                        rfBin.pop_front();
                                    }
                                    rfData.push_back(dataAux);
                                }
                            break;
                            case (STORE::SRFM):
                                if (rfBin.size() != 1 || idx > SRF_M_ENTRIES) {
                                    cout << "Error trying to write to SRFM" << endl;
                                    error = true;
                                    break;
                                }

                                dataAux = MASK & (dq_type) rfBin.front();
                                rfBin.pop_front();
                                rfData.push_back(dataAux);
                            break;
                            case (STORE::SRFA):
                                if (rfBin.size() != 1 || idx > SRF_A_ENTRIES) {
                                    cout << "Error trying to write to SRFA" << endl;
                                    error = true;
                                    break;
                                }

                                dataAux = MASK & (dq_type) rfBin.front();
                                rfBin.pop_front();
                                rfData.push_back(dataAux);
                            break;
                            default:
                                cout << "Error, trying to write to BANK" << endl;
                                error = true;
                            break;
                        }

                        // Parse index to address
                        addr = build_addr({0, 0, 0, 0, storeType, idx}, true);

                        // Write command
                        rawSeq << showbase << hex << addr << "\tWR";
                        while (!rfData.empty()) {
                            rawSeq << "\t" << showbase << hex << rfData.front();
                            rfData.pop_front();
                        }
                        rawSeq << endl;
                    }

                break;
                
                // If EXEC, stop CRF writing mode and write to output the {A,R/W,D} sequence
                // that will trigger execution starting from index 0, taking into account 
                // the loops (and maybe later the NOPs)
                case (INSTR::EXEC):

                    execStop = false;
                    jmpAct = false;
                    jmpNum = 0;
                    lastRow = lastCol = 0;
                    lastMemCmd = "RD";
                    execIdx = 0;

                    // Run through array of instructions
                    while (execIdx < CRF_ENTRIES && !execStop) {
                        
                        memCmd = "DC";

                        nmcInst execInstr = crfInstr.at(execIdx);

                        // Check if we need a specific address 
                        // and decide if we need a WR or RD command
                        if (execInstr.dst == OPC_EVEN_BANK || execInstr.dst == OPC_ODD_BANK) {
                            if (execInstr.dstAddrFile) {
                                error = getAddrFromFile(addrFile, &addr);
                            } else {
                                addr = execInstr.idxDst;
                            }
                            memCmd = "WR";
                        } else if (execInstr.src0 == OPC_EVEN_BANK || execInstr.src0 == OPC_ODD_BANK) {
                            if (execInstr.src0AddrFile) {
                                error = getAddrFromFile(addrFile, &addr);
                            } else {
                                addr = execInstr.idxSrc0;
                            }
                            memCmd = "RD";
                        } else if (execInstr.src1 == OPC_EVEN_BANK || execInstr.src1 == OPC_ODD_BANK) {
                            if (execInstr.src1AddrFile) {
                                error = getAddrFromFile(addrFile, &addr);
                            } else {
                                addr = execInstr.idxSrc1;
                            }
                            memCmd = "RD";
                        } else if (execInstr.src2 == OPC_EVEN_BANK || execInstr.src2 == OPC_ODD_BANK) {
                            if (execInstr.src2AddrFile) {
                                error = getAddrFromFile(addrFile, &addr);
                            } else {
                                addr = execInstr.idxSrc2;
                            }
                            memCmd = "RD";
                        } else {
                            // If not, generate one.
                            addr = build_addr({0, 0, 0, 0, lastRow, lastCol}, false);
                            switch (defaultCmd){
                                case DefaultCmd::defaultWR:
                                    memCmd = "WR";
                                break;
                                case DefaultCmd::defaultInherit:
                                    memCmd = lastMemCmd;
                                break;
                                case DefaultCmd::defaultRD: 
                                default:
                                    memCmd = "RD";
                                break;
                            }
                        }

                        // Store last row and column used to minimize unnecessary Precharges
                        lastRow = get_row(addr);
                        lastCol = get_col(addr);
                        lastMemCmd = memCmd;

                        // Write command
                        rawSeq << showbase << hex << addr << "\t" << memCmd;

                        // Read from data file if needed
                        if (execInstr.dataFile) {
                            error = getDataFromFile(dataFile, rfBin, &execInstr);
                            if (error)  break;
                            while (!execInstr.data.empty()) {
                                rawSeq << "\t" << showbase << hex << execInstr.data.front();
                                execInstr.data.pop_front();   // Pop since next iteration has new data
                            }

                        // Read directly from the structure (data was in assembly file)
                        } else {
                            if (!execInstr.data.empty()) {
                                auto dataItr = execInstr.data.begin();
                                while (dataItr != execInstr.data.end()) {
                                    rawSeq << "\t" << showbase << hex << *dataItr;
                                    ++dataItr;
                                }
                            }
                        }
                        rawSeq << endl;

                        // Check if we stop execution
                        if (execInstr.opCode == OP_EXIT) {
                            execStop = true;
                        // Check if its a jump, and if so proceed accordingly
                        } else if (execInstr.opCode == OP_JUMP) {
                            // If first time seen it, set target and number of loops
                            if (!jmpAct) {
                                jmpAct = true;
                                jmpNum = execInstr.imm1-1;
                                execIdx -= execInstr.imm0;
                            // Else, check the loop counter
                            } else {
                                // If final one, clean loop variables and increase execIdx normally
                                if (jmpNum == 0) {
                                    jmpAct = false;
                                    execIdx++;
                                // Else, update execIdx and decrease loop counter
                                } else {
                                    jmpNum--;
                                    execIdx -= execInstr.imm0;
                                }
                            }
                        } else {
                            execIdx++;
                        }
                    }
                break;

                // Else, decode the instruction, write it into the instruction array mimicking the CRF,
                // generate the traces for writing to the CRF, advance index
                // and check it's not higher than the CRF size
                default:
                    if (!crfWrMode || crfIdx >= CRF_ENTRIES) {
                        cout << "Error when trying to write NMC instructions" << endl;
                        error = true;
                        break;
                    }
                    nmcInst* currInstr = new nmcInst();
                    switch (instrType) {

                        case INSTR::NOP:
                            currInstr->opCode = INSTR2OPCODE.at(instrType);
                            if (!(aistream >> currInstr->imm0)) {
                                cout << "Error when reading NOP parameters" << endl;
                                error = true;
                                break;
                            }
                            crfInstr.at(crfIdx) = *currInstr;
                        break;

                        case INSTR::JUMP:
                            currInstr->opCode = INSTR2OPCODE.at(instrType);
                            if (!(aistream >> currInstr->imm0 >> currInstr->imm1)) {
                                cout << "Error when reading JUMP parameters" << endl;
                                error = true;
                                break;
                            }
                            crfInstr.at(crfIdx) = *currInstr;
                        break;

                        case INSTR::EXIT:
                            currInstr->opCode = INSTR2OPCODE.at(instrType);
                            crfInstr.at(crfIdx) = *currInstr;
                        break;

                        case INSTR::MOV:
                            currInstr->opCode = INSTR2OPCODE.at(instrType);
                            if (!(aistream >> dstAux >> src0Aux)) {
                                cout << "Error when reading MOV parameters" << endl;
                                error = true;
                                break;
                            }
                            currInstr->dstAddrFile = splitStoreIndex(&dstAux, &(currInstr->idxDst));
                            currInstr->src0AddrFile = splitStoreIndex(&src0Aux, &(currInstr->idxSrc0));
                            try {
                                currInstr->dst = STRING2OPCSTORAGE.at(dstAux);
                                currInstr->src0 = STRING2OPCSTORAGE.at(src0Aux);
                            } catch (const std::out_of_range& oor) {
                                cout << "Error when reading MOV parameters" << endl;
                                error = true;
                                break;
                            }

                            if (!error)
                                error = getInstData(aistream, rfBin, currInstr);

                            crfInstr.at(crfIdx) = *currInstr;
                        break;
                        
                        case INSTR::RELU:
                            currInstr->opCode = INSTR2OPCODE.at(instrType);
                            if (!(aistream >> dstAux >> src0Aux)) {
                                cout << "Error when reading MOV parameters" << endl;
                                error = true;
                                break;
                            }
                            currInstr->dstAddrFile = splitStoreIndex(&dstAux, &(currInstr->idxDst));
                            currInstr->src0AddrFile = splitStoreIndex(&src0Aux, &(currInstr->idxSrc0));
                            try {
                                currInstr->dst = STRING2OPCSTORAGE.at(dstAux);
                                currInstr->src0 = STRING2OPCSTORAGE.at(src0Aux);
                            } catch (const std::out_of_range& oor) {
                                cout << "Error when reading MOV parameters" << endl;
                                error = true;
                                break;
                            }

                            if (!error)
                                error = getInstData(aistream, rfBin, currInstr);

                            currInstr->relu = true;
                            crfInstr.at(crfIdx) = *currInstr;
                        break;

                        case INSTR::FILL:
                            cout << "Error, FILL is not yet implemented" << endl;
                            error = true;
                        break;

                        case INSTR::ADD:
                            currInstr->opCode = INSTR2OPCODE.at(instrType);
                            if (!(aistream >> dstAux >> src0Aux >> src1Aux)) {
                                cout << "Error when reading ADD parameters" << endl;
                                error = true;
                                break;
                            }
                            currInstr->dstAddrFile = splitStoreIndex(&dstAux, &(currInstr->idxDst));
                            currInstr->src0AddrFile = splitStoreIndex(&src0Aux, &(currInstr->idxSrc0));
                            currInstr->src1AddrFile = splitStoreIndex(&src1Aux, &(currInstr->idxSrc1));
                            try {
                                currInstr->dst = STRING2OPCSTORAGE.at(dstAux);
                                currInstr->src0 = STRING2OPCSTORAGE.at(src0Aux);
                                currInstr->src1 = STRING2OPCSTORAGE.at(src1Aux);
                            } catch (const std::out_of_range& oor) {
                                cout << "Error when reading ADD parameters" << endl;
                                error = true;
                                break;
                            }

                            if (!error)
                                error = getInstData(aistream, rfBin, currInstr);

                            crfInstr.at(crfIdx) = *currInstr;
                        break;

                        case INSTR::ADDa:
                            currInstr->opCode = INSTR2OPCODE.at(instrType);
                            if (!(aistream >> dstAux >> src0Aux >> src1Aux)) {
                                cout << "Error when reading ADDa parameters" << endl;
                                error = true;
                                break;
                            }
                            currInstr->dstAddrFile = splitStoreIndex(&dstAux, &(currInstr->idxDst));
                            currInstr->src0AddrFile = splitStoreIndex(&src0Aux, &(currInstr->idxSrc0));
                            currInstr->src1AddrFile = splitStoreIndex(&src1Aux, &(currInstr->idxSrc1));
                            try {
                                currInstr->dst = STRING2OPCSTORAGE.at(dstAux);
                                currInstr->src0 = STRING2OPCSTORAGE.at(src0Aux);
                                currInstr->src1 = STRING2OPCSTORAGE.at(src1Aux);
                            } catch (const std::out_of_range& oor) {
                                cout << "Error when reading ADDa parameters" << endl;
                                error = true;
                                break;
                            }

                            if (!error)
                                error = getInstData(aistream, rfBin, currInstr);

                            currInstr->aam = true;
                            crfInstr.at(crfIdx) = *currInstr;
                        break;

                        case INSTR::MUL:
                            currInstr->opCode = INSTR2OPCODE.at(instrType);
                            if (!(aistream >> dstAux >> src0Aux >> src1Aux)) {
                                cout << "Error when reading MUL parameters" << endl;
                                error = true;
                                break;
                            }
                            currInstr->dstAddrFile = splitStoreIndex(&dstAux, &(currInstr->idxDst));
                            currInstr->src0AddrFile = splitStoreIndex(&src0Aux, &(currInstr->idxSrc0));
                            currInstr->src1AddrFile = splitStoreIndex(&src1Aux, &(currInstr->idxSrc1));
                            try {
                                currInstr->dst = STRING2OPCSTORAGE.at(dstAux);
                                currInstr->src0 = STRING2OPCSTORAGE.at(src0Aux);
                                currInstr->src1 = STRING2OPCSTORAGE.at(src1Aux);
                            } catch (const std::out_of_range& oor) {
                                cout << "Error when reading MUL parameters" << endl;
                                error = true;
                                break;
                            }

                            if (!error)
                                error = getInstData(aistream, rfBin, currInstr);

                            crfInstr.at(crfIdx) = *currInstr;
                        break;

                        case INSTR::MULa:
                            currInstr->opCode = INSTR2OPCODE.at(instrType);
                            if (!(aistream >> dstAux >> src0Aux >> src1Aux)) {
                                cout << "Error when reading MULa parameters" << endl;
                                error = true;
                                break;
                            }
                            currInstr->dstAddrFile = splitStoreIndex(&dstAux, &(currInstr->idxDst));
                            currInstr->src0AddrFile = splitStoreIndex(&src0Aux, &(currInstr->idxSrc0));
                            currInstr->src1AddrFile = splitStoreIndex(&src1Aux, &(currInstr->idxSrc1));
                            try {
                                currInstr->dst = STRING2OPCSTORAGE.at(dstAux);
                                currInstr->src0 = STRING2OPCSTORAGE.at(src0Aux);
                                currInstr->src1 = STRING2OPCSTORAGE.at(src1Aux);
                            } catch (const std::out_of_range& oor) {
                                cout << "Error when reading MULa parameters" << endl;
                                error = true;
                                break;
                            }

                            if (!error)
                                error = getInstData(aistream, rfBin, currInstr);

                            currInstr->aam = true;
                            crfInstr.at(crfIdx) = *currInstr;  
                        break;

                        case INSTR::MAD:
                            currInstr->opCode = INSTR2OPCODE.at(instrType);
                            if (!(aistream >> dstAux >> src0Aux >> src1Aux >> src2Aux)) {
                                cout << "Error when reading MAD parameters" << endl;
                                error = true;
                                break;
                            }
                            currInstr->dstAddrFile = splitStoreIndex(&dstAux, &(currInstr->idxDst));
                            currInstr->src0AddrFile = splitStoreIndex(&src0Aux, &(currInstr->idxSrc0));
                            currInstr->src1AddrFile = splitStoreIndex(&src1Aux, &(currInstr->idxSrc1));
                            currInstr->src2AddrFile = splitStoreIndex(&src2Aux, &(currInstr->idxSrc2));
                            try {
                                currInstr->dst = STRING2OPCSTORAGE.at(dstAux);
                                currInstr->src0 = STRING2OPCSTORAGE.at(src0Aux);
                                currInstr->src1 = STRING2OPCSTORAGE.at(src1Aux);
                                currInstr->src2 = STRING2OPCSTORAGE.at(src2Aux);
                            } catch (const std::out_of_range& oor) {
                                cout << "Error when reading MAD parameters" << endl;
                                error = true;
                                break;
                            }

                            if (!error)
                                error = getInstData(aistream, rfBin, currInstr);

                            crfInstr.at(crfIdx) = *currInstr;
                        break;

                        case INSTR::MADa:
                            currInstr->opCode = INSTR2OPCODE.at(instrType);
                            if (!(aistream >> dstAux >> src0Aux >> src1Aux >> src2Aux)) {
                                cout << "Error when reading MADa parameters" << endl;
                                error = true;
                                break;
                            }
                            currInstr->dstAddrFile = splitStoreIndex(&dstAux, &(currInstr->idxDst));
                            currInstr->src0AddrFile = splitStoreIndex(&src0Aux, &(currInstr->idxSrc0));
                            currInstr->src1AddrFile = splitStoreIndex(&src1Aux, &(currInstr->idxSrc1));
                            currInstr->src2AddrFile = splitStoreIndex(&src2Aux, &(currInstr->idxSrc2));
                            try {
                                currInstr->dst = STRING2OPCSTORAGE.at(dstAux);
                                currInstr->src0 = STRING2OPCSTORAGE.at(src0Aux);
                                currInstr->src1 = STRING2OPCSTORAGE.at(src1Aux);
                                currInstr->src2 = STRING2OPCSTORAGE.at(src2Aux);
                            } catch (const std::out_of_range& oor) {
                                cout << "Error when reading MADa parameters" << endl;
                                error = true;
                                break;
                            }

                            if (!error)
                                error = getInstData(aistream, rfBin, currInstr);

                            currInstr->aam = true;
                            crfInstr.at(crfIdx) = *currInstr;
                        break;

                        case INSTR::MAC:
                            currInstr->opCode = INSTR2OPCODE.at(instrType);
                            if (!(aistream >> dstAux >> src0Aux >> src1Aux)) {
                                cout << "Error when reading MAC parameters" << endl;
                                error = true;
                                break;
                            }
                            currInstr->dstAddrFile = splitStoreIndex(&dstAux, &(currInstr->idxDst));
                            currInstr->src0AddrFile = splitStoreIndex(&src0Aux, &(currInstr->idxSrc0));
                            currInstr->src1AddrFile = splitStoreIndex(&src1Aux, &(currInstr->idxSrc1));
                            try {
                                currInstr->dst = STRING2OPCSTORAGE.at(dstAux);
                                currInstr->src0 = STRING2OPCSTORAGE.at(src0Aux);
                                currInstr->src1 = STRING2OPCSTORAGE.at(src1Aux);
                            } catch (const std::out_of_range& oor) {
                                cout << "Error when reading MAC parameters" << endl;
                                error = true;
                                break;
                            }

                            if (!error)
                                error = getInstData(aistream, rfBin, currInstr);

                            crfInstr.at(crfIdx) = *currInstr;  
                        break;

                        case INSTR::MACa:
                            currInstr->opCode = INSTR2OPCODE.at(instrType);
                            if (!(aistream >> dstAux >> src0Aux >> src1Aux)) {
                                cout << "Error when reading MACa parameters" << endl;
                                error = true;
                                break;
                            }
                            currInstr->dstAddrFile = splitStoreIndex(&dstAux, &(currInstr->idxDst));
                            currInstr->src0AddrFile = splitStoreIndex(&src0Aux, &(currInstr->idxSrc0));
                            currInstr->src1AddrFile = splitStoreIndex(&src1Aux, &(currInstr->idxSrc1));
                            try {
                                currInstr->dst = STRING2OPCSTORAGE.at(dstAux);
                                currInstr->src0 = STRING2OPCSTORAGE.at(src0Aux);
                                currInstr->src1 = STRING2OPCSTORAGE.at(src1Aux);
                            } catch (const std::out_of_range& oor) {
                                cout << "Error when reading MACa parameters" << endl;
                                error = true;
                                break;
                            }

                            if (!error)
                                error = getInstData(aistream, rfBin, currInstr);

                            currInstr->aam = true;
                            crfInstr.at(crfIdx) = *currInstr;  
                        break;

                        default:
                            cout << "Error, instruction type not recognized" << endl;
                            error = true;
                        break;
                    }

                    // Parse index to address
#if CRF_BANK_ADDR
                    // If not enough column bits to address CRF, using also bank bits
                    uint bank_aux = (crfIdx >> addr_bits[int(Level::Column)]) & ((1 << addr_bits[int(Level::Bank)]) - 1);
                    uint col_aux = crfIdx & ((1 << addr_bits[int(Level::Column)]) - 1);
                    addr = build_addr({0, 0, 0, bank_aux, STORE::CRF, col_aux}, true);
#else
                    addr = build_addr({0, 0, 0, 0, STORE::CRF, crfIdx}, true);
#endif

                    // Write command
                    rawSeq << showbase << hex << addr << "\tWR\t";
                    uint64_t instTemp = build_instr(*currInstr);
#if DQ_BITS == 16
                    uint16_t instLowerPart = instTemp & 0xFFFF;
                    uint16_t instUpperPart = (instTemp >> 16) & 0xFFFF;
                    rawSeq << hex << instLowerPart << "\t" << instUpperPart << endl;
#else
                    rawSeq << hex << instTemp << endl;
#endif

                    // Advance CRF index
                    crfIdx++;
                break;
            }
        }
    }

    assembly.close();
    rawSeq.close();
    if (argc == 5) {
        dataFile.close();
        addrFile.close();
    }

    cout << "Raw sequence generated" << endl;

    return 0;
}

uint64_t build_addr(vector<uint64_t> addr_vec, bool rf_write)
{
    uint64_t addr_aux = 0;
    uint64_t offset = global_offset; // Don't know really why right now, maybe to address groups of 128

    if (rf_write) {
        addr_vec[int(Level::Row)] |= (1 << (addr_bits[int(Level::Row)]-1)); //Ndryshova kete RK -1 => -3
    }

    for (int i = 0; i < int(Level::MAX); i++) {
        if (addr_bits[int(addr_map[i])]) {
            addr_aux |= (addr_vec[int(addr_map[i])] << offset);
            offset += addr_bits[int(addr_map[i])];
        }
    }

    return addr_aux;
}

bool splitStoreIndex(string* storeTypeString, uint64_t *idx)
{
    bool split = false;
    bool addressFromFile = false;
    uint8_t charidx = 0;
    string part1, part2;

    string aux = *storeTypeString;

    while (!split && charidx < aux.length()) {
        if (aux[charidx] == '[') {
            split = true;
            part1 = aux.substr(0,charidx);
            part2 = aux.substr(charidx+1,aux.length()-charidx-2);
            addressFromFile = !(part2.compare(ADDRFILE));
        } else if (aux[charidx] >= '0' && aux[charidx] <= '9') {
            split = true;
            part1 = aux.substr(0,charidx);
            part2 = aux.substr(charidx);
        }

        ++charidx;
    }

    if (split) {
        *storeTypeString = part1;
        if (!addressFromFile) {
            *idx = stoll(part2,0,0);
        }
    }

    return addressFromFile;
}

uint8_t get_bank(uint64_t addr) {
    uint64_t bank, bankMask;
    uint offset = global_offset; // Initial offset
    uint lvl = 0;

    addr = addr >> offset;  // Remove initial offset
    // Shif to the right until the bank is aligned
    while (addr_map[lvl] != Level::Bank) {
        addr = addr >> addr_bits[int(addr_map[lvl])];
        lvl++;
    }
    // Obtain the mask
    bankMask = (1 << addr_bits[int(addr_map[lvl])]) - 1;
    // Get the bank index
    bank = addr & bankMask;

    return bank;
}

uint16_t get_row(uint64_t addr) {
    uint64_t row, rowMask;
    uint offset = global_offset; // Initial offset
    uint lvl = 0;

    addr = addr >> offset;  // Remove initial offset
    // Shif to the right until the row is aligned
    while (addr_map[lvl] != Level::Row) {
        addr = addr >> addr_bits[int(addr_map[lvl])];
        lvl++;
    }
    // Obtain the mask
    rowMask = (1 << addr_bits[int(addr_map[lvl])]) - 1;
    // Get the row index
    row = addr & rowMask;

    return row;
}

uint16_t get_col(uint64_t addr) {
    uint64_t col, colMask;
    uint offset = global_offset; // Initial offset
    uint lvl = 0;

    addr = addr >> offset;  // Remove initial offset
    // Shif to the right until the column is aligned
    while (addr_map[lvl] != Level::Column) {
        addr = addr >> addr_bits[int(addr_map[lvl])];
        lvl++;
    }
    // Obtain the mask
    colMask = (1 << addr_bits[int(addr_map[lvl])]) - 1;
    // Get the column index
    col = addr & colMask;

    return col;
}

uint64_t build_instr(nmcInst instrData) {
    uint32_t instrWord = 0;

    switch (instrData.opCode) {
        case OP_NOP:
        case OP_JUMP:
        case OP_EXIT:
            instrWord |= (instrData.opCode & ((1 << (OPCODE_STA-OPCODE_END+1)) - 1)) << OPCODE_END;
            instrWord |= (instrData.imm0 & ((1 << (IMM0_STA-IMM0_END+1)) - 1)) << IMM0_END;
            instrWord |= (instrData.imm1 & ((1 << (IMM1_STA-IMM1_END+1)) - 1)) << IMM1_END;
        break;

        case OP_ADD:
        case OP_MUL:
        case OP_MAD:
        case OP_MAC:
            instrWord |= (instrData.opCode & ((1 << (OPCODE_STA-OPCODE_END+1)) - 1)) << OPCODE_END;
            instrWord |= (instrData.dst & ((1 << (DST_STA-DST_END+1)) - 1)) << DST_END;
            instrWord |= (instrData.src0 & ((1 << (SRC0_STA-SRC0_END+1)) - 1)) << SRC0_END;
            instrWord |= (instrData.src1 & ((1 << (SRC1_STA-SRC1_END+1)) - 1)) << SRC1_END;
            instrWord |= (instrData.src2 & ((1 << (SRC2_STA-SRC2_END+1)) - 1)) << SRC2_END;
            instrWord |= (instrData.aam & 1) << AAM_BIT;
            instrWord |= (instrData.idxDst & ((1 << (DST_N_STA-DST_N_END+1)) - 1)) << DST_N_END;
            instrWord |= (instrData.idxSrc0 & ((1 << (SRC0_N_STA-SRC0_N_END+1)) - 1)) << SRC0_N_END;
            instrWord |= (instrData.idxSrc1 & ((1 << (SRC1_N_STA-SRC1_N_END+1)) - 1)) << SRC1_N_END;
        break;

        case OP_MOV:
        case OP_FILL:
            instrWord |= (instrData.opCode & ((1 << (OPCODE_STA-OPCODE_END+1)) - 1)) << OPCODE_END;
            instrWord |= (instrData.dst & ((1 << (DST_STA-DST_END+1)) - 1)) << DST_END;
            instrWord |= (instrData.src0 & ((1 << (SRC0_STA-SRC0_END+1)) - 1)) << SRC0_END;
            instrWord |= (instrData.relu & 1) << RELU_BIT;
            instrWord |= (instrData.idxDst & ((1 << (DST_N_STA-DST_N_END+1)) - 1)) << DST_N_END;
            instrWord |= (instrData.idxSrc0 & ((1 << (SRC0_N_STA-SRC0_N_END+1)) - 1)) << SRC0_N_END;
            instrWord |= (instrData.idxSrc1 & ((1 << (SRC1_N_STA-SRC1_N_END+1)) - 1)) << SRC1_N_END;
        break;
    }

    return instrWord;

}

bool getInstData(istringstream &aistream, deque<rfBin_t> &rfBin, nmcInst *currInstr) {
    string dataString;
    float dataFloat;
#if HALF_FLOAT
    half_float::half dataHalf;
#else
    cnm_union dataVal;
#endif
    uint16_t dataBin;
    dq_type dataAux;

    // Obtain the data to be moved from a bank if needed 
    if (aistream >> dataString) {
        // Read from data file
        if (!dataString.compare(DATAFILE)){
            currInstr->dataFile = true;
        // Read from assembly file
        } else {
#if (HALF_FLOAT)
            dataFloat = stof(dataString); //string to int
            dataHalf = dataFloat;
            dataBin = dataHalf.bin_word();
            rfBin.push_back(dataBin);
            while (aistream >> dataFloat) {
                dataHalf = dataFloat;
                dataBin = dataHalf.bin_word();
                rfBin.push_back(dataBin);
            }
#else
            dataVal.data = stof(dataString); //string to int
            rfBin.push_back(dataVal.bin);
            while (aistream >> dataVal.data) {
                rfBin.push_back(dataVal.bin);
            }
#endif
        }
    }

    while (!rfBin.empty()) {
        // Check if number of data is acceptable
        if (rfBin.size() % SIMD_WIDTH) {
            cout << "Error, number of variables not divisible by number of SIMD channels" << endl;
            return true; // Error
        }
        // Parse data to compatible format
        while (!rfBin.empty()) {
            dataAux = 0;
            for (int j = 0; j < WORDS_PER_BURST; j++) {
                dataAux |= (MASK & (dq_type) rfBin.front()) << j*WORD_BITS;
                rfBin.pop_front();
            }
            currInstr->data.push_back(dataAux);
        }
    }

    return false;
}

bool getDataFromFile (ifstream &dataFile, deque<rfBin_t> &rfBin, nmcInst *currInstr) {
    string diline;
    float dataFloat;
#if (HALF_FLOAT)
    half_float::half dataHalf;
#else
    cnm_union dataVal;
#endif
    uint16_t dataBin;
    uint64_t dataAux;
    
    
    do {
        if (!getline(dataFile, diline)) {
            cout << "Error, data file cannot be read" << endl;
            return true;    // Signal error
        }
    } while (diline.length() == 0 || diline.at(0) == '#');

    istringstream distream(diline);

#if (HALF_FLOAT)
    while (distream >> dataFloat) {
        dataHalf = dataFloat;
        dataBin = dataHalf.bin_word();
        rfBin.push_back(dataBin);
    }
#else
#if DATA_TYPE == 4
    int int8Aux;
    while(distream >> int8Aux){
        dataVal.data = int8Aux;
        rfBin.push_back(dataVal.bin);
    }
#else
    while (distream >> dataVal.data) {
        rfBin.push_back(dataVal.bin);
    }
#endif  // DATA_TYPE == 4
#endif  // HALF_FLOAT

    while (!rfBin.empty()) {
        //Check if number of data is acceptable
        if (rfBin.size() % SIMD_WIDTH) {
            cout << "Error, number of variables not divisible by number of SIMD channels" << endl;
            return true; // Error
        }
        // Parse data to compatible format
        while (!rfBin.empty()) {
            dataAux = 0;
            for (int j = 0; j < WORDS_PER_BURST; j++) {
                dataAux |= (MASK & (uint64_t) rfBin.front()) << (j*WORD_BITS);
                rfBin.pop_front();
            }
            currInstr->data.push_back(dataAux);
        }
    }

    return false;
}

bool getAddrFromFile (ifstream &addrFile, uint64_t *addr) {
    string ailine;

    do {
        if (!getline(addrFile, ailine)) {
            cout << "Error, address file cannot be read" << endl;
            return true;    // Signal error
        }
    } while (ailine.length() == 0 || ailine.at(0) == '#');

    istringstream aistream(ailine);
    aistream >> hex >> *addr;

    return false;
}