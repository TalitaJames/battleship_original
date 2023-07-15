#!/bin/bash

# Compile the code
cd ./src/
rm *.class
javac *.java 
echo "Java Compiled"

# clear the file
echo -e -n "" > ../timeTesting/results_timeData_java.txt
echo -e -n "" > ../timeTesting/results_timeData_bash.txt
echo -e -n "" > ../timeTesting/results_shipCount.txt


startTime=$(date +%s%3N)

for size in {2..7}
do
    echo -e "\nSize $size"

    echo -e -n "\n$size: " >> ../timeTesting/results_timeData_java.txt
    echo -e -n "\n$size: " >> ../timeTesting/results_timeData_bash.txt
    echo -e -n "\n$size: " >> ../timeTesting/results_shipCount.txt

    for ships in "[2:2]" "[2:2,3:3]" "[2:2,3:3,3:a]" "[2:2,3:3,3:a,4:4]" "[2:2,3:3,3:a,4:4,5:5]"
    do
        startRun=$(date +%s%3N)
        startDisplay=$(date +"%T")
        echo -e "\t$startDisplay running test $ships"
        java Runner $size "$ships"
        
        endRun=$(date +%s%3N) # this accounts for the file IO which isn't quite fair but thats do danm bad

        deltaRunTime=$(expr $endRun - $startRun)
        echo -e -n "$deltaRunTime, " >> ../timeTesting/results_timeData_bash.txt

    done

    # Remove the last comma
    truncate -s -1 ../timeTesting/results_timeData_java.txt  
    truncate -s -1 ../timeTesting/results_timeData_bash.txt  
    truncate -s -1 ../timeTesting/results_shipCount.txt
done

endTime=$(date +%s%3N)
deltaTotalTime=$(expr $endTime - $startTime)
echo "Tests Complete after $deltaTotalTime ms or $(expr $deltaTotalTime / 60000000000) mins"

cd ../timeTesting
python3 timePlots.py
