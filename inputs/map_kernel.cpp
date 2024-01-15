#include "map_kernel.h"

int main(int argc, const char *argv[])
{
    if (argc != 5 && argc != 6 && argc != 11) {
        cout << "Usage: " << argv[0] << " <output_name> <kernel> <V> <n>" << endl;
        cout << "OR" << endl;
        cout << "Usage: " << argv[0] << " <output_name> <kernel> <m> <n> <q>" << endl;
        cout << "OR" << endl;
        cout << "Usage: " << argv[0] << " <output_name> <kernel> <ci> <wi> <hi> <k> <stride> <co> <wo> <ho>" << endl;
        cout << argc;
        return 0;
    }

    string ao = "assembly-input/" + string(argv[1]) + ".asm";   // Output assembly file name
    string df = "data-input/" + string(argv[1]) + ".data";      // Output data file name
    string af = "address-input/" + string(argv[1]) + ".addr";   // Output address file name

    // Open output files
    ofstream assembly;
    ofstream dataFile;
    ofstream addrFile;
    assembly.open(ao);
    dataFile.open(df);
    addrFile.open(af);

    std::mt19937 gen(1111);    // Standard mersenne_twister_engine seeded
#if HALF_FLOAT
    std::normal_distribution<float> dis(0, 65504/32768); 
#else
#if INT_TYPE
    std::uniform_int_distribution<cnm_t> dis(-2147483648, 2147483648); //for int8 use (-1,1) to not overflow
#else
    std::normal_distribution<cnm_t> dis(0, 65504/32768); 
#endif
#endif

    // Trial mapping of simple kernels
    int V,n;
    // float **op1, **op2;
    cnm_t **op1, **op2;
    if (argc == 5) {
        V = atoi(argv[3]);
        n = atoi(argv[4]);
        **op1, **op2;
        // op1 = new float *[V];
        // op2 = new float *[V];
        op1 = new cnm_t *[V];
        op2 = new cnm_t *[V];
        for (int i = 0; i<V; i++){
            // op1[i] = new float[n];
            // op2[i] = new float[n];
            op1[i] = new cnm_t[n];
            op2[i] = new cnm_t[n];
        }
        for (int i = 0; i<V; i++){
            for (int j = 0; j<n; j++){
                // op1[i][j] = i+j;
                // op2[i][j] = 1;
                op1[i][j] = dis(gen);
                op2[i][j] = dis(gen);
            }
        }
    }

    // Trial mapping of matrix multiplication
    int m,q;
    // float *opM1, *opM2;
    cnm_t *opM1, *opM2;
    if (argc == 6) {
        m = atoi(argv[3]);
        n = atoi(argv[4]);
        q = atoi(argv[5]);
        // opM1 = new float[m*n];
        // opM2 = new float[n*q];
        opM1 = new cnm_t[m*n];
        opM2 = new cnm_t[n*q];
        for (int i = 0; i<m*n; i++)
            opM1[i] = dis(gen);
        for (int i = 0; i<n*q; i++)
            opM2[i] = dis(gen);
    }

    // Trial mapping of convolution
    int ci, wi, hi, k, stride, co, wo, ho;
    // float *act, *weight, *bias;
    cnm_t *act, *weight, *bias;
    if (argc == 11) {
        ci = atoi(argv[3]);
        wi = atoi(argv[4]);
        hi = atoi(argv[5]);
        k = atoi(argv[6]);
        stride = atoi(argv[7]);
        co = atoi(argv[8]);
        wo = atoi(argv[9]);
        ho = atoi(argv[10]);
        // act = new float[ci*wi*hi];
        // weight = new float[co*ci*k*k];
        // bias = new float[co];
        act = new cnm_t[ci*wi*hi];
        weight = new cnm_t[co*ci*k*k];
        bias = new cnm_t[co];
        for (int i=0; i<ci*wi*hi; i++) {
            act[i] = dis(gen);
        }
        for (int i=0; i<co*ci*k*k; i++) {
            weight[i] = dis(gen);
        }
        for (int i=0; i<co; i++) {
            bias[i] = dis(gen);
        }
    }

    uint8_t kernel_choice = KERNEL.at(argv[2]);

    // Generate correct mapping
    switch(kernel_choice) {
        case EWARW:
            if (CRF_ENTRIES < (6*GRF_ENTRIES + 3*(int(ceil(float(n*V)/float(SIMD_WIDTH*CORES_PER_PCH))) % (2*GRF_ENTRIES)) + 2))
                mapEWAdditionRowWiseCLim(assembly, dataFile, addrFile, op1, op2, V, n);
            else
                mapEWAdditionRowWiseRLim(assembly, dataFile, addrFile, op1, op2, V, n);
        break;
        case EWACW:
            if (CRF_ENTRIES < (6*GRF_ENTRIES + 3*(int(float(n)*ceil(float(V)/float(SIMD_WIDTH*CORES_PER_PCH))) % (2*GRF_ENTRIES)) + 2))
                mapEWAdditionColWiseCLim(assembly, dataFile, addrFile, op1, op2, V, n);
            else
                mapEWAdditionColWiseRLim(assembly, dataFile, addrFile, op1, op2, V, n);
        break;
        case EWMRW: break;
        case EWMCW: break;
        case DP:
            if (CRF_ENTRIES < (4*GRF_ENTRIES + 2*(n % (2*GRF_ENTRIES-1)) + 2))
                mapDotProductCLim(assembly, dataFile, addrFile, op1, op2, V, n);
            else
                mapDotProductRLim(assembly, dataFile, addrFile, op1, op2, V, n);
            // // Print results for checking
            // cout << "--------- RESULTS ----------" << endl;
            // for (int i=0; i<V; i++) {
            //     if(WORD_BITS != 8)
            //         cout << partDotProduct(op1[i], op2[i], n) << " ";
            //     else
            //         cout << int(partDotProduct(op1[i], op2[i], n)) << " "; //need to cast the results for int8 only
            //     if (!((i+1)%SIMD_WIDTH))    cout << endl;
            // }
            // cout << endl;
        break;
        case MMS:
            if (CRF_ENTRIES < (SRF_M_ENTRIES + 4))
                mapMatrixMultSrfCLim(assembly, dataFile, addrFile, opM1, opM2, m, n, q);
            else
                mapMatrixMultSrfRLim(assembly, dataFile, addrFile, opM1, opM2, m, n, q);
            // // Print matrices
            // cout << "--------- MATRIX 1 ----------" << endl;
            // for (int i=0; i<m; i++) {
            //     for (int j=0; j<n; j++) {
            //         cout << opM1[i*n+j] << " ";
            //     }
            //     cout << endl;
            // }
            // cout << "--------- MATRIX 2 ----------" << endl;
            // for (int i=0; i<n; i++) {
            //     for (int j=0; j<q; j++) {
            //         cout << opM2[i*q+j] << " ";
            //     }
            //     cout << endl;
            // }
        
            // // Print results for checking
            // cout << "--------- RESULTS ----------" << endl;
            // for (int i=0; i<m; i++) {
            //     for (int j=0; j<q; j++) {
            //         cout << partMatMultDP(opM1, opM2, m, n, q, i, j, n) << " ";
            //         if (!((j+1)%(SIMD_WIDTH*CORES_PER_PCH)))    cout << endl;
            //     }
            //     cout << endl;
            // }
        break;
        case CCWWR:
            if (CRF_ENTRIES < (SRF_M_ENTRIES + 4))
                mapConvCWWRCLim(assembly, dataFile, addrFile, act, weight, bias, ci, wi, hi, k, co, wo, ho, stride);
            else
                mapConvCWWRRLim(assembly, dataFile, addrFile, act, weight, bias, ci, wi, hi, k, co, wo, ho, stride);
            // Print activations of first channel
            // cout << "--------- ACTIVATIONS ----------" << endl;
            // for (int j=0; j<hi; j++) {
            //     for (int l=0; l<wi; l++) {
            //         cout << act[(ci-1)*hi*wi + j*wi + l] << " ";
            //     }
            //     cout << endl;
            // } 
            // cout << endl;

            // // Print weights
            // cout << "--------- WEIGHTS ----------" << endl;
            // for (int j=0; j<k; j++) {
            //     for (int l=0; l<k; l++) {
            //         cout << weight[(ci-1)*k*k + j*k + l] << " ";
            //     }
            //     cout << endl;
            // } 
            // cout << endl;

            // // Print bias of first channel
            // cout << "--------- BIAS ----------" << endl;
            // cout << bias[0] << endl;
            
            // // Print results for checking
            // cout << "--------- RESULTS ----------" << endl;
            // for (int i=0; i<co; i++) {
            //     for (int j=0; j<ho; j++) {
            //         for (int l=0; l<wo; l++) {
            //             cout << partPixConv(act, weight, bias, stride, k, ci, hi, wi, ci, 0, 0, i, j, l) << " ";
            //             if (!((j*wo+l+1)%(SIMD_WIDTH*CORES_PER_PCH)))    cout << endl;
            //         }
            //     }
            //     cout << endl;
            // }
        break;
        default:    break;
    }

    return 0;
}

// TODO I'd say there's a general bug with peeling if it's an odd number, to check later

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

uint64_t build_addr(vector<uint64_t> addr_vec)
{
    uint64_t addr_aux = 0;
    uint64_t offset = global_offset; // Don't know really why right now, maybe to address groups of 128

    for (int i = 0; i < int(Level::MAX); i++) {
        if (addr_bits[int(addr_map[i])]) {
            if (addr_vec[int(addr_map[i])] >= (1 << addr_bits[int(addr_map[i])]))
                cout << "Warning, " << level_str[int(addr_map[i])] << " index is too large" << endl;
            if (addr_map[i] == Level::Row && addr_vec[int(addr_map[i])] >= (1 << (addr_bits[int(addr_map[i])]-1)))
                cout << "Warning, row MSB is set, which can interfere with PIM computing: rowIdx " << dec << addr_vec[int(addr_map[i])] << endl;
            addr_aux |= (addr_vec[int(addr_map[i])] << offset);
            offset += addr_bits[int(addr_map[i])];
        }
    }
    return addr_aux;
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
#endif

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
#endif