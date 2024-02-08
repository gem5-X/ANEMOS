#include "map_conv.h"

void mapConvCWWRRLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t *act, cnm_t *weight, cnm_t *bias,
                                int ci, int wi, int hi, int k, int co, int wo, int ho, int stride)
{
    int i,j,l,m,n;

    int ext_loops = floor(pow(float(k),2)*float(ci)/float(GRF_ENTRIES)) - 1; 
    int loops = ceil(float(wo)*float(ho)/float(SIMD_WIDTH*CORES_PER_PCH));
    int ext_peeling = int(pow(k,2)*ci) % GRF_ENTRIES;

    uint64_t weightBase = ceil(float(loops * k*k * ci / 2) / float(COLPERROW*CORES_PER_PCH)); // Div 2 because distributed in 2 banks
    uint64_t biasBase = weightBase + ceil(float(ceil(float(co) * k*k * ci / float(SIMD_WIDTH*CORES_PER_PCH)) / 2) / float(COLPERROW));
    uint64_t resBase = biasBase + ceil(float(ceil(float(co) / float(SIMD_WIDTH*CORES_PER_PCH)) / 2) / float(COLPERROW));

    uint64_t rowIdx[2] = {0, resBase};
    uint64_t colIdx[2] = {0,0};
    uint64_t resRowRet, resColRet;              // Stores row and column to return every loop to accumulate the results of one output channel
    uint64_t weightIdx;                         // Index to run through weights
    uint64_t srfWeight[SRF_M_ENTRIES];               // Incedes of weight stored in each scalar register
    uint64_t ciIdx[SRF_M_ENTRIES], hiIdx[SRF_M_ENTRIES], wiIdx[SRF_M_ENTRIES]; // Indeces to run through activations
    uint64_t curKCh, curKRow, curKCol;          // Holds equivalence of weight index for activation indexing
    uint64_t compKCh, compKRow, compKCol;       // Holds equivalence of weight index for computing partial convolution
    uint64_t wiStart, wiEnd, hiStart, hiEnd;    // Holds the limits of the activations considered for the current weight
    uint64_t woIdx, hoIdx;                      // Holds indeces for performing partial convolutions

    // Write assembly instructions
    assembly << "; Channel-wise mapping of convolution with weight reuse, R-limited, ci = " << ci << ", co = ";
    assembly << co << ", k = " << k << ", wo = " << wo << ", ho = " << ho << ", stride = " << stride << endl;

    for (i=0; i<co; i++) {

        assembly << "; Output channel " << i << endl;

        // First set of weights, where bias is also added
        if (ext_loops > -1) {
            assembly << "; First set of weights" << endl;
            assembly << "WRF CRF0" << endl;
            assembly << "MAD GRFB0 EVEN_BANK[AddrFile] SRFM0 SRFA0 DataFile" << endl;
            for (j=0; j<SRF_M_ENTRIES/2; j++) {
                if (j)
                    assembly << "MAC GRFB0 EVEN_BANK[AddrFile] SRFM" << 2*j << " DataFile" << endl;
                assembly << "MAC GRFB0 ODD_BANK[AddrFile] SRFM" << 2*j+1 << " DataFile" << endl;
            }
            assembly << "MOV ODD_BANK[AddrFile] GRFB0" << endl;
            if (loops-1)
                assembly << "JUMP " << SRF_M_ENTRIES+1 << " " << loops-1 << endl;
            assembly << "EXIT" << endl;
            
            for (j=0; j<SRF_M_ENTRIES; j++) { 
                assembly << "WRF SRFM" << j << " DataFile" << endl; // Weights
            }
            assembly << "WRF SRFA0 DataFile" << endl;   // Bias
            
            assembly << "EXEC" << endl;
        }

        // Next set of weights, until reaching ext_peeling
        if (ext_loops > 0) {
            assembly << "; Next set of weights" << endl;
            assembly << "WRF CRF0" << endl;
            assembly << "MOV GRFB0 ODD_BANK[AddrFile] DataFile" << endl;
            for (j=0; j<SRF_M_ENTRIES/2; j++) {
                assembly << "MAC GRFB0 EVEN_BANK[AddrFile] SRFM" << 2*j << " DataFile" << endl;
                assembly << "MAC GRFB0 ODD_BANK[AddrFile] SRFM" << 2*j+1 << " DataFile" << endl;
            }
            assembly << "MOV ODD_BANK[AddrFile] GRFB0" << endl;
            if (loops-1)
                assembly << "JUMP " << SRF_M_ENTRIES+2 << " " << loops-1 << endl;
            assembly << "EXIT" << endl;

            for (j=0; j<ext_loops; j++) {
                for (l=0; l<SRF_M_ENTRIES; l++) {   
                    assembly << "WRF SRFM" << l << " DataFile" << endl; // Weights
                }
                assembly << "EXEC" << endl;
            }
        }

        // Final set of weights (if ext_peeling)
        if (ext_peeling && ext_loops > -1) {
            assembly << "; Peeled set of weights" << endl;
            assembly << "WRF CRF" << ext_peeling+1 << endl; // Move forward the MOV to BANK, JUMP and EXIT
            assembly << "MOV ODD_BANK[AddrFile] GRFB0" << endl;
            if (loops-1)
                assembly << "JUMP " << ext_peeling+2 << " " << loops-1 << endl;
            assembly << "EXIT" << endl;

            for (j=0; j<ext_peeling; j++) {
                assembly << "WRF SRFM" << j << " DataFile" << endl; // Weights
            }
            assembly << "EXEC" << endl;
        } else if (ext_peeling) {
            assembly << "; Peeled set of weights" << endl;
            assembly << "WRF CRF0" << endl;
            assembly << "MAD GRFB0 EVEN_BANK[AddrFile] SRFM0 SRFA0 DataFile" << endl;
            for (j=0; j<ext_peeling/2; j++) {
                if (j)
                    assembly << "MAC GRFB0 EVEN_BANK[AddrFile] SRFM" << 2*j << " DataFile" << endl;
                assembly << "MAC GRFB0 ODD_BANK[AddrFile] SRFM" << 2*j+1 << " DataFile" << endl;
            }
            if (ext_peeling - 2*j)
                assembly << "MAC GRFB0 EVEN_BANK[AddrFile] SRFM" << 2*j << " DataFile" << endl;
            assembly << "MOV ODD_BANK[AddrFile] GRFB0" << endl;
            if (loops-1)
                assembly << "JUMP " << ext_peeling+1 << " " << loops-1 << endl;
            assembly << "EXIT" << endl;
            
            for (j=0; j<ext_peeling; j++) { 
                assembly << "WRF SRFM" << j << " DataFile" << endl; // Weights
            }
            assembly << "WRF SRFA0 DataFile" << endl;   // Bias
            
            assembly << "EXEC" << endl;
        }
    }
    

    // Write address for loops and others
    addrFile << "# Channel-wise mapping of convolution with weight reuse, R-limited, ci = " << ci << ", co = ";
    addrFile << co << ", k = " << k << ", wo = " << wo << ", ho = " << ho << ", stride = " << stride << endl;

    for (i=0; i<co; i++) {

        addrFile << "## Output channel " << i << endl;

        // Reset activation indeces for reading again for the new channel
        rowIdx[0] = 0;          colIdx[0] = 0;
        // Stores where the result row and col index to return to at the end of each loop
        resRowRet = rowIdx[1];  resColRet = colIdx[1];

        // First set of weights, where bias is also added
        if (ext_loops > -1) {
            addrFile << "### First set of weights" << endl;
            for (l=0; l<loops; l++) {
                for (m=0; m<SRF_M_ENTRIES/2; m++) {
                    addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
                    addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
                    if (++colIdx[0] == COLPERROW) {
                        colIdx[0] = 0;
                        rowIdx[0]++;
                    }
                }

                addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
                if (++colIdx[1] == COLPERROW) {
                    colIdx[1] = 0;
                    rowIdx[1]++;
                }
            }
        }

        // Next set of weights, until reaching ext_peeling
        if (ext_loops > 0) {
            addrFile << "### Next set of weights" << endl;
            for (j=0; j<ext_loops; j++) {
                rowIdx[1] = resRowRet;  colIdx[1] = resColRet;
                for (l=0; l<loops; l++) {
                    addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;

                    for (m=0; m<SRF_M_ENTRIES/2; m++) {
                        addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
                        addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
                        if (++colIdx[0] == COLPERROW) {
                            colIdx[0] = 0;
                            rowIdx[0]++;
                        }
                    }

                    addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
                    if (++colIdx[1] == COLPERROW) {
                        colIdx[1] = 0;
                        rowIdx[1]++;
                    }
                }
            }
        }

        // Final set of weights (if ext_peeling)
        if (ext_peeling) {
            addrFile << "### Peeled set of weights" << endl;
            rowIdx[1] = resRowRet;  colIdx[1] = resColRet;
            addrFile << "# Out of the main loop" << endl;
            for (j=0; j<loops; j++) {
                addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;

                for (l=0; l<ext_peeling/2; l++) {
                    addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
                    addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
                    if (++colIdx[0] == COLPERROW) {
                        colIdx[0] = 0;
                        rowIdx[0]++;
                    }
                }
                if (ext_peeling - 2*l)  // If n it's odd, last one is only with EVEN_BANK
                    addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;

                addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
                if (++colIdx[1] == COLPERROW) {
                    colIdx[1] = 0;
                    rowIdx[1]++;
                }
            }
        }
    }
    

    // Write data for loops and others
    dataFile << "# Channel-wise mapping of convolution with weight reuse, R-limited, ci = " << ci << ", co = ";
    dataFile << co << ", k = " << k << ", wo = " << wo << ", ho = " << ho << ", stride = " << stride << endl;
    
    for (i=0; i<co; i++) {

        dataFile << "## Output channel " << i << endl;
        weightIdx = i*ci*pow(k,2);
        for (j=0; j<SRF_M_ENTRIES; j++) {
            srfWeight[j] = weightIdx + j;
            ciIdx[j] = (srfWeight[j] % (ci*k*k))  / int(pow(k,2));
            hiIdx[j] = ((srfWeight[j] % (ci*k*k))  % int(pow(k,2))) / k;
            wiIdx[j] = ((srfWeight[j] % (ci*k*k))  % int(pow(k,2))) % k;
        }

        // First set of weights, where bias is also added
        if (ext_loops > -1) {
            dataFile << "### First set of weights" << endl;
            // Weights
            for (j=0; j<SRF_M_ENTRIES; j++) {
                if(WORD_BITS != 8)
                    dataFile << weight[weightIdx++] << endl;
                else
                    dataFile << int(weight[weightIdx++]) << endl; //need to cast the results for int8 only
            }
            // Bias
            if(WORD_BITS != 8)
                dataFile << bias[i] << endl;
            else
                dataFile << int(bias[i]) << endl; //need to cast the results for int8 only

            // Activations
            for (j=0; j<loops; j++) {
                for (l=0; l<SRF_M_ENTRIES; l++) {
                    // Set the start and end indeces for current weight
                    curKCh = (srfWeight[l] % (ci*k*k))  / int(pow(k,2));           // Current channel of the filter
                    curKRow = ((srfWeight[l] % (ci*k*k))  % int(pow(k,2))) / k;    // Current row of the filter
                    curKCol = ((srfWeight[l] % (ci*k*k))  % int(pow(k,2))) % k;    // Current column of the filter
                    wiStart = curKCol;  wiEnd = wi - k + curKCol + 1;
                    hiStart = curKRow;  hiEnd = hi - k + curKRow + 1;

                    for (m=0; m<SIMD_WIDTH*CORES_PER_PCH; m++) {
                        // Output 0 if not relevant for the weight, otherwise data
                        if(WORD_BITS != 8)
                            dataFile << (hiIdx[l] >= hiEnd ? 0 : act[ciIdx[l]*wi*hi + hiIdx[l]*wi + wiIdx[l]]) << " ";
                        else
                            dataFile << int(hiIdx[l] >= hiEnd ? 0 : act[ciIdx[l]*wi*hi + hiIdx[l]*wi + wiIdx[l]]) << " "; //need to cast the results for int8 only
                        // Run through activations that should be convolved with current weight (avoiding edges if needed)
                        if (++wiIdx[l] == wiEnd) {
                            wiIdx[l] = wiStart;
                            ++hiIdx[l];
                        }
                    }
                    dataFile << endl;
                }
            }

            for (j=0; j<SRF_M_ENTRIES; j++) {
                // Prepare for next set of weights
                srfWeight[j] += SRF_M_ENTRIES;
                ciIdx[j] = ((srfWeight[j] % (ci*k*k))  % (ci*k*k)) / int(pow(k,2));
                hiIdx[j] = ((srfWeight[j] % (ci*k*k))  % int(pow(k,2))) / k;
                wiIdx[j] = ((srfWeight[j] % (ci*k*k))  % int(pow(k,2))) % k;
            }
            compKCh = (srfWeight[0] % (ci*k*k))  / int(pow(k,2));           // Computed channels of the filter so far
            compKRow = ((srfWeight[0] % (ci*k*k))  % int(pow(k,2))) / k;    // Computed rows of the filter so far
            compKCol = ((srfWeight[0] % (ci*k*k))  % int(pow(k,2))) % k;    // Computed columns of the filter so far
        }

        // Next set of weights, until reaching ext_peeling
        dataFile << "### Next set of weights" << endl;
        for (j=0; j<ext_loops; j++) {
            // Reset for computing the partial products
            woIdx = hoIdx = 0;

            // Weights
            for (l=0; l<SRF_M_ENTRIES; l++) {
                if(WORD_BITS != 8)
                    dataFile << weight[weightIdx++] << endl;
                else
                    dataFile << int(weight[weightIdx++]) << endl; //need to cast the results for int8 only
            }

            for (l=0; l<loops; l++) {
                // Provide partial results or zeros
                for (m=0; m<SIMD_WIDTH*CORES_PER_PCH; m++) {if (hoIdx < ho) {
                        if(WORD_BITS != 8)
                            dataFile << partPixConv(act, weight, bias, stride, k, ci, hi, wi, compKCh, compKRow, compKCol, i, hoIdx, woIdx) << " ";
                        else
                            dataFile << int(partPixConv(act, weight, bias, stride, k, ci, hi, wi, compKCh, compKRow, compKCol, i, hoIdx, woIdx)) << " "; //need to cast the results for int8 only
                        if (++woIdx == wo) {
                            woIdx = 0;
                            hoIdx++;
                        }
                    } else {
                        dataFile << 0 << " ";
                    }
                }
                dataFile << endl;

                // Activations
                for (m=0; m<SRF_M_ENTRIES; m++) {
                    // Set the start and end indeces for current weight
                    curKCh = (srfWeight[m] % (ci*k*k))  / int(pow(k,2));           // Current channel of the filter
                    curKRow = ((srfWeight[m] % (ci*k*k))  % int(pow(k,2))) / k;    // Current row of the filter
                    curKCol = ((srfWeight[m] % (ci*k*k))  % int(pow(k,2))) % k;    // Current column of the filter
                    wiStart = curKCol;  wiEnd = wi - k + curKCol + 1;
                    hiStart = curKRow;  hiEnd = hi - k + curKRow + 1;

                    for (n=0; n<SIMD_WIDTH*CORES_PER_PCH; n++) {
                        // Output 0 if not relevant for the weight, otherwise data
                        if(WORD_BITS != 8)
                            dataFile << (hiIdx[m] >= hiEnd ? 0 : act[ciIdx[m]*wi*hi + hiIdx[m]*wi + wiIdx[m]]) << " ";
                        else
                            dataFile << int(hiIdx[m] >= hiEnd ? 0 : act[ciIdx[m]*wi*hi + hiIdx[m]*wi + wiIdx[m]]) << " "; //need to cast the results for int8 only
                        // Run through activations that should be convolved with current weight (avoiding edges if needed)
                        if (++wiIdx[m] == wiEnd) {
                            wiIdx[m] = wiStart;
                            ++hiIdx[m];
                        }
                    }
                    dataFile << endl;
                }
            }

            for (l=0; l<SRF_M_ENTRIES; l++) {
                // Prepare for next set of weights
                srfWeight[l] += SRF_M_ENTRIES;
                ciIdx[l] = (srfWeight[l] % (ci*k*k))  / int(pow(k,2));
                hiIdx[l] = ((srfWeight[l] % (ci*k*k))  % int(pow(k,2))) / k;
                wiIdx[l] = ((srfWeight[l] % (ci*k*k))  % int(pow(k,2))) % k;
            }
            compKCh = (srfWeight[0] % (ci*k*k))  / int(pow(k,2));           // Computed channels of the filter so far
            compKRow = ((srfWeight[0] % (ci*k*k))  % int(pow(k,2))) / k;    // Computed rows of the filter so far
            compKCol = ((srfWeight[0] % (ci*k*k))  % int(pow(k,2))) % k;    // Computed columns of the filter so far
        }

        // Final set of weights (if ext_peeling)
        if (ext_peeling && ext_loops > -1) {
            dataFile << "### Peeled set of weights" << endl;
            // Reset for computing the partial products
            woIdx = hoIdx = 0;

            // Weights
            for (j=0; j<ext_peeling; j++) {
                if(WORD_BITS != 8)
                    dataFile << weight[weightIdx++] << endl;
                else
                    dataFile << int(weight[weightIdx++]) << endl; //need to cast the results for int8 only
            }

            for (j=0; j<loops; j++) {
                // Provide partial results or zeros
                for (l=0; l<SIMD_WIDTH*CORES_PER_PCH; l++) {
                    if (hoIdx < ho) {
                        if(WORD_BITS != 8)
                            dataFile << partPixConv(act, weight, bias, stride, k, ci, hi, wi, compKCh, compKRow, compKCol, i, hoIdx, woIdx) << " ";
                        else
                            dataFile << int(partPixConv(act, weight, bias, stride, k, ci, hi, wi, compKCh, compKRow, compKCol, i, hoIdx, woIdx)) << " "; //need to cast the results for int8 only
                        if (++woIdx == wo) {
                            woIdx = 0;
                            hoIdx++;
                        }
                    } else {
                        dataFile << 0 << " ";
                    }
                }
                dataFile << endl;

                // Activations
                for (l=0; l<ext_peeling; l++) {
                    // Set the start and end indeces for current weight
                    curKCh = (srfWeight[l] % (ci*k*k))  / int(pow(k,2));           // Current channel of the filter
                    curKRow = ((srfWeight[l] % (ci*k*k))  % int(pow(k,2))) / k;    // Current row of the filter
                    curKCol = ((srfWeight[l] % (ci*k*k))  % int(pow(k,2))) % k;    // Current column of the filter
                    wiStart = curKCol;  wiEnd = wi - k + curKCol + 1;
                    hiStart = curKRow;  hiEnd = hi - k + curKRow + 1;

                    for (m=0; m<SIMD_WIDTH*CORES_PER_PCH; m++) {
                        // Output 0 if not relevant for the weight, otherwise data
                        if(WORD_BITS != 8)
                            dataFile << (hiIdx[l] >= hiEnd ? 0 : act[ciIdx[l]*wi*hi + hiIdx[l]*wi + wiIdx[l]]) << " ";
                        else
                            dataFile << int(hiIdx[l] >= hiEnd ? 0 : act[ciIdx[l]*wi*hi + hiIdx[l]*wi + wiIdx[l]]) << " "; //need to cast the results for int8 only
                        // Run through activations that should be convolved with current weight (avoiding edges if needed)
                        if (++wiIdx[l] == wiEnd) {
                            wiIdx[l] = wiStart;
                            ++hiIdx[l];
                        }
                    }
                    dataFile << endl;
                }
            }
        } else if (ext_peeling) {
            dataFile << "### Peeled set of weights" << endl;
            // Weights
            for (j=0; j<ext_peeling; j++) {
                if(WORD_BITS != 8)
                    dataFile << weight[weightIdx++] << endl;
                else
                    dataFile << int(weight[weightIdx++]) << endl; //need to cast the results for int8 only
            }
            // Bias
            if(WORD_BITS != 8)
                dataFile << bias[i] << endl;
            else
                dataFile << int(bias[i]) << endl; //need to cast the results for int8 only

            // Activations
            for (j=0; j<loops; j++) {
                for (l=0; l<ext_peeling; l++) {
                    // Set the start and end indeces for current weight
                    curKCh = (srfWeight[l] % (ci*k*k))  / int(pow(k,2));           // Current channel of the filter
                    curKRow = ((srfWeight[l] % (ci*k*k))  % int(pow(k,2))) / k;    // Current row of the filter
                    curKCol = ((srfWeight[l] % (ci*k*k))  % int(pow(k,2))) % k;    // Current column of the filter
                    wiStart = curKCol;  wiEnd = wi - k + curKCol + 1;
                    hiStart = curKRow;  hiEnd = hi - k + curKRow + 1;

                    for (m=0; m<SIMD_WIDTH*CORES_PER_PCH; m++) {
                        // Output 0 if not relevant for the weight, otherwise data
                        if(WORD_BITS != 8)
                            dataFile << (hiIdx[l] >= hiEnd ? 0 : act[ciIdx[l]*wi*hi + hiIdx[l]*wi + wiIdx[l]]) << " ";
                        else
                            dataFile << int(hiIdx[l] >= hiEnd ? 0 : act[ciIdx[l]*wi*hi + hiIdx[l]*wi + wiIdx[l]]) << " "; //need to cast the results for int8 only
                        // Run through activations that should be convolved with current weight (avoiding edges if needed)
                        if (++wiIdx[l] == wiEnd) {
                            wiIdx[l] = wiStart;
                            ++hiIdx[l];
                        }
                    }
                    dataFile << endl;
                }
            }
        }
    }
}

void mapConvCWWRCLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t *act, cnm_t *weight, cnm_t *bias,
                                int ci, int wi, int hi, int k, int co, int wo, int ho, int stride)
{
    int i,j,l,m,n;

    int crfSegment = CRF_ENTRIES - 4;
    crfSegment = min(crfSegment, SRF_M_ENTRIES);         // If segment is larger than SRF capacity, limit it to the latter
    int ext_loops = floor(pow(float(k),2)*float(ci)/float(crfSegment)) - 1; 
    int loops = ceil(float(wo)*float(ho)/float(SIMD_WIDTH*CORES_PER_PCH));
    int ext_peeling = int(pow(k,2)*ci) % crfSegment;

    uint64_t weightBase = ceil(float(loops * k*k * ci / 2) / float(COLPERROW*CORES_PER_PCH)); // Div 2 because distributed in 2 banks
    uint64_t biasBase = weightBase + ceil(float(ceil(float(co) * k*k * ci / float(SIMD_WIDTH*CORES_PER_PCH)) / 2) / float(COLPERROW));
    uint64_t resBase = biasBase + ceil(float(ceil(float(co) / float(SIMD_WIDTH*CORES_PER_PCH)) / 2) / float(COLPERROW));

    uint64_t rowIdx[2] = {0, resBase};
    uint64_t colIdx[2] = {0,0};
    uint64_t resRowRet, resColRet;              // Stores row and column to return every loop to accumulate the results of one output channel
    uint64_t weightIdx;                         // Index to run through weights
    uint64_t srfWeight[crfSegment];             // Incedes of weight stored in each scalar register
    uint64_t ciIdx[crfSegment], hiIdx[crfSegment], wiIdx[crfSegment];   // Indeces to run through activations
    uint64_t curKCh, curKRow, curKCol;          // Holds equivalence of weight index for activation indexing
    uint64_t compKCh, compKRow, compKCol;       // Holds equivalence of weight index for computing partial convolution
    uint64_t wiStart, wiEnd, hiStart, hiEnd;    // Holds the limits of the activations considered for the current weight
    uint64_t woIdx, hoIdx;                      // Holds indeces for performing partial convolutions

    // Write assembly instructions
    assembly << "; Channel-wise mapping of convolution with weight reuse, C-limited, ci = " << ci << ", co = ";
    assembly << co << ", k = " << k << ", wo = " << wo << ", ho = " << ho << ", stride = " << stride << endl;

    for (i=0; i<co; i++) {

        assembly << "; Output channel " << i << endl;

        // First set of weights, where bias is also added
        if (ext_loops > -1) {
            assembly << "; First set of weights" << endl;
            assembly << "WRF CRF0" << endl;
            assembly << "MAD GRFB0 EVEN_BANK[AddrFile] SRFM0 SRFA0 DataFile" << endl;
            for (j=0; j<crfSegment/2; j++) {
                if (j)
                    assembly << "MAC GRFB0 EVEN_BANK[AddrFile] SRFM" << 2*j << " DataFile" << endl;
                assembly << "MAC GRFB0 ODD_BANK[AddrFile] SRFM" << 2*j+1 << " DataFile" << endl;
            }
            if (crfSegment % 2) {
                assembly << "MAC GRFB0 EVEN_BANK[AddrFile] SRFM" << 2*j << " DataFile" << endl;
            }
            assembly << "MOV ODD_BANK[AddrFile] GRFB0" << endl;
            if (loops-1)
                assembly << "JUMP " << crfSegment+1 << " " << loops-1 << endl;
            assembly << "EXIT" << endl;
            
            for (j=0; j<crfSegment; j++) { 
                assembly << "WRF SRFM" << j << " DataFile" << endl; // Weights
            }
            assembly << "WRF SRFA0 DataFile" << endl;   // Bias
            
            assembly << "EXEC" << endl;
        }

        // Next set of weights, until reaching ext_peeling
        if (ext_loops > 0) {
            assembly << "; Next set of weights" << endl;
            assembly << "WRF CRF0" << endl;
            assembly << "MOV GRFB0 ODD_BANK[AddrFile] DataFile" << endl;
            for (j=0; j<crfSegment/2; j++) {
                assembly << "MAC GRFB0 EVEN_BANK[AddrFile] SRFM" << 2*j << " DataFile" << endl;
                assembly << "MAC GRFB0 ODD_BANK[AddrFile] SRFM" << 2*j+1 << " DataFile" << endl;
            }
            if (crfSegment % 2) {
                assembly << "MAC GRFB0 EVEN_BANK[AddrFile] SRFM" << 2*j << " DataFile" << endl;
            }
            assembly << "MOV ODD_BANK[AddrFile] GRFB0" << endl;
            if (loops-1)
                assembly << "JUMP " << crfSegment+2 << " " << loops-1 << endl;
            assembly << "EXIT" << endl;

            for (j=0; j<ext_loops; j++) {
                for (l=0; l<crfSegment; l++) {   
                    assembly << "WRF SRFM" << l << " DataFile" << endl; // Weights
                }
                assembly << "EXEC" << endl;
            }
        }

        // Final set of weights (if ext_peeling)
        if (ext_peeling && ext_loops > -1) {
            assembly << "; Peeled set of weights" << endl;
            assembly << "WRF CRF" << ext_peeling+1 << endl; // Move forward the MOV to BANK, JUMP and EXIT
            assembly << "MOV ODD_BANK[AddrFile] GRFB0" << endl;
            if (loops-1)
                assembly << "JUMP " << ext_peeling+2 << " " << loops-1 << endl;
            assembly << "EXIT" << endl;

            for (j=0; j<ext_peeling; j++) {
                assembly << "WRF SRFM" << j << " DataFile" << endl; // Weights
            }
            assembly << "EXEC" << endl;
        } else if (ext_peeling) {
            assembly << "; Peeled set of weights" << endl;
            assembly << "WRF CRF0" << endl;
            assembly << "MAD GRFB0 EVEN_BANK[AddrFile] SRFM0 SRFA0 DataFile" << endl;
            for (j=0; j<ext_peeling/2; j++) {
                if (j)
                    assembly << "MAC GRFB0 EVEN_BANK[AddrFile] SRFM" << 2*j << " DataFile" << endl;
                assembly << "MAC GRFB0 ODD_BANK[AddrFile] SRFM" << 2*j+1 << " DataFile" << endl;
            }
            if (ext_peeling - 2*j)
                assembly << "MAC GRFB0 EVEN_BANK[AddrFile] SRFM" << 2*j << " DataFile" << endl;
            assembly << "MOV ODD_BANK[AddrFile] GRFB0" << endl;
            if (loops-1)
                assembly << "JUMP " << ext_peeling+1 << " " << loops-1 << endl;
            assembly << "EXIT" << endl;
            
            for (j=0; j<ext_peeling; j++) { 
                assembly << "WRF SRFM" << j << " DataFile" << endl; // Weights
            }
            assembly << "WRF SRFA0 DataFile" << endl;   // Bias
            
            assembly << "EXEC" << endl;
        }
    }
    

    // Write address for loops and others
    addrFile << "# Channel-wise mapping of convolution with weight reuse, C-limited, ci = " << ci << ", co = ";
    addrFile << co << ", k = " << k << ", wo = " << wo << ", ho = " << ho << ", stride = " << stride << endl;

    for (i=0; i<co; i++) {

        addrFile << "## Output channel " << i << endl;

        // Reset activation indeces for reading again for the new channel
        rowIdx[0] = 0;          colIdx[0] = 0;
        // Stores where the result row and col index to return to at the end of each loop
        resRowRet = rowIdx[1];  resColRet = colIdx[1];

        // First set of weights, where bias is also added
        if (ext_loops > -1) {
            addrFile << "### First set of weights" << endl;
            for (l=0; l<loops; l++) {
                for (m=0; m<crfSegment/2; m++) {
                    addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
                    addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
                    if (++colIdx[0] == COLPERROW) {
                        colIdx[0] = 0;
                        rowIdx[0]++;
                    }
                }
                if (crfSegment % 2) {
                    addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
                    if (++colIdx[0] == COLPERROW) {
                        colIdx[0] = 0;
                        rowIdx[0]++;
                    }
                }

                addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
                if (++colIdx[1] == COLPERROW) {
                    colIdx[1] = 0;
                    rowIdx[1]++;
                }
            }
        }

        // Next set of weights, until reaching ext_peeling
        if (ext_loops > 0) {
            addrFile << "### Next set of weights" << endl;
            for (j=0; j<ext_loops; j++) {
                rowIdx[1] = resRowRet;  colIdx[1] = resColRet;
                for (l=0; l<loops; l++) {
                    addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;

                    for (m=0; m<crfSegment/2; m++) {
                        addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
                        addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
                        if (++colIdx[0] == COLPERROW) {
                            colIdx[0] = 0;
                            rowIdx[0]++;
                        }
                    }
                    if (crfSegment % 2) {
                        addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
                        if (++colIdx[0] == COLPERROW) {
                            colIdx[0] = 0;
                            rowIdx[0]++;
                        }
                    }

                    addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
                    if (++colIdx[1] == COLPERROW) {
                        colIdx[1] = 0;
                        rowIdx[1]++;
                    }
                }
            }
        }

        // Final set of weights (if ext_peeling)
        if (ext_peeling) {
            addrFile << "### Peeled set of weights" << endl;
            rowIdx[1] = resRowRet;  colIdx[1] = resColRet;
            addrFile << "# Out of the main loop" << endl;
            for (j=0; j<loops; j++) {
                addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;

                for (l=0; l<ext_peeling/2; l++) {
                    addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
                    addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
                    if (++colIdx[0] == COLPERROW) {
                        colIdx[0] = 0;
                        rowIdx[0]++;
                    }
                }
                if (ext_peeling - 2*l)  // If n it's odd, last one is only with EVEN_BANK
                    addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;

                addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
                if (++colIdx[1] == COLPERROW) {
                    colIdx[1] = 0;
                    rowIdx[1]++;
                }
            }
        }
    }
    

    // Write data for loops and others
    dataFile << "# Channel-wise mapping of convolution with weight reuse, C-limited, ci = " << ci << ", co = ";
    dataFile << co << ", k = " << k << ", wo = " << wo << ", ho = " << ho << ", stride = " << stride << endl;
    
    for (i=0; i<co; i++) {

        dataFile << "## Output channel " << i << endl;
        weightIdx = i*ci*pow(k,2);
        for (j=0; j<crfSegment; j++) {
            srfWeight[j] = weightIdx + j;
            ciIdx[j] = (srfWeight[j] % (ci*k*k))  / int(pow(k,2));
            hiIdx[j] = ((srfWeight[j] % (ci*k*k))  % int(pow(k,2))) / k;
            wiIdx[j] = ((srfWeight[j] % (ci*k*k))  % int(pow(k,2))) % k;
        }

        // First set of weights, where bias is also added
        if (ext_loops > -1) {
            dataFile << "### First set of weights" << endl;
            // Weights
            for (j=0; j<crfSegment; j++) {
                if(WORD_BITS != 8)
                    dataFile << weight[weightIdx++] << endl;
                else
                    dataFile << int(weight[weightIdx++]) << endl; //need to cast the results for int8 only
            }
            // Bias
            if(WORD_BITS != 8)
                dataFile << bias[i] << endl;
            else
                dataFile << int(bias[i]) << endl; //need to cast the results for int8 only

            // Activations
            for (j=0; j<loops; j++) {
                for (l=0; l<crfSegment; l++) {
                    // Set the start and end indeces for current weight
                    curKCh = (srfWeight[l] % (ci*k*k))  / int(pow(k,2));           // Current channel of the filter
                    curKRow = ((srfWeight[l] % (ci*k*k))  % int(pow(k,2))) / k;    // Current row of the filter
                    curKCol = ((srfWeight[l] % (ci*k*k))  % int(pow(k,2))) % k;    // Current column of the filter
                    wiStart = curKCol;  wiEnd = wi - k + curKCol + 1;
                    hiStart = curKRow;  hiEnd = hi - k + curKRow + 1;

                    for (m=0; m<SIMD_WIDTH*CORES_PER_PCH; m++) {
                        // Output 0 if not relevant for the weight, otherwise data
                        if(WORD_BITS != 8)
                            dataFile << (hiIdx[l] >= hiEnd ? 0 : act[ciIdx[l]*wi*hi + hiIdx[l]*wi + wiIdx[l]]) << " ";
                        else
                            dataFile << int(hiIdx[l] >= hiEnd ? 0 : act[ciIdx[l]*wi*hi + hiIdx[l]*wi + wiIdx[l]]) << " "; //need to cast the results for int8 only
                        // Run through activations that should be convolved with current weight (avoiding edges if needed)
                        if (++wiIdx[l] == wiEnd) {
                            wiIdx[l] = wiStart;
                            ++hiIdx[l];
                        }
                    }
                    dataFile << endl;
                }
            }

            for (j=0; j<crfSegment; j++) {
                // Prepare for next set of weights
                srfWeight[j] += crfSegment;
                ciIdx[j] = ((srfWeight[j] % (ci*k*k))  % (ci*k*k)) / int(pow(k,2));
                hiIdx[j] = ((srfWeight[j] % (ci*k*k))  % int(pow(k,2))) / k;
                wiIdx[j] = ((srfWeight[j] % (ci*k*k))  % int(pow(k,2))) % k;
            }
            compKCh = (srfWeight[0] % (ci*k*k))  / int(pow(k,2));           // Computed channels of the filter so far
            compKRow = ((srfWeight[0] % (ci*k*k))  % int(pow(k,2))) / k;    // Computed rows of the filter so far
            compKCol = ((srfWeight[0] % (ci*k*k))  % int(pow(k,2))) % k;    // Computed columns of the filter so far
        }

        // Next set of weights, until reaching ext_peeling
        dataFile << "### Next set of weights" << endl;
        for (j=0; j<ext_loops; j++) {
            // Reset for computing the partial products
            woIdx = hoIdx = 0;

            // Weights
            for (l=0; l<crfSegment; l++) {
                if(WORD_BITS != 8)
                    dataFile << weight[weightIdx++] << endl;
                else
                    dataFile << int(weight[weightIdx++]) << endl; //need to cast the results for int8 only
            }

            for (l=0; l<loops; l++) {
                // Provide partial results or zeros
                for (m=0; m<SIMD_WIDTH*CORES_PER_PCH; m++) {if (hoIdx < ho) {
                        if(WORD_BITS != 8)
                            dataFile << partPixConv(act, weight, bias, stride, k, ci, hi, wi, compKCh, compKRow, compKCol, i, hoIdx, woIdx) << " ";
                        else
                            dataFile << int(partPixConv(act, weight, bias, stride, k, ci, hi, wi, compKCh, compKRow, compKCol, i, hoIdx, woIdx)) << " "; //need to cast the results for int8 only
                        if (++woIdx == wo) {
                            woIdx = 0;
                            hoIdx++;
                        }
                    } else {
                        dataFile << 0 << " ";
                    }
                }
                dataFile << endl;

                // Activations
                for (m=0; m<crfSegment; m++) {
                    // Set the start and end indeces for current weight
                    curKCh = (srfWeight[m] % (ci*k*k))  / int(pow(k,2));           // Current channel of the filter
                    curKRow = ((srfWeight[m] % (ci*k*k))  % int(pow(k,2))) / k;    // Current row of the filter
                    curKCol = ((srfWeight[m] % (ci*k*k))  % int(pow(k,2))) % k;    // Current column of the filter
                    wiStart = curKCol;  wiEnd = wi - k + curKCol + 1;
                    hiStart = curKRow;  hiEnd = hi - k + curKRow + 1;

                    for (n=0; n<SIMD_WIDTH*CORES_PER_PCH; n++) {
                        // Output 0 if not relevant for the weight, otherwise data
                        if(WORD_BITS != 8)
                            dataFile << (hiIdx[m] >= hiEnd ? 0 : act[ciIdx[m]*wi*hi + hiIdx[m]*wi + wiIdx[m]]) << " ";
                        else
                            dataFile << int(hiIdx[m] >= hiEnd ? 0 : act[ciIdx[m]*wi*hi + hiIdx[m]*wi + wiIdx[m]]) << " "; //need to cast the results for int8 only
                        // Run through activations that should be convolved with current weight (avoiding edges if needed)
                        if (++wiIdx[m] == wiEnd) {
                            wiIdx[m] = wiStart;
                            ++hiIdx[m];
                        }
                    }
                    dataFile << endl;
                }
            }

            for (l=0; l<crfSegment; l++) {
                // Prepare for next set of weights
                srfWeight[l] += crfSegment;
                ciIdx[l] = (srfWeight[l] % (ci*k*k))  / int(pow(k,2));
                hiIdx[l] = ((srfWeight[l] % (ci*k*k))  % int(pow(k,2))) / k;
                wiIdx[l] = ((srfWeight[l] % (ci*k*k))  % int(pow(k,2))) % k;
            }
            compKCh = (srfWeight[0] % (ci*k*k))  / int(pow(k,2));           // Computed channels of the filter so far
            compKRow = ((srfWeight[0] % (ci*k*k))  % int(pow(k,2))) / k;    // Computed rows of the filter so far
            compKCol = ((srfWeight[0] % (ci*k*k))  % int(pow(k,2))) % k;    // Computed columns of the filter so far
        }

        // Final set of weights (if ext_peeling)
        if (ext_peeling && ext_loops > -1) {
            dataFile << "### Peeled set of weights" << endl;
            // Reset for computing the partial products
            woIdx = hoIdx = 0;

            // Weights
            for (j=0; j<ext_peeling; j++) {
                if(WORD_BITS != 8)
                    dataFile << weight[weightIdx++] << endl;
                else
                    dataFile << int(weight[weightIdx++]) << endl; //need to cast the results for int8 only
            }

            for (j=0; j<loops; j++) {
                // Provide partial results or zeros
                for (l=0; l<SIMD_WIDTH*CORES_PER_PCH; l++) {
                    if (hoIdx < ho) {
                        if(WORD_BITS != 8)
                            dataFile << partPixConv(act, weight, bias, stride, k, ci, hi, wi, compKCh, compKRow, compKCol, i, hoIdx, woIdx) << " ";
                        else
                            dataFile << int(partPixConv(act, weight, bias, stride, k, ci, hi, wi, compKCh, compKRow, compKCol, i, hoIdx, woIdx)) << " "; //need to cast the results for int8 only
                        if (++woIdx == wo) {
                            woIdx = 0;
                            hoIdx++;
                        }
                    } else {
                        dataFile << 0 << " ";
                    }
                }
                dataFile << endl;

                // Activations
                for (l=0; l<ext_peeling; l++) {
                    // Set the start and end indeces for current weight
                    curKCh = (srfWeight[l] % (ci*k*k))  / int(pow(k,2));           // Current channel of the filter
                    curKRow = ((srfWeight[l] % (ci*k*k))  % int(pow(k,2))) / k;    // Current row of the filter
                    curKCol = ((srfWeight[l] % (ci*k*k))  % int(pow(k,2))) % k;    // Current column of the filter
                    wiStart = curKCol;  wiEnd = wi - k + curKCol + 1;
                    hiStart = curKRow;  hiEnd = hi - k + curKRow + 1;

                    for (m=0; m<SIMD_WIDTH*CORES_PER_PCH; m++) {
                        // Output 0 if not relevant for the weight, otherwise data
                        if(WORD_BITS != 8)
                            dataFile << (hiIdx[l] >= hiEnd ? 0 : act[ciIdx[l]*wi*hi + hiIdx[l]*wi + wiIdx[l]]) << " ";
                        else
                            dataFile << int(hiIdx[l] >= hiEnd ? 0 : act[ciIdx[l]*wi*hi + hiIdx[l]*wi + wiIdx[l]]) << " "; //need to cast the results for int8 only
                        // Run through activations that should be convolved with current weight (avoiding edges if needed)
                        if (++wiIdx[l] == wiEnd) {
                            wiIdx[l] = wiStart;
                            ++hiIdx[l];
                        }
                    }
                    dataFile << endl;
                }
            }
        } else if (ext_peeling) {
            dataFile << "### Peeled set of weights" << endl;
            // Weights
            for (j=0; j<ext_peeling; j++) {
                if(WORD_BITS != 8)
                    dataFile << weight[weightIdx++] << endl;
                else
                    dataFile << int(weight[weightIdx++]) << endl; //need to cast the results for int8 only
            }
            // Bias
            if(WORD_BITS != 8)
                dataFile << bias[i] << endl;
            else
                dataFile << int(bias[i]) << endl; //need to cast the results for int8 only

            // Activations
            for (j=0; j<loops; j++) {
                for (l=0; l<ext_peeling; l++) {
                    // Set the start and end indeces for current weight
                    curKCh = (srfWeight[l] % (ci*k*k))  / int(pow(k,2));           // Current channel of the filter
                    curKRow = ((srfWeight[l] % (ci*k*k))  % int(pow(k,2))) / k;    // Current row of the filter
                    curKCol = ((srfWeight[l] % (ci*k*k))  % int(pow(k,2))) % k;    // Current column of the filter
                    wiStart = curKCol;  wiEnd = wi - k + curKCol + 1;
                    hiStart = curKRow;  hiEnd = hi - k + curKRow + 1;

                    for (m=0; m<SIMD_WIDTH*CORES_PER_PCH; m++) {
                        // Output 0 if not relevant for the weight, otherwise data
                        if(WORD_BITS != 8)
                            dataFile << (hiIdx[l] >= hiEnd ? 0 : act[ciIdx[l]*wi*hi + hiIdx[l]*wi + wiIdx[l]]) << " ";
                        else
                            dataFile << int(hiIdx[l] >= hiEnd ? 0 : act[ciIdx[l]*wi*hi + hiIdx[l]*wi + wiIdx[l]]) << " "; //need to cast the results for int8 only
                        // Run through activations that should be convolved with current weight (avoiding edges if needed)
                        if (++wiIdx[l] == wiEnd) {
                            wiIdx[l] = wiStart;
                            ++hiIdx[l];
                        }
                    }
                    dataFile << endl;
                }
            }
        }
    }
}

