#!/bin/bash
para_num(){
  if test $# -lt 2; then
    echo "Please Input DATE & DATAFILENAME(.dat)! or more with ROOTNAME "
    exit 1
  fi
}

para_num $@
DATE=$1
DATAFILENAME=$2
ROOTNAME=$3

if [ -z "$ROOTNAME" ]; then
  ROOTNAME="re.root"
fi

#Copy data file to local build directory
scp lunon@192.168.7.150:/data/DAQ/Beam_2510/$DATE/$DATAFILENAME /home/kkbo/beamtest/data/
DATAFILE="/home/kkbo/beamtest/data/$DATAFILENAME"

if test ! -f $DATAFILE; then
  echo "$DATAFILE does not exist!"
  exit 1
fi

#Decode data
/home/kkbo/beamtest/2025/ECAL/build/ECALdig2root2025 $DATAFILE $ROOTNAME

ROOTFILE="/home/kkbo/beamtest/root/$ROOTNAME"

#Draw variables (example: nosie, pedestal, etc.)
#/home/kkbo/beamtest/draw/drawval/build/drawval $ROOTFILE 

#Draw sum waveform
#/home/kkbo/beamtest/draw/drawval/build/drawsumwave $ROOTFILE 

#Draw 5x5 sum waveform (max 5000 events)
/home/kkbo/beamtest/draw/drawval/build/draw5x5sumwave $ROOTFILE 

#Draw single channel waveform (input: x y event; default root: /home/kkbo/beamtest/root/re.root)
#/home/kkbo/beamtest/draw/drawwave/build/drawwave 3 3 100 

#Draw 5x5 single channel waveform (input: event; default root: /home/kkbo/beamtest/root/re.root)
#/home/kkbo/beamtest/draw/drawwave/build/draw5x5 100

#Draw high gain vs low gain ratio
#/home/kkbo/beamtest/draw/drawval/build/drawHLratio $ROOTFILE

#Draw MiP peak histogram (or more with cutvalue default cut 1000)
#/home/kkbo/beamtest/draw/elecal/build/drawPeak $ROOTFILE
#/home/kkbo/beamtest/draw/elecal/build/drawPeak $ROOTFILE 1500
