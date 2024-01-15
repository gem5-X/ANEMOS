/* 
 * Copyright EPFL 2023
 * Rafael Medina Morillas
 * 
 * Riselda Kodra
 *
 * Description of the In-Memory Computing Core.
 *
 */

#ifndef IMC_CORE_H_
#define IMC_CORE_H_

#include "control_unit.h"
#include "fpu.h"
#include "crf.h"
#include "grf.h"
#include "srf.h"
#include "tristate_buffer.h"

class imc_core: public sc_module {
public:

#ifdef __SYNTHESIS__

	sc_in_clk       				clk;
	sc_in<bool>     				rst;
	sc_in<bool>						RD;			// DRAM read command
	sc_in<bool>						WR;			// DRAM write command
	sc_in<bool>						ACT;		// DRAM activate command
//    sc_in<bool>						RSTB;		//
	sc_in<bool>						AB_mode;	// Signals if the All-Banks mode is enabled
	sc_in<bool>						pim_mode;	// Signals if the PIM mode is enabled
	sc_in<sc_uint<BANK_BITS> >		bank_addr;	// Address of the bank
	sc_in<sc_uint<ROW_BITS> >		row_addr;	// Address of the bank row
	sc_in<sc_uint<COL_BITS> >		col_addr;	// Address of the bank column
	sc_in<sc_uint<DQ_BITS> >		DQ;			// Data input from DRAM controller (output makes no sense)
	sc_in<sc_biguint<GRF_WIDTH> >	even_in;	// Direct data in/out to the even bank
	sc_in<sc_biguint<GRF_WIDTH> >	odd_in;		// Direct data in/out to the odd bank
	sc_out<sc_biguint<GRF_WIDTH> >	even_out;	// Direct data in/out to the even bank
	sc_out<sc_biguint<GRF_WIDTH> >	odd_out;	// Direct data in/out to the odd bank

	// ** INTERNAL SIGNALS AND VARIABLES **
	// Basic control
	sc_signal<uint8_t>					PC;
	sc_signal<uint32_t>					instr;
	sc_signal<sc_biguint<GRF_WIDTH> >	data_out;
	// CRF
	sc_signal<bool>		crf_wr_en;
	sc_signal<uint>		crf_wr_addr;
	// SRF
	sc_signal<uint>				srf_rd_addr, srf_wr_addr;
	sc_signal<bool>				srf_rd_a_nm, srf_wr_a_nm, srf_wr_en;
	sc_signal<uint8_t>			srf_wr_from;
	sc_signal<cnm_synth>	srf_out;
	// GRF_A
	sc_signal<uint>				grfa_rd_addr1, grfa_rd_addr2, grfa_wr_addr;
	sc_signal<bool>				grfa_wr_en, grfa_relu_en;
	sc_signal<uint8_t>			grfa_wr_from;
	sc_signal<cnm_synth>	grfa_out1[SIMD_WIDTH], grfa_out2[SIMD_WIDTH];
	// GRF_B
	sc_signal<uint>				grfb_rd_addr1, grfb_rd_addr2, grfb_wr_addr;
	sc_signal<bool>				grfb_wr_en, grfb_relu_en;
	sc_signal<uint8_t>			grfb_wr_from;
	sc_signal<cnm_synth>	grfb_out1[SIMD_WIDTH], grfb_out2[SIMD_WIDTH];
	// FPU
	sc_signal<bool>				fpu_mult_en, fpu_add_en, fpu_out_sel;
	sc_signal<uint8_t>			fpu_mult_in1_sel, fpu_mult_in2_sel, fpu_add_in1_sel, fpu_add_in2_sel;
	sc_signal<cnm_synth>	fpu_out[SIMD_WIDTH];
	// BANKS
	sc_signal<bool>	even_out_en, odd_out_en;

