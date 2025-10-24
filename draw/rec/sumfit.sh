#!/bin/bash
para_num(){
  if test $# -lt 2; then
    echo "Please Input Part & Mode"
    exit 1
  fi
}
para_num $@
part=$1
mode=$2
if [ ${part} -eq 1 ]; then
  ./fit.sh 20 ${mode}
  ./fit.sh 50 ${mode}
  ./fit.sh 100 ${mode}
  ./fit.sh 200 ${mode}
  ./fit.sh 300 ${mode}
  ./fit.sh 400 ${mode}
  ./fit.sh 500 ${mode}
  ./fit.sh 600 ${mode}
elif [ ${part} -eq 2 ]; then
  ./fit.sh 800 ${mode}
  ./fit.sh 1000 ${mode}
  ./fit.sh 1500 ${mode}
  ./fit.sh 2000 ${mode}
  ./fit.sh 2500 ${mode}
  ./fit.sh 3000 ${mode}
  ./fit.sh 3500 ${mode}
else
  echo "Part should be 1 or 2"
fi
