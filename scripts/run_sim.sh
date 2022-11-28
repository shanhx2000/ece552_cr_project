#!/bin/bash
pj_dir="../ece552_cr_project"
CRC2_dir="../ChampSim_CRC2"
exec_dir=$pj_dir/"exec"
result_dir=$pj_dir/"results"/"2e8Tr1e9Te"

# Verify Directory
if [ ! -d $pj_dir ]
then
    echo "Directory "$pj_dir" DOES NOT exists."
    exit 1
fi
if [ ! -d $CRC2_dir ]
then
    echo "Directory "$CRC2_dir" DOES NOT exists."
    exit 1
fi

# 1. Single-core SPEC CPU 2006 benchmarks without prefetcher
# $ g++ -Wall --std=c++11 -o lru-config1 example/lru.cc lib/config1.a
# $ ./lru-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/bzip2_10M.trace.gz

# 2. Single-core SPEC CPU 2006 benchmarks with prefetcher
# $ g++ -Wall --std=c++11 -o lru-config1 example/lru.cc lib/config2.a
# $ ./lru-config2 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/bzip2_10M.trace.gz

# 3. 4-core multi-app SPEC CPU 2006 benchmarks without prefetcher
# $ g++ -Wall --std=c++11 -o lru-config3 example/lru-8MB.cc lib/config3.a
# $ ./lru-config3 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/bzip2_10M.trace.gz trace/mcf_10M.trace.gz trace/libquantum_10M.trace.gz trace/xalancbmk_10M.trace.gz

# 4. 4-core multi-app SPEC CPU 2006 benchmarks with prefetcher
# $ g++ -Wall --std=c++11 -o lru-config4 example/lru-8MB.cc lib/config4.a
# $ ./lru-config4 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/bzip2_10M.trace.gz trace/mcf_10M.trace.gz trace/libquantum_10M.trace.gz trace/xalancbmk_10M.trace.gz

# 5. 4-core multi-thread CloudSuite benchmarks without prefetcher
# $ g++ -Wall --std=c++11 -o lru-config3 example/lru-8MB.cc lib/config3.a
# $ ./lru-config3 -warmup_instructions 1000000 -simulation_instructions 10000000 -cloudsuite -traces trace/cassandra_core_0.trace.gz trace/cassandra_core_1.trace.gz trace/cassandra_core_2.trace.gz trace/cassandra_core_3.trace.gz

# 6. 4-core multi-thread CloudSuite benchmarks with prefetcher 
# $ g++ -Wall --std=c++11 -o lru-config4 example/lru-8MB.cc lib/config4.a
# $ ./lru-config4 -warmup_instructions 1000000 -simulation_instructions 10000000 -cloudsuite -traces trace/cassandra_core_0.trace.gz trace/cassandra_core_1.trace.gz trace/cassandra_core_2.trace.gz trace/cassandra_core_3.trace.gz

# exec_files=$(ls $pj_dir/exec/)
# exec_files=(lru-config1 ship-config1 ship++-config1 srrip-config1)
exec_files=(ship-config1)

# TODO: Change it back for the final test.
warmup_insn_num=200000000 # final test: 1000000
simula_insn_num=1000000000 # final test: 10000000

# SPEC CPU 2006 (Single-core)
# 200M warmup and 1B detailed execution
# Measure the geometric mean IPC speedup over the baseline LRU policy

# SPEC CPU 2006 (Multi-core)
# Run at least 1B instructions for all cores
# Measure the weighted IPC speedup over the baseline LRU policy

# CloudSuite (Multi-core)
# Run at least 1B instructions for all cores
#  Measure the IPC speedup over the LRU (based on the slowest trace)
#  Measure the average speedup across 6 different samples

for exec in ${exec_files[@]};
do
    echo $exec
    crp_name=$(echo $exec | cut -d'-' -f 1)
    cf_name=$(echo $exec | cut -d'-' -f 2)
    if [[ $cf_name =~ config[12] ]];
    then
        ./$exec_dir/$exec -warmup_instructions $warmup_insn_num -simulation_instructions $simula_insn_num -traces $CRC2_dir/trace/bzip2_10M.trace.gz > $result_dir/$exec-SPEC2006.out
    elif [[ $cf_name =~ config[34] ]];
    then
        ./$exec_dir/$exec -warmup_instructions $warmup_insn_num -simulation_instructions $simula_insn_num -traces $CRC2_dir/trace/bzip2_10M.trace.gz $CRC2_dir/trace/mcf_10M.trace.gz $CRC2_dir/trace/libquantum_10M.trace.gz $CRC2_dir/trace/xalancbmk_10M.trace.gz > $result_dir/$exec-SPEC2006.out
        ./$exec_dir/$exec -warmup_instructions $warmup_insn_num -simulation_instructions $simula_insn_num -cloudsuite -traces $CRC2_dir/trace/cassandra_core_0.trace.gz $CRC2_dir/trace/cassandra_core_1.trace.gz $CRC2_dir/trace/cassandra_core_2.trace.gz $CRC2_dir/trace/cassandra_core_3.trace.gz > $result_dir/$exec-cloudsuite.out
    fi
done
