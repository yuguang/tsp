#!/bin/sh
for i in `seq 1 200`;
do
  for j in `seq 5 25`;
  do
    for k in `seq 1 6`;
    do
      timeout 300 ./tour -b $(($i*50)) -k $j -m $k
    done
  done
done   
