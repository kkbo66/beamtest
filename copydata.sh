#!/bin/bash
para_num(){
  if test $# -lt 3; then
    echo "Please Input DATE & DATAFILENUMBEGIN & DATAFILENUMEND"
    exit 1
  fi
}

para_num $@
DATE=$1
let DATAFILENUMBEGIN=$2
let DATAFILENUMEND=$3
for (( i=$DATAFILENUMBEGIN; i<=$DATAFILENUMEND; i++ ))
do
  scp -r lunon@192.168.7.150:/data/DAQ/Beam_2510/${DATE}_${i} /home/kkbo/beamtest/data/${DATE}_${i}/ 
  echo "Data ${DATE}_${i} Copy Finished!"
  #scp -r /home/kkbo/beamtest/data/${DATE}_${i}/ ustc01:/ustcfs3/stcf/STCFUser/bwang/osk/beamdata/
  #echo "Data ${DATE}_${i} Upload Finished!"
  #rm -r /home/kkbo/beamtest/data/${DATE}_${i}/
done
echo "All Data Copy Finished!"
