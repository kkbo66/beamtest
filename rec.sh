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

mkdir -p /home/kkbo/beamtest/data/$DATE 
mkdir -p /home/kkbo/beamtest/root/$DATE

#Copy data file to local build directory
for (( i=$DATAFILENUMBEGIN; i<=$DATAFILENUMEND; i++ ))
do
  if [ -f /home/kkbo/beamtest/data/$DATE/data_ECAL*_${i}.dat ]; then
    echo "File data_ECAL*_${i}.dat already exists. Skipping download."
    continue
  fi
  scp -r lunon@192.168.7.150:/data/DAQ/Beam_2510/$DATE/data_ECAL*_${i}.dat /home/kkbo/beamtest/data/$DATE/
done

cd /home/kkbo/beamtest/2025/ECAL/build/

for file in /home/kkbo/beamtest/data/$DATE/data*ECAL*.dat
do
  DATAFILE=$file
  DATAFILENAME=$(basename $DATAFILE)
  ROOTNAME=${DATAFILENAME/.dat/.root}
  echo "Processing File: $DATAFILE"
  echo "Output Root File: $ROOTNAME"
  ./ECALdig2root2025 $DATAFILE $ROOTNAME
  mv $ROOTNAME /home/kkbo/beamtest/root/$DATE/
  DIGIROOTNAME=${DATAFILENAME/.dat/_digi.root}
  echo "Digi Root File: $DIGIROOTNAME"
  echo "Running ECALDigi..."
  echo "Input Root File: /home/kkbo/beamtest/root/$DATE/$ROOTNAME"
  ./ECALDigi /home/kkbo/beamtest/root/$DATE/$ROOTNAME $DIGIROOTNAME
  mv $DIGIROOTNAME /home/kkbo/beamtest/root/$DATE
  RECONROOTNAME=${DATAFILENAME/.dat/_recon.root}
  echo "Reconstructed Root File: $RECONROOTNAME"
  echo "Running Reconstruction..."
  ./Reconstruction /home/kkbo/beamtest/root/$DATE/$DIGIROOTNAME $RECONROOTNAME
  mv $RECONROOTNAME /home/kkbo/beamtest/root/$DATE
  echo "Done!"
done

cd ~/beamtest/

rm -rf /home/kkbo/beamtest/data/$DATE/data*ECAL*.dat
