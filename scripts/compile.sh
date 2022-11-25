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

# Single core with 2MB LLC
# with(config2)/without(config1)
# single_core_config_files=($CRC2_dir/lib/config1.a $CRC2_dir/lib/config2.a)
# single_core_source_files=($CRC2_dir/example/srrip.cc  $CRC2_dir/example/lru.cc $pj_dir/src/ship.cpp)

single_core_source_files=($pj_dir/src/ship.cpp)
single_core_config_files=($CRC2_dir/lib/config1.a)

for cf in ${single_core_config_files[@]};
do
    for crp in ${single_core_source_files[@]};
    do
        cf_name=$(echo $(basename "${cf}") | cut -d'.' -f 1)
        crp_name=$(echo $(basename "${crp}") | cut -d'.' -f 1 | cut -d'-' -f 1)
        exec_name=$crp_name'-'$cf_name
        g++ -Wall --std=c++11 -o $exec_dir/$exec_name $crp $cf
    done
done

# ##############################################################
# ####################     Multi-Core      #####################
# ##############################################################


# # A 4-core configuration with 8MB of shared LLC
# # with(config2)/without(config1)
# multi_core_config_files=($CRC2_dir/lib/config3.a $CRC2_dir/lib/config4.a)
# multi_core_source_files=($CRC2_dir/example/lru-8MB.cc)

# for cf in ${multi_core_config_files[@]};
# do
#     for crp in ${multi_core_source_files[@]};
#     do
#         cf_name=$(echo $(basename "${cf}") | cut -d'.' -f 1)
#         crp_name=$(echo $(basename "${crp}") | cut -d'.' -f 1 | cut -d'-' -f 1)
#         exec_name=$crp_name'-'$cf_name
#         g++ -Wall --std=c++11 -o $exec_dir/$exec_name $crp $cf
#     done
# done