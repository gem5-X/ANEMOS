#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>

#include "half.hpp"
#include "datatypes.h"
#include "../../src/defs.h"

using namespace std;

#define HALF_MASK ((1 << 16) - 1)
#if WORD_BITS != 64
    #define MASK ((1ul << WORD_BITS) - 1)
#else 
    #define MASK 0xFFFFFFFFFFFFFFFF
#endif

int main(int argc, const char *argv[])
{
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <results-file>" << endl;
        return 0;
    }

    string ri = argv[1];    // Input results file name
    string riline;
    string cycle, address;
    dq_type dataAux;
    half_float::half half_aux;
    #if !(HALF_FLOAT)
    #if INT_TYPE
        cnm_t temp_aux;
    #else 
        cnm_union temp_aux;
    #endif
    #endif

    // Open input file
    ifstream results;
    results.open(ri);

    while (getline(results, riline)) {

        istringstream riiss(riline);

        riiss >> cycle >> address;
        cout << cycle << " " << address << "\t";
        #if HALF_FLOAT
                while (riiss >> showbase >> hex >> dataAux) {
                    for (int i=0; DQ_BITS/WORD_BITS; i++) {
                        half_aux = half_float::half(half_float::detail::binary, dataAux & HALF_MASK);
                        cout << half_aux << " ";
                        dataAux = dataAux >> 16;
                    }
                    cout << "\t";
                }

                cout << endl;
            
        #else
                while (riiss >> showbase >> hex >> dataAux) {
                    for (int i=0; i<(DQ_BITS/WORD_BITS); i++) {
                        
                        #if INT_TYPE
                            temp_aux = (dataAux & MASK);
                            cout << (int)temp_aux << " ";
                        #else 
                            temp_aux.bin = (dataAux & MASK);
                            cout << temp_aux.data << " ";
                        #endif
                        dataAux = dataAux >> WORD_BITS;
                    }
                    cout << "\t";
                }

                cout << endl;
            
        #endif
    }
}
