#!/bin/bash
para_num(){
  if test $# -lt 2; then
    echo "Please Input DATE & TXTNAME"
    exit 1
  fi
}

file_recreate(){
  FILENAME=$1
  if [ -f $FILENAME ]; then
    rm $FILENAME
    touch $FILENAME
  else
    touch $FILENAME
  fi
}

para_num $@
DATE=$1
TXTNAME=$2

FILEPATH="/home/kkbo/beamtest/root/"
TXTPATH="./"
file_recreate ${TXTPATH}${TXTNAME}.txt
for file in `ls ${FILEPATH}${DATE}/*recon.root`
do
  echo $file >> ${TXTPATH}${TXTNAME}.txt
done


