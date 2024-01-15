#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <random>

#include "../src/defs.h"

using namespace std;

// Definition of the address mapping
enum class Level : int {Channel, Rank, BankGroup, Bank, Row, Column, MAX};
string level_str[int(Level::MAX)] = {"Ch", "Ra", "Bg", "Ba", "Ro", "Co"};
int addr_bits[int(Level::MAX)] = {CHANNEL_BITS, RANK_BITS, BG_BITS, BANK_BITS, ROW_BITS, COL_BITS};
Level map[int(Level::MAX)] = {Level::Channel, Level::Column, Level::Rank,
                        Level::BankGroup, Level::Bank, Level::Row};
int global_offset = GLOBAL_OFFSET;

long unsigned int build_addr(vector<uint> addr_vec);

int main(int argc, const char *argv[])
{
    // Randomization tools
    std::mt19937 gen(11111);    // Standard mersenne_twister_engine seeded
    std::uniform_int_distribution<> dis_bool(0, 1);
    std::uniform_int_distribution<> dis_channel(0, (1 << addr_bits[int(Level::Channel)])-1);
    std::uniform_int_distribution<> dis_rank(0, (1 << addr_bits[int(Level::Rank)])-1);
    std::uniform_int_distribution<> dis_bg(0, (1 << addr_bits[int(Level::BankGroup)])-1);
    std::uniform_int_distribution<> dis_bank(0, (1 << addr_bits[int(Level::Bank)])-1);
    std::uniform_int_distribution<> dis_row(0, (1 << addr_bits[int(Level::Row)])-1);
    std::uniform_int_distribution<> dis_column(0, (1 << addr_bits[int(Level::Column)])-1);

    uint chan, rank, bg, bank, row, col;
    chan = rank = bg = bank = row = col = 0;
    string cmd = "RD";
    
    ofstream output, addresses;
    output.open("debug.trace");
    addresses.open("debug.addresses");

    output << showbase << internal << setfill ('0') << hex;
    addresses << showbase;

    // Check that the same row is kept open in all BGs and banks
    addresses << "Check that the same row is kept open in all BGs and banks" << endl;
    row = dis_row(gen);
    for (bg = 0; bg < 4; bg++)
        for (bank = 0; bank < 4; bank++) {
            col = dis_column(gen);
            output << build_addr({chan, rank, bg, bank, row, col}) << "\t";
            output << (dis_bool(gen) ? "WR" : "RD") << endl;
            addresses << hex << build_addr({chan, rank, bg, bank, row, col}) << "\t";
            addresses << dec << chan << " " << rank << " " << bg << " " << bank << " " << row << " " << col << endl;
        }

    // What if we access all columns in a row, varying BG and bank?
    addresses << "What if we access all columns in a row, varying BG and bank?" << endl;
    row = dis_row(gen);
    for (col = 0; col < 1<<addr_bits[int(Level::Column)]; col++) {
        bg = dis_bg(gen);
        bank = dis_bank(gen);
        output << build_addr({chan, rank, bg, bank, row, col}) << "\t";
        output << (dis_bool(gen) ? "WR" : "RD") << endl;
        addresses << hex << build_addr({chan, rank, bg, bank, row, col}) << "\t";
        addresses << dec << chan << " " << rank << " " << bg << " " << bank << " " << row << " " << col << endl;
    }

    // What if same col and row, varying the rest?
    addresses << "What if same col and row, varying the rest?" << endl;
    row = dis_row(gen);
    col = dis_column(gen);
    for (int i = 0; i < 16; i++) {
        bg = dis_bg(gen);
        bank = dis_bank(gen);
        output << build_addr({chan, rank, bg, bank, row, col}) << "\t";
        output << (dis_bool(gen) ? "WR" : "RD") << endl;
        addresses << hex << build_addr({chan, rank, bg, bank, row, col}) << "\t";
        addresses << dec << chan << " " << rank << " " << bg << " " << bank << " " << row << " " << col << endl;
    }

    // Check that reads and writes to the same address don't cause hazards
    addresses << "Check that reads and writes to the same address don't cause hazards" << endl;
    bg = dis_bg(gen);
    bank = dis_bank(gen);
    row = dis_row(gen);
    col = dis_column(gen);
    cout << chan << " " << rank << " " << bg << " " << bank << " " << row << " " << col << endl;
    for (int i = 0; i < 16; i++) {
        output << build_addr({chan, rank, bg, bank, row, col}) << "\t";
        output << (i%2 ? "RD" : "WR") << endl;
        addresses << hex << build_addr({chan, rank, bg, bank, row, col}) << "\t";
        addresses << dec << chan << " " << rank << " " << bg << " " << bank << " " << row << " " << col << endl;
    }

    // Check that access to diff rows in the same bank and bank group does ACT-RD/WR-PRE
    addresses << "Check that access to diff rows in the same bank and bank group does ACT-RD/WR-PRE" << endl;
    bank = dis_bank(gen);
    bg = dis_bg(gen);
    for (int i = 0; i < 16; i++) {
        row = dis_row(gen);
        col = dis_column(gen);
        output << build_addr({chan, rank, bg, bank, row, col}) << "\t";
        output << (dis_bool(gen) ? "WR" : "RD") << endl;
        addresses << hex << build_addr({chan, rank, bg, bank, row, col}) << "\t";
        addresses << dec << chan << " " << rank << " " << bg << " " << bank << " " << row << " " << col << endl;
    }

    // Check that access to diff rows in the same bank group, diff banks does ACT-RD/WR-PRE 
    addresses << "Check that access to diff rows in the same bank group, diff banks does ACT-RD/WR-PRE" << endl;
    bg = dis_bg(gen);
    for (int i = 0; i < 16; i++) {
        bank = dis_bank(gen);
        row = dis_row(gen);
        col = dis_column(gen);
        output << build_addr({chan, rank, bg, bank, row, col}) << "\t";
        output << (dis_bool(gen) ? "WR" : "RD") << endl;
        addresses << hex << build_addr({chan, rank, bg, bank, row, col}) << "\t";
        addresses << dec << chan << " " << rank << " " << bg << " " << bank << " " << row << " " << col << endl;
    }

    // Check that access to diff rows in diff bank groups, same bank does ACT-RD/WR-PRE
    addresses << "Check that access to diff rows in diff bank groups, same bank does ACT-RD/WR-PRE" << endl;
    bank = dis_bank(gen);
    for (int i = 0; i < 16; i++) {
        bg = dis_bg(gen);
        row = dis_row(gen);
        col = dis_column(gen);
        output << build_addr({chan, rank, bg, bank, row, col}) << "\t";
        output << (dis_bool(gen) ? "WR" : "RD") << endl;
        addresses << hex << build_addr({chan, rank, bg, bank, row, col}) << "\t";
        addresses << dec << chan << " " << rank << " " << bg << " " << bank << " " << row << " " << col << endl;
    }

    // Check that access to diff rows in diff bgs and banks does ACT-RD/WR-PRE
    addresses << "Check that access to diff rows in diff bgs and banks does ACT-RD/WR-PRE" << endl;
    for (int i = 0; i < 16; i++) {
        bg = dis_bg(gen);
        bank = dis_bank(gen);
        row = dis_row(gen);
        col = dis_column(gen);
        output << build_addr({chan, rank, bg, bank, row, col}) << "\t";
        output << (dis_bool(gen) ? "WR" : "RD") << endl;
        addresses << hex << build_addr({chan, rank, bg, bank, row, col}) << "\t";
        addresses << dec << chan << " " << rank << " " << bg << " " << bank << " " << row << " " << col << endl;
    }

    // Check that the same row is kept open in all BGs and banks, but not in all channels
    addresses << "Check that the same row is kept open in all BGs and banks, but not in all channels" << endl;
    row = dis_row(gen);
    for (int i = 0; i < 64; i++) {
        chan = dis_channel(gen);
        bg = dis_bg(gen);
        bank = dis_bank(gen);
        col = dis_column(gen);
        output << build_addr({chan, rank, bg, bank, row, col}) << "\t";
        output << (dis_bool(gen) ? "WR" : "RD") << endl;
        addresses << hex << build_addr({chan, rank, bg, bank, row, col}) << "\t";
        addresses << dec << chan << " " << rank << " " << bg << " " << bank << " " << row << " " << col << endl;
    }

    output.close();
    return 0;
}

// Function for building the address using the indices of the different levels
long unsigned int build_addr(vector<uint> addr_vec)
{
    long unsigned int addr_aux = 0;
    uint offset = 6; // Don't know really why right now, maybe to address groups of 128

    for (int i = 0; i < int(Level::MAX); i++) {
        if (addr_bits[int(map[i])]) {
            addr_aux |= ((long uint) addr_vec[int(map[i])] << offset);
            offset += addr_bits[int(map[i])];
        }
    }

    return addr_aux;
}