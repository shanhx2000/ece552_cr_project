#!/bin/bash

pj_dir="../ece552_cr_project"
CRC2_dir="../ChampSim_CRC2"
ChampSim_dir="../ChampSim"
exec_dir=$pj_dir/"exec"
result_dir=$pj_dir/"results"
traces_download_dir="../traces_download"

# Verify Directory and build dependant directory. 
./scripts/init.sh
if [ $? -eq 1 ]; then
    exit 1
fi

dl_trace() {
    url=$1
    xzfilename=$((echo $url) | rev | cut -d'/' -f 1 | rev)
    xzfilename=${xzfilename::-5}
    name=$((echo $xzfilename) | cut -d'.' -f 1)
    gzfilename=$name.trace.gz

    echo $name $xzfilename

    cd ../traces_download/xz
    
    tracefilename=$name.trace
    if [ ! -e $xzfilename ] && [ ! -e $tracefilename ] && [ ! -e $gzfilename ]
    then
        echo "wget"
        wget -q -O $xzfilename $url
    fi
    if [ ! -e $tracefilename ] && [ ! -e $gzfilename ]
    then
        echo "xz"
        xz -d -v $xzfilename
    fi
    # TODO: Try to reduce the size of instructions
    if [ ! -e $gzfilename ]
    then
        echo "gz"
        gzip -v $tracefilename
    fi
    cd ../../ece552_cr_project
}

urls=($(<$pj_dir/scripts/urls_files))
for url in ${urls[@]};
do
    dl_trace $url
done
