#!/bin/sh
while getopts ":n:" opt; do
    case $opt in
        n ) FILENUM=$OPTARG ;;
    esac
done

for i in 100, 1000, 5000;
do
  for j in `seq 5 25`;
  do
    for k in `seq 1 6`;
    do
      timeout 300 ./tour -b $i -k $j -m $k -f $FILENUM
    done
  done
done   
