#include "utils.h"

uint64_t build_addr(vector<uint64_t> addr_vec)
{
    // Definition of the address mapping
    enum class Level : int {Channel, Rank, BankGroup, Bank, Row, Column, MAX};
    string level_str[int(Level::MAX)] = {"Ch", "Ra", "Bg", "Ba", "Ro", "Co"};
    int addr_bits[int(Level::MAX)] = {CHANNEL_BITS, RANK_BITS, BG_BITS, BANK_BITS, ROW_BITS, COL_BITS};
    Level addr_map[int(Level::MAX)] = {Level::Channel, Level::Column, Level::Rank,
                                       Level::BankGroup, Level::Bank, Level::Row};
    int global_offset = GLOBAL_OFFSET;

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
    // Definition of the address mapping
    enum class Level : int {Channel, Rank, BankGroup, Bank, Row, Column, MAX};
    string level_str[int(Level::MAX)] = {"Ch", "Ra", "Bg", "Ba", "Ro", "Co"};
    int addr_bits[int(Level::MAX)] = {CHANNEL_BITS, RANK_BITS, BG_BITS, BANK_BITS, ROW_BITS, COL_BITS};
    Level addr_map[int(Level::MAX)] = {Level::Channel, Level::Column, Level::Rank,
                                       Level::BankGroup, Level::Bank, Level::Row};
    int global_offset = GLOBAL_OFFSET;

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
    // Definition of the address mapping
    enum class Level : int {Channel, Rank, BankGroup, Bank, Row, Column, MAX};
    string level_str[int(Level::MAX)] = {"Ch", "Ra", "Bg", "Ba", "Ro", "Co"};
    int addr_bits[int(Level::MAX)] = {CHANNEL_BITS, RANK_BITS, BG_BITS, BANK_BITS, ROW_BITS, COL_BITS};
    Level addr_map[int(Level::MAX)] = {Level::Channel, Level::Column, Level::Rank,
                                       Level::BankGroup, Level::Bank, Level::Row};
    int global_offset = GLOBAL_OFFSET;

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
    // Definition of the address mapping
    enum class Level : int {Channel, Rank, BankGroup, Bank, Row, Column, MAX};
    string level_str[int(Level::MAX)] = {"Ch", "Ra", "Bg", "Ba", "Ro", "Co"};
    int addr_bits[int(Level::MAX)] = {CHANNEL_BITS, RANK_BITS, BG_BITS, BANK_BITS, ROW_BITS, COL_BITS};
    Level addr_map[int(Level::MAX)] = {Level::Channel, Level::Column, Level::Rank,
                                       Level::BankGroup, Level::Bank, Level::Row};
    int global_offset = GLOBAL_OFFSET;
    
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
