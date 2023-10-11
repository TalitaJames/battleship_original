#!/bin/bash

if [ "$#" -ne 3 ]; then
    size=7
    ships="[2:2,3:3,3:a]"
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


# for i in {0..10}; do
fileNameDate=$(date +%Y%m%d-%H%M)
# note to increase heap memory, use -Xmx
java Runner $size "$ships" $threads  | tee ../out/logs/$fileNameDate\_talita.log
# done
      

# cd ..
# echo "How many turns?"
# read turnCount
# python3 ./py_plotting/heatmap.py $turnCount