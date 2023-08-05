#!/bin/bash

if [ "$#" -ne 3 ]; then
    size=5
    ships="[3:3, 2:2]"
    threads=20
else
    size=$1
    ships=$2
    threads=$3
fi

cd ./src/
rm *.class
javac *.java 
echo "Java Compiled!"

fileNameDate=$(date +%Y%m%d_%H%M)
java -Xss128m Runner $size "$ships" $threads  | tee ~/code/battleship/out/javaOut_$fileNameDate.log

# cd ..
# echo "How many turns?"
# read turnCount
# python3 ./py_plotting/heatmap.py $turnCount