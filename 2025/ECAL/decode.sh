#!/bin/bash
para_num(){
  if test $# -lt 1; then
    echo "Please Input data file realpath(/data/DAQ/Beam_2510/*.dat)"
    exit 1
  fi
}
# 输入参数为DAQ服务器中的数据文件夹路径
para_num $@
DATAFILEPATH=$1

# Copy data file to local build directory
dirname=`basename $DATAFILEPATH`
mkdir /home/wangjunhao/beamtest/2025/ECAL/build/data/$dirname
pushd /home/wangjunhao/beamtest/2025/ECAL/build/data/$dirname
scp lunon@192.168.7.150:$DATAFILEPATH/data*ECAL*.dat .
ls -tr data*ECAL*.dat >filelist.txt

# Decode data
/home/wangjunhao/beamtest/2025/ECAL/build/ECALdig2root2025 /home/wangjunhao/beamtest/2025/ECAL/build/data/$dirname/filelist.txt
hadd total.root data*ECAL*.root 

# Draw graphs
/home/wangjunhao/beamtest/draw/drawval/build/drawval total.root

# remove dat files
rm data*ECAL*

popd