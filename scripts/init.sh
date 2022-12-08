#!/bin/bash
pj_dir="../ece552_cr_project"
CRC2_dir="../ChampSim_CRC2"
ChampSim_dir="../ChampSim"
exec_dir=$pj_dir/"exec"
result_dir=$pj_dir/"results"
traces_download_dir="../traces_download"

# Verify Directory
check_directory () {
    if [ ! -d $1 ]
    then
        echo "Directory "$1" DOES NOT exists."
        exit 1
    fi
}

check_directory $pj_dir
check_directory $CRC2_dir
# check_directory $ChampSim_dir # Commented it since we decide not to use the latest version in the end. 
check_directory $exec_dir
check_directory $result_dir
if [ ! -d $traces_download_dir ]
then
    echo "Directory "$1" DOES NOT exists. Making..."
    mkdir $traces_download_dir
    mkdir $traces_download_dir/xz
    mkdir $traces_download_dir/gz
fi
exit 0
