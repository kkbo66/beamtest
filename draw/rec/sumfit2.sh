#!/bin/bash
para_num(){
  if test $# -lt 1; then
    echo "Please Input  Mode"
    exit 1
  fi
}
para_num $@
mode=$1
./fit2.sh 200 ${mode} 3 2 3x3 0.2
./fit2.sh 300 ${mode} 3 2 5x5 0.2
./fit2.sh 400 ${mode} 3 2 3x3 0.2
./fit2.sh 500 ${mode} 3 2 5x5 0.2
./fit2.sh 600 ${mode} 3 2 5x5 0.2
./fit2.sh 800 ${mode} 10 5 5x5 0.2
./fit2.sh 1000 ${mode} 10 5 5x5 0.2
./fit2.sh 1500 ${mode} 10 5 5x5 0.2
./fit2.sh 2000 ${mode} 10 5 5x5 0.2
./fit2.sh 2500 ${mode} 10 5 5x5 0.2
./fit2.sh 3000 ${mode} 10 5 5x5 0.2
./fit2.sh 3500 ${mode} 10 5 5x5 0.2
