#include "../../ChampSim_CRC2/inc/champsim_crc2.h"
#include "sdbp.h"
#include "utils.h"

#define NUM_CORE 4
#define LLC_SETS NUM_CORE*2048
#define LLC_WAYS 16

//definitions to Sampling Predictor
#define SAMPLER_SETS 32*4
#define SAMPLER_ASSOC 13
#define SAMPLER_MODULUS LLC_SETS/SAMPLER_SETS


#define PREDICTOR_NUM_TABLES 3
#define PREDICTOR_TABLE_ENTRIES 4096*4
#define PREDICTOR_INDEX_BITS 14 //log2(4096) = 12
#define PREDICTOR_COUNTER_WIDTH 2
#define PREDICTOR_COUNTER_MAX 4
#define PREDICTOR_THRESHOLD 8

#define TRACE_BITS 16
#define TAG_BITS 16


uint32_t lru[LLC_SETS][LLC_WAYS]; //LRU gives the baseline replacement policies for SDBP
bool prediction[LLC_SETS][LLC_WAYS];
sampler *samp;
int predictor_tables[PREDICTOR_NUM_TABLES][PREDICTOR_TABLE_ENTRIES];

// initialize replacement state
void InitReplacementState()
{
    //init LRU
    cout << "Initialize LRU replacement state" << endl;
    for (int i=0; i<LLC_SETS; i++) {
        for (int j=0; j<LLC_WAYS; j++) {
            lru[i][j] = j;
        }
    }

    //init SDBP Sampling Predictor
    samp = new sampler();

    //init predictor tables value as 0
    memset(predictor_tables,0,sizeof(predictor_tables));
    memset(prediction,false,sizeof(prediction));
}


uint32_t Get_LRU_Victim(uint32_t set){
    for (int i=0; i<LLC_WAYS; i++)
            if (lru[set][i] == (LLC_WAYS-1))
                return i;
    return 0;
}

// find replacement victim
// return value should be 0 ~ 15 or 16 (bypass)
uint32_t GetVictimInSet (uint32_t cpu, uint32_t set, const BLOCK *current_set, uint64_t PC, uint64_t paddr, uint32_t type)
{
    uint32_t r=Get_LRU_Victim(set); // start from LRU replacement victim

    //seek for predicted dead block, if found, replace the LRU solutions
    for(unsigned int i =0; i < LLC_WAYS; i++)
        if(prediction[set][i]){
            r = i; 
            break;
        }

    //bypass blocks that is dead on arrival 
    //dont see the gain, commented out
    // int predict = samp->pred->get_prediction(cpu,get_trace(PC));
    // if(predict) r = -1;

    return r;
}


void UpdateLRUState(uint32_t set, uint32_t way)//function to update LRU
{
    // update lru replacement state
    for (uint32_t i=0; i<LLC_WAYS; i++) {
        if (lru[set][i] < lru[set][way]) {
            lru[set][i]++;

            if (lru[set][i] == LLC_WAYS)
                assert(0);
        }
    }
    lru[set][way] = 0; // promote to the MRU position
}

// called on every cache hit and cache fill
void UpdateReplacementState (uint32_t cpu, uint32_t set, uint32_t way, uint64_t paddr, uint64_t PC, uint64_t victim_addr, uint32_t type, uint8_t hit)
{
    UpdateLRUState(set,way);

    if(type == WRITEBACK) // improves the performance greatly.. dont know why
        return;
    //check if it is the sampler set
    if(set % SAMPLER_MODULUS ==0){
        //compute the actual sampler set index, access the sampler
        int samp_index = set / SAMPLER_MODULUS;
        if (samp_index >=0 && samp_index < SAMPLER_SETS)
            samp->update_sampler(cpu,set,paddr,PC);
    }


    //update the prediction result for this trace for the next round
    prediction[set][way] = samp->pred->get_prediction(cpu,get_trace(PC));
}

