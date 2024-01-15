#include "HBM2_AB.h"
#include "DRAM.h"

#include <vector>
#include <functional>
#include <cassert>

using namespace std;
using namespace ramulator;

string HBM2_AB::standard_name = "HBM2_AB";
string HBM2_AB::level_str [int(Level::MAX)] = {"Ch", "Ra", "Bg", "Ba", "Ro", "Co"};

map<string, enum HBM2_AB::Org> HBM2_AB::org_map = {
    {"HBM2_1Gb", HBM2_AB::Org::HBM2_1Gb},
    {"HBM2_2Gb", HBM2_AB::Org::HBM2_2Gb},
    {"HBM2_4Gb", HBM2_AB::Org::HBM2_4Gb},
    {"HBM2_8Gb", HBM2_AB::Org::HBM2_8Gb},
    {"HBM2_16Gb", HBM2_AB::Org::HBM2_16Gb},
};

map<string, enum HBM2_AB::Speed> HBM2_AB::speed_map = {
    {"HBM2_2Gbps", HBM2_AB::Speed::HBM2_2Gbps},
    {"HBM2_300MHz", HBM2_AB::Speed::HBM2_300MHz},
};

HBM2_AB::HBM2_AB(Org org, Speed speed)
    : org_entry(org_table[int(org)]),
    speed_entry(speed_table[int(speed)]),
    read_latency(speed_entry.nCL + speed_entry.nBL)
{
    init_speed();
    init_prereq();
    init_rowhit(); // SAUGATA: added row hit function
    init_rowopen();
    init_lambda();
    init_timing();
}

HBM2_AB::HBM2_AB(const string& org_str, const string& speed_str) :
    HBM2_AB(org_map[org_str], speed_map[speed_str])
{
}

void HBM2_AB::set_channel_number(int channel) {
  org_entry.count[int(Level::Channel)] = channel;
}

void HBM2_AB::set_rank_number(int rank) {
  org_entry.count[int(Level::Rank)] = rank;
}


void HBM2_AB::init_speed()   // @NOTE Changed for 300 MHz
{
    const static int RFC_TABLE[int(Speed::MAX)][int(Org::MAX)] = {
        {72, 110, 170, 260, 400},
        {22, 33, 51, 78, 120}
    };
    const static int REFI1B_TABLE[int(Speed::MAX)][int(Org::MAX)] = {
        {16, 32, 64, 128, 256},
        {8, 16, 32, 64, 128}
    };
    const static int XS_TABLE[int(Speed::MAX)][int(Org::MAX)] = {
        {80, 120, 179, 268, 402},
        {24, 36, 54, 80, 120}
    };

    int speed = 0, density = 0;
    switch (speed_entry.rate) {
        case 2000: speed = 0; break;
        case 2400: speed = 1; break;
        default: assert(false);
    };
    switch (org_entry.size >> 10){
        case 1: density = 0; break;
        case 2: density = 1; break;
        case 4: density = 2; break;
        case 8: density = 3; break;
        case 16: density = 4; break;
        default: assert(false);
    }
    speed_entry.nRFC = RFC_TABLE[speed][density];
    speed_entry.nREFI1B = REFI1B_TABLE[speed][density];
    speed_entry.nXS = XS_TABLE[speed][density];
}


