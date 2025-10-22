#!/bin/bash
para_num(){
  if test $# -lt 4; then
    echo "Please Input DATE & DATAFILENUMBEGIN & DATAFILENUMEND & MIPFILE"
    exit 1
  fi
}
para_num $@
DATE=$1
let DATAFILENUMBEGIN=$2
let DATAFILENUMEND=$3
MIPFILE=$4

let DEALTIME=$((($DATAFILENUMEND - $DATAFILENUMBEGIN + 1) / 10 + 1))
echo "Total Deal Time is $DEALTIME"

for ((i=0;i<DEALTIME;i++))
do
  let BEGINNUM=$(($DATAFILENUMBEGIN + i * 10))
  let ENDNUM=$(($BEGINNUM + 9))
  if test $ENDNUM -gt $DATAFILENUMEND; then
    let ENDNUM=$DATAFILENUMEND
  fi
  echo "Dealing with Data File No. $BEGINNUM to $ENDNUM"
  /home/kkbo/beamtest/rec.sh $DATE $BEGINNUM $ENDNUM
  ./getlist.sh $DATE 10GeVmip${MIPFILE}
  mv 10GeVmip${MIPFILE}.txt ./build/10GeVmip/
  cd build
  ./getmip 10GeVmip/10GeVmip${MIPFILE}.txt
  mv /home/kkbo/beamtest/draw/figureroot/MIP_Spectra.root /home/kkbo/beamtest/draw/figureroot/miproot/MIP_Spectra_${MIPFILE}_${BEGINNUM}_to_${ENDNUM}.root
  cd ..
  rm -r /home/kkbo/beamtest/root/$DATE
  rm -r /home/kkbo/beamtest/data/$DATE
done
