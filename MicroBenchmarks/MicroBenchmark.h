// A microbenchmark runner. 

#ifndef MICROBENCHMARK_H
#define MICROBENCHMARK_H

#include "../../ChampSim_CRC2/inc/champsim_crc2.h"

using namespace std;

class Parameters {
public:
    uint32_t num_core;
    uint32_t llc_sets;
    uint32_t llc_ways;
    Parameters() {
        // num_core = 1;
        // llc_sets = num_core * 2048;
        // llc_ways = 16;
        num_core = 1;
        llc_sets = num_core * 4;
        llc_ways = 2;
    }

};

uint32_t HashFunc(uint64_t PC);

class Access {
    // A class for each cache access. 
public:
    uint64_t PC,
             address,
             cpu;
    uint32_t type;
    Access() {}
    Access(uint64_t _PC, 
           uint64_t _address,
           uint32_t _type=LOAD,
           uint64_t _cpu=0) {
            this->PC = _PC;
            this->address = _address;
            this->type = _type;
            this->cpu = _cpu;
           }
};

#endif
