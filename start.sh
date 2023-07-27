#!/bin/bash

if [ "$#" -ne 2 ]; then
    size=5
    ships="[3:3, 2:2]"
else
    size=$1
    ships=$2
fi

cd ./src/
rm *.class
javac *.java 

java Runner $size "$ships"

# cd ..
# echo "How many turns?"
# read turnCount
# python3 ./py_plotting/heatmap.py $turnCount