	// Input registers
//	sc_signal<bool>						RD_reg;			// DRAM read command
//	sc_signal<bool>						WR_reg;			// DRAM write command
//	sc_signal<bool>						ACT_reg;		// DRAM activate command
////    sc_signal<bool>						RSTB_reg;		//
//	sc_signal<bool>						AB_mode_reg;	// Signals if the All-Banks mode is enabled
//	sc_signal<bool>						pim_mode_reg;	// Signals if the PIM mode is enabled
//	sc_signal<sc_uint<BANK_BITS> >		bank_addr_reg;	// Address of the bank
//	sc_signal<sc_uint<ROW_BITS> >		row_addr_reg;	// Address of the bank row
//	sc_signal<sc_uint<COL_BITS> >		col_addr_reg;	// Address of the bank column
//	sc_signal<uint64_t>					DQ_reg;			// Data input from DRAM controller (output makes no sense)
//	sc_signal<sc_biguint<GRF_WIDTH> >	even_in_reg;	// Direct data in/out to the even bank
//	sc_signal<sc_biguint<GRF_WIDTH> >	odd_in_reg;		// Direct data in/out to the odd bank

	// Auxiliar signals
	sc_signal<uint32_t>				ext2crf;
	sc_signal<cnm_synth>		ext2srf, ext2grf[SIMD_WIDTH];
	sc_signal<cnm_synth>		even2grfa[SIMD_WIDTH], odd2grfb[SIMD_WIDTH];
	sc_signal<sc_lv<GRF_WIDTH> >	grfa2even, grfb2odd;

	// Internal modules
	control_unit *cu;
	fpu *fpunit;
	crf *controlrf;
	grf *grfa, *grfb;
	srf *scalarrf;
//	tristate_buffer<GRF_WIDTH> *even_buf, *odd_buf;


