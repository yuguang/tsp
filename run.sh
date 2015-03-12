#!/bin/sh
for i in `seq 50 1000`;
do
      for j in `seq 5 15`;
      do
	    for k in `seq 1 4`;
	    do
		  OUTPUT="$(./tour -b $i -r $j -m $k)"
	    done  
      done   
done   