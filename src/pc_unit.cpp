/*
 * Copyright EPFL 2021
 * Rafael Medina Morillas
 *
 * Implementation of the Program Counter Unit.
 *
 */

#include "pc_unit.h"

void pc_unit::clk_thread() {
    // Reset
    pc_reg = 0;

    wait();

    // Clocked behaviour
    while (1) {
        pc_reg = pc_nxt;

        wait();
    }
}

void pc_unit::comb_method() {
    pc_nxt = pc_reg;

    if (pc_rst) {
        pc_nxt = 0;
    } else if (jump_en) {
        pc_nxt = pc_reg - jump_num->read();
    } else if (count_en) {
        if (pc_reg < CRF_ENTRIES - 1)
            pc_nxt = pc_reg + 1;
        else
            pc_nxt = 0;
    }

    pc_out->write(pc_reg);
}
