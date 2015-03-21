#!/bin/bash
n=$(nproc);
for i in $(seq 1 $n); 
do 
    ./tour.sh -n $i &
done
wait