#if (HALF_FLOAT)

float partPixConv(float *act, float *weight, float *bias, int stride, int k,
                    int ci, int hi, int wi, int ck, int hk, int wk, int co, int ho, int wo) {
    half_float::half res = half_float::half_cast<half_float::half>(bias[co]);

    // Convolve through all input channels except current one
    for (int i=0; i<ck; i++) {
        for (int j=0; j<k; j++) {
            for (int l=0; l<k; l++) {
                res += half_float::half_cast<half_float::half>(act[i*hi*wi + (ho*stride + j)*wi + (wo*stride + l)]) *
                        half_float::half_cast<half_float::half>(weight[co*ci*k*k + i*k*k + j*k + l]);
            }
        }
    }

    // Convolve partially through current input channel
    for (int j=0; j<hk; j++) {
        for (int l=0; l<k; l++) {
            res += half_float::half_cast<half_float::half>(act[ck*hi*wi + (ho*stride + j)*wi + (wo*stride + l)]) *
                    half_float::half_cast<half_float::half>(weight[co*ci*k*k + ck*k*k + j*k + l]);
        }
    }
    for (int l=0; l<wk; l++) {
        res += half_float::half_cast<half_float::half>(act[ck*hi*wi + (ho*stride + hk)*wi + (wo*stride + l)]) *
                half_float::half_cast<half_float::half>(weight[co*ci*k*k + ck*k*k + hk*k + l]);
    }

    // cout << res << endl;

    return float(res);
}

