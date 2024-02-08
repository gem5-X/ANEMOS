#include "map_mm.h"

void mapMatrixMultSrfRLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t *op1, cnm_t *op2, int m, int n, int q)
{
    int i,j,k,l,p;
    int ext_loops = floor(float(n)/float(SRF_M_ENTRIES));
    int loops = ceil(float(q)/float(SIMD_WIDTH*CORES_PER_PCH)) ;
    int ext_peeling = n % SRF_M_ENTRIES;
    uint64_t op2Base = ceil(float(m)*float(n)/float(SIMD_WIDTH)/float(COLPERROW*CORES_PER_PCH) / 2);          // Op2 will start in this row
    uint64_t resBase = op2Base + ceil((n * ceil(float(q)/float(SIMD_WIDTH))) / float(COLPERROW*CORES_PER_PCH) / 2); // Result will start in this row
    uint64_t zeroBase = resBase + ceil((m * ceil(float(q)/float(SIMD_WIDTH))) / float(COLPERROW*CORES_PER_PCH));    // Column fill with zeros, after the result
    uint64_t rowIdx[2] = {op2Base,resBase};
    uint64_t colIdx[2] = {0,0};
    uint64_t op1Idx = 0;    // Index to run through matrix 1
    uint64_t op2Idx = 0;    // Index to run through matrix 2
    uint64_t curResRow, curResCol;  // Hold the indeces of the results being written in every external iteration
        
    // Write assembly instructions
    assembly << "; Mapping of matrix multiplication using SRF, R-limited, mxn * nxq, m = ";
    assembly << m << ", n = " << n << ", q = " << q << endl;

    if (ext_loops) {
        assembly << "WRF CRF0" << endl;
        assembly << "MOV GRFB0 ODD_BANK[AddrFile] DataFile" << endl;
        for (i=0; i<SRF_M_ENTRIES/2; i++) {
            assembly << "MAC GRFB0 EVEN_BANK[AddrFile] SRFM" << 2*i << " DataFile" << endl;
            assembly << "MAC GRFB0 ODD_BANK[AddrFile] SRFM" << 2*i+1 << " DataFile" << endl;
        }
        assembly << "MOV ODD_BANK[AddrFile] GRFB0" << endl;
        if (loops-1)
            assembly << "JUMP " << SRF_M_ENTRIES+2 << " " << loops-1 << endl;
        assembly << "EXIT" << endl;

        for (i=0; i<m*ext_loops; i++) {
            for (j=0; j<SRF_M_ENTRIES; j++) {   
                assembly << "WRF SRFM" << j << " DataFile" << endl; // Weights
            }
            assembly << "EXEC" << endl;
        }
    }

    if (ext_peeling && ext_loops) {
        assembly << "; Peeled set of matrices" << endl;
        assembly << "WRF CRF" << ext_peeling+1 << endl; // Move forward the MOV to BANK, JUMP and EXIT
        assembly << "MOV ODD_BANK[AddrFile] GRFB0" << endl;
        if (loops-1)
            assembly << "JUMP " << ext_peeling+2 << " " << loops-1 << endl;
        assembly << "EXIT" << endl;

        for (i=0; i<m; i++) {
            for (j=0; j<ext_peeling; j++) {   
                assembly << "WRF SRFM" << j << " DataFile" << endl; // Weights
            }
            assembly << "EXEC" << endl;
        }

    } else if (ext_peeling) {
        assembly << "WRF CRF0" << endl;
        assembly << "MOV GRFB0 ODD_BANK[AddrFile] DataFile" << endl;
        for (i=0; i<ext_peeling/2; i++) {
            assembly << "MAC GRFB0 EVEN_BANK[AddrFile] SRFM" << 2*i << " DataFile" << endl;
            assembly << "MAC GRFB0 ODD_BANK[AddrFile] SRFM" << 2*i+1 << " DataFile" << endl;
        }
        if (ext_peeling - 2*i)
            assembly << "MAC GRFB0 EVEN_BANK[AddrFile] SRFM" << 2*i << " DataFile" << endl;
        assembly << "MOV ODD_BANK[AddrFile] GRFB0" << endl;
        if (loops-1)
            assembly << "JUMP " << ext_peeling+2 << " " << loops-1 << endl;
        assembly << "EXIT" << endl;

        for (i=0; i<m; i++) {
            for (j=0; j<ext_peeling; j++) {   
                assembly << "WRF SRFM" << j << " DataFile" << endl; // Weights
            }
            assembly << "EXEC" << endl;
        }
    }

    // Write address for loops and others
    addrFile << "# Mapping of matrix multiplication using SRF, R-limited, mxn * nxq, m = ";
    addrFile << m << ", n = " << n << ", q = " << q << endl;
    for (i=0; i<m; i++) {
        rowIdx[0] = op2Base;    colIdx[0] = 0;          // Go back to the start of op2 for computing next result row
        curResRow = rowIdx[1];  curResCol = colIdx[1];  // Store indeces for accumulating at every ext iteration 
        for (j=0; j<ext_loops; j++) {
            rowIdx[1] = curResRow;  colIdx[1] = curResCol;
            for (k=0; k<loops; k++) {
                // (j * SRF_M_ENTRIES * ceil(float(q)/float(SIMD_WIDTH)) + k) is # DRAM columns from op2 start to current
                rowIdx[0] = op2Base + (j * SRF_M_ENTRIES * int(ceil(float(q/2)/float(SIMD_WIDTH*CORES_PER_PCH))) + k) / COLPERROW;
                colIdx[0] = (j * SRF_M_ENTRIES * int(ceil(float(q/2)/float(SIMD_WIDTH*CORES_PER_PCH))) + k) % COLPERROW;
                if (colIdx[0] >= COLPERROW) cout << "j " << j << " k " << k << " col0 " << colIdx[0] << endl;
                // MOV to GRF_B
                addrFile << showbase << hex << (j ? build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) : build_addr({0,0,0,1,zeroBase,0})) << endl;
                // MACs
                for (l=0; l<SRF_M_ENTRIES/2; l++) {
                    addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
                    addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
                    //  ceil(float(q/2)/float(SIMD_WIDTH)) is the ammount of DRAM columns a op2 row takes
                    // / 2 because we use divide it in 2 banks
                    rowIdx[0] += (colIdx[0] + int(ceil(float(q/2)/float(SIMD_WIDTH*CORES_PER_PCH)))) / COLPERROW;
                    colIdx[0] = (colIdx[0] + int(ceil(float(q/2)/float(SIMD_WIDTH*CORES_PER_PCH)))) % COLPERROW;
                    if (colIdx[0] >= COLPERROW) cout << "j " << j << " k " << k << " l " << " col0 " << colIdx[0] << endl;
                }
                // MOV to BANK
                addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
                if (++colIdx[1] == COLPERROW) {
                    colIdx[1] = 0;
                    rowIdx[1]++;
                }
            }
        }
    }

    if (ext_peeling) {
        addrFile << "# Peeled set of the matrices" << endl;
        // Go back to first results to continue accumulating
        rowIdx[1] = resBase;        colIdx[1] = 0;

        for (i=0; i<m; i++) {
            for (k=0; k<loops; k++) {
                // (j * SRF_M_ENTRIES * ceil(float(q)/float(SIMD_WIDTH)) + k) is # DRAM columns from op2 start to current
                rowIdx[0] = resBase + k / COLPERROW;
                colIdx[0] = (j * SRF_M_ENTRIES * int(ceil(float(q/2)/float(SIMD_WIDTH*CORES_PER_PCH))) + k) % COLPERROW;
                // MOV to GRF_B
                addrFile << showbase << hex << (ext_loops ? build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) : build_addr({0,0,0,1,zeroBase,0})) << endl;
                // MACs
                for (l=0; l<ext_peeling/2; l++) {
                    addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
                    addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
                    //  ceil(float(q/2)/float(SIMD_WIDTH)) is the ammount of DRAM columns a op2 row takes
                    // / 2 because we use divide it in 2 banks
                    rowIdx[0] += (colIdx[0] + int(ceil(float(q/2)/float(SIMD_WIDTH*CORES_PER_PCH)))) / COLPERROW;
                    colIdx[0] = (colIdx[0] + int(ceil(float(q/2)/float(SIMD_WIDTH*CORES_PER_PCH)))) % COLPERROW;
                }
                if (ext_peeling - 2*l)  // If n it's odd, last one is only with EVEN_BANK
                    addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
                // MOV to BANK
                addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
                if (++colIdx[1] == COLPERROW) {
                    colIdx[1] = 0;
                    rowIdx[1]++;
                }
            }
        }
    }

    // Write data for loops and others
    dataFile << "# Mapping of matrix multiplication using SRF, R-limited, mxn * nxq, m = ";
    dataFile << m << ", n = " << n << ", q = " << q << endl;
    for (i=0; i<m; i++) {
        op1Idx = i*n;   // Set the op1 index to the start of the row (to jump over the peeled part)
        for (j=0; j<ext_loops; j++) {
            // Move the op1 to the SRF
            for (k=0; k<SRF_M_ENTRIES; k++) {
                if(WORD_BITS != 8)
                    dataFile << op1[op1Idx++] << endl;
                else
                    dataFile << int(op1[op1Idx++]) << endl;
            }
            for (k=0; k<loops; k++) {
                // Move 0 or partial result to GRFB (0 if starting to accumulate a result or if out of bounds)
                for (l=0; l<SIMD_WIDTH*CORES_PER_PCH; l++) {
                    if(WORD_BITS != 8)
                        dataFile << (j && (k*SIMD_WIDTH+l<q) ? partMatMultDP(op1, op2, m, n, q, i, k*SIMD_WIDTH+l, j*SRF_M_ENTRIES) : 0) << " ";
                    else
                        dataFile << int(j && (k*SIMD_WIDTH+l<q) ? partMatMultDP(op1, op2, m, n, q, i, k*SIMD_WIDTH+l, j*SRF_M_ENTRIES) : 0) << " "; //need to cast the results for int8 only
                }
                dataFile << endl;
                // Run through op2 for MAC
                for (l=0; l<SRF_M_ENTRIES; l++) {
                    op2Idx = (j * SRF_M_ENTRIES + l) * q + k*SIMD_WIDTH*CORES_PER_PCH; // Set the op2 index to the first lane of the SIMD
                    for (p=0; p<SIMD_WIDTH*CORES_PER_PCH; p++) {
                        if(WORD_BITS != 8)
                            dataFile << (op2Idx < (j*SRF_M_ENTRIES+l+1)*q ? op2[op2Idx++] : 0) << " ";
                        else
                            dataFile << int(op2Idx < (j*SRF_M_ENTRIES+l+1)*q ? op2[op2Idx++] : 0) << " "; //need to cast the results for int8 only
                    }
                    dataFile << endl;
                }
            }
        }
    }

    if (ext_peeling) {
        dataFile << "# Peeled set of the matrices" << endl;
        for (i=0; i<m; i++) {
            op1Idx = i*n+ext_loops*SRF_M_ENTRIES;   // Set the op1 index to the start of the row (to jump over the peeled part)
            // Move the op1 to the SRF
            for (j=0; j<ext_peeling; j++) {
                if(WORD_BITS != 8)
                    dataFile << op1[op1Idx++] << endl;
                else
                    dataFile << int(op1[op1Idx++]) << endl; //need to cast the results for int8 only
            }
            for (j=0; j<loops; j++) {
                // Move 0 or partial result to GRFB (0 if starting to accumulate a result or if out of bounds)
                for (k=0; k<SIMD_WIDTH*CORES_PER_PCH; k++) {
                    if(WORD_BITS != 8)
                        dataFile << (ext_loops && (j*SIMD_WIDTH+k<q) ? partMatMultDP(op1, op2, m, n, q, i, j*SIMD_WIDTH+k, ext_loops*SRF_M_ENTRIES) : 0) << " ";
                    else
                        dataFile << int(ext_loops && (j*SIMD_WIDTH+k<q) ? partMatMultDP(op1, op2, m, n, q, i, j*SIMD_WIDTH+k, ext_loops*SRF_M_ENTRIES) : 0) << " "; //need to cast the results for int8 only
                }
                dataFile << endl;
                // Run through op2 for MAC
                for (k=0; k<ext_peeling; k++) {
                    op2Idx = (ext_loops * SRF_M_ENTRIES + k) * q + j*SIMD_WIDTH*CORES_PER_PCH; // Set the op2 index to the first lane of the SIMD
                    for (p=0; p<SIMD_WIDTH*CORES_PER_PCH; p++) {
                        if(WORD_BITS != 8)
                            dataFile << (op2Idx < (ext_loops*SRF_M_ENTRIES+k+1)*q ? op2[op2Idx++] : 0) << " ";
                        else 
                            dataFile << int(op2Idx < (ext_loops*SRF_M_ENTRIES+k+1)*q ? op2[op2Idx++] : 0) << " "; //need to cast the results for int8 only
                    }
                    dataFile << endl;
                }
            }
        }
    }
}

void mapMatrixMultSrfCLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t *op1, cnm_t *op2, int m, int n, int q)
{
    int i,j,k,l,p;
    int crfSegment = CRF_ENTRIES - 4;
    crfSegment = min(crfSegment, SRF_M_ENTRIES);         // If segment is larger than SRF capacity, limit it to the latter
    int ext_loops = floor(float(n)/float(crfSegment));
    int loops = ceil(float(q)/float(SIMD_WIDTH*CORES_PER_PCH)) ;
    int ext_peeling = n % crfSegment;
    uint64_t op2Base = ceil(float(m)*float(n)/float(SIMD_WIDTH*CORES_PER_PCH)/float(COLPERROW) / 2);          // Op2 will start in this row
    uint64_t resBase = op2Base + ceil((n * ceil(float(q)/float(SIMD_WIDTH*CORES_PER_PCH))) / float(COLPERROW) / 2); // Result will start in this row
    uint64_t zeroBase = resBase + ceil((m * ceil(float(q)/float(SIMD_WIDTH*CORES_PER_PCH))) / float(COLPERROW));    // Column fill with zeros, after the result
    uint64_t rowIdx[2] = {op2Base,resBase};
    uint64_t colIdx[2] = {0,0};
    uint64_t op1Idx = 0;    // Index to run through matrix 1
    uint64_t op2Idx = 0;    // Index to run through matrix 2
    uint64_t curResRow, curResCol;  // Hold the indeces of the results being written in every external iteration
        
    // Write assembly instructions
    assembly << "; Mapping of matrix multiplication using SRF, C-limited, mxn * nxq, m = ";
    assembly << m << ", n = " << n << ", q = " << q << endl;

    if (ext_loops) {
        assembly << "WRF CRF0" << endl;
        assembly << "MOV GRFB0 ODD_BANK[AddrFile] DataFile" << endl;
        for (i=0; i<crfSegment/2; i++) {
            assembly << "MAC GRFB0 EVEN_BANK[AddrFile] SRFM" << 2*i << " DataFile" << endl;
            assembly << "MAC GRFB0 ODD_BANK[AddrFile] SRFM" << 2*i+1 << " DataFile" << endl;
        }
        if (crfSegment % 2) {
            assembly << "MAC GRFB0 EVEN_BANK[AddrFile] SRFM" << 2*i << " DataFile" << endl;
        }
        assembly << "MOV ODD_BANK[AddrFile] GRFB0" << endl;
        if (loops-1)
            assembly << "JUMP " << crfSegment+2 << " " << loops-1 << endl;
        assembly << "EXIT" << endl;

        for (i=0; i<m*ext_loops; i++) {
            for (j=0; j<crfSegment; j++) {   
                assembly << "WRF SRFM" << j << " DataFile" << endl; // Weights
            }
            assembly << "EXEC" << endl;
        }
    }

    if (ext_peeling && ext_loops) {
        assembly << "; Peeled set of matrices" << endl;
        assembly << "WRF CRF" << ext_peeling+1 << endl; // Move forward the MOV to BANK, JUMP and EXIT
        assembly << "MOV ODD_BANK[AddrFile] GRFB0" << endl;
        if (loops-1)
            assembly << "JUMP " << ext_peeling+2 << " " << loops-1 << endl;
        assembly << "EXIT" << endl;

        for (i=0; i<m; i++) {
            for (j=0; j<ext_peeling; j++) {   
                assembly << "WRF SRFM" << j << " DataFile" << endl; // Weights
            }
            assembly << "EXEC" << endl;
        }

    } else if (ext_peeling) {
        assembly << "WRF CRF0" << endl;
        assembly << "MOV GRFB0 ODD_BANK[AddrFile] DataFile" << endl;
        for (i=0; i<ext_peeling/2; i++) {
            assembly << "MAC GRFB0 EVEN_BANK[AddrFile] SRFM" << 2*i << " DataFile" << endl;
            assembly << "MAC GRFB0 ODD_BANK[AddrFile] SRFM" << 2*i+1 << " DataFile" << endl;
        }
        if (ext_peeling - 2*i)
            assembly << "MAC GRFB0 EVEN_BANK[AddrFile] SRFM" << 2*i << " DataFile" << endl;
        assembly << "MOV ODD_BANK[AddrFile] GRFB0" << endl;
        if (loops-1)
            assembly << "JUMP " << ext_peeling+2 << " " << loops-1 << endl;
        assembly << "EXIT" << endl;

        for (i=0; i<m; i++) {
            for (j=0; j<ext_peeling; j++) {   
                assembly << "WRF SRFM" << j << " DataFile" << endl; // Weights
            }
            assembly << "EXEC" << endl;
        }
    }

    // Write address for loops and others
    addrFile << "# Mapping of matrix multiplication using SRF, C-limited, mxn * nxq, m = ";
    addrFile << m << ", n = " << n << ", q = " << q << endl;
    for (i=0; i<m; i++) {
        rowIdx[0] = op2Base;    colIdx[0] = 0;          // Go back to the start of op2 for computing next result row
        curResRow = rowIdx[1];  curResCol = colIdx[1];  // Store indeces for accumulating at every ext iteration 
        for (j=0; j<ext_loops; j++) {
            rowIdx[1] = curResRow;  colIdx[1] = curResCol;
            for (k=0; k<loops; k++) {
                // (j * crfSegment * ceil(float(q)/float(SIMD_WIDTH)) + k) is # DRAM columns from op2 start to current
                rowIdx[0] = op2Base + (j * crfSegment * int(ceil(float(q/2)/float(SIMD_WIDTH*CORES_PER_PCH))) + k) / COLPERROW;
                colIdx[0] = (j * crfSegment * int(ceil(float(q/2)/float(SIMD_WIDTH*CORES_PER_PCH))) + k) % COLPERROW;
                // MOV to GRF_B
                addrFile << showbase << hex << (j ? build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) : build_addr({0,0,0,1,zeroBase,0})) << endl;
                // MACs
                for (l=0; l<crfSegment/2; l++) {
                    addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
                    addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
                    //  ceil(float(q/2)/float(SIMD_WIDTH)) is the ammount of DRAM columns a op2 row takes
                    // / 2 because we use divide it in 2 banks
                    rowIdx[0] += (colIdx[0] + int(ceil(float(q/2)/float(SIMD_WIDTH*CORES_PER_PCH)))) / COLPERROW;
                    colIdx[0] = (colIdx[0] + int(ceil(float(q/2)/float(SIMD_WIDTH*CORES_PER_PCH)))) % COLPERROW;
                }
                if (crfSegment % 2) {
                    addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
                    //  ceil(float(q/2)/float(SIMD_WIDTH)) is the ammount of DRAM columns a op2 row takes
                    // / 2 because we use divide it in 2 banks
                    rowIdx[0] += (colIdx[0] + int(ceil(float(q/2)/float(SIMD_WIDTH*CORES_PER_PCH)))) / COLPERROW;
                    colIdx[0] = (colIdx[0] + int(ceil(float(q/2)/float(SIMD_WIDTH*CORES_PER_PCH)))) % COLPERROW;
                }
                // MOV to BANK
                addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
                if (++colIdx[1] == COLPERROW) {
                    colIdx[1] = 0;
                    rowIdx[1]++;
                }
            }
        }
    }

    if (ext_peeling) {
        addrFile << "# Peeled set of matrices" << endl;
        // Go back to first results to continue accumulating
        rowIdx[1] = resBase;        colIdx[1] = 0;

        for (i=0; i<m; i++) {
            for (k=0; k<loops; k++) {
                // (j * crfSegment * ceil(float(q)/float(SIMD_WIDTH)) + k) is # DRAM columns from op2 start to current
                rowIdx[0] = resBase + k / COLPERROW;
                colIdx[0] = (j * crfSegment * int(ceil(float(q/2)/float(SIMD_WIDTH*CORES_PER_PCH))) + k) % COLPERROW;
                // MOV to GRF_B
                addrFile << showbase << hex << (ext_loops ? build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) : build_addr({0,0,0,1,zeroBase,0})) << endl;
                // MACs
                for (l=0; l<ext_peeling/2; l++) {
                    addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
                    addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
                    //  ceil(float(q/2)/float(SIMD_WIDTH)) is the ammount of DRAM columns a op2 row takes
                    // / 2 because we use divide it in 2 banks
                    rowIdx[0] += (colIdx[0] + int(ceil(float(q/2)/float(SIMD_WIDTH*CORES_PER_PCH)))) / COLPERROW;
                    colIdx[0] = (colIdx[0] + int(ceil(float(q/2)/float(SIMD_WIDTH*CORES_PER_PCH)))) % COLPERROW;
                }
                if (ext_peeling - 2*l)  // If n it's odd, last one is only with EVEN_BANK
                    addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
                // MOV to BANK
                addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
                if (++colIdx[1] == COLPERROW) {
                    colIdx[1] = 0;
                    rowIdx[1]++;
                }
            }
        }
    }

    // Write data for loops and others
    dataFile << "# Mapping of matrix multiplication using SRF, C-limited, mxn * nxq, m = ";
    dataFile << m << ", n = " << n << ", q = " << q << endl;
    for (i=0; i<m; i++) {
        op1Idx = i*n;   // Set the op1 index to the start of the row (to jump over the peeled part)
        for (j=0; j<ext_loops; j++) {
            // Move the op1 to the SRF
            for (k=0; k<crfSegment; k++) {
                if(WORD_BITS != 8)
                    dataFile << op1[op1Idx++] << endl;
                else
                    dataFile << int(op1[op1Idx++]) << endl; //need to cast the results for int8 only
            }
            for (k=0; k<loops; k++) {
                // Move 0 or partial result to GRFB (0 if starting to accumulate a result or if out of bounds)
                for (l=0; l<SIMD_WIDTH*CORES_PER_PCH; l++) {
                    if(WORD_BITS != 8)
                        dataFile << (j && (k*SIMD_WIDTH+l<q) ? partMatMultDP(op1, op2, m, n, q, i, k*SIMD_WIDTH+l, j*crfSegment) : 0) << " ";
                    else
                        dataFile << int(j && (k*SIMD_WIDTH+l<q) ? partMatMultDP(op1, op2, m, n, q, i, k*SIMD_WIDTH+l, j*crfSegment) : 0) << " "; //need to cast the results for int8 only
                }
                dataFile << endl;
                // Run through op2 for MAC
                for (l=0; l<crfSegment; l++) {
                    op2Idx = (j * crfSegment + l) * q + k*SIMD_WIDTH*CORES_PER_PCH; // Set the op2 index to the first lane of the SIMD
                    for (p=0; p<SIMD_WIDTH*CORES_PER_PCH; p++) {
                        if(WORD_BITS != 8)
                            dataFile << (op2Idx < (j*crfSegment+l+1)*q ? op2[op2Idx++] : 0) << " ";
                        else
                            dataFile << int(op2Idx < (j*crfSegment+l+1)*q ? op2[op2Idx++] : 0) << " "; //need to cast the results for int8 only
                    }
                    dataFile << endl;
                }
            }
        }
    }

    if (ext_peeling) {
        dataFile << "# Peeled set of matrices" << endl;
        for (i=0; i<m; i++) {
            op1Idx = i*n+ext_loops*crfSegment;   // Set the op1 index to the start of the row (to jump over the peeled part)
            // Move the op1 to the SRF
            for (j=0; j<ext_peeling; j++) {
                if(WORD_BITS != 8)
                    dataFile << op1[op1Idx++] << endl;
                else 
                    dataFile << int(op1[op1Idx++]) << endl; //need to cast the results for int8 only
            }
            for (j=0; j<loops; j++) {
                // Move 0 or partial result to GRFB (0 if starting to accumulate a result or if out of bounds)
                for (k=0; k<SIMD_WIDTH*CORES_PER_PCH; k++) {
                    if(WORD_BITS != 8)
                        dataFile << (ext_loops && (j*SIMD_WIDTH+k<q) ? partMatMultDP(op1, op2, m, n, q, i, j*SIMD_WIDTH+k, ext_loops*crfSegment) : 0) << " ";
                    else
                        dataFile << int(ext_loops && (j*SIMD_WIDTH+k<q) ? partMatMultDP(op1, op2, m, n, q, i, j*SIMD_WIDTH+k, ext_loops*crfSegment) : 0) << " "; //need to cast the results for int8 only
                }
                dataFile << endl;
                // Run through op2 for MAC
                for (k=0; k<ext_peeling; k++) {
                    op2Idx = (ext_loops * crfSegment + k) * q + j*SIMD_WIDTH*CORES_PER_PCH; // Set the op2 index to the first lane of the SIMD
                    for (p=0; p<SIMD_WIDTH*CORES_PER_PCH; p++) {
                        if(WORD_BITS != 8)
                            dataFile << (op2Idx < (ext_loops*crfSegment+k+1)*q ? op2[op2Idx++] : 0) << " ";
                        else
                            dataFile << int(op2Idx < (ext_loops*crfSegment+k+1)*q ? op2[op2Idx++] : 0) << " "; //need to cast the results for int8 only
                    }
                    dataFile << endl;
                }
            }
        }
    }
}

#if (HALF_FLOAT)

float partMatMultDP(float *op1, float *op2, int m, int n, int q, int rowOp1, int colOp2, int len) {
    half_float::half res = half_float::half_cast<half_float::half>(0);
    for (int i=0; i<len; i++) {
        res += half_float::half_cast<half_float::half>(op1[rowOp1*n+i]) * half_float::half_cast<half_float::half>(op2[i*q+colOp2]);
    }
    return float(res);
}

#else

cnm_t partMatMultDP(cnm_t *op1, cnm_t *op2, int m, int n, int q, int rowOp1, int colOp2, int len) {
    cnm_t res = cnm_t(0);
    for (int i=0; i<len; i++) {
        res += (cnm_t)(op1[rowOp1*n+i]) * (cnm_t)(op2[i*q+colOp2]);
    }
    return (cnm_t)(res);
}

#endif