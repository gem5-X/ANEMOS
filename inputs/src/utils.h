#ifndef UTILS_H
#define UTILS_H

#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <random>

#include "half.hpp"
#include "datatypes.h"
#include "../../src/defs.h"
#include "../../src/opcodes.h"

using namespace std;

// Definition of the address mapping
enum class Level : int {Channel, Rank, BankGroup, Bank, Row, Column, MAX};
const string level_str[int(Level::MAX)] = {"Ch", "Ra", "Bg", "Ba", "Ro", "Co"};
const int addr_bits[int(Level::MAX)] = {CHANNEL_BITS, RANK_BITS, BG_BITS, BANK_BITS, ROW_BITS, COL_BITS};
const Level addr_map[int(Level::MAX)] = {Level::Channel, Level::Column, Level::Rank,
                        Level::BankGroup, Level::Bank, Level::Row};
const int global_offset = GLOBAL_OFFSET;

#define COLPERROW   (1 << addr_bits[int(Level::Column)])

// Function for building addresses
uint64_t build_addr(vector<uint64_t> addr_vec);

// Function for getting the bank index from an address
uint8_t get_bank(uint64_t addr);

// Function for getting the row index from an address
uint16_t get_row(uint64_t addr);

// Function for getting the column index from an address
uint16_t get_col(uint64_t addr);

#endif  // UTILS_H