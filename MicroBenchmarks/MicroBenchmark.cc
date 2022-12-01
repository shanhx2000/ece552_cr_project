#include "../../ChampSim_CRC2/inc/champsim_crc2.h"
#include "../MicroBenchmarks/MicroBenchmark.h"

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

Parameters micro_benchmark_parameters;

uint32_t HashFunc(uint64_t PC)
{
    return PC%micro_benchmark_parameters.llc_sets;
}

class CacheTable {
public:
    uint32_t num_core;
    uint32_t llc_sets;
    uint32_t llc_ways;
    vector<vector<BLOCK*>> table;
    
    CacheTable(Parameters micro_benchmark_parameters) {
        this->num_core = micro_benchmark_parameters.num_core;
        this->llc_sets = micro_benchmark_parameters.llc_sets;
        this->llc_ways = micro_benchmark_parameters.llc_ways;
        this->table.resize(this->num_core,vector<BLOCK*>(this->llc_sets));
        for(int i=0;i<this->num_core;++i)
            for(int j=0;j<this->llc_sets;++j)
                table[i][j] = new BLOCK[this->llc_sets];
    }
    ~CacheTable() {
        for(int i=0;i<this->num_core;++i)
                for(int j=0;j<this->llc_sets;++j)
                    delete [] table[i][j];
        }
    uint32_t FindinSet(uint32_t core, uint32_t set, uint64_t address) {
        int flag = 0;
        for(int i=0;i<this->llc_ways;++i)
            if (table[core][set][i].valid)
                if (table[core][set][i].full_addr==address)
                    return i;
        return this->llc_ways+1;
    }
    uint32_t AddEle(uint32_t core, uint32_t set, uint32_t way, const BLOCK* block) {
        table[core][set][way] = *block; // Assum block.valid=1
    }
    void PopEle(uint32_t core, uint32_t set, uint32_t way) {
        table[core][set][way].valid = false;
    }

    void PrintTable() {
        cout << "Printing Table:" << endl;
        for(int i=0;i<this->num_core;++i) {
            cout << "Core " << i << ":" << endl;
            for(int j=0;j<this->llc_sets;++j) {
                cout << "Set " << j << ": ";
                for(int k=0;k<this->llc_ways;++k)
                    cout << "(" << this->table[i][j][k].valid << "," << this->table[i][j][k].full_addr << "," << this->table[i][j][k].data << ") ";
                cout << endl;
            }
        }
    }
};
#define INVALID_ADDRESS 99999999
int main() {
    cout << "Please change your config to follow micro_benchmark_parameters. " << endl;
    vector<Access> accesses;
    int PC_couter = 0;

    // load 3, 6, 9, 12
    // set  3, 2, 1, 0
    // Cache 
    // 0    3   x
    // 1    2   x
    // 2    1   x
    // 3    0   x
    for (int i = 0 ; i < 4 ; ++ i ) {
        accesses.push_back(Access(PC_couter, (i+1)*3));
        PC_couter += 2;
    }

    // load 3, 6, 9, 12
    // set  3, 2, 1, 0
    // Cache 
    // 0    3   x
    // 1    2   4   5   6   7
    // 2    1   x
    // 3    0   x    
    for (int i = 0 ; i < 4 ; ++ i ) {
        accesses.push_back(Access(PC_couter, (i+1)*4+1));
        PC_couter += 3;
    }

    // load 3, 6, 9, 12
    // set  3, 2, 1, 0
    // Cache 
    // 0    3   x
    // 1    2   4   5   6   7   8   10
    // 2    1   9   11
    // 3    0   x    
    for (int i = 0 ; i < 4 ; ++ i ) {
        accesses.push_back(Access(PC_couter, (i+1)*128+(i%2)+1));
        PC_couter += 1;
    }

    for (int i = 0 ; i < 6 ; ++ i ) {
        if (i%3 == 0)
            accesses.push_back(Access(PC_couter, (i+1)*8+0));
        else
            accesses.push_back(Access(PC_couter, (5)*8+0));
        PC_couter += 1;
    }

    CacheTable table = CacheTable(micro_benchmark_parameters);

    InitReplacementState();

    for (int idx = 0; idx < accesses.size() ; ++idx)
    {
        Access acc = accesses[idx];
        uint32_t core = acc.cpu;
        uint32_t set = HashFunc(acc.address);
        uint64_t paddr = acc.address;
        uint32_t way = table.FindinSet(core, set, acc.address);
        BLOCK new_block;
        new_block.cpu = core;
        new_block.full_addr = acc.address;
        new_block.data = idx;
        new_block.valid = 1;

        if ( way < table.llc_ways ) // Hit
        {
            if(acc.type == WRITEBACK) {
                table.table[core][set][way].data = new_block.data;
                table.table[core][set][way].dirty = true;
            }
            UpdateReplacementState(core, set, way, paddr, acc.PC, INVALID_ADDRESS, acc.type, true);
        }
        else // Miss
        {
            // First find in Victim
            const BLOCK *current_set = table.table[core][set];
            uint32_t way = GetVictimInSet(core, set, current_set, acc.PC, paddr, acc.type);
            
            // PopVictim
            uint64_t victim_addr = table.table[core][set][way].full_addr;
            uint64_t victim_data = table.table[core][set][way].data;
            table.PopEle(core, set, way);

            // Add new block
            table.AddEle(core, set, way, &new_block);
            if(acc.type == WRITEBACK) {
                table.table[core][set][way].data = new_block.data;
                table.table[core][set][way].dirty = true;
            }
            UpdateReplacementState(core, set, way, paddr, acc.PC, victim_addr, acc.type, false);
        }
        
        cout << "Step " << idx << " Printing." << endl;
        PrintStats();
        table.PrintTable();
        cout << "Step " << idx << " Finished." << endl << endl;
    }
    cout << "Simulation Finished!" << endl;
    PrintStats();
    
    return 0;
}