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
echo "Java Compiled!"

fileNameDate=$(date +%Y%m%d_%H%M)
java Runner $size "$ships"  | tee ~/code/battleship/out/javaOut_$fileNameDate.log

# cd ..
# echo "How many turns?"
# read turnCount
# python3 ./py_plotting/heatmap.py $turnCount