#else

cnm_t partPixConv(cnm_t *act, cnm_t *weight, cnm_t *bias, int stride, int k,
                    int ci, int hi, int wi, int ck, int hk, int wk, int co, int ho, int wo) {
    cnm_t res = cnm_t(bias[co]);

    // Convolve through all input channels except current one
    for (int i=0; i<ck; i++) {
        for (int j=0; j<k; j++) {
            for (int l=0; l<k; l++) {
                res += cnm_t(act[i*hi*wi + (ho*stride + j)*wi + (wo*stride + l)]) *
                        cnm_t(weight[co*ci*k*k + i*k*k + j*k + l]);
            }
        }
    }

    // Convolve partially through current input channel
    for (int j=0; j<hk; j++) {
        for (int l=0; l<k; l++) {
            res += cnm_t(act[ck*hi*wi + (ho*stride + j)*wi + (wo*stride + l)]) *
                    cnm_t(weight[co*ci*k*k + ck*k*k + j*k + l]);
        }
    }
    for (int l=0; l<wk; l++) {
        res += cnm_t(act[ck*hi*wi + (ho*stride + hk)*wi + (wo*stride + l)]) *
                cnm_t(weight[co*ci*k*k + ck*k*k + hk*k + l]);
    }

    // cout << res << endl;

    return cnm_t(res);
}

#endif  // HALF_FLOAT