// use this function to print out your own stats on every heartbeat 
void PrintStats_Heartbeat()
{

}

// use this function to print out your own stats at the end of simulation
void PrintStats()
{

}

//helper functions to sdbp
//extract the lower 16 bits out of PC as hashing number
uint32_t get_trace(uint64_t PC){
    return PC & ((1<<TRACE_BITS)-1);
}

//get index number of the predictor table using CPU, trace and table_num
uint32_t predictor::get_signature(uint32_t CPU, uint32_t trace, int table_num){
    uint32_t x = fi (trace ^ (CPU << 2), table_num);
	return x & ((1<<PREDICTOR_INDEX_BITS)-1); // get the lower 12 bit index bits
}

//update the counter value, increment if dead, decrease the counter if not
void predictor::block_dead(uint32_t CPU, uint32_t trace, bool ifdead){
    
    for(int i=0;i<PREDICTOR_NUM_TABLES;i++){
        
        uint32_t index = get_signature(CPU,trace,i);

        if(ifdead){
            if(predictor_tables[i][index]<PREDICTOR_COUNTER_MAX)
                predictor_tables[i][index]++;
        }else{
            //decrease the counter otherwise
            if(i%2 ==0){
                predictor_tables[i][index]--;//even number table decrease by one
            }else{
                predictor_tables[i][index]>>=1; //odd number table decrease exponentially
            }
        }            
    }
}

//predict if a given block is considered dead
bool predictor::get_prediction(uint32_t CPU,uint32_t trace){
    
    int sum = 0;

    for(int i=0;i<PREDICTOR_NUM_TABLES;i++)
        sum += predictor_tables[i][get_signature(CPU,trace,i)];
    return sum>=PREDICTOR_THRESHOLD;

    return false;
    
}

//initialize sampler and its substruct
sampler::sampler(void){
    pred = new predictor ();
    sets = new sampler_set [SAMPLER_SETS];
}

sampler_set::sampler_set(void){
    entries = new sampler_entry[SAMPLER_ASSOC];

    //init sampler LRU values
    for(int i=0;i<SAMPLER_ASSOC;i++)
        entries[i].lru_stack_position = i;
}

//update sampler
void sampler::update_sampler(uint32_t CPU, uint32_t set,uint64_t tag, uint64_t PC){
    
    sampler_entry *entries = &sets[set].entries[0]; // identify the target sampler set
    uint32_t partial_tag = tag & (((1<<TAG_BITS)-1)<<4); //extract the lower 16-bit of the address 

    int i; 
    for(i=0;i<SAMPLER_ASSOC;i++)
        if(entries[i].valid && entries[i].tag == partial_tag){
            pred->block_dead(CPU,entries[i].trace,false);
            break;
        }
    
    //if we dont find a match
    if(i == SAMPLER_ASSOC){

        // look for invalid block to replace
        for(i=0;i<SAMPLER_ASSOC;i++) 
            if(entries[i].valid == false)
                break;
        
        //no invalid block, look for dead block
        for(i=0;i<SAMPLER_ASSOC;i++) 
            if(entries[i].prediction)
                break;
        
        //if both miss, use LRU block
        if(i==SAMPLER_ASSOC){
            int j;
            for (j=0;j<SAMPLER_ASSOC;j++)
                if(entries[j].lru_stack_position == (unsigned int)(SAMPLER_ASSOC -1))
                    break;
            i = j;
        }

        pred->block_dead(CPU,entries[i].trace,true);
        entries[i].tag = partial_tag;
        entries[i].valid = true;
    }

    entries[i].trace = get_trace(PC);
    entries[i].prediction = pred->get_prediction(CPU,entries[i].trace);

    //now the replaced entry should be moved to MRU position
    for(int way=0;way<SAMPLER_ASSOC;way++)
        entries[way].lru_stack_position++;
    entries[i].lru_stack_position = 0;
}