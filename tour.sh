#!/bin/sh
while getopts ":n:" opt; do
    case $opt in
        n ) FILENUM=$OPTARG ;;
    esac
done

for i in 100, 1000, 5000;
do
  for j in `seq 5 19`;
  do
    SEED=$[ 1 + $[ RANDOM % 100 ]];
    for k in `seq 1 6`;
    do
      timeout 600 ./tour -b $i -s $SEED -k $j -m $k -f $FILENUM
    done
  done
done   

for i in 100, 1000, 5000;
do
  for j in `seq 20 25`;
  do
    SEED=$[ 1 + $[ RANDOM % 100 ]];
    for k in `seq 2 6`;
    do
      timeout 600 ./tour -b $i -s $SEED -k $j -m $k -f $FILENUM
    done
  done
done   

for i in 100, 1000, 5000;
do
  for j in `seq 26 32`;
  do
    SEED=$[ 1 + $[ RANDOM % 100 ]];
    for k in 2, 4, 6;
    do
      timeout 600 ./tour -b $i -s $SEED -k $j -m $k -f $FILENUM
    done
  done
done   