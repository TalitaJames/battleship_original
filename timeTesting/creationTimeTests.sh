#!/bin/bash

# Compile the code
cd ./src/
rm *.class
javac *.java 
echo "Java Compiled"

# clear the file
echo -e -n "" > ../timeTesting/results_timeData.txt
echo -e -n "" > ../timeTesting/results_shipCount.txt


for size in {2..7}
do
    echo -e "\nSize $size"

    echo -e -n "\n$size: " >> ../timeTesting/results_timeData.txt
    echo -e -n "\n$size: " >> ../timeTesting/results_shipCount.txt

    for ships in "[2:2]" "[2:2,3:3]" "[2:2,3:3,3:a]" "[2:2,3:3,3:a,4:4]" "[2:2,3:3,3:a,4:4,5:5]"
    do
        now=$(date +"%T")
        echo -e "\t$now running test $ships"
        java Runner $size "$ships"
    done

    # Remove the last comma
    truncate -s -1 ../timeTesting/results_timeData.txt  
    truncate -s -1 ../timeTesting/results_shipCount.txt
done

echo "Tests Complete"
cd ../timeTesting
python3 timePlots.py