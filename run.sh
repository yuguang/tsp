#!/bin/sh
for i in `seq 50 1000`;
do
  for j in `seq 5 25`;
  do
    for k in `seq 1 5`;
    do
      timeout 900 ./tour -b $i -k $j -m $k
    done
  done
done   