	SC_HAS_PROCESS(imc_core);
	imc_core(sc_module_name name) : sc_module(name)
	{
		int i;

		cu = new control_unit("control_unit");
		cu->clk(clk);
		cu->rst(rst);
//		cu->RD(RD_reg);
//		cu->WR(WR_reg);
//		cu->ACT(ACT_reg);
////	    cu->RSTB(RSTB_reg);
//		cu->AB_mode(AB_mode_reg);
//		cu->pim_mode(pim_mode_reg);
//		cu->bank_addr(bank_addr_reg);
//		cu->row_addr(row_addr_reg);
//		cu->col_addr(col_addr_reg);
//		cu->DQ(DQ_reg);
		cu->RD(RD);
		cu->WR(WR);
		cu->ACT(ACT);
//	    cu->RSTB(RSTB);
		cu->AB_mode(AB_mode);
		cu->pim_mode(pim_mode);
		cu->bank_addr(bank_addr);
		cu->row_addr(row_addr);
		cu->col_addr(col_addr);
		cu->DQ(DQ);
		cu->instr(instr);
		cu->pc_out(PC);
		cu->data_out(data_out);
		// CRF Control
		cu->crf_wr_en(crf_wr_en);
		cu->crf_wr_addr(crf_wr_addr);
		// SRF Control
		cu->srf_rd_addr(srf_rd_addr);
		cu->srf_rd_a_nm(srf_rd_a_nm);
		cu->srf_wr_en(srf_wr_en);
		cu->srf_wr_addr(srf_wr_addr);
		cu->srf_wr_a_nm(srf_wr_a_nm);
		cu->srf_wr_from(srf_wr_from);
		// GRF_A Control
		cu->grfa_rd_addr1(grfa_rd_addr1);
		cu->grfa_rd_addr2(grfa_rd_addr2);
		cu->grfa_wr_en(grfa_wr_en);
		cu->grfa_relu_en(grfa_relu_en);
		cu->grfa_wr_addr(grfa_wr_addr);
		cu->grfa_wr_from(grfa_wr_from);
		// GRF_B Control
		cu->grfb_rd_addr1(grfb_rd_addr1);
		cu->grfb_rd_addr2(grfb_rd_addr2);
		cu->grfb_wr_en(grfb_wr_en);
		cu->grfb_relu_en(grfb_relu_en);
		cu->grfb_wr_addr(grfb_wr_addr);
		cu->grfb_wr_from(grfb_wr_from);
		// FPU Control
		cu->fpu_mult_en(fpu_mult_en);
		cu->fpu_add_en(fpu_add_en);
		cu->fpu_mult_in1_sel(fpu_mult_in1_sel);
		cu->fpu_mult_in2_sel(fpu_mult_in2_sel);
		cu->fpu_add_in1_sel(fpu_add_in1_sel);
		cu->fpu_add_in2_sel(fpu_add_in2_sel);
		cu->fpu_out_sel(fpu_out_sel);
		// BANKS Control
		cu->even_out_en(even_out_en);
		cu->odd_out_en(odd_out_en);

		fpunit = new fpu("FPU");
		fpunit->clk(clk);
		fpunit->rst(rst);
		fpunit->mult_en(fpu_mult_en);
		fpunit->add_en(fpu_add_en);
		fpunit->srf_in(srf_out);
		fpunit->mult_in1_sel(fpu_mult_in1_sel);
		fpunit->mult_in2_sel(fpu_mult_in2_sel);
		fpunit->add_in1_sel(fpu_add_in1_sel);
		fpunit->add_in2_sel(fpu_add_in2_sel);
		fpunit->out_sel(fpu_out_sel);
		for (i=0; i<SIMD_WIDTH; i++) {
			fpunit->grfa_in1[i](grfa_out1[i]);
			fpunit->grfa_in2[i](grfa_out2[i]);
			fpunit->grfb_in1[i](grfb_out1[i]);
			fpunit->grfb_in2[i](grfb_out2[i]);
			fpunit->even_in[i](even2grfa[i]);
			fpunit->odd_in[i](odd2grfb[i]);
			fpunit->output[i](fpu_out[i]);
		}

		controlrf = new crf("CRF");
		controlrf->clk(clk);
		controlrf->rst(rst);
		controlrf->PC(PC);
		controlrf->instr(instr);
		controlrf->wr_en(crf_wr_en);
		controlrf->wr_addr(crf_wr_addr);
		controlrf->wr_port(ext2crf);

		grfa = new grf("GRF_A");
		grfa->clk(clk);
		grfa->rst(rst);
		grfa->rd_addr1(grfa_rd_addr1);
		grfa->rd_addr2(grfa_rd_addr2);
		grfa->wr_en(grfa_wr_en);
		grfa->relu_en(grfa_relu_en);
		grfa->wr_addr(grfa_wr_addr);
		grfa->wr_from(grfa_wr_from);
		for (i=0; i<SIMD_WIDTH; i++) {
			grfa->rd_port1[i](grfa_out1[i]);
			grfa->rd_port2[i](grfa_out2[i]);
			grfa->ext_in[i](ext2grf[i]);
			grfa->fpu_in[i](fpu_out[i]);
			grfa->srf_in[i](srf_out);
			grfa->grfa_in[i](grfa_out1[i]);
			grfa->grfb_in[i](grfb_out1[i]);
			grfa->bank_in[i](even2grfa[i]);
		}

		grfb = new grf("GRF_B");
		grfb->clk(clk);
		grfb->rst(rst);
		grfb->rd_addr1(grfb_rd_addr1);
		grfb->rd_addr2(grfb_rd_addr2);
		grfb->wr_en(grfb_wr_en);
		grfb->relu_en(grfb_relu_en);
		grfb->wr_addr(grfb_wr_addr);
		grfb->wr_from(grfb_wr_from);
		for (i=0; i<SIMD_WIDTH; i++) {
			grfb->rd_port1[i](grfb_out1[i]);
			grfb->rd_port2[i](grfb_out2[i]);
			grfb->ext_in[i](ext2grf[i]);
			grfb->fpu_in[i](fpu_out[i]);
			grfb->srf_in[i](srf_out);
			grfb->grfa_in[i](grfa_out1[i]);
			grfb->grfb_in[i](grfb_out1[i]);
			grfb->bank_in[i](odd2grfb[i]);
		}

		scalarrf = new srf("SRF");
		scalarrf->clk(clk);
		scalarrf->rst(rst);
		scalarrf->rd_addr(srf_rd_addr);
		scalarrf->rd_a_nm(srf_rd_a_nm);
		scalarrf->rd_port(srf_out);
		scalarrf->wr_en(srf_wr_en);
		scalarrf->wr_addr(srf_wr_addr);
		scalarrf->wr_a_nm(srf_wr_a_nm);
		scalarrf->wr_from(srf_wr_from);
		scalarrf->ext_in(ext2srf);
		scalarrf->srf_in(srf_out);
		scalarrf->grfa_in(grfa_out1[0]);
		scalarrf->grfb_in(grfb_out1[0]);

//		even_buf = new tristate_buffer<GRF_WIDTH>("Even_tristate_buffer");
//		even_buf->input(grfa2even);
//		even_buf->enable(even_out_en);
//		even_buf->output(even_bus);
//
//		odd_buf = new tristate_buffer<GRF_WIDTH>("Odd_tristate_buffer");
//		odd_buf->input(grfb2odd);
//		odd_buf->enable(odd_out_en);
//		odd_buf->output(odd_bus);

//		SC_THREAD(clk_thread);
//        sensitive << clk.pos();
//        async_reset_signal_is(rst, false);

		SC_METHOD(comb_method);
		sensitive << data_out << even_in << odd_in;
		for (i=0; i<SIMD_WIDTH; i++) {
			sensitive << grfa_out1[i] << grfb_out1[i];
		}
	}

#else

