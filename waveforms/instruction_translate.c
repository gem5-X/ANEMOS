#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Instructions format
#define OPCODE_STA  31
#define OPCODE_END  28
#define IMM0_STA    18
#define IMM0_END    11
#define IMM1_STA    10
#define IMM1_END    0
#define DST_STA     27
#define DST_END     25
#define SRC0_STA    24
#define SRC0_END    22
#define SRC1_STA    21
#define SRC1_END    19
#define SRC2_STA    18
#define SRC2_END    16
#define RELU_BIT    15
#define AAM_BIT     15
#define DST_N_STA   14
#define DST_N_END   10
#define SRC0_N_STA  9
#define SRC0_N_END  5
#define SRC1_N_STA  4
#define SRC1_N_END  0

// OPCODES operation identifiers
enum OPCODE_VALUES{
	OP_NOP = 0,
	OP_JUMP = 1,
	OP_EXIT = 2,
	OP_MOV = 4,
	OP_FILL = 5,
	OP_ADD = 8,
	OP_MUL = 9,
	OP_MAD = 10,
	OP_MAC = 11
};

// OPCODES storage identifiers
enum OPC_STORAGE{
	OPC_GRF_A = 0,
	OPC_GRF_B = 1,
	OPC_SRF_M = 2,
	OPC_SRF_A = 3,
	OPC_EVEN_BANK = 4,
	OPC_ODD_BANK = 5
};

void print_storage (char *buffer, unsigned int op_storage) {
    switch (op_storage) {
        case OPC_GRF_A:     strcat(buffer,"GRF_A ");      break;
        case OPC_GRF_B:     strcat(buffer,"GRF_B ");      break;
        case OPC_SRF_M:     strcat(buffer,"SRF_M ");      break;
        case OPC_SRF_A:     strcat(buffer,"SRF_A ");      break;
        case OPC_EVEN_BANK: strcat(buffer,"EVEN_BANK ");  break;
        case OPC_ODD_BANK:  strcat(buffer,"ODD_BANK ");   break;
        default:                                          break;
    }
}

int main (int argc, char **argv) {

    /* Create bitmasks for interpreting the instruction word */
    unsigned int opcode_bm = ((1 << (OPCODE_STA-OPCODE_END+1)) - 1) << OPCODE_END;
    unsigned int imm0_bm = ((1 << (IMM0_STA-IMM0_END+1)) - 1) << IMM0_END;
    unsigned int imm1_bm = ((1 << (IMM1_STA-IMM1_END+1)) - 1) << IMM1_END;
    unsigned int dst_bm = ((1 << (DST_STA-DST_END+1)) - 1) << DST_END;
    unsigned int src0_bm = ((1 << (SRC0_STA-SRC0_END+1)) - 1) << SRC0_END;
    unsigned int src1_bm = ((1 << (SRC1_STA-SRC1_END+1)) - 1) << SRC1_END;
    unsigned int src2_bm = ((1 << (SRC2_STA-SRC2_END+1)) - 1) << SRC2_END;
    unsigned int relu_bm = ((1 << (RELU_BIT-RELU_BIT+1)) - 1) << RELU_BIT;
    unsigned int aam_bm = ((1 << (AAM_BIT-AAM_BIT+1)) - 1) << AAM_BIT;
    unsigned int dstn_bm = ((1 << (DST_N_STA-DST_N_END+1)) - 1) << DST_N_END;
    unsigned int src0n_bm = ((1 << (SRC0_N_STA-SRC0_N_END+1)) - 1) << SRC0_N_END;
    unsigned int src1n_bm = ((1 << (SRC1_N_STA-SRC1_N_END+1)) - 1) << SRC1_N_END;

    while(!feof(stdin)) {
        char buf[1025], buf2[1025], buf3[33];

        buf[0] = 0;
        buf2[0] = 0;
        buf3[0] = 0;
        /* Read instruction word */
        fscanf(stdin, "%s", buf);
        if (buf[0]) {
            /* Translate binary instruction string into int */
            unsigned int instruction;
            sscanf(buf, "%x", &instruction);

            /* Interpret instruction and obtain descriptive string */
            unsigned int opcode = (instruction & opcode_bm) >> OPCODE_END;
            unsigned int imm0 = (instruction & imm0_bm) >> IMM0_END;
            unsigned int imm1 = (instruction & imm1_bm) >> IMM1_END;
            unsigned int dst = (instruction & dst_bm) >> DST_END;
            unsigned int src0 = (instruction & src0_bm) >> SRC0_END;
            unsigned int src1 = (instruction & src1_bm) >> SRC1_END;
            unsigned int src2 = (instruction & src2_bm) >> SRC2_END;
            unsigned int relu = (instruction & relu_bm) >> RELU_BIT;
            unsigned int aam = (instruction & aam_bm) >> AAM_BIT;
            unsigned int dstn = (instruction & dstn_bm) >> DST_N_END;
            unsigned int src0n = (instruction & src0n_bm) >> SRC0_N_END;
            unsigned int src1n = (instruction & src1n_bm) >> SRC1_N_END;

            switch (opcode) {
                case OP_NOP:    strcat(buf2,"OP_NOP ");     break;
                case OP_JUMP:   strcat(buf2,"OP_JUMP ");    break;
                case OP_EXIT:   strcat(buf2,"OP_EXIT ");    break;
                case OP_MOV:    strcat(buf2,"OP_MOV ");     break;
                case OP_FILL:   strcat(buf2,"OP_FILL ");    break;
                case OP_ADD:    strcat(buf2,"OP_ADD ");     break;
                case OP_MUL:    strcat(buf2,"OP_MUL ");     break;
                case OP_MAD:    strcat(buf2,"OP_MAD ");     break;
                case OP_MAC:    strcat(buf2,"OP_MAC ");     break;
                default:                                    break;
            }

            if (opcode == OP_NOP || opcode == OP_JUMP || opcode == OP_EXIT) {
                sprintf(buf3, "%d ", imm0);
                strcat(buf2,buf3); 
                sprintf(buf3, "%d ", imm1);
                strcat(buf2,buf3); 
            } else if (opcode == OP_MOV || opcode == OP_FILL) {
                print_storage(buf2, dst);
                print_storage(buf2, src0);
                if (relu)   strcat(buf2,"RELU ");
                sprintf(buf3, "%d ", dstn);
                strcat(buf2,buf3); 
                sprintf(buf3, "%d ", src0n);
                strcat(buf2,buf3); 
            } else {
                print_storage(buf2, dst);
                print_storage(buf2, src0);
                print_storage(buf2, src1);
                print_storage(buf2, src2);
                if (aam)   strcat(buf2,"AAM ");
                sprintf(buf3, "%d ", dstn);
                strcat(buf2,buf3); 
                sprintf(buf3, "%d ", src0n);
                strcat(buf2,buf3); 
                sprintf(buf3, "%d ", src1n);
                strcat(buf2,buf3); 
            }

            /* Printf the translated string to stdout */
            printf("%s\n", buf2);
            fflush(stdout);
        }
    }

    return 0;
}
