/*
*
* The timing parameters are based on NVmain model STTRAM_Everspin
*
*/

#ifndef __STTRAM_AB_H
#define __STTRAM_AB_H

#include "DRAM.h"
#include "Request.h"
#include <vector>
#include <functional>

using namespace std;

namespace ramulator
{

class STTRAM_AB
{
public:
    static string standard_name;
    enum class Org;
    enum class Speed;
    STTRAM_AB(Org org, Speed speed);
    STTRAM_AB(const string& org_str, const string& speed_str);
    
    static map<string, enum Org> org_map;
    static map<string, enum Speed> speed_map;
    /* Level */
    enum class Level : int
    { 
        Channel, Rank, BankGroup, Bank, Row, Column, MAX
    };
    
    static std::string level_str [int(Level::MAX)];

    /* Command */
    enum class Command : int
    { 
        ACT, PRE, PREA, 
        RD,  WR,  RDA,  WRA, 
        REF, PDE, PDX,  SRE, SRX, 
        MAX
    };

    string command_name[int(Command::MAX)] = {
        "ACT", "PRE", "PREA", 
        "RD",  "WR",  "RDA",  "WRA", 
        "REF", "PDE", "PDX",  "SRE", "SRX"
    };

    Level scope[int(Command::MAX)] = {
        Level::Row,    Level::Bank,   Level::Rank,   
        Level::Column, Level::Column, Level::Column, Level::Column,
        Level::Rank,   Level::Rank,   Level::Rank,   Level::Rank,   Level::Rank
    };

    bool is_opening(Command cmd) 
    {
        switch(int(cmd)) {
            case int(Command::ACT):
                return true;
            default:
                return false;
        }
    }

    bool is_accessing(Command cmd) 
    {
        switch(int(cmd)) {
            case int(Command::RD):
            case int(Command::WR):
            case int(Command::RDA):
            case int(Command::WRA):
                return true;
            default:
                return false;
        }
    }

    bool is_closing(Command cmd) 
    {
        switch(int(cmd)) {
            case int(Command::RDA):
            case int(Command::WRA):
            case int(Command::PRE):
            case int(Command::PREA):
                return true;
            default:
                return false;
        }
    }

    bool is_refreshing(Command cmd) 
    {
        switch(int(cmd)) {
            case int(Command::REF):
                return true;
            default:
                return false;
        }
    }

    /* State */
    enum class State : int
    {
        Opened, Closed, PowerUp, ActPowerDown, PrePowerDown, SelfRefresh, MAX
    } start[int(Level::MAX)] = {
        State::MAX, State::PowerUp, State::MAX, State::Closed, State::Closed, State::MAX
    };

    /* Translate */
    Command translate[int(Request::Type::MAX)] = {
        Command::RD,  Command::WR,
        Command::REF, Command::PDE, Command::SRE
    };

    /* Prereq */
    function<Command(DRAM<STTRAM_AB>*, Command cmd, int)> prereq[int(Level::MAX)][int(Command::MAX)];

    // SAUGATA: added function object container for row hit status
    /* Row hit */
    function<bool(DRAM<STTRAM_AB>*, Command cmd, int)> rowhit[int(Level::MAX)][int(Command::MAX)];
    function<bool(DRAM<STTRAM_AB>*, Command cmd, int)> rowopen[int(Level::MAX)][int(Command::MAX)];

    /* Timing */
    struct TimingEntry
    {
        Command cmd;
        int dist;
        int val;
        bool sibling;
    }; 
    vector<TimingEntry> timing[int(Level::MAX)][int(Command::MAX)];

    /* Lambda */
    function<void(DRAM<STTRAM_AB>*, int)> lambda[int(Level::MAX)][int(Command::MAX)];

    /* Organization */
    enum class Org : int
    {
        STTRAM_2Gb_x4,   STTRAM_2Gb_x8,   STTRAM_2Gb_x16,
        STTRAM_4Gb_x4,   STTRAM_4Gb_x8,   STTRAM_4Gb_x16,
        STTRAM_8Gb_x4,   STTRAM_8Gb_x8,   STTRAM_8Gb_x16,
        MAX
    };

    struct OrgEntry {
        int size;
        int dq;
        int count[int(Level::MAX)];
    } org_table[int(Org::MAX)] = {
        {2<<10,  4, {0, 0, 4, 4, 1<<15, 1<<10}}, {2<<10,  8, {0, 0, 4, 4, 1<<14, 1<<10}}, {2<<10, 16, {0, 0, 2, 4, 1<<14, 1<<10}},
        {4<<10,  4, {0, 0, 4, 4, 1<<16, 1<<10}}, {4<<10,  8, {0, 0, 4, 4, 1<<15, 1<<10}}, {4<<10, 16, {0, 0, 2, 4, 1<<15, 1<<10}},
        {8<<10,  4, {0, 0, 4, 4, 1<<17, 1<<10}}, {8<<10,  8, {0, 0, 4, 4, 1<<16, 1<<10}}, {8<<10, 16, {0, 0, 2, 4, 1<<16, 1<<10}}
    }, org_entry;

