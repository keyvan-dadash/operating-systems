#!/bin/bash


trails=1000
for (( i = 1; i <= $1; i *= 2 ))
do
    ./build $i $trails
    wait
done
