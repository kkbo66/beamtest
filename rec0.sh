#!/bin/bash
para_num(){
  if test $# -lt 2; then
    echo "Please Input DATE & DATAFILENUM!"
    exit 1
  fi
}

para_num $@
DATE=$1
let DATAFILENUM=$2

mkdir -p /home/kkbo/beamtest/data/$DATE 
mkdir -p /home/kkbo/beamtest/root/$DATE

#Copy data file to local build directory
scp -r lunon@192.168.7.150:/data/DAQ/Beam_2510/$DATE/data*ECAL*.dat /home/kkbo/beamtest/data/$DATE/

let num=0
for file in /home/kkbo/beamtest/data/$DATE/data*ECAL*.dat
do
  if [ $num -lt $DATAFILENUM ]; then
    DATAFILE=$file
    DATAFILENAME=$(basename $DATAFILE)
    ROOTNAME=${DATAFILENAME/.dat/.root}
    echo "Processing File: $DATAFILE"
    echo "Output Root File: $ROOTNAME"
    #/home/kkbo/beamtest/2025/ECAL/build/ECALdig2root2025 $DATAFILE $ROOTNAME
    #mv $ROOTNAME /home/kkbo/beamtest/root/$DATE/
    #DIGIROOTNAME=${DATAFILENAME/.dat/_digi.root}
    #/home/kkbo/beamtest/2025/ECAL/build/ECALDigi /home/kkbo/beamtest/root/$DATE/$ROOTNAME $DIGIROOTNAME
    #mv $DIGIROOTNAME /home/kkbo/beamtest/root/$DATE
    #RECONROOTNAME=${DATAFILENAME/.dat/_recon.root}
    #/home/kkbo/beamtest/2025/ECAL/build/Reconstruction /home/kkbo/beamtest/root/$DATE/$DIGIROOTNAME $RECONROOTNAME
    #mv $RECONROOTNAME /home/kkbo/beamtest/root/$DATE
    echo "Done!"
  fi
  let num=num+1
done

rm -rf /home/kkbo/beamtest/data/$DATE/data*ECAL*.dat
