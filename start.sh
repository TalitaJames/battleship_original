#!/bin/bash

if [ "$#" -ne 3 ]; then
    size=4
    ships="[2:2, 3:a, 3:b, 4:4]"
    threads=1
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