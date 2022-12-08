#!/bin/bash
pj_dir="../ece552_cr_project"
CRC2_dir="../ChampSim_CRC2"
exec_dir=$pj_dir/exec

rm -rf $exec_dir
mkdir $exec_dir

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

##############################################################
###################     Single-Core      #####################
##############################################################

micbcmk_dir=$pj_dir/MicroBenchmarks
g++ -Wall --std=c++11 -w $micbcmk_dir/Champsim_crc2.cc $micbcmk_dir/MicroBenchmark.cc $pj_dir/MicBCMK_src/ship.cpp -o MicroBenchmarks_exec/ship
g++ -Wall --std=c++11 -w $micbcmk_dir/Champsim_crc2.cc $micbcmk_dir/MicroBenchmark.cc $pj_dir/MicBCMK_src/sdbp.cc -o MicroBenchmarks_exec/sdbp


# # Single core with 2MB LLC
# # with(config2)/without(config1)
# # single_core_config_files=($CRC2_dir/lib/config1.a $CRC2_dir/lib/config2.a)
# # single_core_source_files=($CRC2_dir/example/srrip.cc  $CRC2_dir/example/lru.cc $pj_dir/src/ship.cpp)

# single_core_source_files=($CRC2_dir/example/srrip.cc  $CRC2_dir/example/lru.cc $pj_dir/src/ship.cpp $pj_dir/example/ship++.cc)
# single_core_config_files=($CRC2_dir/lib/config1.a)

# for cf in ${single_core_config_files[@]};
# do
#     for crp in ${single_core_source_files[@]};
#     do
#         cf_name=$(echo $(basename "${cf}") | cut -d'.' -f 1)
#         crp_name=$(echo $(basename "${crp}") | cut -d'.' -f 1 | cut -d'-' -f 1)
#         exec_name=$crp_name'-'$cf_name
#         g++ -Wall --std=c++11 -o $exec_dir/$exec_name $crp $cf
#     done
# done