void HBM2_AB::init_prereq()  // @NOTE to access rows of rest of banks, we could keep the row id and run through the sibling banks and BGs
{                           // However, if we update correctly, this should not be needed
    // RD
    prereq[int(Level::Rank)][int(Command::RD)] = [] (DRAM<HBM2_AB>* node, Command cmd, int id) {
        switch (int(node->state)) {
            case int(State::PowerUp): return Command::MAX;
            case int(State::ActPowerDown): return Command::PDX;
            case int(State::PrePowerDown): return Command::PDX;
            case int(State::SelfRefresh): return Command::SRX;
            default: assert(false);
        }};
    prereq[int(Level::Bank)][int(Command::RD)] = [] (DRAM<HBM2_AB>* node, Command cmd, int id) {
        switch (int(node->state)) {
            case int(State::Closed): return Command::ACT;
            case int(State::Opened):
                if (node->row_state.find(id) != node->row_state.end())
                    return cmd;
                else return Command::PRE;
            default: assert(false);
        }};

    // WR
    prereq[int(Level::Rank)][int(Command::WR)] = prereq[int(Level::Rank)][int(Command::RD)];
    prereq[int(Level::Bank)][int(Command::WR)] = prereq[int(Level::Bank)][int(Command::RD)];

    // REF
    prereq[int(Level::Rank)][int(Command::REF)] = [] (DRAM<HBM2_AB>* node, Command cmd, int id) {
        for (auto bg : node->children)
            for (auto bank: bg->children) {
                if (bank->state == State::Closed)
                    continue;
                return Command::PREA;
            }
        return Command::REF;};

    // REFSB
    prereq[int(Level::Bank)][int(Command::REFSB)] = [] (DRAM<HBM2_AB>* node, Command cmd, int id) {
        if (node->state == State::Closed) return Command::REFSB;
        return Command::PRE;};

    // PD
    prereq[int(Level::Rank)][int(Command::PDE)] = [] (DRAM<HBM2_AB>* node, Command cmd, int id) {
        switch (int(node->state)) {
            case int(State::PowerUp): return Command::PDE;
            case int(State::ActPowerDown): return Command::PDE;
            case int(State::PrePowerDown): return Command::PDE;
            case int(State::SelfRefresh): return Command::SRX;
            default: assert(false);
        }};

    // SR
    prereq[int(Level::Rank)][int(Command::SRE)] = [] (DRAM<HBM2_AB>* node, Command cmd, int id) {
        switch (int(node->state)) {
            case int(State::PowerUp): return Command::SRE;
            case int(State::ActPowerDown): return Command::PDX;
            case int(State::PrePowerDown): return Command::PDX;
            case int(State::SelfRefresh): return Command::SRE;
            default: assert(false);
        }};
}

// SAUGATA: added row hit check functions to see if the desired location is currently open
void HBM2_AB::init_rowhit()  // @NOTE to access rows of rest of banks, we could keep the row id and run through the sibling banks and BGs
{                           // However, if we update correctly, this should not be needed
    // RD
    rowhit[int(Level::Bank)][int(Command::RD)] = [] (DRAM<HBM2_AB>* node, Command cmd, int id) {
        switch (int(node->state)) {
            case int(State::Closed): return false;
            case int(State::Opened):
                if (node->row_state.find(id) != node->row_state.end())
                    return true;
                return false;
            default: assert(false);
        }};

    // WR
    rowhit[int(Level::Bank)][int(Command::WR)] = rowhit[int(Level::Bank)][int(Command::RD)];
}

void HBM2_AB::init_rowopen() // @NOTE to access rows of rest of banks, we could keep the row id and run through the sibling banks and BGs
{                           // However, if we update correctly, this should not be needed
    // RD
    rowopen[int(Level::Bank)][int(Command::RD)] = [] (DRAM<HBM2_AB>* node, Command cmd, int id) {
        switch (int(node->state)) {
            case int(State::Closed): return false;
            case int(State::Opened): return true;
            default: assert(false);
        }};

    // WR
    rowopen[int(Level::Bank)][int(Command::WR)] = rowopen[int(Level::Bank)][int(Command::RD)];
}

