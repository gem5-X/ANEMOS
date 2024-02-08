#include "map_dp.h"

void mapDotProductRLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t **op1, cnm_t **op2, int V, int n)
{
    int i,j,k,l;
    int ext_loops = ceil(floor(V)/floor(SIMD_WIDTH*CORES_PER_PCH));
    int loops = floor(float(n)/(2.0*GRF_ENTRIES-1.0)) ;
    int peeling = n % (2*GRF_ENTRIES-1);
    int totalCol = n * ceil(float(V)/float(SIMD_WIDTH)) / 2;                // Div 2 because distributed in 2 banks
    uint64_t op2Base = ceil(float(totalCol)/float(COLPERROW*CORES_PER_PCH));              // Op2 will start in this row
    uint64_t resBase = op2Base + ceil(float(totalCol)/float(COLPERROW*CORES_PER_PCH));    // Result will start in this row
    uint64_t zeroBase = resBase + (ceil(float(V)/float(SIMD_WIDTH*CORES_PER_PCH)/2));     // Column fill with zeros, after the result
    uint64_t rowIdx[3] = {0,op2Base,resBase};
    uint64_t colIdx[3] = {0,0,0};
    int nIdx[2] = {0,0};
    int VIdx[2] = {0,0};

    // Write assembly instructions
    assembly << "; Mapping of dot product, R-limited, V = " << V << ", n = " << n << endl;
    assembly << "WRF CRF0" << endl;
    assembly << "MOV GRFB" << GRF_ENTRIES-1 << " ODD_BANK[AddrFile] DataFile" << endl;

    if (loops) {
        for (i=0; i<GRF_ENTRIES; i++) {
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            if (i != GRF_ENTRIES-1)
                assembly << "MOV GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        for (i=0; i<GRF_ENTRIES; i++) {
            assembly << "MAC GRFB" << GRF_ENTRIES-1 << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            if (i != GRF_ENTRIES-1)
                assembly << "MAC GRFB" << GRF_ENTRIES-1 << " GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (loops-1)
            assembly << "JUMP " << 4*GRF_ENTRIES-2 << " " << loops-1 << endl;
    }

    if (peeling) {
        assembly << "; Out of the main loop" << endl;
        for (i=0; i<peeling/2; i++) { 
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "MOV GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (peeling - 2*i)  // If n it's odd, last one is only with GRFA
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
        for (i=0; i<peeling/2; i++) {
            assembly << "MAC GRFB" << GRF_ENTRIES-1 << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "MAC GRFB" << GRF_ENTRIES-1 << " GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (peeling - 2*i)  // If n it's odd, last one is only with GRFA
            assembly << "MAC GRFB" << GRF_ENTRIES-1 << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
    }

    assembly << "MOV ODD_BANK[AddrFile] GRFB" << GRF_ENTRIES-1 << endl;
    assembly << "EXIT" << endl;
    for (i=0; i<ext_loops; i++) { assembly << "EXEC" << endl; }

    // Write address for loops and others
    addrFile << "# Mapping of dot product, R-limited, V = " << V << ", n = " << n << endl;
    for (i=0; i<ext_loops; i++) {
        addrFile << showbase << hex << build_addr({0,0,0,1,zeroBase,0}) << endl;    // Initialize with zeros
        for (j=0; j<loops; j++) {
            for (k=0; k<GRF_ENTRIES; k++){
                // MOVs to GRFs
                addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
                if (k != GRF_ENTRIES-1)
                    addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
                if (++colIdx[0] == COLPERROW) {
                    colIdx[0] = 0;
                    rowIdx[0]++;
                }
            }
            for (k=0; k<GRF_ENTRIES; k++){
                // ALU ops
                addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;
                if (k != GRF_ENTRIES-1)
                    addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
                if (++colIdx[1] == COLPERROW) {
                    colIdx[1] = 0;
                    rowIdx[1]++;
                }
            }
        }

        if (peeling) {
            addrFile << "# Out of the main loop" << endl;
            for (j=0; j<peeling/2; j++) {
                // MOVs to GRFs
                addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
                addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
                if (++colIdx[0] == COLPERROW) {
                    colIdx[0] = 0;
                    rowIdx[0]++;
                }
            }
            if (peeling - 2*j)  // If n it's odd, last one is only with GRFA
                addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
            for (j=0; j<peeling/2; j++) {
                // ALU ops
                addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;
                addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
                if (++colIdx[1] == COLPERROW) {
                    colIdx[1] = 0;
                    rowIdx[1]++;
                }
            }
            if (peeling - 2*j)  // If n it's odd, last one is only with GRFA
                addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;
        }
        // MOV result to BANK
        addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[2],colIdx[2]}) << endl;
        if (++colIdx[2] == COLPERROW) {
            colIdx[2] = 0;
            rowIdx[2]++;
        }
    }

    // Write data for loops and others
    dataFile << "# Mapping of dot product, R-limited, V = " << V << ", n = " << n << endl;
    for (i=0; i<ext_loops ; i++) {
        for (l=0; l<SIMD_WIDTH*CORES_PER_PCH; l++) {  // Initialize to zero
            dataFile << 0 << " "; 
        }
        dataFile << endl;
        for (j=0; j<loops; j++) {
            for (k=0; k<2*GRF_ENTRIES-1; k++){
                // MOVs to GRFs
                for (l=0; l<SIMD_WIDTH*CORES_PER_PCH; l++) {
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
            for (k=0; k<2*GRF_ENTRIES-1; k++){
                // ALU ops
                for (l=0; l<SIMD_WIDTH*CORES_PER_PCH; l++) {
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

        if (peeling) {
            dataFile << "# Out of the main loop" << endl;
            for (j=0; j<peeling; j++) {
                // MOVs to GRFs
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
            for (j=0; j<peeling; j++) {
                // ALU ops
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
}

void mapDotProductCLim (ofstream &assembly, ofstream &dataFile, ofstream &addrFile,
                                cnm_t **op1, cnm_t **op2, int V, int n)
{
    int i,j,k,l;
    int crfSegment = floor((float(CRF_ENTRIES)-4.0)/2.0);
    crfSegment = min(crfSegment, 2*GRF_ENTRIES);           // If segment is larger than GRF capacity, limit it to the latter
    int ext_loops = ceil(float(V)/float(SIMD_WIDTH*CORES_PER_PCH));
    int loops = floor(float(n)/crfSegment) ;
    int peeling = n % crfSegment;
    int totalCol = n * ceil(float(V)/float(SIMD_WIDTH)) / 2;                    // Div 2 because distributed in 2 banks
    uint64_t op2Base = ceil(float(totalCol)/(float(COLPERROW*CORES_PER_PCH)+1.0));            // Op2 will start in this row
    uint64_t resBase = op2Base + ceil(float(totalCol)/(float(COLPERROW*CORES_PER_PCH)+1.0));  // Result will start in this row
    uint64_t zeroBase = resBase + (ceil(float(V)/float(SIMD_WIDTH*CORES_PER_PCH)/2));         // Column fill with zeros, after the result
    uint64_t rowIdx[3] = {0,op2Base,resBase};
    uint64_t colIdx[3] = {0,0,0};
    int nIdx[2] = {0,0};
    int VIdx[2] = {0,0};

    // Write assembly instructions
    assembly << "; Mapping of dot product, C-limited, V = " << V << ", n = " << n << endl;

    if (loops) {
        assembly << "WRF CRF0" << endl;
        assembly << "MOV GRFB" << GRF_ENTRIES-1 << " ODD_BANK[AddrFile] DataFile" << endl;
        for (i=0; i<crfSegment/2; i++) {
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "MOV GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (crfSegment % 2) {
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
        }
        for (i=0; i<crfSegment/2; i++) {
            assembly << "MAC GRFB" << GRF_ENTRIES-1 << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "MAC GRFB" << GRF_ENTRIES-1 << " GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (crfSegment % 2) {
            assembly << "MAC GRFB" << GRF_ENTRIES-1 << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
        }
        if (loops-1)
            assembly << "JUMP " << 2*crfSegment << " " << loops-1 << endl;
        assembly << "MOV ODD_BANK[AddrFile] GRFB" << GRF_ENTRIES-1 << endl;
        assembly << "EXIT" << endl;
        for (i=0; i<ext_loops; i++) { assembly << "EXEC" << endl; }
    }

    if (peeling) {
        assembly << "; Out of the main loop" << endl;
        assembly << "WRF CRF0" << endl;
        assembly << "MOV GRFB" << GRF_ENTRIES-1 << " ODD_BANK[AddrFile] DataFile" << endl;
        for (i=0; i<peeling/2; i++) { 
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "MOV GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (peeling - 2*i)  // If n it's odd, last one is only with GRFA
            assembly << "MOV GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
        for (i=0; i<peeling/2; i++) {
            assembly << "MAC GRFB" << GRF_ENTRIES-1 << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
            assembly << "MAC GRFB" << GRF_ENTRIES-1 << " GRFB" << i << " ODD_BANK[AddrFile] DataFile" << endl;
        }
        if (peeling - 2*i)  // If n it's odd, last one is only with GRFA
            assembly << "MAC GRFB" << GRF_ENTRIES-1 << " GRFA" << i << " EVEN_BANK[AddrFile] DataFile" << endl;
        assembly << "MOV ODD_BANK[AddrFile] GRFB" << GRF_ENTRIES-1 << endl;
        assembly << "EXIT" << endl;
        for (i=0; i<ext_loops; i++) { assembly << "EXEC" << endl; }    
    }

    // Write address for loops and others  
    addrFile << "# Mapping of dot product, C-limited, V = " << V << ", n = " << n << endl;
    for (i=0; i<ext_loops; i++) {
        if (loops)
            addrFile << showbase << hex << build_addr({0,0,0,1,zeroBase,0}) << endl;    // Initialize with zeros
        for (j=0; j<loops; j++) {
            for (k=0; k<crfSegment/2; k++){
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
            for (k=0; k<crfSegment/2; k++){
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
        }
        // Increase col indeces (and row if necessary) in peeling-1,
        // for going to the next S vectors after getting the partial result
        if (loops && peeling) {
            rowIdx[0] += (colIdx[0] + peeling - 1 + (crfSegment%2)) / COLPERROW;
            colIdx[0] = (colIdx[0] + peeling - 1 + (crfSegment%2)) % COLPERROW;
            rowIdx[1] += (colIdx[1] + peeling - 1 + (crfSegment%2)) / COLPERROW;
            colIdx[1] = (colIdx[1] + peeling - 1 + (crfSegment%2)) % COLPERROW;
        }
        if (loops) {
            // MOV (partial) result to BANK
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[2],colIdx[2]}) << endl;
            if (++colIdx[2] == COLPERROW) {
                colIdx[2] = 0;
                rowIdx[2]++;
            }
        }
    }

    // Reset result indeces for reading partial results and writing final ones
    rowIdx[0] = 0;        colIdx[0] = 0;
    rowIdx[1] = op2Base;  colIdx[1] = 0;
    rowIdx[2] = resBase;  colIdx[2] = 0;

    addrFile << "# Out of the main loop" << endl;
    for (i=0; i<ext_loops; i++) {
        if (peeling) {
            if (loops) {
                // Jump to the peeled part of the group of S vectors
                rowIdx[0] += (colIdx[0] + int(ceil((n - peeling) / 2.0))) / COLPERROW;
                colIdx[0] = (colIdx[0] + int(ceil((n - peeling) / 2.0))) % COLPERROW;
                rowIdx[1] += (colIdx[1] + int(ceil((n - peeling) / 2.0))) / COLPERROW;
                colIdx[1] = (colIdx[1] + int(ceil((n - peeling) / 2.0))) % COLPERROW;
            }

            // Retrieve partial result or zeros
            addrFile << showbase << hex << (loops ? build_addr({0,0,0,1,rowIdx[2],colIdx[2]}) : build_addr({0,0,0,1,zeroBase,0})) << endl;
            for (j=0; j<peeling/2; j++) {
                // MOVs to GRFs
                addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;
                addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[0],colIdx[0]}) << endl;
                if (++colIdx[0] == COLPERROW) {
                    colIdx[0] = 0;
                    rowIdx[0]++;
                }
            }
            if (peeling - 2*j){  // If n it's odd, last one is only with GRFA
                addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[0],colIdx[0]}) << endl;  
                cout << i << " 2st part last row " << rowIdx[0] << " col " << colIdx[0] << endl;              
                if (++colIdx[0] == COLPERROW) {
                    colIdx[0] = 0;
                    rowIdx[0]++;
                }
            }

            for (j=0; j<peeling/2; j++) {
                // ALU ops
                addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;
                addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[1],colIdx[1]}) << endl;
                if (++colIdx[1] == COLPERROW) {
                    colIdx[1] = 0;
                    rowIdx[1]++;
                }
            }
            if (peeling - 2*j){  // If n it's odd, last one is only with GRFA
                addrFile << showbase << hex << build_addr({0,0,0,0,rowIdx[1],colIdx[1]}) << endl;                
                if (++colIdx[1] == COLPERROW) {
                    colIdx[1] = 0;
                    rowIdx[1]++;
                }
            }

            // MOV result to BANK
            addrFile << showbase << hex << build_addr({0,0,0,1,rowIdx[2],colIdx[2]}) << endl;
            if (++colIdx[2] == COLPERROW) {
                colIdx[2] = 0;
                rowIdx[2]++;
            }
        }
    }

    // Write data for loops and others
    dataFile << "# Mapping of dot product, C-limited, V = " << V << ", n = " << n << endl;
    for (i=0; i<ext_loops ; i++) {
        if (loops) {
            for (l=0; l<SIMD_WIDTH*CORES_PER_PCH; l++) {  // Initialize to zero
                dataFile << 0 << " "; 
            }
            dataFile << endl;
        }
        for (j=0; j<loops; j++) {
            for (k=0; k<crfSegment; k++){
                // MOVs to GRFs
                for (l=0; l<SIMD_WIDTH*CORES_PER_PCH; l++) {
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
            for (k=0; k<crfSegment; k++){
                // ALU ops
                for (l=0; l<SIMD_WIDTH*CORES_PER_PCH; l++) {
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
        // Jump to next group of vectors, as dimensions left will be done in the peeled part
        if (loops && peeling) {
            VIdx[0] += SIMD_WIDTH*CORES_PER_PCH;  nIdx[0] = 0;
            VIdx[1] += SIMD_WIDTH*CORES_PER_PCH;  nIdx[1] = 0;
        }
    }

    // Reset V and n indices for the peeling part
    VIdx[0] = 0;    nIdx[0] = 0;
    VIdx[1] = 0;    nIdx[1] = 0;
    
    dataFile << "# Out of the main loop" << endl;
    for (i=0; i<ext_loops ; i++) {
        if (peeling) {
            if (loops) {
                // Jump to the dimensions of the vector that are left
                nIdx[0] = n - peeling;
                nIdx[1] = n - peeling;
            }
            // Provide partial result or zeros
            for (j=0; j<SIMD_WIDTH*CORES_PER_PCH; j++) {
                if (loops && (VIdx[0]+j < V))
                    if(WORD_BITS != 8)
                        dataFile << partDotProduct(op1[VIdx[0]+j], op2[VIdx[1]+j], nIdx[0]) << " ";
                    else
                        dataFile << int(partDotProduct(op1[VIdx[0]+j], op2[VIdx[1]+j], nIdx[0])) << " "; //need to cast the results for int8 only
                else 
                    dataFile << 0 << " "; 
            }
            dataFile << endl;
            for (j=0; j<peeling; j++) {
                // MOVs to GRFs
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
            for (j=0; j<peeling; j++) {
                // ALU ops
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
}

#if (HALF_FLOAT)

float partDotProduct(float *op1, float *op2, int n) {  
    half_float::half res = half_float::half_cast<half_float::half>(0);
    for (int i=0; i<n; i++) {
        res += half_float::half_cast<half_float::half>(op1[i]) * half_float::half_cast<half_float::half>(op2[i]);
    }
    return float(res);
}

#else

cnm_t partDotProduct(cnm_t *op1, cnm_t *op2, int n) {   
    cnm_t res = cnm_t(0);
    for (int i=0; i<n; i++){
        res += (cnm_t)(op1[i])*(cnm_t)(op2[i]);
    }
    return (cnm_t)(res);
}

#endif  // HALF_FLOAT