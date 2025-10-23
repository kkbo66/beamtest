#!/bin/bash
para_num(){
  if test $# -lt 1; then
    echo "Please Input Energy"
    exit 1
  fi
}

file_create(){
  if [ ! -f $1 ]; then
    echo "File $1 does not exist. Creating it now."
    touch $1 
  fi
}

para_num $@
energy=$1

TXTPATH="./energylist/"

file_create EnergyResolution.txt
root -l -b -q "drawfitshower.cxx(\"${TXTPATH}${energy}MeVep.txt\",${energy})" > ${energy}MeVep_fit.log 
num=$(grep "Resolution" ${energy}MeVep_fit.log | sed -n 's/.*Resolution: *\([0-9.]\+\) %.*/\1/p')
if grep -q "^${energy}\b " EnergyResolution.txt; then
  sed -i "s/^${energy}\b .*/${energy} ${num}/" EnergyResolution.txt
else
  echo "${energy} ${num}" >> EnergyResolution.txt
fi
rm ${energy}MeVep_fit.log