    sc_in_clk                   clk;
    sc_in<bool>                 rst;
    sc_in<bool>                 RD;			// DRAM read command
    sc_in<bool>                 WR;			// DRAM write command
    sc_in<bool>                 ACT;		// DRAM activate command
//    sc_in<bool>					RSTB;		//
    sc_in<bool>                 AB_mode;	// Signals if the All-Banks mode is enabled
    sc_in<bool>                 pim_mode;	// Signals if the PIM mode is enabled
    sc_in<sc_uint<BANK_BITS> >  bank_addr;	// Address of the bank
    sc_in<sc_uint<ROW_BITS> >   row_addr;	// Address of the bank row
    sc_in<sc_uint<COL_BITS> >   col_addr;	// Address of the bank column
    sc_in<sc_uint<DQ_BITS> >    DQ;	        // Data input from DRAM controller (output makes no sense)
    sc_inout_rv<GRF_WIDTH>      even_bus;	// Direct data in/out to the even bank
    sc_inout_rv<GRF_WIDTH>      odd_bus;	// Direct data in/out to the odd bank

    // ** INTERNAL SIGNALS AND VARIABLES **
    // Basic control
    sc_signal<uint8_t>                  PC;
    sc_signal<uint32_t>                 instr;
    sc_signal<sc_biguint<GRF_WIDTH> >   data_out;
    // CRF
    sc_signal<bool> crf_wr_en;
    sc_signal<uint> crf_wr_addr;
    // SRF
    sc_signal<uint>     srf_rd_addr, srf_wr_addr;
    sc_signal<bool>     srf_rd_a_nm, srf_wr_a_nm, srf_wr_en;
    sc_signal<uint8_t>  srf_wr_from;
    sc_signal<cnm_t>    srf_out;
    // GRF_A
    sc_signal<uint>     grfa_rd_addr1, grfa_rd_addr2, grfa_wr_addr;
    sc_signal<bool>     grfa_wr_en, grfa_relu_en;
    sc_signal<uint8_t>  grfa_wr_from;
    sc_signal<cnm_t>    grfa_out1[SIMD_WIDTH], grfa_out2[SIMD_WIDTH];
    // GRF_B
    sc_signal<uint>     grfb_rd_addr1, grfb_rd_addr2, grfb_wr_addr;
    sc_signal<bool>     grfb_wr_en, grfb_relu_en;
    sc_signal<uint8_t>  grfb_wr_from;
    sc_signal<cnm_t>    grfb_out1[SIMD_WIDTH], grfb_out2[SIMD_WIDTH];
    // FPU
    sc_signal<bool>     fpu_mult_en, fpu_add_en, fpu_out_sel;
    sc_signal<uint8_t>  fpu_mult_in1_sel, fpu_mult_in2_sel, fpu_add_in1_sel, fpu_add_in2_sel;
    sc_signal<cnm_t>    fpu_out[SIMD_WIDTH];
    // BANKS
    sc_signal<bool> even_out_en, odd_out_en;

