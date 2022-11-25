# Cache Replacement Policy Evaluation

## Directory Tree

This is a project that hopes to verify the result presented in Hawkeye Cache Replacement Policy. 
The ChampSim environment is needed and should be put parrallel with this directory. The experiments follow the ![CRC2](https://www.dropbox.com/s/o6ct9p7ekkxaoz4/ChampSim_CRC2_ver2.0.tar.gz?dl=1). Use `crp` for cache replacement policy, `cf` for configuration, and `bcmk` for benchmark in the followings

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
2. Change number of instructions to simulate in `run_sim.sh` when final testing. 
