#!/bin/bash
para_num(){
  if test $# -lt 2; then
    echo "Please Input DATE & DATAFILENUM!"
    exit 1
  fi
}

para_num $@
DATE=$1
DATAFILENUM=$2

mkdir -p /home/kkbo/beamtest/data/$DATE 

#Copy data file to local build directory
scp -r lunon@192.168.7.150:/data/DAQ/Beam_2510/$DATE/data*ECAL*.dat /home/kkbo/beamtest/data/$DATE/

let num = 0
for file in /home/kkbo/beamtest/data/$DATE/data*ECAL*.dat
do
  if [ $num -eq $DATAFILENUM ]; then
    DATAFILE=$file
    DATAFILENAME=$(basename $DATAFILE)
    ROOTNAME=${DATAFILENAME/.dat/.root}
    #/home/kkbo/beamtest/2025/ECAL/build/ECALdig2root2025 $DATAFILE $ROOTNAME
  fi
  let num=num+1
done

#Decode data