    void set_channel_number(int channel);
    void set_rank_number(int rank);

    /* Speed */
    enum class Speed : int
    {
        STTRAM_1600K, STTRAM_1600L,
        STTRAM_1866M, STTRAM_1866N,
        STTRAM_2133P, STTRAM_2133R,
        STTRAM_2400R, STTRAM_2400U,
        STTRAM_3200,
        STTRAM_400MHz,
        MAX
    };

    enum class RefreshMode : int
    {
        Refresh_1X,
        Refresh_2X,
        Refresh_4X,
        MAX
    } refresh_mode = RefreshMode::Refresh_1X;

    int prefetch_size = 8; // 8n prefetch DDR
    int channel_width = 64;

    struct SpeedEntry {
        int rate;
        double freq, tCK;
        int nBL, nCCDS, nCCDL, nRTRS;
        int nCL, nRCD, nRP, nCWL;
        int nRAS, nRC;
        int nRTP, nWTRS, nWTRL, nWR;
        int nRRDS, nRRDL, nFAW;
        int nRFC, nREFI;
        int nPD, nXP, nXPDLL; // XPDLL not found in STTRAM??
        int nCKESR, nXS, nXSDLL; // nXSDLL TBD (nDLLK), nXS = (tRFC+10ns)/tCK
    } speed_table[int(Speed::MAX)] = {
        {1600, (400.0/3)*6, (3/0.4)/6, 4, 4, 5, 2, 11, 11, 11,  9, 28, 39, 6, 2, 6, 12, 0, 0, 0, 0, 0, 4, 5, 0, 5, 0, 0},
        {1600, (400.0/3)*6, (3/0.4)/6, 4, 4, 5, 2, 12, 12, 12,  9, 28, 40, 6, 2, 6, 12, 0, 0, 0, 0, 0, 4, 5, 0, 5, 0, 0},
        {1866, (400.0/3)*7, (3/0.4)/7, 4, 4, 5, 2, 13, 13, 13, 10, 32, 45, 7, 3, 7, 14, 0, 0, 0, 0, 0, 5, 6, 0, 6, 0, 0},
        {1866, (400.0/3)*7, (3/0.4)/7, 4, 4, 5, 2, 14, 14, 14, 10, 32, 46, 7, 3, 7, 14, 0, 0, 0, 0, 0, 5, 6, 0, 6, 0, 0},
        {2133, (400.0/3)*8, (3/0.4)/8, 4, 4, 6, 2, 15, 15, 15, 11, 36, 51, 8, 3, 8, 16, 0, 0, 0, 0, 0, 6, 7, 0, 7, 0, 0},
        {2133, (400.0/3)*8, (3/0.4)/8, 4, 4, 6, 2, 16, 16, 16, 11, 36, 52, 8, 3, 8, 16, 0, 0, 0, 0, 0, 6, 7, 0, 7, 0, 0},
        {2400, (400.0/3)*9, (3/0.4)/9, 4, 4, 6, 2, 16, 16, 16, 12, 39, 55, 9, 3, 9, 18, 0, 0, 0, 0, 0, 6, 8, 0, 7, 0, 0},
        {2400, (400.0/3)*9, (3/0.4)/9, 4, 4, 6, 2, 18, 18, 18, 12, 39, 57, 9, 3, 9, 18, 0, 0, 0, 0, 0, 6, 8, 0, 7, 0, 0},
        {3200, 1600, 0.625, prefetch_size/2/*DDR*/, 4,     10,   2,    22, 22,  22, 16,  56,  78, 12,  4,    12,   24, 8,    10,   40,  0,   0,    8,  10, 0,     8,     0,  0},
        {3200, 400,  2.500, 4,                      2,     2,    1,    6,  14,  1,  10,  24,  25,  5,  5,    5,    19, 4,    4,    10,  0,   0,    2,  3,  0,     2,     0,  0}
        //rate, freq, tCK,  nBL,                    nCCDS  nCCDL nRTRS nCL nRCD nRP nCWL nRAS nRC nRTP nWTRS nWTRL nWR nRRDS nRRDL nFAW nRFC nREFI nPD nXP nXPDLL nCKESR nXS nXSDLL
    }, speed_entry;

    int read_latency;

private:
    void init_speed();
    void init_lambda();
    void init_prereq();
    void init_rowhit();  // SAUGATA: added function to check for row hits
    void init_rowopen();
    void init_timing();
};

} /*namespace ramulator*/

#endif /*__STTRAM_AB_H*/