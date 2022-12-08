# Cache Replacement Policy Evaluation

## Directory Tree

This is a project that hopes to verify the result presented in Hawkeye Cache Replacement Policy. 
The ChampSim environment is needed and should be put parrallel with this directory. The experiments follow the ![CRC2](https://crc2.ece.tamu.edu/). Use `crp` for cache replacement policy, `cf` for configuration, and `bcmk` for benchmark in the followings

```
/ChampSim_CRC2
    /ChampSim_CRC2/trace

/ece552_cr_project # Current dir
    ./exec # Execute files: crp-cf
    ./results # Store outputs: crp-cf-bcmk.out
    ./scripts # Scripts
    ./src # Your source files (cache replacement policy impl)
```

## How to run scripts

Run everythin under `/ece552_cr_project`.
```
pwd
# xxx/ece552_cr_project
./scripts/compile.sh
./scripts/run_sim.sh
```

## How to change scripts

1. Add you source files in `compile.sh`
2. *IMPORTANT:* Change number of instructions to simulate in `run_sim.sh` when final testing. Please also change the directory of results correspondingly. 

## How to run micro-benchmark

1. Change your source file config to follow that in microbenchmark.cc. 
2. Revise commands in ./scripts/run_microbenchmark.sh. 
3. Run ./scripts/run_microbenchmark.sh 
4. Run ./MicroBenchmarks_exec/ship 
