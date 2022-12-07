// SHiP: Signature-based Hit Predictor for High Performance Caching
// Impl by Haoxuan Shan
// Developed given srrip.cc provided in CRC2

#include "../../ChampSim_CRC2/inc/champsim_crc2.h"

#include<vector>
#include<algorithm>
#include<iterator>
#include<random>
#include<numeric>
#include<assert.h>
// #include "../MicroBenchmarks/MicroBenchmark.h"

#define NUM_CORE 1
#define LLC_SETS NUM_CORE*4
#define LLC_WAYS 2

// #define NUM_CORE 1
// #define LLC_SETS NUM_CORE*4
// #define LLC_WAYS 2

#define maxRRPV 3
#define longRRPV 2
#define interRRPV 1
#define bestRRPV 0

uint32_t line_rrpv[LLC_SETS][LLC_WAYS]; // Line rrpv index
uint32_t line_sign[LLC_SETS][LLC_WAYS]; // Line Signature
bool line_outc[LLC_SETS][LLC_WAYS]; // Line Outcome
// follow ship++, split cache line to cores. 
uint32_t core_belonging[LLC_SETS][LLC_WAYS]; // which core this line is for

// Number of sets sampled. 
// SHiP: "Overall, 256 out of the total 4096 cache sets offers a good design point between performance benefit and hardware cost"
#define SAMPLE_NUM ((LLC_SETS > 64) ? (LLC_SETS>>4) : LLC_SETS)

// if a set should be used to train the SHCT
bool is_ship_sample[LLC_SETS] = {0};

// Signature History Counter Table (SHCT)
// per-core 16K entry. 14-bit signature = 16k entry. 3-bit per entry
#define maxSHCTv 8
#define SHCT_SIZE (1<<14)
uint32_t SHCT[NUM_CORE][SHCT_SIZE];
#define SHCT_INC(v) (v+1 < maxSHCTv) ? v+1 : v
#define SHCT_DEC(v) (v > 0) ? v-1 : v

// initialize replacement state
void InitReplacementState()
{
    cout << "Initialize SHiP init state" << endl;
    // Init
    for (int i=0; i<LLC_SETS; i++) {
        for (int j=0; j<LLC_WAYS; j++) {
            line_rrpv[i][j] = maxRRPV;
            line_sign[i][j] = SHCT_SIZE;
            line_outc[i][j] = false;
            core_belonging[i][j] = 0;
        }
        is_ship_sample[i] = false;
    }

    for (int i=0; i<NUM_CORE; i++) {
        for (int j=0; j<SHCT_SIZE; j++) {
            SHCT[i][j] = 1; // Assume weakly re-use start
        }
    }

    // Select Sampling Set
    // If c++ 17, we can use std::sample instead. 
    // For now, impl in a naive way since using c++ 11
    // std::sample(ints.begin(), ints.end(), std::back_inserter(idx),
                // 5, std::mt19937{std::random_device{}()});
    using namespace std;
    vector<int> idx(LLC_SETS);
    iota(idx.begin(), idx.end(), 1);
    random_device rd;
    mt19937 gen(rd());
    shuffle(idx.begin(), idx.end(), gen);
    // cout << "Picked Samples:" << endl;
    // for (auto x : idx)
    //     cout << x << " ";
    // cout << endl;
    // TODO
    int max_sample = (LLC_SETS < SAMPLE_NUM) ? LLC_SETS : SAMPLE_NUM;
    cout << max_sample << " " << LLC_SETS << " " << SAMPLE_NUM << endl;
    for ( int i = 0 ; i < max_sample ; ++ i ) {
        is_ship_sample[idx[i]-1] = true;
        cout << idx[i]-1 << "picked" << endl;
    }

    // for ( int i = 0 ; i < LLC_SETS ; ++ i ) // For testing
    //     is_ship_sample[i] = true;
    cout << "End SHiP init state" << endl;
}

