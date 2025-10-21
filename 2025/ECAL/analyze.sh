#!/bin/bash
para_num(){
  if test $# -lt 1; then
    echo "Please Input data file realpath(/data/DAQ/Beam_2510/*.dat)"
    exit 1
  fi
}

para_num $@
DATAFILEPATH=$1
ROOTNAME=$2


if [ -z "$ROOTNAME" ]; then
  ROOTNAME="re.root"
fi

#Copy data file to local build directory
scp lunon@192.168.7.150:$DATAFILEPATH /home/wangjunhao/beamtest/2025/ECAL/build/data
DATAFILENAME=$(basename $DATAFILEPATH)
DATAFILE="/home/wangjunhao/beamtest/2025/ECAL/build/data$DATAFILENAME"

if test ! -f $DATAFILE; then
  echo "$DATAFILE does not exist!"
  exit 1
fi

#Decode data
/home/wangjunhao/beamtest/2025/ECAL/build/ECALdig2root2025 $DATAFILE $ROOTNAME

ROOTFILE="/home/wangjunhao/beamtest/root/$ROOTNAME"

#Draw variables (example: nosie, pedestal, etc.)
/home/wangjunhao/beamtest/draw/drawval/build/drawval $ROOTFILE 

#Draw sum waveform
#/home/wangjunhao/beamtest/draw/drawval/build/drawsumwave $ROOTFILE 

#Draw 5x5 sum waveform (max 5000 events)
/home/wangjunhao/beamtest/draw/drawval/build/draw5x5sumwave $ROOTFILE 

#Draw single channel waveform (input: x y event; default root: /home/wangjunhao/beamtest/root/re.root)
#/home/wangjunhao/beamtest/draw/drawwave/build/drawwave 3 3 100 

#Draw 5x5 single channel waveform (input: event; default root: /home/wangjunhao/beamtest/root/re.root)
#/home/wangjunhao/beamtest/draw/drawwave/build/draw5x5 100

#Draw high gain vs low gain ratio
# /home/wangjunhao/beamtest/draw/drawval/build/drawhlratio $ROOTFILE

#Draw MiP peak histogram (or more with cutvalue default cut 1000)
/home/wangjunhao/beamtest/draw/elecal/build/drawPeak $ROOTFILE
#/home/wangjunhao/beamtest/draw/elecal/build/drawPeak $ROOTFILE 1500
