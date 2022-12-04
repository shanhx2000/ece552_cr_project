#!/bin/bash

pj_dir="../ece552_cr_project"
CRC2_dir="../ChampSim_CRC2"
exec_dir=$pj_dir/"exec"
result_dir=$pj_dir/"results"/"2e8Tr1e9Te"

# exec_files=$(ls $pj_dir/exec/)
# exec_files=(lru-config1 ship-config1 ship++-config1 srrip-config1)
exec_files=(ship-config4)
# exec_files=(lru-config4)

# TODO: Change it back for the final test.
warmup_insn_num=50000000 # 50M
simula_insn_num=200000000 # 200M

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
    fi
done
