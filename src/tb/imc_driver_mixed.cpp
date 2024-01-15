#include "../cnm_base.h"

#if MIXED_SIM	// Testbench for SystemC + RTL simulation

#include "imc_driver.h"
#include <random>
#include <cmath>

using half_float::half;

void imc_driver::driver_thread() {	// TODO: make pipeline-depth-generic. I think best way is removing most nops, and waiting "real-time" depending on depth, more similar to original work.

	uint clk_period = CLK_PERIOD;
    uint i,j,k;
	uint batch;
    half *half_aux;
    uint64_t DQ_tosend = 0;
    sc_bigint<GRF_WIDTH> bank_tosend = 0;
    sc_lv<GRF_WIDTH> allzs(SC_LOGIC_Z);
    sc_uint<ROW_BITS> row_aux = 0;

	// Randomization tools
	std::mt19937 gen(11111);    // Standard mersenne_twister_engine seeded
	std::uniform_int_distribution<> dis_DQ(0, 10000000);

	// Instruction word
	sc_uint<32> instruction;
	sc_uint<OPCODE_STA-OPCODE_END+1> OPCODE;
	sc_uint<IMM0_STA-IMM0_END+1> IMM0;
	sc_uint<IMM1_STA-IMM1_END+1> IMM1;
	sc_uint<DST_STA-DST_END+1> DST;
	sc_uint<SRC0_STA-SRC0_END+1> SRC0;
	sc_uint<SRC1_STA-SRC1_END+1> SRC1;
	sc_uint<SRC2_STA-SRC2_END+1> SRC2;
	bool RELU;
	bool AAM;
	sc_uint<DST_N_STA-DST_N_END+1> DST_N;
	sc_uint<SRC0_N_STA-SRC0_N_END+1> SRC0_N;
	sc_uint<SRC1_N_STA-SRC1_N_END+1> SRC1_N;

	// Initialization
	rst->write(SC_LOGIC_0);
	RD->write(SC_LOGIC_0);
	WR->write(SC_LOGIC_0);
	ACT->write(SC_LOGIC_0);
	AB_mode->write(SC_LOGIC_0);
	pim_mode->write(SC_LOGIC_0);
	bank_addr->write(0);
	row_addr->write(0);
	col_addr->write(0);
	DQ->write(0);
	even_bus->write(allzs);
	odd_bus->write(allzs);

	wait(clk_period/2, RESOLUTION);
	rst->write(SC_LOGIC_1);
	wait(0, RESOLUTION);

	wait(clk_period/2 + 1, RESOLUTION);



	/// ---------------------------------------------------------------------
	///  --- Test of MAD, MUL, NOP and EXIT (no BANKS) ---
	///		8x 	GRF_A := GRF_A * SRF_M + SRF_A
	///		4x 	GRF_B := GRF_B * GRF_B
	///		1x 	NOP(MULT_STAGES)
	///		12x	MOV (GRF_A(0) <- results)
	///		1x	EXIT

	// Fill the GRFs and SRF
	for (i=0; i<8; i++) {
		wait(0, RESOLUTION);
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_GRF_A;	// Filling GRF_A
		row_addr->write(row_aux);
		col_addr->write(i);
		for (j=0; j<4; j++) {
			DQ_tosend = 0;
			for (k=0; k<4; k++) {
				half_aux = new half(16*i + 4*j + k + 0.1);
				DQ_tosend |= ((uint64_t) half_aux->bin_word()) << k*16;
			}
			DQ->write(DQ_tosend);
			wait(clk_period, RESOLUTION);
			wait(0, RESOLUTION);
		}

		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_SRF_M;	// Filling SRF_M
		row_addr->write(row_aux);
		col_addr->write(i);
		half_aux = new half(-10.0*i);
		DQ->write((uint64_t) half_aux->bin_word());
		wait(clk_period, RESOLUTION);

		wait(0, RESOLUTION);
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_SRF_A;	// Filling SRF_A
		row_addr->write(row_aux);
		col_addr->write(i);
		half_aux = new half(0.05*i+0.05);
		DQ->write((uint64_t) half_aux->bin_word());
		wait(clk_period, RESOLUTION);

		wait(0, RESOLUTION);
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_GRF_B;	// Filling GRF_B
		row_addr->write(row_aux);
		col_addr->write(i);
		for (j=0; j<4; j++) {
			DQ_tosend = 0;
			for (k=0; k<4; k++) {
				half_aux = new half(16*i + 4*j + k + 100.0);
				DQ_tosend |= ((uint64_t) half_aux->bin_word()) << k*16;
			}
			DQ->write(DQ_tosend);
			wait(clk_period, RESOLUTION);
			wait(0, RESOLUTION);
		}
	} // Finished by 810ns (at 100MHz)

	// Write the instructions to the CRF
	for (i=0; i<8; i++) {
		OPCODE = OP_MAD;
		SRC0 = OPC_GRF_A;
		SRC1 = OPC_SRF_M;
		SRC2 = OPC_SRF_A;
		DST = OPC_GRF_A;
		AAM = SC_LOGIC_0;
		SRC0_N = i;
		SRC1_N = i;
		DST_N = i;
		instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
		instruction.range(SRC0_STA,SRC0_END) = SRC0;
		instruction.range(SRC1_STA,SRC1_END) = SRC1;
		instruction.range(SRC2_STA,SRC2_END) = SRC2;
		instruction.range(DST_STA,DST_END) = DST;
		instruction.range(AAM_BIT, AAM_BIT) = AAM;
		instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
		instruction.range(SRC1_N_STA,SRC1_N_END) = SRC1_N;
		instruction.range(DST_N_STA,DST_N_END) = DST_N;
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the first MAD instructions
		row_addr->write(row_aux);
		col_addr->write(i);
		DQ->write((uint64_t) instruction.to_uint());
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}

	for (i=0; i<4; i++) {
		OPCODE = OP_MUL;
		SRC0 = OPC_GRF_B;
		SRC1 = OPC_GRF_B;
		DST = OPC_GRF_B;
		AAM = SC_LOGIC_0;
		SRC0_N = i;
		SRC1_N = 7-i;
		DST_N = i;
		instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
		instruction.range(SRC0_STA,SRC0_END) = SRC0;
		instruction.range(SRC1_STA,SRC1_END) = SRC1;
		instruction.range(DST_STA,DST_END) = DST;
		instruction.range(AAM_BIT, AAM_BIT) = AAM;
		instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
		instruction.range(SRC1_N_STA,SRC1_N_END) = SRC1_N;
		instruction.range(DST_N_STA,DST_N_END) = DST_N;
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the MUL instructions
		row_addr->write(row_aux);
		col_addr->write(i+8);
		DQ->write((uint64_t) instruction.to_uint());
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}	// Finished by 930ns (at 100MHz)

	OPCODE = OP_NOP;
	IMM0 = MULT_STAGES;
	IMM1 = 45;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(IMM0_STA,IMM0_END) = IMM0;
	instruction.range(IMM1_STA,IMM1_END) = IMM1;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the NOP
	row_addr->write(row_aux);
	col_addr->write(12);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	for (i=0; i<8; i++) {
		OPCODE = OP_MOV;
		SRC0 = OPC_GRF_A;
		DST = OPC_GRF_A;
		RELU = SC_LOGIC_0;
		SRC0_N = i;
		DST_N = 0;
		instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
		instruction.range(SRC0_STA,SRC0_END) = SRC0;
		instruction.range(DST_STA,DST_END) = DST;
		instruction.range(RELU_BIT, RELU_BIT) = RELU;
		instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
		instruction.range(DST_N_STA,DST_N_END) = DST_N;
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the MOV instructions
		row_addr->write(row_aux);
		col_addr->write(i+13);
		DQ->write((uint64_t) instruction.to_uint());
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}	// Finished by 1020ns (at 100MHz)

	for (i=0; i<4; i++) {
		OPCODE = OP_MOV;
		SRC0 = OPC_GRF_B;
		DST = OPC_GRF_A;
		RELU = SC_LOGIC_0;
		SRC0_N = i;
		DST_N = 0;
		instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
		instruction.range(SRC0_STA,SRC0_END) = SRC0;
		instruction.range(DST_STA,DST_END) = DST;
		instruction.range(RELU_BIT, RELU_BIT) = RELU;
		instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
		instruction.range(DST_N_STA,DST_N_END) = DST_N;
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the MOV instructions
		row_addr->write(row_aux);
		col_addr->write(i+21);
		DQ->write((uint64_t) instruction.to_uint());
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}	// Finished by 1060ns (at 100MHz)

	OPCODE = OP_EXIT;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(IMM0_STA,IMM0_END) = IMM0;
	instruction.range(IMM1_STA,IMM1_END) = IMM1;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the EXIT
	row_addr->write(row_aux);
	col_addr->write(25);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION); // Finished by 1070ns (at 100MHz)

	// Execute
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_0);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_0;	// Executing PIM
	row_addr->write(row_aux);
	// 8 MAD operations
	i = 8;
	batch = std::max(1,MULT_STAGES-1);
	batch = std::min(i,batch);
	while (i>0) {
		wait(batch*clk_period, RESOLUTION);
		WR->write(SC_LOGIC_0);
		wait((MULT_STAGES)*clk_period, RESOLUTION);
		WR->write(SC_LOGIC_1);
		i -= batch;
		batch = std::min(i,batch);
	}
	// 4 MUL operations
	wait(4*clk_period, RESOLUTION);
	// Rest of operations
	wait((MULT_STAGES+13)*clk_period, RESOLUTION);
	WR->write(SC_LOGIC_0);
	pim_mode->write(SC_LOGIC_0);
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION); // Finished by ?ns (at 100MHz)



	/// ---------------------------------------------------------------------
	/// --- Test of MOV and ReLU (no BANKS) ---
	///		32x	MOV (GRF_A(4) <- GRF_A(3), GRF_A(4) <- GRF_B(3) (16 mov + 16 verif, with ReLU when possible)

	// Fill the GRFs and SRF
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_GRF_A;	// Filling GRF_A
	row_addr->write(row_aux);
	col_addr->write(3);
	for (j=0; j<4; j++) {
		DQ_tosend = 0;
		for (k=0; k<4; k++) {
			half_aux = new half(pow(-1.0,k)*(40.0*j + 15.0*k + 6.0));
			DQ_tosend |= ((uint64_t) half_aux->bin_word()) << k*16;
		}
		DQ->write(DQ_tosend);
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}

	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_GRF_B;	// Filling GRF_B
	row_addr->write(row_aux);
	col_addr->write(3);
	for (j=0; j<4; j++) {
		DQ_tosend = 0;
		for (k=0; k<4; k++) {
			half_aux = new half(pow(-1.0,k+1)*(31.0*j + 23.0*k + -14.0));
			DQ_tosend |= ((uint64_t) half_aux->bin_word()) << k*16;
		}
		DQ->write(DQ_tosend);
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}

	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_SRF_M;	// Filling SRF_M
	row_addr->write(row_aux);
	col_addr->write(3);
	half_aux = new half(-10.0);
	DQ->write((uint64_t) half_aux->bin_word());
	wait(clk_period, RESOLUTION);

	wait(0, RESOLUTION);
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_SRF_A;	// Filling SRF_A
	row_addr->write(row_aux);
	col_addr->write(3);
	half_aux = new half(10);
	DQ->write((uint64_t) half_aux->bin_word());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);
	// Finished by 1600ns (at 100MHz)

	// Write the instructions to the CRF
	for (i=OPC_GRF_A; i<=OPC_SRF_A; i++) {
		for (j=OPC_GRF_A; j<=OPC_SRF_A; j++) {
			OPCODE = OP_MOV;
			SRC0 = j;
			DST = i;
			RELU = SC_LOGIC_1;
			SRC0_N = 3;
			DST_N = 4;
			instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
			instruction.range(SRC0_STA,SRC0_END) = SRC0;
			instruction.range(DST_STA,DST_END) = DST;
			instruction.range(RELU_BIT, RELU_BIT) = RELU;
			instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
			instruction.range(DST_N_STA,DST_N_END) = DST_N;
			WR->write(SC_LOGIC_1);
			AB_mode->write(SC_LOGIC_1);
			pim_mode->write(SC_LOGIC_1);
			row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
			row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the MOV instructions
			row_addr->write(row_aux);
			col_addr->write(i*8+j*2);
			DQ->write((uint64_t) instruction.to_uint());
			wait(clk_period, RESOLUTION);
			wait(0, RESOLUTION);

			OPCODE = OP_MOV;
			SRC0 = i;
			DST = i;
			RELU = SC_LOGIC_0;
			SRC0_N = 4;
			DST_N = 4;
			instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
			instruction.range(SRC0_STA,SRC0_END) = SRC0;
			instruction.range(DST_STA,DST_END) = DST;
			instruction.range(RELU_BIT, RELU_BIT) = RELU;
			instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
			instruction.range(DST_N_STA,DST_N_END) = DST_N;
			WR->write(SC_LOGIC_1);
			AB_mode->write(SC_LOGIC_1);
			pim_mode->write(SC_LOGIC_1);
			row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
			row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the MOV instructions
			row_addr->write(row_aux);
			col_addr->write(i*8+j*2+1);
			DQ->write((uint64_t) instruction.to_uint());
			wait(clk_period, RESOLUTION);
			wait(0, RESOLUTION);
		}
	}	// Finished by 1920ns (at 100MHz)

	// Execute
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_0);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_0;	// Execting the PIM
	row_addr->write(row_aux);;
	wait(32*clk_period, RESOLUTION);
	WR->write(SC_LOGIC_0);
	pim_mode->write(SC_LOGIC_0);
	wait(6*clk_period, RESOLUTION);
	wait(0, RESOLUTION); // Finished by 2300ns (at 100MHz)



	/// ---------------------------------------------------------------------
	/// --- Test of ADD, JUMP and MAC (no BANKS) ---
	///		1x 	GRF_A := GRF_A + GRF_A (accumulating)
	///		1x	NOP(ADD_STAGES-1)
	///		1x	JUMP -2 x7 (For 8 total repetitions)
	///		8x GRF_B := GRF_A * SRF_M + GRF_B
	///		1x	NOP(MULT_STAGES+ADD_STAGES)
	///		9x	MOV (GRF_A(0) <- results)
	///		1x	EXIT

	// Fill the GRFs and SRF
	wait(0, RESOLUTION);
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_GRF_A;	// Filling GRF_A
	row_addr->write(row_aux);
	col_addr->write(0);
	for (j=0; j<4; j++) {
		DQ_tosend = 0;
		for (k=0; k<4; k++) {
			half_aux = new half(0);
			DQ_tosend |= ((uint64_t) half_aux->bin_word()) << k*16;
		}
		DQ->write(DQ_tosend);
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}

	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_GRF_A;	// Filling GRF_A
	row_addr->write(row_aux);
	col_addr->write(1);
	for (j=0; j<4; j++) {
		DQ_tosend = 0;
		for (k=0; k<4; k++) {
			half_aux = new half(-4.0*j - k - 1.0);
			DQ_tosend |= ((uint64_t) half_aux->bin_word()) << k*16;
		}
		DQ->write(DQ_tosend);
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}

	for (i=0; i<8; i++) {
		if (i > 1) {
			WR->write(SC_LOGIC_1);
			AB_mode->write(SC_LOGIC_1);
			pim_mode->write(SC_LOGIC_1);
			row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
			row_aux.range(RF_SEL_BITS-1,0) = RF_GRF_A;	// Filling GRF_A
			row_addr->write(row_aux);
			col_addr->write(i);
			for (j=0; j<4; j++) {
				DQ_tosend = 0;
				for (k=0; k<4; k++) {
					half_aux = new half(0.016*i + 0.004*j + 0.001*k + 0.0001);
					DQ_tosend |= ((uint64_t) half_aux->bin_word()) << k*16;
				}
				DQ->write(DQ_tosend);
				wait(clk_period, RESOLUTION);
				wait(0, RESOLUTION);
			}
		}

		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_SRF_M;	// Filling SRF_M
		row_addr->write(row_aux);
		col_addr->write(i);
		half_aux = new half(-40.0*i + 140);
		DQ->write((uint64_t) half_aux->bin_word());
		wait(clk_period, RESOLUTION);

		wait(0, RESOLUTION);
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_GRF_B;	// Filling GRF_B
		row_addr->write(row_aux);
		col_addr->write(i);
		for (j=0; j<4; j++) {
			DQ_tosend = 0;
			for (k=0; k<4; k++) {
				half_aux = new half(16*i + 4*j + k);
				DQ_tosend |= ((uint64_t) half_aux->bin_word()) << k*16;
			}
			DQ->write(DQ_tosend);
			wait(clk_period, RESOLUTION);
			wait(0, RESOLUTION);
		}
	} // Finished by 3020ns (at 100MHz)

	// Write the instructions to the CRF

	OPCODE = OP_ADD;
	SRC0 = OPC_GRF_A;
	SRC1 = OPC_GRF_A;
	DST = OPC_GRF_A;
	AAM = SC_LOGIC_0;
	SRC0_N = 0;
	SRC1_N = 1;
	DST_N = 0;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(SRC0_STA,SRC0_END) = SRC0;
	instruction.range(SRC1_STA,SRC1_END) = SRC1;
	instruction.range(DST_STA,DST_END) = DST;
	instruction.range(AAM_BIT, AAM_BIT) = AAM;
	instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
	instruction.range(SRC1_N_STA,SRC1_N_END) = SRC1_N;
	instruction.range(DST_N_STA,DST_N_END) = DST_N;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the ADD instruction
	row_addr->write(row_aux);
	col_addr->write(0);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	OPCODE = OP_NOP;
	IMM0 = std::max(1,ADD_STAGES-1);
	IMM1 = 87;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(IMM0_STA,IMM0_END) = IMM0;
	instruction.range(IMM1_STA,IMM1_END) = IMM1;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the NOP
	row_addr->write(row_aux);
	col_addr->write(1);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	OPCODE = OP_JUMP;
	IMM0 = 2;
	IMM1 = 7;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(IMM0_STA,IMM0_END) = IMM0;
	instruction.range(IMM1_STA,IMM1_END) = IMM1;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the JUMP for going 2 inst back 7 times
	row_addr->write(row_aux);
	col_addr->write(2);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	for (i=0; i<8; i++) {
		OPCODE = OP_MAC;
		SRC0 = OPC_GRF_A;
		SRC1 = OPC_SRF_M;
		DST = OPC_GRF_B;
		AAM = SC_LOGIC_0;
		SRC0_N = i;
		SRC1_N = i;
		DST_N = i;
		instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
		instruction.range(SRC0_STA,SRC0_END) = SRC0;
		instruction.range(SRC1_STA,SRC1_END) = SRC1;
		instruction.range(SRC2_STA,SRC2_END) = SRC2;
		instruction.range(DST_STA,DST_END) = DST;
		instruction.range(AAM_BIT, AAM_BIT) = AAM;
		instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
		instruction.range(SRC1_N_STA,SRC1_N_END) = SRC1_N;
		instruction.range(DST_N_STA,DST_N_END) = DST_N;
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the next MAC instructions
		row_addr->write(row_aux);
		col_addr->write(i+3);
		DQ->write((uint64_t) instruction.to_uint());
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}	// Finished by 3130ns (at 100MHz)

	OPCODE = OP_NOP;
	IMM0 = MULT_STAGES+ADD_STAGES;
	IMM1 = 87;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(IMM0_STA,IMM0_END) = IMM0;
	instruction.range(IMM1_STA,IMM1_END) = IMM1;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the NOP
	row_addr->write(row_aux);
	col_addr->write(11);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	OPCODE = OP_MOV;
	SRC0 = OPC_GRF_A;
	DST = OPC_GRF_A;
	RELU = SC_LOGIC_0;
	SRC0_N = 0;
	DST_N = 0;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(SRC0_STA,SRC0_END) = SRC0;
	instruction.range(DST_STA,DST_END) = DST;
	instruction.range(RELU_BIT, RELU_BIT) = RELU;
	instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
	instruction.range(DST_N_STA,DST_N_END) = DST_N;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the MOV instructions
	row_addr->write(row_aux);
	col_addr->write(12);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	for (i=0; i<8; i++) {
		OPCODE = OP_MOV;
		SRC0 = OPC_GRF_B;
		DST = OPC_GRF_A;
		RELU = SC_LOGIC_0;
		SRC0_N = i;
		DST_N = 0;
		instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
		instruction.range(SRC0_STA,SRC0_END) = SRC0;
		instruction.range(DST_STA,DST_END) = DST;
		instruction.range(RELU_BIT, RELU_BIT) = RELU;
		instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
		instruction.range(DST_N_STA,DST_N_END) = DST_N;
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the MOV instructions
		row_addr->write(row_aux);
		col_addr->write(i+13);
		DQ->write((uint64_t) instruction.to_uint());
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}	// Finished by 3230ns (at 100MHz)

	OPCODE = OP_EXIT;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(IMM0_STA,IMM0_END) = IMM0;
	instruction.range(IMM1_STA,IMM1_END) = IMM1;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the EXIT
	row_addr->write(row_aux);
	col_addr->write(21);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION); // Finished by 3240ns (at 100MHz)

	// Execute
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_0);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_0;	// Executing the PIM
	row_addr->write(row_aux);
	wait((8*(2+std::max(1,ADD_STAGES-1))+18+MULT_STAGES+ADD_STAGES)*clk_period, RESOLUTION);
	WR->write(SC_LOGIC_0);
	wait(10*clk_period, RESOLUTION);
	wait(0, RESOLUTION); // Finished by 4100ns (at 100MHz)



	/// ---------------------------------------------------------------------
	/// --- Test of MOV and ReLU (with BANKS)
	///		24x	MOV (to and from BANKS, trying also ReLU, and checking)

	// Fill the GRFs and SRF
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_GRF_A;		// Filling GRF_A
	row_addr->write(row_aux);
	col_addr->write(7);
	for (j=0; j<4; j++) {
		DQ_tosend = 0;
		for (k=0; k<4; k++) {
			half_aux = new half(pow(-1.0,k)*(121.0*j + 3.2*k + 6.0));
			DQ_tosend |= ((uint64_t) half_aux->bin_word()) << k*16;
		}
		DQ->write(DQ_tosend);
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}

	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_GRF_B;		// Filling GRF_B
	row_addr->write(row_aux);
	col_addr->write(7);
	for (j=0; j<4; j++) {
		DQ_tosend = 0;
		for (k=0; k<4; k++) {
			half_aux = new half(pow(-1.0,k+1)*(0.002*j + 0.000021*k + -0.00003));
			DQ_tosend |= ((uint64_t) half_aux->bin_word()) << k*16;
		}
		DQ->write(DQ_tosend);
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}

	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_SRF_M;		// Filling SRF_M
	row_addr->write(row_aux);
	col_addr->write(7);
	half_aux = new half(-9995.8);
	DQ->write((uint64_t) half_aux->bin_word());
	wait(clk_period, RESOLUTION);

	wait(0, RESOLUTION);
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_SRF_A;		// Filling SRF_A
	row_addr->write(row_aux);
	col_addr->write(7);
	half_aux = new half(12345);
	DQ->write((uint64_t) half_aux->bin_word());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);
	// Finished by 4200ns (at 100MHz)

	// Write the instructions to the CRF
	for (i=OPC_EVEN_BANK; i<=OPC_ODD_BANK; i++) {
		for (j=OPC_GRF_A; j<=OPC_SRF_A; j++) {
			OPCODE = OP_MOV;
			SRC0 = j;
			DST = i;
			RELU = (bool) (j % 2);
			SRC0_N = 7;
			DST_N = 0;
			instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
			instruction.range(SRC0_STA,SRC0_END) = SRC0;
			instruction.range(DST_STA,DST_END) = DST;
			instruction.range(RELU_BIT, RELU_BIT) = RELU;
			instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
			instruction.range(DST_N_STA,DST_N_END) = DST_N;
			WR->write(SC_LOGIC_1);
			AB_mode->write(SC_LOGIC_1);
			pim_mode->write(SC_LOGIC_1);
			row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
			row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the MOV instructions
			row_addr->write(row_aux);
			col_addr->write((i-OPC_EVEN_BANK)*4+j);
			DQ->write((uint64_t) instruction.to_uint());
			wait(clk_period, RESOLUTION);
			wait(0, RESOLUTION);
		}
	}	// Finished by 4280ns (at 100MHz)

	for (i=OPC_GRF_A; i<=OPC_SRF_A; i++) {
		for (j=OPC_EVEN_BANK; j<=OPC_ODD_BANK; j++) {
			OPCODE = OP_MOV;
			SRC0 = j;
			DST = i;
			RELU = SC_LOGIC_1;
			SRC0_N = 7;
			DST_N = 0;
			instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
			instruction.range(SRC0_STA,SRC0_END) = SRC0;
			instruction.range(DST_STA,DST_END) = DST;
			instruction.range(RELU_BIT, RELU_BIT) = RELU;
			instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
			instruction.range(DST_N_STA,DST_N_END) = DST_N;
			WR->write(SC_LOGIC_1);
			AB_mode->write(SC_LOGIC_1);
			pim_mode->write(SC_LOGIC_1);
			row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
			row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the MOV instructions
			row_addr->write(row_aux);
			col_addr->write(i*6+(j-OPC_EVEN_BANK)*3+8);
			DQ->write((uint64_t) instruction.to_uint());
			wait(clk_period, RESOLUTION);
			wait(0, RESOLUTION);

			OPCODE = OP_NOP;
			IMM0 = 1;
			IMM1 = 255;
			instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
			instruction.range(IMM0_STA,IMM0_END) = IMM0;
			instruction.range(IMM1_STA,IMM1_END) = IMM1;
			WR->write(SC_LOGIC_1);
			AB_mode->write(SC_LOGIC_1);
			pim_mode->write(SC_LOGIC_1);
			row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
			row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the NOP
			row_addr->write(row_aux);
			col_addr->write(i*6+(j-OPC_EVEN_BANK)*3+9);
			DQ->write((uint64_t) instruction.to_uint());
			wait(clk_period, RESOLUTION);
			wait(0, RESOLUTION);

			OPCODE = OP_MOV;
			SRC0 = i;
			DST = i;
			RELU = SC_LOGIC_0;
			SRC0_N = 0;
			DST_N = 0;
			instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
			instruction.range(SRC0_STA,SRC0_END) = SRC0;
			instruction.range(DST_STA,DST_END) = DST;
			instruction.range(RELU_BIT, RELU_BIT) = RELU;
			instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
			instruction.range(DST_N_STA,DST_N_END) = DST_N;
			WR->write(SC_LOGIC_1);
			AB_mode->write(SC_LOGIC_1);
			pim_mode->write(SC_LOGIC_1);
			row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
			row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the MOV instructions
			row_addr->write(row_aux);
			col_addr->write(i*6+(j-OPC_EVEN_BANK)*3+10);
			DQ->write((uint64_t) instruction.to_uint());
			wait(clk_period, RESOLUTION);
			wait(0, RESOLUTION);
		}
	}	// Finished by 4520ns (at 100MHz)

	// Execute
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_0);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_0;	// Executing the PIM
	row_addr->write(row_aux);
	wait(8*clk_period, RESOLUTION);
	wait(0, RESOLUTION);	// End of MOVs to BANKS

	for (i=OPC_GRF_A; i<=OPC_SRF_A; i++) {
		even_bus->write(allzs);
		odd_bus->write(allzs);
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);

		bank_tosend = 0;
		for (k=0; k<16; k++) {
			half_aux = new half(pow(-1.0,k+i)*(22.1*k + 6.32*i));
			bank_tosend |= ((sc_biguint<GRF_WIDTH>) half_aux->bin_word()) << k*16;
		}
		even_bus->write(bank_tosend);
		odd_bus->write(allzs);
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);

		even_bus->write(allzs);
		odd_bus->write(allzs);
		wait(2*clk_period, RESOLUTION);
		wait(0, RESOLUTION);

		bank_tosend = 0;
		for (k=0; k<16; k++) {
			half_aux = new half(pow(-1.0,k+i)*(0.0000013*k + 0.0000005*i));
			bank_tosend |= ((sc_biguint<GRF_WIDTH>) half_aux->bin_word()) << k*16;
		}
		odd_bus->write(bank_tosend);
		even_bus->write(allzs);
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);

		even_bus->write(allzs);
		odd_bus->write(allzs);
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}

	WR->write(SC_LOGIC_0);
	pim_mode->write(SC_LOGIC_0);
	wait(6*clk_period, RESOLUTION);
	wait(0, RESOLUTION); // Finished by 4900ns (at 100MHz)

	// TODO what if the addition in a MAC or MAD is to a BANK? assume it will be there in time, or register at the next clk (5 stages pipe) (I think the second)

	/// ---------------------------------------------------------------------
	///  --- Test of MAD, MUL, NOP and EXIT (with BANKS) ---
	///		4x 	GRF_A := GRF_B * SRF_M + ODD_BANK (with AAM)
	///		1x  NOP(6clk)
	///		4x 	GRF_A := EVEN_BANK * ODD_BANK + SRF_A (without AAM)
	///		4x 	GRF_B := EVEN_BANK * GRF_B	(with and without AAM)
	///		12x	MOV (GRF_A(0) <- results)
	///		1x	EXIT

	// Fill the GRFs and SRF
	for (i=0; i<8; i++) {
		wait(0, RESOLUTION);
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_GRF_B;		// Filling GRF_B
		row_addr->write(row_aux);
		col_addr->write(i);
		for (j=0; j<4; j++) {
			DQ_tosend = 0;
			for (k=0; k<4; k++) {
				half_aux = new half(160*i + 40*j + k + 123.4);
				DQ_tosend |= ((uint64_t) half_aux->bin_word()) << k*16;
			}
			DQ->write(DQ_tosend);
			wait(clk_period, RESOLUTION);
			wait(0, RESOLUTION);
		}

		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_SRF_M;		// Filling SRF_M
		row_addr->write(row_aux);
		col_addr->write(i);
		half_aux = new half(-0.015*i);
		DQ->write((uint64_t) half_aux->bin_word());
		wait(clk_period, RESOLUTION);

		wait(0, RESOLUTION);
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_SRF_A;		// Filling SRF_A
		row_addr->write(row_aux);
		col_addr->write(i);
		half_aux = new half(2*i-0.5);
		DQ->write((uint64_t) half_aux->bin_word());
		wait(clk_period, RESOLUTION);
	} // Finished by 5380ns (at 100MHz)

	// Write the instructions to the CRF
	wait(0, RESOLUTION);
	for (i=0; i<4; i++) {
		OPCODE = OP_MAD;
		SRC0 = OPC_GRF_B;
		SRC1 = OPC_SRF_M;
		SRC2 = OPC_ODD_BANK;
		DST = OPC_GRF_A;
		AAM = SC_LOGIC_1;
		SRC0_N = 5;
		SRC1_N = 6;
		DST_N = 7;
		instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
		instruction.range(SRC0_STA,SRC0_END) = SRC0;
		instruction.range(SRC1_STA,SRC1_END) = SRC1;
		instruction.range(SRC2_STA,SRC2_END) = SRC2;
		instruction.range(DST_STA,DST_END) = DST;
		instruction.range(AAM_BIT, AAM_BIT) = AAM;
		instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
		instruction.range(SRC1_N_STA,SRC1_N_END) = SRC1_N;
		instruction.range(DST_N_STA,DST_N_END) = DST_N;
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the first MAD instructions
		row_addr->write(row_aux);
		col_addr->write(i);
		DQ->write((uint64_t) instruction.to_uint());
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}

	OPCODE = OP_NOP;
	IMM0 = 5;
	IMM1 = 0;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(IMM0_STA,IMM0_END) = IMM0;
	instruction.range(IMM1_STA,IMM1_END) = IMM1;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the first NOP
	row_addr->write(row_aux);
	col_addr->write(4);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	for (i=0; i<4; i++) {
		OPCODE = OP_MAD;
		SRC0 = OPC_EVEN_BANK;
		SRC1 = OPC_ODD_BANK;
		SRC2 = OPC_SRF_A;
		DST = OPC_GRF_A;
		AAM = SC_LOGIC_0;
		SRC0_N = i+4;
		SRC1_N = i+4;
		DST_N = i+4;
		instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
		instruction.range(SRC0_STA,SRC0_END) = SRC0;
		instruction.range(SRC1_STA,SRC1_END) = SRC1;
		instruction.range(SRC2_STA,SRC2_END) = SRC2;
		instruction.range(DST_STA,DST_END) = DST;
		instruction.range(AAM_BIT, AAM_BIT) = AAM;
		instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
		instruction.range(SRC1_N_STA,SRC1_N_END) = SRC1_N;
		instruction.range(DST_N_STA,DST_N_END) = DST_N;
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the next MAD instructions
		row_addr->write(row_aux);
		col_addr->write(i+5);
		DQ->write((uint64_t) instruction.to_uint());
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}

	OPCODE = OP_NOP;
	IMM0 = 4;
	IMM1 = 0;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(IMM0_STA,IMM0_END) = IMM0;
	instruction.range(IMM1_STA,IMM1_END) = IMM1;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the second NOP
	row_addr->write(row_aux);
	col_addr->write(9);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	for (i=0; i<2; i++) {
		OPCODE = OP_MUL;
		SRC0 = OPC_EVEN_BANK;
		SRC1 = OPC_GRF_B;
		DST = OPC_GRF_B;
		AAM = SC_LOGIC_1;
		SRC0_N = i;
		SRC1_N = 7-i;
		DST_N = i;
		instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
		instruction.range(SRC0_STA,SRC0_END) = SRC0;
		instruction.range(SRC1_STA,SRC1_END) = SRC1;
		instruction.range(DST_STA,DST_END) = DST;
		instruction.range(AAM_BIT, AAM_BIT) = AAM;
		instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
		instruction.range(SRC1_N_STA,SRC1_N_END) = SRC1_N;
		instruction.range(DST_N_STA,DST_N_END) = DST_N;
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the MUL instructions with AAM
		row_addr->write(row_aux);
		col_addr->write(2*i+10);
		DQ->write((uint64_t) instruction.to_uint());
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);

		OPCODE = OP_MUL;
		SRC0 = OPC_EVEN_BANK;
		SRC1 = OPC_GRF_B;
		DST = OPC_GRF_B;
		AAM = SC_LOGIC_0;
		SRC0_N = 2*i+1;
		SRC1_N = 7-i;
		DST_N = 2*i+1;
		instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
		instruction.range(SRC0_STA,SRC0_END) = SRC0;
		instruction.range(SRC1_STA,SRC1_END) = SRC1;
		instruction.range(DST_STA,DST_END) = DST;
		instruction.range(AAM_BIT, AAM_BIT) = AAM;
		instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
		instruction.range(SRC1_N_STA,SRC1_N_END) = SRC1_N;
		instruction.range(DST_N_STA,DST_N_END) = DST_N;
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the MUL instructions without AAM
		row_addr->write(row_aux);
		col_addr->write(2*i+11);
		DQ->write((uint64_t) instruction.to_uint());
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}

	OPCODE = OP_NOP;
	IMM0 = 2;
	IMM1 = 0;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(IMM0_STA,IMM0_END) = IMM0;
	instruction.range(IMM1_STA,IMM1_END) = IMM1;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the third NOP
	row_addr->write(row_aux);
	col_addr->write(14);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	for (i=0; i<8; i++) {
		OPCODE = OP_MOV;
		SRC0 = OPC_GRF_A;
		DST = OPC_GRF_A;
		RELU = SC_LOGIC_0;
		SRC0_N = i;
		DST_N = 0;
		instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
		instruction.range(SRC0_STA,SRC0_END) = SRC0;
		instruction.range(DST_STA,DST_END) = DST;
		instruction.range(RELU_BIT, RELU_BIT) = RELU;
		instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
		instruction.range(DST_N_STA,DST_N_END) = DST_N;
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the MOV instructions
		row_addr->write(row_aux);
		col_addr->write(i+15);
		DQ->write((uint64_t) instruction.to_uint());
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}

	for (i=0; i<4; i++) {
		OPCODE = OP_MOV;
		SRC0 = OPC_GRF_B;
		DST = OPC_GRF_A;
		RELU = SC_LOGIC_0;
		SRC0_N = i;
		DST_N = 0;
		instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
		instruction.range(SRC0_STA,SRC0_END) = SRC0;
		instruction.range(DST_STA,DST_END) = DST;
		instruction.range(RELU_BIT, RELU_BIT) = RELU;
		instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
		instruction.range(DST_N_STA,DST_N_END) = DST_N;
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the MOV instructions
		row_addr->write(row_aux);
		col_addr->write(i+23);
		DQ->write((uint64_t) instruction.to_uint());
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}

	OPCODE = OP_EXIT;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(IMM0_STA,IMM0_END) = IMM0;
	instruction.range(IMM1_STA,IMM1_END) = IMM1;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the EXIT
	row_addr->write(row_aux);
	col_addr->write(27);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION); // Finished by 5660ns (at 100MHz)

	// Execute
	WR->write(SC_LOGIC_0);
	AB_mode->write(SC_LOGIC_0);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_0;	// Executing the PIM
	row_addr->write(row_aux);
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	for (i=0; i<4; i++) {
		WR->write(SC_LOGIC_1);
		row_aux.range(RF_SEL_BITS-1,0) = 0;
		row_addr->write(row_aux);
		col_addr->write(9*i);	// To get with AAM RF indexes 0,0 - 1,1 - 2,2 - 3,3
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);

		WR->write(SC_LOGIC_0);
		bank_tosend = 0;
		for (k=0; k<16; k++) {
			half_aux = new half(12*k + 6.32*i);
			bank_tosend |= ((sc_biguint<GRF_WIDTH>) half_aux->bin_word()) << k*16;
		}
		even_bus->write(allzs);
		odd_bus->write(bank_tosend);

		wait((ADD_STAGES-1)*clk_period, RESOLUTION);	// For getting also the right value when doing the addition
		wait(0, RESOLUTION);
	}

	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	WR->write(SC_LOGIC_1);
	even_bus->write(allzs);
	odd_bus->write(allzs);
	wait(6*clk_period, RESOLUTION); // 4 for the NOP, 1 for the load delay
	wait(0, RESOLUTION);

	for (i=0; i<4; i++) {
		bank_tosend = 0;
		for (k=0; k<16; k++) {
			half_aux = new half(0.002*k + 0.0001*i);
			bank_tosend |= ((sc_biguint<GRF_WIDTH>) half_aux->bin_word()) << k*16;
		}
		even_bus->write(bank_tosend);
		bank_tosend = 0;
		for (k=0; k<16; k++) {
			half_aux = new half(-3220.0*k + 100.0*i);
			bank_tosend |= ((sc_biguint<GRF_WIDTH>) half_aux->bin_word()) << k*16;
		}
		odd_bus->write(bank_tosend);
		if (i == 3)	WR->write(SC_LOGIC_0);
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}	// End of MADs

	WR->write(SC_LOGIC_1);

	wait(4*clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	for (i=0; i<2; i++) {
		row_aux.range(RF_SEL_BITS-1,0) = 0;
		row_addr->write(row_aux);
		col_addr->write(18*i);	// To get with AAM RF indexes 0,0 - 2,2
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);

		bank_tosend = 0;
		for (k=0; k<16; k++) {
			half_aux = new half(-1.0*k + 0.33333*i);
			bank_tosend |= ((sc_biguint<GRF_WIDTH>) half_aux->bin_word()) << k*16;
		}
		even_bus->write(bank_tosend);
		odd_bus->write(allzs);
		row_aux.range(RF_SEL_BITS-1,0) = 1;
		row_addr->write(row_aux);
		col_addr->write(7);
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);

		bank_tosend = 0;
		for (k=0; k<16; k++) {
			half_aux = new half(-1.0*k + 0.33333*(i+2));
			bank_tosend |= ((sc_biguint<GRF_WIDTH>) half_aux->bin_word()) << k*16;
		}
		even_bus->write(bank_tosend);
		odd_bus->write(allzs);
	}	// End of MULs

	wait(3*clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	even_bus->write(allzs);
	odd_bus->write(allzs);
	wait(12*clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	WR->write(SC_LOGIC_0);
	pim_mode->write(SC_LOGIC_0);
	wait(9*clk_period, RESOLUTION);
	wait(0, RESOLUTION); // Finished by 6300ns (at 100MHz)


	/// ---------------------------------------------------------------------
	/// --- Test of ADD, JUMP and MAC (with BANKS) ---
	///		1x 	GRF_A := EVEN_BANK + ODD_BANK
	///		1x	NOP(ADD_STAGES+1)
	///		1x 	MOV (EVEN_BANK <- GRF_A(0))
	///		1x	JUMP -3 x3 (For 4 total repetitions)
	///		1x 	GRF_B := ODD_BANK * SRF_M + GRF_B (AAM)
	///		1x	NOP(MULT_STAGES+ADD_STAGES)
	///		1x	JUMP -2 x7 (For 8 total repetitions)
	///		1x	MOV (GRF_A(0) <- result)
	///		1x	EXIT

	// Fill the GRFs and SRF
	for (i=0; i<8; i++) {
		wait(0, RESOLUTION);
		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_GRF_B;		// Filling GRF_B
		row_addr->write(row_aux);
		col_addr->write(i);
		for (j=0; j<4; j++) {
			DQ_tosend = 0;
			for (k=0; k<4; k++) {
				half_aux = new half(-(k+j*4.0+i*32.0)/16.0);
				DQ_tosend |= ((uint64_t) half_aux->bin_word()) << k*16;
			}
			DQ->write(DQ_tosend);
			wait(clk_period, RESOLUTION);
			wait(0, RESOLUTION);
		}

		WR->write(SC_LOGIC_1);
		AB_mode->write(SC_LOGIC_1);
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
		row_aux.range(RF_SEL_BITS-1,0) = RF_SRF_M;		// Filling SRF_M
		row_addr->write(row_aux);
		col_addr->write(i);
		half_aux = new half(1/pow(2.0,i));
		DQ->write((uint64_t) half_aux->bin_word());
		wait(clk_period, RESOLUTION);
	} // Finished by 6700ns (at 100MHz)

	// Write the instructions to the CRF
	wait(0, RESOLUTION);

	OPCODE = OP_ADD;
	SRC0 = OPC_EVEN_BANK;
	SRC1 = OPC_ODD_BANK;
	DST = OPC_GRF_A;
	AAM = SC_LOGIC_0;
	SRC0_N = 0;
	SRC1_N = 1;
	DST_N = 0;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(SRC0_STA,SRC0_END) = SRC0;
	instruction.range(SRC1_STA,SRC1_END) = SRC1;
	instruction.range(DST_STA,DST_END) = DST;
	instruction.range(AAM_BIT, AAM_BIT) = AAM;
	instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
	instruction.range(SRC1_N_STA,SRC1_N_END) = SRC1_N;
	instruction.range(DST_N_STA,DST_N_END) = DST_N;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the ADD instruction
	row_addr->write(row_aux);
	col_addr->write(0);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	OPCODE = OP_NOP;
	IMM0 = ADD_STAGES+1;
	IMM1 = 1024;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(IMM0_STA,IMM0_END) = IMM0;
	instruction.range(IMM1_STA,IMM1_END) = IMM1;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the NOP instruction
	row_addr->write(row_aux);
	col_addr->write(1);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	OPCODE = OP_MOV;
	SRC0 = OPC_GRF_A;
	DST = OPC_EVEN_BANK;
	RELU = SC_LOGIC_0;
	SRC0_N = 0;
	DST_N = 0;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(SRC0_STA,SRC0_END) = SRC0;
	instruction.range(DST_STA,DST_END) = DST;
	instruction.range(RELU_BIT, RELU_BIT) = RELU;
	instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
	instruction.range(DST_N_STA,DST_N_END) = DST_N;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the MOV instructions
	row_addr->write(row_aux);
	col_addr->write(2);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	OPCODE = OP_JUMP;
	IMM0 = 3;
	IMM1 = 3;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(IMM0_STA,IMM0_END) = IMM0;
	instruction.range(IMM1_STA,IMM1_END) = IMM1;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the JUMP for going 3 inst back 3 times
	row_addr->write(row_aux);
	col_addr->write(3);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	OPCODE = OP_MAC;
	SRC0 = OPC_ODD_BANK;
	SRC1 = OPC_SRF_M;
	DST = OPC_GRF_B;
	AAM = SC_LOGIC_1;
	SRC0_N = 5;
	SRC1_N = 6;
	DST_N = 7;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(SRC0_STA,SRC0_END) = SRC0;
	instruction.range(SRC1_STA,SRC1_END) = SRC1;
	instruction.range(SRC2_STA,SRC2_END) = SRC2;
	instruction.range(DST_STA,DST_END) = DST;
	instruction.range(AAM_BIT, AAM_BIT) = AAM;
	instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
	instruction.range(SRC1_N_STA,SRC1_N_END) = SRC1_N;
	instruction.range(DST_N_STA,DST_N_END) = DST_N;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the MAC instruction
	row_addr->write(row_aux);
	col_addr->write(4);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	OPCODE = OP_NOP;
	IMM0 = MULT_STAGES+ADD_STAGES;
	IMM1 = 0;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(IMM0_STA,IMM0_END) = IMM0;
	instruction.range(IMM1_STA,IMM1_END) = IMM1;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the NOP instructions
	row_addr->write(row_aux);
	col_addr->write(5);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	OPCODE = OP_JUMP;
	IMM0 = 2;
	IMM1 = 7;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(IMM0_STA,IMM0_END) = IMM0;
	instruction.range(IMM1_STA,IMM1_END) = IMM1;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the JUMP for going 2 inst back 7 times
	row_addr->write(row_aux);
	col_addr->write(6);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	OPCODE = OP_MOV;
	SRC0 = OPC_GRF_B;
	DST = OPC_GRF_A;
	RELU = SC_LOGIC_0;
	SRC0_N = 0;
	DST_N = 0;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(SRC0_STA,SRC0_END) = SRC0;
	instruction.range(DST_STA,DST_END) = DST;
	instruction.range(RELU_BIT, RELU_BIT) = RELU;
	instruction.range(SRC0_N_STA,SRC0_N_END) = SRC0_N;
	instruction.range(DST_N_STA,DST_N_END) = DST_N;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the MOV instructions
	row_addr->write(row_aux);
	col_addr->write(7);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	OPCODE = OP_EXIT;
	instruction.range(OPCODE_STA,OPCODE_END) = OPCODE;
	instruction.range(IMM0_STA,IMM0_END) = IMM0;
	instruction.range(IMM1_STA,IMM1_END) = IMM1;
	WR->write(SC_LOGIC_1);
	AB_mode->write(SC_LOGIC_1);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_1;	// Writing to RFs
	row_aux.range(RF_SEL_BITS-1,0) = RF_CRF;		// Writing the EXIT
	row_addr->write(row_aux);
	col_addr->write(8);
	DQ->write((uint64_t) instruction.to_uint());
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION); // Finished by 6790ns (at 100MHz)

	// Execute
	WR->write(SC_LOGIC_0);
	AB_mode->write(SC_LOGIC_0);
	pim_mode->write(SC_LOGIC_1);
	row_aux.range(ROW_BITS-1, ROW_BITS-1) = SC_LOGIC_0;	// Executing the PIM
	row_addr->write(row_aux);
	wait(clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	for (i=0; i<4; i++) {
		WR->write(SC_LOGIC_1);
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);

		bank_tosend = 0;
		for (k=0; k<16; k++) {
			half_aux = new half(10.0*k + 100.0*i+1.0);
			bank_tosend |= ((sc_biguint<GRF_WIDTH>) half_aux->bin_word()) << k*16;
		}
		even_bus->write(bank_tosend);
		bank_tosend = 0;
		for (k=0; k<16; k++) {
			half_aux = new half(200.0*k + 20.0*i+2.0);
			bank_tosend |= ((sc_biguint<GRF_WIDTH>) half_aux->bin_word()) << k*16;
		}
		odd_bus->write(bank_tosend);

		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);
		even_bus->write(allzs);
		odd_bus->write(allzs);

		wait((2+ADD_STAGES)*clk_period, RESOLUTION);	// For getting also the right value when doing the addition
		wait(0, RESOLUTION);
	}

	for (i=0; i<8; i++) {
		// Write addr
		pim_mode->write(SC_LOGIC_1);
		row_aux.range(RF_SEL_BITS-1,0) = 0;
		row_addr->write(row_aux);
		col_addr->write(i);
		wait(clk_period, RESOLUTION);
		wait(0, RESOLUTION);

		// Write bank
		bank_tosend = 0;
		for (k=0; k<16; k++) {
			half_aux = new half(13.0*k + 500.0*i);
			bank_tosend |= ((sc_biguint<GRF_WIDTH>) half_aux->bin_word()) << k*16;
		}
		odd_bus->write(bank_tosend);

		wait((1+MULT_STAGES+ADD_STAGES)*clk_period, RESOLUTION);
		wait(0, RESOLUTION);
	}

	even_bus->write(allzs);
	odd_bus->write(allzs);
	wait(2*clk_period, RESOLUTION);
	wait(0, RESOLUTION);

	WR->write(SC_LOGIC_0);
	pim_mode->write(SC_LOGIC_0);
	wait(6*clk_period, RESOLUTION);
	wait(0, RESOLUTION); // Finished by 8200ns (at 100MHz)
}

#endif
