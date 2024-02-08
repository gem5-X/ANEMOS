#include "map_va.h"

void mapEWAdditionRowWiseRLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t **op1, cnm_t **op2, int V, int n)
{
    int i,j,k;
    int loops = floor(ceil(float(n*V)/float(SIMD_WIDTH*CORES_PER_PCH))/(2*GRF_ENTRIES));
    int peeling = int(ceil(float(n*V)/float(SIMD_WIDTH*CORES_PER_PCH))) % (2*GRF_ENTRIES);
    int totalCol = ceil((float(n*V)/2.0)/float(SIMD_WIDTH));                    // Div 2 because distributed in 2 banks
    uint64_t op2Base = ceil(float(totalCol)/float(COLPERROW*CORES_PER_PCH));    // Op2 will start in this row. Take into account parallel bank access within pCH.
    uint64_t resBase = op2Base + ceil(float(totalCol)/float(COLPERROW*CORES_PER_PCH));  // Result will start in this row
    uint64_t rowIdx[3] = {0,op2Base,resBase};
    uint64_t colIdx[3] = {0,0,0};
    int nIdx[2] = {0,0};
    int VIdx[2] = {0,0};

    // Write assembly instructions
    assembly << "; Row-wise mapping of element-wise addition, R-limited, V = " << V << ", n = " << n << endl;
    assembly << "WRF CRF0" << endl;

    if (loops) {
        for (i=0; i<GRF_ENTRIES; i++) {
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "MOV GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        for (i=0; i<GRF_ENTRIES; i++) {
            assembly << "ADD GRFA" << i << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "ADD GRFB" << i << " GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        for (i=0; i<GRF_ENTRIES; i++) {
            assembly << "MOV EVEN_BANK[AddrFile] GRFA" << i << endl;
            assembly << "MOV ODD_BANK[AddrFile] GRFB" << i << endl;
        }
        if (loops-1)
            assembly << "JUMP " << 6*GRF_ENTRIES << " " << loops-1 << endl;
    }

    if (peeling) {
        assembly << "; Out of the main loop" << endl;
        for (i=0; i<peeling/2; i++) {
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "MOV GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (peeling - 2*i)  // If n*V it's odd, last one is only with GRF
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
        for (i=0; i<peeling/2; i++) {
            assembly << "ADD GRFA" << i << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "ADD GRFB" << i << " GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (peeling - 2*i)  // If n*V it's odd, last one is only with GRF
            assembly << "ADD GRFA" << i << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
        for (i=0; i<peeling/2; i++) {
            assembly << "MOV EVEN_BANK[AddrFile] GRFA" << i << endl;
            assembly << "MOV ODD_BANK[AddrFile] GRFB" << i << endl;
        }
        if (peeling - 2*i)  // If n*V it's odd, last one is only with GRF
            assembly << "MOV EVEN_BANK[AddrFile] GRFA" << i << endl;
    }
    assembly << "EXIT" << endl << "EXEC" << endl;

    // Write address for loops and others
    addrFile << "# Row-wise mapping of element-wise addition, R-limited, V = " << V << ", n = " << n << endl;
    for (i=0; i<loops; i++) {
        for (j=0; j<GRF_ENTRIES; j++){
            // MOVs to GRFs
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
            if (++colIdx[0] == COLPERROW) {
                colIdx[0] = 0;
                rowIdx[0]++;
            }
        }
        for (j=0; j<GRF_ENTRIES; j++){
            // ALU ops
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
            if (++colIdx[1] == COLPERROW) {
                colIdx[1] = 0;
                rowIdx[1]++;
            }
        }
        for (j=0; j<GRF_ENTRIES; j++){
            // MOVs to BANKs
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[2],colIdx[2]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[2],colIdx[2]}) << endl;
            if (++colIdx[2] == COLPERROW) {
                colIdx[2] = 0;
                rowIdx[2]++;
            }
        }
    }

    if (peeling) {
        addrFile << "# Out of the main loop" << endl;
        for (i=0; i<peeling/2; i++) {
            // MOVs to GRFs
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
            if (++colIdx[0] == COLPERROW) {
                colIdx[0] = 0;
                rowIdx[0]++;
            }
        }
        if (peeling - 2*i)  // If n*V it's odd, last one is only with GRF
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
        for (i=0; i<peeling/2; i++) {
            // ALU ops
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
            if (++colIdx[1] == COLPERROW) {
                colIdx[1] = 0;
                rowIdx[1]++;
            }
        }
        if (peeling - 2*i)  // If n*V it's odd, last one is only with GRF
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;
        for (i=0; i<peeling/2; i++) {
            // MOVs to BANKs
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[2],colIdx[2]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[2],colIdx[2]}) << endl;
            if (++colIdx[2] == COLPERROW) {
                colIdx[2] = 0;
                rowIdx[2]++;
            }
        }
        if (peeling - 2*i)  // If n*V it's odd, last one is only with GRF
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[2],colIdx[2]}) << endl;
    }

    // Write data for loops and others
    dataFile << "# Row-wise mapping of element-wise addition, R-limited, V = " << V << ", n = " << n << endl;
    for (i=0; i<loops; i++) {
        for (j=0; j<GRF_ENTRIES; j++){
            // MOVs to GRFs
            for (k=0; k<2*SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (k == SIMD_WIDTH*CORES_PER_PCH)
                    dataFile << endl;
                if(WORD_BITS != 8)
                    dataFile << op1[VIdx[0]][nIdx[0]] << " ";
                else     
                    dataFile << int(op1[VIdx[0]][nIdx[0]]) << " "; //need to cast the results for int8 only
                if (++nIdx[0] == n) {
                    nIdx[0] = 0;
                    if (++VIdx[0] == V)
                        VIdx[0] = 0;
                }
            }
            dataFile << endl;
        }
        for (j=0; j<GRF_ENTRIES; j++){
            // ALU ops
            for (k=0; k<2*SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (k == SIMD_WIDTH*CORES_PER_PCH)
                    dataFile << endl;
                if(WORD_BITS != 8)
                    dataFile << op2[VIdx[1]][nIdx[1]] << " ";
                else
                    dataFile << int(op2[VIdx[1]][nIdx[1]]) << " "; //need to cast the results for int8 only
                if (++nIdx[1] == n) {
                    nIdx[1] = 0;
                    if (++VIdx[1] == V)
                        VIdx[1] = 0;
                }
            }
            dataFile << endl;
        }
    }

    if (peeling) {
        dataFile << "# Out of the main loop" << endl;
        for (i=0; i<peeling/2; i++) {
            // MOVs to GRFs
            for (k=0; k<2*SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (k == SIMD_WIDTH*CORES_PER_PCH)
                    dataFile << endl;
                if(WORD_BITS != 8)
                    dataFile << op1[VIdx[0]][nIdx[0]] << " ";
                else
                    dataFile << int(op1[VIdx[0]][nIdx[0]]) << " "; //need to cast the results for int8 only
                if (++nIdx[0] == n) {
                    nIdx[0] = 0;
                    if (++VIdx[0] == V)
                        VIdx[0] = 0;
                }
            }
            dataFile << endl;
        }
        if (peeling - 2*i) {    // If n*V it's odd, last one is only with GRF
            for (k=0; k<SIMD_WIDTH*CORES_PER_PCH; k++) {
                if(WORD_BITS != 8)
                    dataFile << op1[VIdx[0]][nIdx[0]] << " ";
                else 
                    dataFile << int(op1[VIdx[0]][nIdx[0]]) << " "; //need to cast the results for int8 only
                if (++nIdx[0] == n) {
                    nIdx[0] = 0;
                    if (++VIdx[0] == V)
                        VIdx[0] = 0;
                }
            }
            dataFile << endl;
        }
        for (i=0; i<peeling/2; i++) {
            // ALU ops
            for (k=0; k<2*SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (k == SIMD_WIDTH*CORES_PER_PCH)
                    dataFile << endl;
                if(WORD_BITS != 8)
                    dataFile << op2[VIdx[1]][nIdx[1]] << " ";
                else 
                    dataFile << int(op2[VIdx[1]][nIdx[1]]) << " "; //need to cast the results for int8 only
                if (++nIdx[1] == n) {
                    nIdx[1] = 0;
                    if (++VIdx[1] == V)
                        VIdx[1] = 0;
                }
            }
            dataFile << endl;
        }
        if (peeling - 2*i) {    // If n*V it's odd, last one is only with GRF
            for (k=0; k<SIMD_WIDTH*CORES_PER_PCH; k++) {
                if(WORD_BITS != 8)
                    dataFile << op2[VIdx[1]][nIdx[1]] << " ";
                else 
                    dataFile << int(op2[VIdx[1]][nIdx[1]]) << " "; //need to cast the results for int8 only
                if (++nIdx[1] == n) {
                    nIdx[1] = 0;
                    if (++VIdx[1] == V)
                        VIdx[1] = 0;
                }
            }
            dataFile << endl;
        }
    }

}

void mapEWAdditionRowWiseCLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t **op1, cnm_t **op2, int V, int n)
{
    int i,j,k;
    int crfSegment = floor((float(CRF_ENTRIES)-2.0)/3.0);
    crfSegment = min(crfSegment, 2*GRF_ENTRIES);           // If segment is larger than GRF capacity, limit it to the latter
    int loops = floor(ceil(float(n*V)/float(SIMD_WIDTH*CORES_PER_PCH))/crfSegment);
    int peeling = int(ceil(float(n*V)/float(SIMD_WIDTH*CORES_PER_PCH))) % crfSegment;
    int totalCol = ceil((float(n*V)/2.0)/float(SIMD_WIDTH));                    // Div 2 because distributed in 2 banks
    uint64_t op2Base = ceil(float(totalCol)/(float(COLPERROW*CORES_PER_PCH)+1.0));              // Op2 will start in this row (+ 1 in case crfSegment is odd)
    uint64_t resBase = op2Base + ceil(float(totalCol)/(float(COLPERROW*CORES_PER_PCH)+1.0));    // Result will start in this row
    uint64_t rowIdx[3] = {0,op2Base,resBase};
    uint64_t colIdx[3] = {0,0,0};
    int nIdx[2] = {0,0};
    int VIdx[2] = {0,0};

    // Write assembly instructions
    assembly << "; Row-wise mapping of element-wise addition, C-limited, V = " << V << ", n = " << n << endl;

    if (loops) {
        assembly << "WRF CRF0" << endl;
        for (i=0; i<crfSegment/2; i++) {
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "MOV GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (crfSegment % 2) {
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
        }
        for (i=0; i<crfSegment/2; i++) {
            assembly << "ADD GRFA" << i << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "ADD GRFB" << i << " GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (crfSegment % 2) {
            assembly << "ADD GRFA" << i << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
        }
        for (i=0; i<crfSegment/2; i++) {
            assembly << "MOV EVEN_BANK[AddrFile] GRFA" << i << endl;
            assembly << "MOV ODD_BANK[AddrFile] GRFB" << i << endl;
        }
        if (crfSegment % 2) {
            assembly << "MOV EVEN_BANK[AddrFile] GRFA" << i << endl;
        }
        if (loops-1)
            assembly << "JUMP " << 3*crfSegment << " " << loops-1 << endl;
        assembly << "EXIT" << endl << "EXEC" << endl;
    }

    if (peeling) {
        assembly << "; Out of the main loop" << endl;
        assembly << "WRF CRF0" << endl;
        for (i=0; i<peeling/2; i++) {
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "MOV GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (peeling - 2*i)  // If n*V it's odd, last one is only with GRF
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
        for (i=0; i<peeling/2; i++) {
            assembly << "ADD GRFA" << i << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "ADD GRFB" << i << " GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (peeling - 2*i)  // If n*V it's odd, last one is only with GRF
            assembly << "ADD GRFA" << i << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
        for (i=0; i<peeling/2; i++) {
            assembly << "MOV EVEN_BANK[AddrFile] GRFA" << i << endl;
            assembly << "MOV ODD_BANK[AddrFile] GRFB" << i << endl;
        }
        if (peeling - 2*i)  // If n*V it's odd, last one is only with GRF
            assembly << "MOV EVEN_BANK[AddrFile] GRFA" << i << endl;
        assembly << "EXIT" << endl << "EXEC" << endl;
    }

    // Write address for loops and others
    addrFile << "# Row-wise mapping of element-wise addition, C-limited, V = " << V << ", n = " << n << endl;
    for (i=0; i<loops; i++) {
        for (j=0; j<crfSegment/2; j++){
            // MOVs to GRFs
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
        for (j=0; j<crfSegment/2; j++){
            // ALU ops
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
            if (++colIdx[1] == COLPERROW) {
                colIdx[1] = 0;
                rowIdx[1]++;
            }
        }
        if (crfSegment % 2) {
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;
            if (++colIdx[1] == COLPERROW) {
                colIdx[1] = 0;
                rowIdx[1]++;
            }
        }
        for (j=0; j<crfSegment/2; j++){
            // MOVs to BANKs
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[2],colIdx[2]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[2],colIdx[2]}) << endl;
            if (++colIdx[2] == COLPERROW) {
                colIdx[2] = 0;
                rowIdx[2]++;
            }
        }
        if (crfSegment % 2) {
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[2],colIdx[2]}) << endl;
            if (++colIdx[2] == COLPERROW) {
                colIdx[2] = 0;
                rowIdx[2]++;
            }
        }
    }

    if (peeling) {
        addrFile << "# Out of the main loop" << endl;
        for (i=0; i<peeling/2; i++) {
            // MOVs to GRFs
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
            if (++colIdx[0] == COLPERROW) {
                colIdx[0] = 0;
                rowIdx[0]++;
            }
        }
        if (peeling - 2*i)  // If n*V it's odd, last one is only with GRF
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
        for (i=0; i<peeling/2; i++) {
            // ALU ops
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
            if (++colIdx[1] == COLPERROW) {
                colIdx[1] = 0;
                rowIdx[1]++;
            }
        }
        if (peeling - 2*i)  // If n*V it's odd, last one is only with GRF
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;
        for (i=0; i<peeling/2; i++) {
            // MOVs to BANKs
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[2],colIdx[2]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[2],colIdx[2]}) << endl;
            if (++colIdx[2] == COLPERROW) {
                colIdx[2] = 0;
                rowIdx[2]++;
            }
        }
        if (peeling - 2*i)  // If n*V it's odd, last one is only with GRF
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[2],colIdx[2]}) << endl;
    }

    // Write data for loops and others
    dataFile << "# Row-wise mapping of element-wise addition, C-limited, V = " << V << ", n = " << n << endl;
    for (i=0; i<loops; i++) {
        for (j=0; j<crfSegment/2; j++){
            // MOVs to GRFs
            for (k=0; k<2*SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (k == SIMD_WIDTH*CORES_PER_PCH)
                    dataFile << endl;
                if (WORD_BITS != 8)
                    dataFile << op1[VIdx[0]][nIdx[0]] << " ";
                else 
                    dataFile << int(op1[VIdx[0]][nIdx[0]]) << " ";
                if (++nIdx[0] == n) {
                    nIdx[0] = 0;
                    if (++VIdx[0] == V)
                        VIdx[0] = 0;
                }
            }
            dataFile << endl;
        }
        if (crfSegment%2) {
            for (k=0; k<SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (WORD_BITS != 8)
                    dataFile << op1[VIdx[0]][nIdx[0]] << " ";
                else 
                    dataFile << int(op1[VIdx[0]][nIdx[0]]) << " ";
                if (++nIdx[0] == n) {
                    nIdx[0] = 0;
                    if (++VIdx[0] == V)
                        VIdx[0] = 0;
                }
            }
            dataFile << endl;
        }
        for (j=0; j<crfSegment/2; j++){
            // ALU ops
            for (k=0; k<2*SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (k == SIMD_WIDTH*CORES_PER_PCH)
                    dataFile << endl;
                if (WORD_BITS != 8)
                    dataFile << op2[VIdx[1]][nIdx[1]] << " ";
                else 
                    dataFile << int(op2[VIdx[1]][nIdx[1]]) << " ";
                if (++nIdx[1] == n) {
                    nIdx[1] = 0;
                    if (++VIdx[1] == V)
                        VIdx[1] = 0;
                }
            }
            dataFile << endl;
        }
        if (crfSegment%2) {
            for (k=0; k<SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (WORD_BITS != 8)
                    dataFile << op2[VIdx[1]][nIdx[1]] << " ";
                else 
                    dataFile << int(op2[VIdx[1]][nIdx[1]]) << " ";
                if (++nIdx[1] == n) {
                    nIdx[1] = 0;
                    if (++VIdx[1] == V)
                        VIdx[1] = 0;
                }
            }
            dataFile << endl;
        }
    }

    if (peeling) {
        dataFile << "# Out of the main loop" << endl;
        for (i=0; i<peeling/2; i++) {
            // MOVs to GRFs
            for (k=0; k<2*SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (k == SIMD_WIDTH*CORES_PER_PCH)
                    dataFile << endl;
                if (WORD_BITS != 8)
                    dataFile << op1[VIdx[0]][nIdx[0]] << " ";
                else 
                    dataFile << int(op1[VIdx[0]][nIdx[0]]) << " ";
                if (++nIdx[0] == n) {
                    nIdx[0] = 0;
                    if (++VIdx[0] == V)
                        VIdx[0] = 0;
                }
            }
            dataFile << endl;
        }
        if (peeling - 2*i) {    // If n*V it's odd, last one is only with GRF
            for (k=0; k<SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (WORD_BITS != 8)
                    dataFile << op1[VIdx[0]][nIdx[0]] << " ";
                else 
                    dataFile << int(op1[VIdx[0]][nIdx[0]]) << " ";
                if (++nIdx[0] == n) {
                    nIdx[0] = 0;
                    if (++VIdx[0] == V)
                        VIdx[0] = 0;
                }
            }
            dataFile << endl;
        }
        for (i=0; i<peeling/2; i++) {
            // ALU ops
            for (k=0; k<2*SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (k == SIMD_WIDTH*CORES_PER_PCH)
                    dataFile << endl;
                if (WORD_BITS != 8)
                    dataFile << op2[VIdx[1]][nIdx[1]] << " ";
                else
                    dataFile << int(op2[VIdx[1]][nIdx[1]]) << " ";
                if (++nIdx[1] == n) {
                    nIdx[1] = 0;
                    if (++VIdx[1] == V)
                        VIdx[1] = 0;
                }
            }
            dataFile << endl;
        }
        if (peeling - 2*i) {    // If n*V it's odd, last one is only with GRF
            for (k=0; k<SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (WORD_BITS != 8)
                    dataFile << op2[VIdx[1]][nIdx[1]] << " ";
                else 
                    dataFile << int(op2[VIdx[1]][nIdx[1]]) << " ";
                if (++nIdx[1] == n) {
                    nIdx[1] = 0;
                    if (++VIdx[1] == V)
                        VIdx[1] = 0;
                }
            }
            dataFile << endl;
        }
    }

}

void mapEWAdditionColWiseRLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t **op1, cnm_t **op2, int V, int n)
{
    int i,j,k;
    int loops = floor(float(n)*ceil(float(V)/float(SIMD_WIDTH*CORES_PER_PCH))/(2.0*GRF_ENTRIES));
    int peeling = int(float(n)*ceil(float(V)/float(SIMD_WIDTH*CORES_PER_PCH))) % (2*GRF_ENTRIES);
    int totalCol = n * ceil(float(V)/float(SIMD_WIDTH)) / 2;                // Div 2 because distributed in 2 banks
    uint64_t op2Base = ceil(float(totalCol)/float(COLPERROW*CORES_PER_PCH));              // Op2 will start in this row
    uint64_t resBase = op2Base + ceil(float(totalCol)/float(COLPERROW*CORES_PER_PCH));    // Result will start in this row
    uint64_t rowIdx[3] = {0,op2Base,resBase};
    uint64_t colIdx[3] = {0,0,0};
    int nIdx[2] = {0,0};
    int VIdx[2] = {0,0};

    // Write assembly instructions
    assembly << "; Column-wise mapping of element-wise addition, R-limited, V = " << V << ", n = " << n << endl;
    assembly << "WRF CRF0" << endl;

    if (loops) {
        for (i=0; i<GRF_ENTRIES; i++) {
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "MOV GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        for (i=0; i<GRF_ENTRIES; i++) {
            assembly << "ADD GRFA" << i << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "ADD GRFB" << i << " GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        for (i=0; i<GRF_ENTRIES; i++) {
            assembly << "MOV EVEN_BANK[AddrFile] GRFA" << i << endl;
            assembly << "MOV ODD_BANK[AddrFile] GRFB" << i << endl;
        }
        if (loops-1)
            assembly << "JUMP " << 6*GRF_ENTRIES << " " << loops-1 << endl;
    }

    if (peeling) {
        assembly << "; Out of the main loop" << endl;
        for (i=0; i<peeling/2; i++) {
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "MOV GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (peeling - 2*i)  // If n it's odd, last one is only with GRF
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
        for (i=0; i<peeling/2; i++) {
            assembly << "ADD GRFA" << i << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "ADD GRFB" << i << " GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (peeling - 2*i)  // If n it's odd, last one is only with GRF
            assembly << "ADD GRFA" << i << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
        for (i=0; i<peeling/2; i++) {
            assembly << "MOV EVEN_BANK[AddrFile] GRFA" << i << endl;
            assembly << "MOV ODD_BANK[AddrFile] GRFB" << i << endl;
        }
        if (peeling - 2*i)  // If n it's odd, last one is only with GRF
            assembly << "MOV EVEN_BANK[AddrFile] GRFA" << i << endl;
    }
    assembly << "EXIT" << endl << "EXEC" << endl;

    // Write address for loops and others
    addrFile << "# Column-wise mapping of element-wise addition, R-limited, V = " << V << ", n = " << n << endl;
    for (i=0; i<loops; i++) {
        for (j=0; j<GRF_ENTRIES; j++){
            // MOVs to GRFs
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
            if (++colIdx[0] == COLPERROW) {
                colIdx[0] = 0;
                rowIdx[0]++;
            }
        }
        for (j=0; j<GRF_ENTRIES; j++){
            // ALU ops
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
            if (++colIdx[1] == COLPERROW) {
                colIdx[1] = 0;
                rowIdx[1]++;
            }
        }
        for (j=0; j<GRF_ENTRIES; j++){
            // MOVs to BANKs
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[2],colIdx[2]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[2],colIdx[2]}) << endl;
            if (++colIdx[2] == COLPERROW) {
                colIdx[2] = 0;
                rowIdx[2]++;
            }
        }
    }

    if (peeling) {
        addrFile << "# Out of the main loop" << endl;
        for (i=0; i<peeling/2; i++) {
            // MOVs to GRFs
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
            if (++colIdx[0] == COLPERROW) {
                colIdx[0] = 0;
                rowIdx[0]++;
            }
        }
        if (peeling - 2*i)  // If n it's odd, last one is only with GRF
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
        for (i=0; i<peeling/2; i++) {
            // ALU ops
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
            if (++colIdx[1] == COLPERROW) {
                colIdx[1] = 0;
                rowIdx[1]++;
            }
        }
        if (peeling - 2*i)  // If n it's odd, last one is only with GRF
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;
        for (i=0; i<peeling/2; i++) {
            // MOVs to BANKs
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[2],colIdx[2]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[2],colIdx[2]}) << endl;
            if (++colIdx[2] == COLPERROW) {
                colIdx[2] = 0;
                rowIdx[2]++;
            }
        }
        if (peeling - 2*i)  // If n it's odd, last one is only with GRF
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[2],colIdx[2]}) << endl;
    }

    // Write data for loops and others
    dataFile << "# Column-wise mapping of element-wise addition, R-limited, V = " << V << ", n = " << n << endl;
    for (i=0; i<loops; i++) {
        for (j=0; j<GRF_ENTRIES; j++){
            // MOVs to GRFs
            for (k=0; k<2*SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (k == SIMD_WIDTH*CORES_PER_PCH)
                    dataFile << endl;
                if(WORD_BITS !=8)
                    dataFile << (VIdx[0] < V ? op1[VIdx[0]][nIdx[0]] : 0) << " ";
                else 
                    dataFile << int((VIdx[0] < V ? op1[VIdx[0]][nIdx[0]] : 0)) << " ";
                if (!(++VIdx[0] % (SIMD_WIDTH*CORES_PER_PCH))){ // Ended dimension of S vectors
                    if (++nIdx[0] == n) {       // Finished all dimensions of S vectors
                        nIdx[0] = 0;
                    } else {
                        VIdx[0] -= SIMD_WIDTH*CORES_PER_PCH;
                    }
                }
            }
            dataFile << endl;
        }
        for (j=0; j<GRF_ENTRIES; j++){
            // ALU ops
            for (k=0; k<2*SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (k == SIMD_WIDTH*CORES_PER_PCH)
                    dataFile << endl;
                if(WORD_BITS !=8)
                    dataFile << (VIdx[1] < V ? op2[VIdx[1]][nIdx[1]] : 0) << " ";
                else
                    dataFile << int(VIdx[1] < V ? op2[VIdx[1]][nIdx[1]] : 0) << " ";
                if (!(++VIdx[1] % (SIMD_WIDTH*CORES_PER_PCH))){ // Ended dimension of S vectors
                    if (++nIdx[1] == n) {       // Finished all dimensions of S vectors
                        nIdx[1] = 0;
                    } else {
                        VIdx[1] -= SIMD_WIDTH*CORES_PER_PCH;
                    }
                }
            }
            dataFile << endl;
        }
    }

    if (peeling) {
        dataFile << "# Out of the main loop" << endl;
        for (i=0; i<peeling/2; i++) {
            // MOVs to GRFs
            for (k=0; k<2*SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (k == SIMD_WIDTH*CORES_PER_PCH)
                    dataFile << endl;
                if(WORD_BITS !=8)
                    dataFile << (VIdx[0] < V ? op1[VIdx[0]][nIdx[0]] : 0) << " ";
                else 
                    dataFile << int(VIdx[0] < V ? op1[VIdx[0]][nIdx[0]] : 0) << " ";
                if (!(++VIdx[0] % (SIMD_WIDTH*CORES_PER_PCH))){ // Ended dimension of S vectors
                    if (++nIdx[0] == n) {       // Finished all dimensions of S vectors
                        nIdx[0] = 0;
                    } else {
                        VIdx[0] -= SIMD_WIDTH*CORES_PER_PCH;
                    }
                }
            }
            dataFile << endl;
        }
        if (peeling - 2*i) {    // If n it's odd, last one is only with GRF
            for (k=0; k<SIMD_WIDTH*CORES_PER_PCH; k++) {
                if(WORD_BITS !=8)
                    dataFile << (VIdx[0] < V ? op1[VIdx[0]][nIdx[0]] : 0) << " ";
                else 
                    dataFile << int(VIdx[0] < V ? op1[VIdx[0]][nIdx[0]] : 0) << " ";
                if (!(++VIdx[0] % (SIMD_WIDTH*CORES_PER_PCH))){ // Ended dimension of S vectors
                    if (++nIdx[0] == n) {       // Finished all dimensions of S vectors
                        nIdx[0] = 0;
                    } else {
                        VIdx[0] -= SIMD_WIDTH*CORES_PER_PCH;
                    }
                }
            }
            dataFile << endl;
        }
        for (i=0; i<peeling/2; i++) {
            // ALU ops
            for (k=0; k<2*SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (k == SIMD_WIDTH*CORES_PER_PCH)
                    dataFile << endl;
                if(WORD_BITS !=8)
                    dataFile << (VIdx[1] < V ? op2[VIdx[1]][nIdx[1]] : 0) << " ";
                else 
                    dataFile << int(VIdx[1] < V ? op2[VIdx[1]][nIdx[1]] : 0) << " ";
                if (!(++VIdx[1] % (SIMD_WIDTH*CORES_PER_PCH))){ // Ended dimension of S vectors
                    if (++nIdx[1] == n) {       // Finished all dimensions of S vectors
                        nIdx[1] = 0;
                    } else {
                        VIdx[1] -= SIMD_WIDTH*CORES_PER_PCH;
                    }
                }
            }
            dataFile << endl;
        }
        if (peeling - 2*i) {    // If n it's odd, last one is only with GRF
            for (k=0; k<SIMD_WIDTH*CORES_PER_PCH; k++) {
                if(WORD_BITS !=8)
                    dataFile << (VIdx[1] < V ? op2[VIdx[1]][nIdx[1]] : 0) << " ";
                else 
                    dataFile << int(VIdx[1] < V ? op2[VIdx[1]][nIdx[1]] : 0) << " ";
                if (!(++VIdx[1] % (SIMD_WIDTH*CORES_PER_PCH))){ // Ended dimension of S vectors
                    if (++nIdx[1] == n) {       // Finished all dimensions of S vectors
                        nIdx[1] = 0;
                    } else {
                        VIdx[1] -= SIMD_WIDTH*CORES_PER_PCH;
                    }
                }
            }
            dataFile << endl;
        }
    }

}

void mapEWAdditionColWiseCLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t **op1, cnm_t **op2, int V, int n)
{
    int i,j,k;
    int crfSegment = floor((float(CRF_ENTRIES)-2.0)/3.0);
    crfSegment = min(crfSegment, 2*GRF_ENTRIES);           // If segment is larger than GRF capacity, limit it to the latter
    int loops = floor(float(n)*ceil(float(V)/float(SIMD_WIDTH*CORES_PER_PCH))/crfSegment);
    int peeling = int(float(n)*ceil(float(V)/float(SIMD_WIDTH*CORES_PER_PCH))) % crfSegment;
    int totalCol = n * ceil(float(V)/float(SIMD_WIDTH)) / 2;                    // Div 2 because distributed in 2 banks
    uint64_t op2Base = ceil(float(totalCol)/(float(COLPERROW*CORES_PER_PCH)+1.0));            // Op2 will start in this row (+ 1 in case crfSegment is odd)
    uint64_t resBase = op2Base + ceil(float(totalCol)/(float(COLPERROW*CORES_PER_PCH)+1.0));  // Result will start in this row
    uint64_t rowIdx[3] = {0,op2Base,resBase};
    uint64_t colIdx[3] = {0,0,0};
    int nIdx[2] = {0,0};
    int VIdx[2] = {0,0};

    // Write assembly instructions
    assembly << "; Column-wise mapping of element-wise addition, C-limited, V = " << V << ", n = " << n << endl;

    if (loops) {
        assembly << "WRF CRF0" << endl;
        for (i=0; i<crfSegment/2; i++) {
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "MOV GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (crfSegment % 2) {
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
        }
        for (i=0; i<crfSegment/2; i++) {
            assembly << "ADD GRFA" << i << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "ADD GRFB" << i << " GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (crfSegment % 2) {
            assembly << "ADD GRFA" << i << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
        }
        for (i=0; i<crfSegment/2; i++) {
            assembly << "MOV EVEN_BANK[AddrFile] GRFA" << i << endl;
            assembly << "MOV ODD_BANK[AddrFile] GRFB" << i << endl;
        }
        if (crfSegment % 2) {
            assembly << "MOV EVEN_BANK[AddrFile] GRFA" << i << endl;
        }
        if (loops-1)
            assembly << "JUMP " << 3*crfSegment << " " << loops-1 << endl;
        assembly << "EXIT" << endl << "EXEC" << endl;
    }

    if (peeling) {
        assembly << "; Out of the main loop" << endl;
        assembly << "WRF CRF0" << endl;
        for (i=0; i<peeling/2; i++) {
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "MOV GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (peeling - 2*i)  // If n it's odd, last one is only with GRF
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
        for (i=0; i<peeling/2; i++) {
            assembly << "ADD GRFA" << i << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "ADD GRFB" << i << " GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (peeling - 2*i)  // If n it's odd, last one is only with GRF
            assembly << "ADD GRFA" << i << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
        for (i=0; i<peeling/2; i++) {
            assembly << "MOV EVEN_BANK[AddrFile] GRFA" << i << endl;
            assembly << "MOV ODD_BANK[AddrFile] GRFB" << i << endl;
        }
        if (peeling - 2*i)  // If n it's odd, last one is only with GRF
            assembly << "MOV EVEN_BANK[AddrFile] GRFA" << i << endl;
        assembly << "EXIT" << endl << "EXEC" << endl;
    }

    // Write address for loops and others
    addrFile << "# Column-wise mapping of element-wise addition, C-limited, V = " << V << ", n = " << n << endl;
    for (i=0; i<loops; i++) {
        for (j=0; j<crfSegment/2; j++){
            // MOVs to GRFs
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
        for (j=0; j<crfSegment/2; j++){
            // ALU ops
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
            if (++colIdx[1] == COLPERROW) {
                colIdx[1] = 0;
                rowIdx[1]++;
            }
        }
        if (crfSegment % 2) {
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;
            if (++colIdx[1] == COLPERROW) {
                colIdx[1] = 0;
                rowIdx[1]++;
            }
        }
        for (j=0; j<crfSegment/2; j++){
            // MOVs to BANKs
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[2],colIdx[2]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[2],colIdx[2]}) << endl;
            if (++colIdx[2] == COLPERROW) {
                colIdx[2] = 0;
                rowIdx[2]++;
            }
        }
        if (crfSegment % 2) {
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[2],colIdx[2]}) << endl;
            if (++colIdx[2] == COLPERROW) {
                colIdx[2] = 0;
                rowIdx[2]++;
            }
        }
    }

    if (peeling) {
        addrFile << "# Out of the main loop" << endl;
        for (i=0; i<peeling/2; i++) {
            // MOVs to GRFs
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
            if (++colIdx[0] == COLPERROW) {
                colIdx[0] = 0;
                rowIdx[0]++;
            }
        }
        if (peeling - 2*i)  // If n it's odd, last one is only with GRF
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
        for (i=0; i<peeling/2; i++) {
            // ALU ops
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
            if (++colIdx[1] == COLPERROW) {
                colIdx[1] = 0;
                rowIdx[1]++;
            }
        }
        if (peeling - 2*i)  // If n it's odd, last one is only with GRF
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;
        for (i=0; i<peeling/2; i++) {
            // MOVs to BANKs
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[2],colIdx[2]}) << endl;
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[2],colIdx[2]}) << endl;
            if (++colIdx[2] == COLPERROW) {
                colIdx[2] = 0;
                rowIdx[2]++;
            }
        }
        if (peeling - 2*i)  // If n it's odd, last one is only with GRF
            addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[2],colIdx[2]}) << endl;
    }

    // Write data for loops and others
    dataFile << "# Column-wise mapping of element-wise addition, C-limited, V = " << V << ", n = " << n << endl;
    for (i=0; i<loops; i++) {
        for (j=0; j<crfSegment/2; j++){
            // MOVs to GRFs
            for (k=0; k<2*SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (k == SIMD_WIDTH*CORES_PER_PCH)
                    dataFile << endl;
                if(WORD_BITS != 8)
                    dataFile << (VIdx[0] < V ? op1[VIdx[0]][nIdx[0]] : 0) << " "; 
                else
                    dataFile << int(VIdx[0] < V ? op1[VIdx[0]][nIdx[0]] : 0) << " "; //need to cast the results for int8 only
                if (!(++VIdx[0] % (SIMD_WIDTH*CORES_PER_PCH))){ // Ended dimension of S vectors
                    if (++nIdx[0] == n) {       // Finished all dimensions of S vectors
                        nIdx[0] = 0;
                    } else {
                        VIdx[0] -= SIMD_WIDTH*CORES_PER_PCH;
                    }
                }
            }
            dataFile << endl;
        }
        if (crfSegment%2) {
            for (k=0; k<SIMD_WIDTH*CORES_PER_PCH; k++) {
                if(WORD_BITS != 8)
                    dataFile << (VIdx[0] < V ? op1[VIdx[0]][nIdx[0]] : 0) << " ";
                else 
                    dataFile << int(VIdx[0] < V ? op1[VIdx[0]][nIdx[0]] : 0) << " "; //need to cast the results for int8 only
                if (!(++VIdx[0] % (SIMD_WIDTH*CORES_PER_PCH))){ // Ended dimension of S vectors
                    if (++nIdx[0] == n) {       // Finished all dimensions of S vectors
                        nIdx[0] = 0;
                    } else {
                        VIdx[0] -= SIMD_WIDTH*CORES_PER_PCH;
                    }
                }
            }
            dataFile << endl;
        }
        for (j=0; j<crfSegment/2; j++){
            // ALU ops
            for (k=0; k<2*SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (k == SIMD_WIDTH*CORES_PER_PCH)
                    dataFile << endl;
                if(WORD_BITS != 8)
                    dataFile << (VIdx[1] < V ? op2[VIdx[1]][nIdx[1]] : 0) << " ";
                else
                    dataFile << int(VIdx[1] < V ? op2[VIdx[1]][nIdx[1]] : 0) << " "; //need to cast the results for int8 only
                if (!(++VIdx[1] % (SIMD_WIDTH*CORES_PER_PCH))){ // Ended dimension of S vectors
                    if (++nIdx[1] == n) {       // Finished all dimensions of S vectors
                        nIdx[1] = 0;
                    } else {
                        VIdx[1] -= SIMD_WIDTH*CORES_PER_PCH;
                    }
                }
            }
            dataFile << endl;
        }
        if (crfSegment%2) {
            for (k=0; k<SIMD_WIDTH*CORES_PER_PCH; k++) {
                if(WORD_BITS != 8){
                    dataFile << op2[VIdx[1]][nIdx[1]] << " ";
                    dataFile << (VIdx[1] < V ? op2[VIdx[1]][nIdx[1]] : 0) << " ";}
                else{
                    dataFile << int(op2[VIdx[1]][nIdx[1]]) << " ";
                    dataFile << int(VIdx[1] < V ? op2[VIdx[1]][nIdx[1]] : 0) << " ";} //need to cast the results for int8 only
                if (!(++VIdx[1] % (SIMD_WIDTH*CORES_PER_PCH))){ // Ended dimension of S vectors
                    if (++nIdx[1] == n) {       // Finished all dimensions of S vectors
                        nIdx[1] = 0;
                    } else {
                        VIdx[1] -= SIMD_WIDTH*CORES_PER_PCH;
                    }
                }
            }
            dataFile << endl;
        }
    }

    if (peeling) {
        dataFile << "# Out of the main loop" << endl;
        for (i=0; i<peeling/2; i++) {
            // MOVs to GRFs
            for (k=0; k<2*SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (k == SIMD_WIDTH*CORES_PER_PCH)
                    dataFile << endl;
                if(WORD_BITS != 8)
                    dataFile << (VIdx[0] < V ? op1[VIdx[0]][nIdx[0]] : 0) << " ";
                else
                    dataFile << int(VIdx[0] < V ? op1[VIdx[0]][nIdx[0]] : 0) << " "; //need to cast the results for int8 only
                if (!(++VIdx[0] % (SIMD_WIDTH*CORES_PER_PCH))){ // Ended dimension of S vectors
                    if (++nIdx[0] == n) {       // Finished all dimensions of S vectors
                        nIdx[0] = 0;
                    } else {
                        VIdx[0] -= SIMD_WIDTH*CORES_PER_PCH;
                    }
                }
            }
            dataFile << endl;
        }
        if (peeling - 2*i) {    // If n it's odd, last one is only with GRF
            for (k=0; k<SIMD_WIDTH*CORES_PER_PCH; k++) {
                if(WORD_BITS != 8)
                    dataFile << (VIdx[0] < V ? op1[VIdx[0]][nIdx[0]] : 0) << " ";
                else 
                    dataFile << int(VIdx[0] < V ? op1[VIdx[0]][nIdx[0]] : 0) << " "; //need to cast the results for int8 only
                if (!(++VIdx[0] % (SIMD_WIDTH*CORES_PER_PCH))){ // Ended dimension of S vectors
                    if (++nIdx[0] == n) {       // Finished all dimensions of S vectors
                        nIdx[0] = 0;
                    } else {
                        VIdx[0] -= SIMD_WIDTH*CORES_PER_PCH;
                    }
                }
            }
            dataFile << endl;
        }
        for (i=0; i<peeling/2; i++) {
            // ALU ops
            for (k=0; k<2*SIMD_WIDTH*CORES_PER_PCH; k++) {
                if (k == SIMD_WIDTH*CORES_PER_PCH)
                    dataFile << endl;
                if(WORD_BITS != 8)
                    dataFile << (VIdx[1] < V ? op2[VIdx[1]][nIdx[1]] : 0) << " ";
                else
                    dataFile << int(VIdx[1] < V ? op2[VIdx[1]][nIdx[1]] : 0) << " "; //need to cast the results for int8 only
                if (!(++VIdx[1] % (SIMD_WIDTH*CORES_PER_PCH))){ // Ended dimension of S vectors
                    if (++nIdx[1] == n) {       // Finished all dimensions of S vectors
                        nIdx[1] = 0;
                    } else {
                        VIdx[1] -= SIMD_WIDTH*CORES_PER_PCH;
                    }
                }
            }
            dataFile << endl;
        }
        if (peeling - 2*i) {    // If n it's odd, last one is only with GRF
            for (k=0; k<SIMD_WIDTH*CORES_PER_PCH; k++) {
                if(WORD_BITS != 8)
                    dataFile << (VIdx[1] < V ? op2[VIdx[1]][nIdx[1]] : 0) << " ";
                else
                    dataFile << int(VIdx[1] < V ? op2[VIdx[1]][nIdx[1]] : 0) << " "; //need to cast the results for int8 only
                if (!(++VIdx[1] % (SIMD_WIDTH*CORES_PER_PCH))){ // Ended dimension of S vectors
                    if (++nIdx[1] == n) {       // Finished all dimensions of S vectors
                        nIdx[1] = 0;
                    } else {
                        VIdx[1] -= SIMD_WIDTH*CORES_PER_PCH;
                    }
                }
            }
            dataFile << endl;
        }
    }

}
