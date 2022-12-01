// A microbenchmark runner supporting files. 

#include "../../ChampSim_CRC2/inc/champsim_crc2.h"

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <iostream>
#include <string>

// CACHE ACCESS TYPE
#define LOAD      0
#define RFO       1
#define PREFETCH  2
#define WRITEBACK 3
#define NUM_TYPES 4

using namespace std;


// void InitReplacementState(),
//      UpdateReplacementState(uint32_t cpu, uint32_t set, uint32_t way, uint64_t paddr, uint64_t PC, uint64_t victim_addr, uint32_t type, uint8_t hit),
//      PrintStats_Heartbeat(),
//      PrintStats();

// uint32_t GetVictimInSet(uint32_t cpu, uint32_t set, const BLOCK *current_set, uint64_t PC, uint64_t paddr, uint32_t type);

uint64_t get_cycle_count() {std::abort(); return 0;}
uint64_t get_instr_count(uint32_t cpu) {std::abort(); return 0;}
uint64_t get_config_number() {std::abort(); return 0;}

