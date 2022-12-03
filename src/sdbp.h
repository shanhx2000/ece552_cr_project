#include "cstring"
#include <math.h>

uint32_t get_trace(uint64_t PC);

struct sampler;

struct sampler_entry{
	unsigned int 	
		lru_stack_position,
		tag,
		trace,
		prediction;
		
	bool
		valid;

	// constructor for sampler entry

	sampler_entry (void) {
		lru_stack_position = 0;
		valid = false;
		tag = 0;
		trace = 0;
		prediction = 0;
	};
};

struct sampler_set {
    sampler_entry *entries;
    sampler_set(void); // constructor for sampler set
};

struct predictor{
    bool get_prediction(uint32_t CPU,uint32_t trace);
    void block_dead(uint32_t CPU, uint32_t trace, bool ifdead);
    uint32_t get_signature(uint32_t CPU, uint32_t trace, int table_num);
};

struct sampler{
    predictor *pred;
    sampler_set *sets;

    void update_sampler(uint32_t CPU, uint32_t set,uint64_t tag, uint64_t PC);
    sampler(void); // constructor for sampler
};