// find replacement victim
// return value should be 0 ~ 15 or 16 (bypass)
uint32_t GetVictimInSet (uint32_t cpu, uint32_t set, const BLOCK *current_set, uint64_t PC, uint64_t paddr, uint32_t type)
{
    // cout << "Vic" << endl;
    
    // This part is the same with rrpv. Nothing to be changed. 
    // look for the maxRRPV line
    int itr = 0;
    while (1)
    {
        for (int i=0; i<LLC_WAYS; i++)
            if (line_rrpv[set][i] == maxRRPV)
                return i;
        for (int i=0; i<LLC_WAYS; i++)
            line_rrpv[set][i]++;
        itr++;
        if (itr > maxRRPV)
            assert(0);
    }

    // cout << "Vic End" << endl;

    // WE SHOULD NOT REACH HERE
    assert(0);
    return 0;
}

uint32_t generate_signature(uint64_t PC, uint32_t type)
{
    uint64_t use_PC = (type == PREFETCH ) ? ((PC << 1) + 1) : (PC<<1);
    uint32_t new_sig = use_PC%SHCT_SIZE;
    return new_sig;
}

// called on every cache hit and cache fill
void UpdateReplacementState (uint32_t cpu, uint32_t set, uint32_t way, uint64_t paddr, uint64_t PC, uint64_t victim_addr, uint32_t type, uint8_t hit)
{
    // cout << "Upd" << endl;
    assert (set < LLC_SETS && way < LLC_WAYS);
    // cout << "Good input" << endl;

    // handle writeback access
    if (type == WRITEBACK) {
        if (!hit)
            line_rrpv[set][way] = longRRPV;
        return;
    }

    uint32_t sig = line_sign[set][way];
    if (hit) { // A cache hit
        assert (sig != SHCT_SIZE);
        line_rrpv[set][way] = bestRRPV;
        line_outc[set][way] = true;
        if (is_ship_sample[set]) {
            cout << "SHCT++" << endl;
            SHCT[core_belonging[set][way]][sig] = SHCT_INC(SHCT[core_belonging[set][way]][sig]);
        }
        return;
    }

    // A cache fill
    cout << "Fiil with " << is_ship_sample[set] << " " << line_sign[set][way] << endl;
    if (is_ship_sample[set] && line_sign[set][way] != SHCT_SIZE) {
        cout << "A cache fill to sample out=" << line_outc[set][way] << endl;
        uint32_t core = core_belonging[set][way];
        if(line_outc[set][way] != true) {
            cout << "SHCT--" << endl;
            SHCT[core][line_sign[set][way]] = SHCT_DEC(SHCT[core][line_sign[set][way]]);
        }
    }
    uint32_t new_signature = generate_signature(PC, type);
    core_belonging[set][way] = cpu;
    line_outc[set][way] = false;
    line_sign[set][way] = new_signature;    
    if (SHCT[cpu][new_signature] == 0)
        line_rrpv[set][way] = longRRPV; // 2 // long-distance
    else
        line_rrpv[set][way] = interRRPV; // intermediate-distance
    
    // cout << "upd_end" << endl;
}

// use this function to print out your own stats on every heartbeat 
void PrintStats_Heartbeat()
{
    // Nothing needed.
}

// use this function to print out your own stats at the end of simulation
void PrintStats()
{
    // using namespace std;

    // Nothing needed.
    // int x = 0;
    // cout << "#Sampled sets:";
    // for (int i = 0 ; i < LLC_SETS; i++ )
    // {
    //     if (is_ship_sample[i])
    //         x++;
    //     if (is_ship_sample[i])
    //         cout << i << " ";
    // }
    
    // cout << " total=" << x << endl;

    for (int i = 0 ; i < LLC_SETS; ++ i)
        for (int j=0;j<LLC_WAYS;++j)
        {
            if(line_sign[i][j] == SHCT_SIZE)
                continue;
            cout << "[" << i << "][" << j << "] sig=" << line_sign[i][j] << " rrpv=" << line_rrpv[i][j] << " core=" << core_belonging[i][j] << " counter=" << SHCT[core_belonging[i][j]][line_sign[i][j]] << "\n";
        }

    // int y[maxSHCTv] = {0};
    // for (int i = 1 ; i < SHCT_SIZE ; i++)
    //     if(SHCT[0][i] != 1)
    //         y[SHCT[0][i]]++;
    // for (int i=0;i<maxSHCTv;++i)
    //     cout << y[i] <<" ";
    // cout << endl;


}
