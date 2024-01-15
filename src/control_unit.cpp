/*
 * Copyright EPFL 2021
 * Rafael Medina Morillas
 *
 * Implementation of the Control Unit.
 *
 */

#include "control_unit.h"

void control_unit::comb_method() {
    pc_out->write(pc);
}