void HBM2_AB::init_lambda()  // @NOTE to access rows of rest of banks, we keep the row id and run through the sibling banks and BGs
{
    lambda[int(Level::Bank)][int(Command::ACT)] = [] (DRAM<HBM2_AB>* node, int id) {
        for (auto bg : node->parent->parent->children)  // Open row at all banks at every BG
            for (auto bank: bg->children) {
                bank->state = State::Opened;
                bank->row_state[id] = State::Opened;}};
    lambda[int(Level::Bank)][int(Command::PRE)] = [] (DRAM<HBM2_AB>* node, int id) {
        for (auto bg : node->parent->parent->children)  // Close row at all banks at every BG
            for (auto bank: bg->children) {
                bank->state = State::Closed;
                bank->row_state.clear();}};
    lambda[int(Level::Rank)][int(Command::PREA)] = [] (DRAM<HBM2_AB>* node, int id) {
        for (auto bg : node->children)
            for (auto bank : bg->children) {
                bank->state = State::Closed;
                bank->row_state.clear();
            }};
    lambda[int(Level::Rank)][int(Command::REF)] = [] (DRAM<HBM2_AB>* node, int id) {};
    lambda[int(Level::Bank)][int(Command::RD)] = [] (DRAM<HBM2_AB>* node, int id) {};
    lambda[int(Level::Bank)][int(Command::WR)] = [] (DRAM<HBM2_AB>* node, int id) {};
    lambda[int(Level::Bank)][int(Command::RDA)] = [] (DRAM<HBM2_AB>* node, int id) {
        for (auto bg : node->parent->parent->children)  // Close row at all banks at every BG
            for (auto bank: bg->children) {
                bank->state = State::Closed;
                bank->row_state.clear();}};
    lambda[int(Level::Bank)][int(Command::WRA)] = [] (DRAM<HBM2_AB>* node, int id) {
        for (auto bg : node->parent->parent->children)  // Close row at all banks at every BG
            for (auto bank: bg->children) {
                bank->state = State::Closed;
                bank->row_state.clear();}};
    lambda[int(Level::Rank)][int(Command::PDE)] = [] (DRAM<HBM2_AB>* node, int id) {
        for (auto bg : node->children)
            for (auto bank : bg->children) {
                if (bank->state == State::Closed)
                    continue;
                node->state = State::ActPowerDown;
                return;
            }
        node->state = State::PrePowerDown;};
    lambda[int(Level::Rank)][int(Command::PDX)] = [] (DRAM<HBM2_AB>* node, int id) {
        node->state = State::PowerUp;};
    lambda[int(Level::Rank)][int(Command::SRE)] = [] (DRAM<HBM2_AB>* node, int id) {
        node->state = State::SelfRefresh;};
    lambda[int(Level::Rank)][int(Command::SRX)] = [] (DRAM<HBM2_AB>* node, int id) {
        node->state = State::PowerUp;};
}


