#!/bin/bash
para_num(){
  if test $# -lt 2; then
    echo "Please Input Energy & Mode"
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
mode=$2

TXTPATH="./energylist/"
if [ ${mode} -eq 1 ]; then
  file_create EnergyResolution.txt
  file_create EntryNumber.txt
  root -l -b -q "drawfitshower.cxx(\"${TXTPATH}${energy}MeVep.txt\",${energy})" > ${energy}MeVep_fit.log 
  num=$(grep "Resolution" ${energy}MeVep_fit.log | sed -n 's/.*Resolution: *\([0-9.]\+\) %.*/\1/p')
  entries=$(grep "Event entries" ${energy}MeVep_fit.log | sed -n 's/.*Event entries: *\([0-9]\+\).*/\1/p')
  if grep -q "^${energy}\b " EnergyResolution.txt; then
    sed -i "s/^${energy}\b .*/${energy} ${num}/" EnergyResolution.txt
  else
    echo "${energy} ${num}" >> EnergyResolution.txt
  fi
  echo "Energy: ${energy} MeV, Entry Number: ${entries}" >> EntryNumber.txt
  rm ${energy}MeVep_fit.log
elif [ ${mode} -eq 2 ]; then
  file_create EnergyResolutionmode2.txt
  file_create EntryNumbermode2.txt
  root -l -b -q "drawfitshower.cxx(\"${TXTPATH}${energy}MeVepmode2.txt\",${energy})" > ${energy}MeVep_fit.log 
  num=$(grep "Resolution" ${energy}MeVep_fit.log | sed -n 's/.*Resolution: *\([0-9.]\+\) %.*/\1/p')
  entries=$(grep "Event entries" ${energy}MeVep_fit.log | sed -n 's/.*Event entries: *\([0-9]\+\).*/\1/p')
  if grep -q "^${energy}\b " EnergyResolution.txt; then
    sed -i "s/^${energy}\b .*/${energy} ${num}/" >>  EnergyResolutionmode2.txt
  else
    echo "${energy} ${num}" >> EnergyResolutionmode2.txt
  fi
  echo "Energy: ${energy} MeV, Entry Number: ${entries}" >> EntryNumbermode2.txt
  rm ${energy}MeVep_fit.log
fi
