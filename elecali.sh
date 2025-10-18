#!/bin/bash
para_num(){
  if test $# -lt 2; then
    echo "Please Input ROOTFILELIST & ELECLIST!"
    exit 1
  fi
}

file_exist(){
  if [ ! -f $1 ]; then
    echo "$1 does not exist!"
    exit 1
  fi
}

para_num $@
ROOTFILELIST=$1
ELECLIST=$2

file_exist $ROOTFILELIST
file_exist $ELECLIST

#Get Peak Values
/home/kkbo/beamtest/draw/elecal/getPeak $ROOTFILELIST
#Draw Calibration Plots
/home/kkbo/beamtest/draw/elecal/drawCal $ROOTFILELIST $ELECLIST

