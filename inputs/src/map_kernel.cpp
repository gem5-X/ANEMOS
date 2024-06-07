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
#if DATA_TYPE == 4
    std::uniform_int_distribution<cnm_t> dis(-1, 1);
#else
    std::uniform_int_distribution<cnm_t> dis(-32768, 32767); //for int8 use (-1,1) to not overflow
#endif  // DATA_TYPE == 4
#else
    std::normal_distribution<cnm_t> dis(0, 65504/32768); 
#endif  // INT_TYPE
#endif  // HALF_FLOAT

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
            // cout << "--------- RESULTS ----------" << endl;
            // for (int i =0; i < V; i++){
            //     for(int j =0; j < n; j++){
            //         cout << op1[i][j] + op2[i][j] << "\t";
            //         if (!((i+1)%SIMD_WIDTH))    cout << endl;
            //     }
            //     cout << endl;
            // }
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