void HBM2_AB::init_timing()  // @NOTE all banks of all BGs in a rank are accessed, so timings of BGs and banks at the rank level
{
    SpeedEntry& s = speed_entry;
    vector<TimingEntry> *t;

    /*** Channel ***/
    t = timing[int(Level::Channel)];

    // CAS <-> CAS
    t[int(Command::RD)].push_back({Command::RD, 1, s.nBL});
    t[int(Command::RD)].push_back({Command::RDA, 1, s.nBL});
    t[int(Command::RDA)].push_back({Command::RD, 1, s.nBL});
    t[int(Command::RDA)].push_back({Command::RDA, 1, s.nBL});
    t[int(Command::WR)].push_back({Command::WR, 1, s.nBL});
    t[int(Command::WR)].push_back({Command::WRA, 1, s.nBL});
    t[int(Command::WRA)].push_back({Command::WR, 1, s.nBL});
    t[int(Command::WRA)].push_back({Command::WRA, 1, s.nBL});


    /*** Rank ***/
    t = timing[int(Level::Rank)];

    // CAS <-> CAS  // In AllBanks mode, we never have nCCDS or nWTRS, only nCCDL and nWTRL
    t[int(Command::RD)].push_back({Command::RD, 1, s.nCCDL});
    t[int(Command::RD)].push_back({Command::RDA, 1, s.nCCDL});
    t[int(Command::RDA)].push_back({Command::RD, 1, s.nCCDL});
    t[int(Command::RDA)].push_back({Command::RDA, 1, s.nCCDL});
    t[int(Command::WR)].push_back({Command::WR, 1, s.nCCDL});
    t[int(Command::WR)].push_back({Command::WRA, 1, s.nCCDL});
    t[int(Command::WRA)].push_back({Command::WR, 1, s.nCCDL});
    t[int(Command::WRA)].push_back({Command::WRA, 1, s.nCCDL});
    t[int(Command::RD)].push_back({Command::WR, 1, s.nCL + s.nCCDL + 2 - s.nCWL});
    t[int(Command::RD)].push_back({Command::WRA, 1, s.nCL + s.nCCDL + 2 - s.nCWL});
    t[int(Command::RDA)].push_back({Command::WR, 1, s.nCL + s.nCCDL + 2 - s.nCWL});
    t[int(Command::RDA)].push_back({Command::WRA, 1, s.nCL + s.nCCDL + 2 - s.nCWL});
    t[int(Command::WR)].push_back({Command::RD, 1, s.nCWL + s.nBL + s.nWTRL});
    t[int(Command::WR)].push_back({Command::RDA, 1, s.nCWL + s.nBL + s.nWTRL});
    t[int(Command::WRA)].push_back({Command::RD, 1, s.nCWL + s.nBL + s.nWTRL});
    t[int(Command::WRA)].push_back({Command::RDA, 1, s.nCWL + s.nBL + s.nWTRL});

    t[int(Command::RD)].push_back({Command::PREA, 1, s.nRTP});
    t[int(Command::WR)].push_back({Command::PREA, 1, s.nCWL + s.nBL + s.nWR});

    // CAS <-> RAS  // Upgraded from Bank level to Rank level
    t[int(Command::ACT)].push_back({Command::RD, 1, s.nRCDR});
    t[int(Command::ACT)].push_back({Command::RDA, 1, s.nRCDR});
    t[int(Command::ACT)].push_back({Command::WR, 1, s.nRCDW});
    t[int(Command::ACT)].push_back({Command::WRA, 1, s.nRCDW});

    t[int(Command::RD)].push_back({Command::PRE, 1, s.nRTP});
    t[int(Command::WR)].push_back({Command::PRE, 1, s.nCWL + s.nBL + s.nWR});

    t[int(Command::RDA)].push_back({Command::ACT, 1, s.nRTP + s.nRP});
    t[int(Command::WRA)].push_back({Command::ACT, 1, s.nCWL + s.nBL + s.nWR + s.nRP});

    // CAS <-> PD
    t[int(Command::RD)].push_back({Command::PDE, 1, s.nCL + s.nBL + 1});
    t[int(Command::RDA)].push_back({Command::PDE, 1, s.nCL + s.nBL + 1});
    t[int(Command::WR)].push_back({Command::PDE, 1, s.nCWL + s.nBL + s.nWR});
    t[int(Command::WRA)].push_back({Command::PDE, 1, s.nCWL + s.nBL + s.nWR + 1}); // +1 for pre
    t[int(Command::PDX)].push_back({Command::RD, 1, s.nXP});
    t[int(Command::PDX)].push_back({Command::RDA, 1, s.nXP});
    t[int(Command::PDX)].push_back({Command::WR, 1, s.nXP});
    t[int(Command::PDX)].push_back({Command::WRA, 1, s.nXP});

    // CAS <-> SR: none (all banks have to be precharged)

    // RAS <-> RAS  // In AllBanks mode, we never have nRRDS, only nRRDL
                    // Some upgrades from Bank level to Rank level
    t[int(Command::ACT)].push_back({Command::ACT, 1, s.nRRDL});
    t[int(Command::ACT)].push_back({Command::ACT, 4, s.nFAW});
    t[int(Command::ACT)].push_back({Command::ACT, 1, s.nRC});
    t[int(Command::ACT)].push_back({Command::PREA, 1, s.nRAS});
    t[int(Command::ACT)].push_back({Command::PRE, 1, s.nRAS});
    t[int(Command::PREA)].push_back({Command::ACT, 1, s.nRP});
    t[int(Command::PRE)].push_back({Command::ACT, 1, s.nRP});

    // RAS <-> REF
    t[int(Command::PRE)].push_back({Command::REF, 1, s.nRP});
    t[int(Command::PREA)].push_back({Command::REF, 1, s.nRP});
    t[int(Command::REF)].push_back({Command::ACT, 1, s.nRFC});

    // RAS <-> PD
    t[int(Command::ACT)].push_back({Command::PDE, 1, 1});
    t[int(Command::PDX)].push_back({Command::ACT, 1, s.nXP});
    t[int(Command::PDX)].push_back({Command::PRE, 1, s.nXP});
    t[int(Command::PDX)].push_back({Command::PREA, 1, s.nXP});

    // RAS <-> SR
    t[int(Command::PRE)].push_back({Command::SRE, 1, s.nRP});
    t[int(Command::PREA)].push_back({Command::SRE, 1, s.nRP});
    t[int(Command::SRX)].push_back({Command::ACT, 1, s.nXS});

    // REF <-> REF
    t[int(Command::REF)].push_back({Command::REF, 1, s.nRFC});

    // REF <-> PD
    t[int(Command::REF)].push_back({Command::PDE, 1, 1});
    t[int(Command::PDX)].push_back({Command::REF, 1, s.nXP});

    // REF <-> SR
    t[int(Command::SRX)].push_back({Command::REF, 1, s.nXS});

    // REFSB    // Upgraded from Bank level to Rank level
    t[int(Command::PRE)].push_back({Command::REFSB, 1, s.nRP});
    t[int(Command::REFSB)].push_back({Command::REFSB, 1, s.nRFC});
    t[int(Command::REFSB)].push_back({Command::ACT, 1, s.nRFC});

    // PD <-> PD
    t[int(Command::PDE)].push_back({Command::PDX, 1, s.nPD});
    t[int(Command::PDX)].push_back({Command::PDE, 1, s.nXP});

    // PD <-> SR
    t[int(Command::PDX)].push_back({Command::SRE, 1, s.nXP});
    t[int(Command::SRX)].push_back({Command::PDE, 1, s.nXS});

    // SR <-> SR
    t[int(Command::SRE)].push_back({Command::SRX, 1, s.nCKESR});
    t[int(Command::SRX)].push_back({Command::SRE, 1, s.nXS});

    // /*** Bank Group ***/
    // t = timing[int(Level::BankGroup)];
    // // CAS <-> CAS
    // t[int(Command::RD)].push_back({Command::RD, 1, s.nCCDL});
    // t[int(Command::RD)].push_back({Command::RDA, 1, s.nCCDL});
    // t[int(Command::RDA)].push_back({Command::RD, 1, s.nCCDL});
    // t[int(Command::RDA)].push_back({Command::RDA, 1, s.nCCDL});
    // t[int(Command::WR)].push_back({Command::WR, 1, s.nCCDL});
    // t[int(Command::WR)].push_back({Command::WRA, 1, s.nCCDL});
    // t[int(Command::WRA)].push_back({Command::WR, 1, s.nCCDL});
    // t[int(Command::WRA)].push_back({Command::WRA, 1, s.nCCDL});
    // t[int(Command::WR)].push_back({Command::WR, 1, s.nCCDL});
    // t[int(Command::WR)].push_back({Command::WRA, 1, s.nCCDL});
    // t[int(Command::WRA)].push_back({Command::WR, 1, s.nCCDL});
    // t[int(Command::WRA)].push_back({Command::WRA, 1, s.nCCDL});
    // t[int(Command::WR)].push_back({Command::RD, 1, s.nCWL + s.nBL + s.nWTRL});
    // t[int(Command::WR)].push_back({Command::RDA, 1, s.nCWL + s.nBL + s.nWTRL});
    // t[int(Command::WRA)].push_back({Command::RD, 1, s.nCWL + s.nBL + s.nWTRL});
    // t[int(Command::WRA)].push_back({Command::RDA, 1, s.nCWL + s.nBL + s.nWTRL});

    // // RAS <-> RAS
    // t[int(Command::ACT)].push_back({Command::ACT, 1, s.nRRDL});

    // /*** Bank ***/
    // t = timing[int(Level::Bank)];

    // // CAS <-> RAS
    // t[int(Command::ACT)].push_back({Command::RD, 1, s.nRCDR});
    // t[int(Command::ACT)].push_back({Command::RDA, 1, s.nRCDR});
    // t[int(Command::ACT)].push_back({Command::WR, 1, s.nRCDW});
    // t[int(Command::ACT)].push_back({Command::WRA, 1, s.nRCDW});

    // t[int(Command::RD)].push_back({Command::PRE, 1, s.nRTP});
    // t[int(Command::WR)].push_back({Command::PRE, 1, s.nCWL + s.nBL + s.nWR});

    // t[int(Command::RDA)].push_back({Command::ACT, 1, s.nRTP + s.nRP});
    // t[int(Command::WRA)].push_back({Command::ACT, 1, s.nCWL + s.nBL + s.nWR + s.nRP});

    // // RAS <-> RAS
    // t[int(Command::ACT)].push_back({Command::ACT, 1, s.nRC});
    // t[int(Command::ACT)].push_back({Command::PRE, 1, s.nRAS});
    // t[int(Command::PRE)].push_back({Command::ACT, 1, s.nRP});

    // // REFSB
    // t[int(Command::PRE)].push_back({Command::REFSB, 1, s.nRP});
    // t[int(Command::REFSB)].push_back({Command::REFSB, 1, s.nRFC});
    // t[int(Command::REFSB)].push_back({Command::ACT, 1, s.nRFC});
}