    // Auxiliar signals
    sc_signal<uint32_t>             ext2crf;
    sc_signal<cnm_t>                ext2srf, ext2grf[SIMD_WIDTH];
    sc_signal<cnm_t>                even2grfa[SIMD_WIDTH], odd2grfb[SIMD_WIDTH];
    sc_signal<sc_lv<GRF_WIDTH> >    grfa2even, grfb2odd;

    // Internal modules
    control_unit *cu;
    fpu *fpunit;
    crf *controlrf;
    grf *grfa, *grfb;
    srf *scalarrf;
    tristate_buffer<GRF_WIDTH> *even_buf, *odd_buf;

    SC_HAS_PROCESS(imc_core);
    imc_core(sc_module_name name) : sc_module(name) {

        int i;

        cu = new control_unit("control_unit");
        cu->clk(clk);
        cu->rst(rst);
        cu->RD(RD);
        cu->WR(WR);
        cu->ACT(ACT);
//	    cu->RSTB(RSTB);
        cu->AB_mode(AB_mode);
        cu->pim_mode(pim_mode);
        cu->bank_addr(bank_addr);
        cu->row_addr(row_addr);
        cu->col_addr(col_addr);
        cu->DQ(DQ);
        cu->instr(instr);
        cu->pc_out(PC);
        cu->data_out(data_out);
        // CRF Control
        cu->crf_wr_en(crf_wr_en);
        cu->crf_wr_addr(crf_wr_addr);
        // SRF Control
        cu->srf_rd_addr(srf_rd_addr);
        cu->srf_rd_a_nm(srf_rd_a_nm);
        cu->srf_wr_en(srf_wr_en);
        cu->srf_wr_addr(srf_wr_addr);
        cu->srf_wr_a_nm(srf_wr_a_nm);
        cu->srf_wr_from(srf_wr_from);
        // GRF_A Control
        cu->grfa_rd_addr1(grfa_rd_addr1);
        cu->grfa_rd_addr2(grfa_rd_addr2);
        cu->grfa_wr_en(grfa_wr_en);
        cu->grfa_relu_en(grfa_relu_en);
        cu->grfa_wr_addr(grfa_wr_addr);
        cu->grfa_wr_from(grfa_wr_from);
        // GRF_B Control
        cu->grfb_rd_addr1(grfb_rd_addr1);
        cu->grfb_rd_addr2(grfb_rd_addr2);
        cu->grfb_wr_en(grfb_wr_en);
        cu->grfb_relu_en(grfb_relu_en);
        cu->grfb_wr_addr(grfb_wr_addr);
        cu->grfb_wr_from(grfb_wr_from);
        // FPU Control
        cu->fpu_mult_en(fpu_mult_en);
        cu->fpu_add_en(fpu_add_en);
        cu->fpu_mult_in1_sel(fpu_mult_in1_sel);
        cu->fpu_mult_in2_sel(fpu_mult_in2_sel);
        cu->fpu_add_in1_sel(fpu_add_in1_sel);
        cu->fpu_add_in2_sel(fpu_add_in2_sel);
        cu->fpu_out_sel(fpu_out_sel);
        // BANKS Control
        cu->even_out_en(even_out_en);
        cu->odd_out_en(odd_out_en);

        fpunit = new fpu("FPU");
        fpunit->clk(clk);
        fpunit->rst(rst);
        fpunit->mult_en(fpu_mult_en);
        fpunit->add_en(fpu_add_en);
        fpunit->srf_in(srf_out);
        fpunit->mult_in1_sel(fpu_mult_in1_sel);
        fpunit->mult_in2_sel(fpu_mult_in2_sel);
        fpunit->add_in1_sel(fpu_add_in1_sel);
        fpunit->add_in2_sel(fpu_add_in2_sel);
        fpunit->out_sel(fpu_out_sel);
        for (i = 0; i < SIMD_WIDTH; i++) {
            fpunit->grfa_in1[i](grfa_out1[i]);
            fpunit->grfa_in2[i](grfa_out2[i]);
            fpunit->grfb_in1[i](grfb_out1[i]);
            fpunit->grfb_in2[i](grfb_out2[i]);
            fpunit->even_in[i](even2grfa[i]);
            fpunit->odd_in[i](odd2grfb[i]);
            fpunit->output[i](fpu_out[i]);
        }

        controlrf = new crf("CRF");
        controlrf->clk(clk);
        controlrf->rst(rst);
        controlrf->PC(PC);
        controlrf->instr(instr);
        controlrf->wr_en(crf_wr_en);
        controlrf->wr_addr(crf_wr_addr);
        controlrf->wr_port(ext2crf);

        grfa = new grf("GRF_A");
        grfa->clk(clk);
        grfa->rst(rst);
        grfa->rd_addr1(grfa_rd_addr1);
        grfa->rd_addr2(grfa_rd_addr2);
        grfa->wr_en(grfa_wr_en);
        grfa->relu_en(grfa_relu_en);
        grfa->wr_addr(grfa_wr_addr);
        grfa->wr_from(grfa_wr_from);
        for (i = 0; i < SIMD_WIDTH; i++) {
            grfa->rd_port1[i](grfa_out1[i]);
            grfa->rd_port2[i](grfa_out2[i]);
            grfa->ext_in[i](ext2grf[i]);
            grfa->fpu_in[i](fpu_out[i]);
            grfa->srf_in[i](srf_out);
            grfa->grfa_in[i](grfa_out1[i]);
            grfa->grfb_in[i](grfb_out1[i]);
            grfa->bank_in[i](even2grfa[i]);
        }

        grfb = new grf("GRF_B");
        grfb->clk(clk);
        grfb->rst(rst);
        grfb->rd_addr1(grfb_rd_addr1);
        grfb->rd_addr2(grfb_rd_addr2);
        grfb->wr_en(grfb_wr_en);
        grfb->relu_en(grfb_relu_en);
        grfb->wr_addr(grfb_wr_addr);
        grfb->wr_from(grfb_wr_from);
        for (i = 0; i < SIMD_WIDTH; i++) {
            grfb->rd_port1[i](grfb_out1[i]);
            grfb->rd_port2[i](grfb_out2[i]);
            grfb->ext_in[i](ext2grf[i]);
            grfb->fpu_in[i](fpu_out[i]);
            grfb->srf_in[i](srf_out);
            grfb->grfa_in[i](grfa_out1[i]);
            grfb->grfb_in[i](grfb_out1[i]);
            grfb->bank_in[i](odd2grfb[i]);
        }

        scalarrf = new srf("SRF");
        scalarrf->clk(clk);
        scalarrf->rst(rst);
        scalarrf->rd_addr(srf_rd_addr);
        scalarrf->rd_a_nm(srf_rd_a_nm);
        scalarrf->rd_port(srf_out);
        scalarrf->wr_en(srf_wr_en);
        scalarrf->wr_addr(srf_wr_addr);
        scalarrf->wr_a_nm(srf_wr_a_nm);
        scalarrf->wr_from(srf_wr_from);
        scalarrf->ext_in(ext2srf);
        scalarrf->srf_in(srf_out);
        scalarrf->grfa_in(grfa_out1[0]);
        scalarrf->grfb_in(grfb_out1[0]);

        even_buf = new tristate_buffer<GRF_WIDTH>("Even_tristate_buffer");
        even_buf->input(grfa2even);
        even_buf->enable(even_out_en);
        even_buf->output(even_bus);

        odd_buf = new tristate_buffer<GRF_WIDTH>("Odd_tristate_buffer");
        odd_buf->input(grfb2odd);
        odd_buf->enable(odd_out_en);
        odd_buf->output(odd_bus);

        SC_METHOD(comb_method);
        sensitive << data_out << even_bus << odd_bus;
        for (i = 0; i < SIMD_WIDTH; i++) {
            sensitive << grfa_out1[i] << grfb_out1[i];
        }
    }

#endif
#if __SYNTHESIS__
	void clk_thread();	// Registers the input signals
#endif
    void comb_method();
};

#endif /* IMC_CORE_H_ */
