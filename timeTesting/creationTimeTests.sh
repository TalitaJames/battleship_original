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

for size in {2..10}
do
    echo -e "\nSize $size"

    echo -e -n "\n$size: " >> ../timeTesting/results_timeData_java.txt
    echo -e -n "\n$size: " >> ../timeTesting/results_timeData_bash.txt
    echo -e -n "\n$size: " >> ../timeTesting/results_shipCount.txt

    for ships in "[2:2]" "[2:2,3:3]" "[2:2,3:3,3:a]" "[2:2,3:3,3:a,4:4]" #"[2:2,3:3,3:a,4:4,5:5]"
    do
        startRun=$(date +%s%3N)
        startDisplay=$(date +"%T")
        echo -e "\t$startDisplay running test $ships"
        
        fileNameDate=$(date +%Y%m%d_%H)
        java -Xss20m Runner $size "$ships" | tee ../out/javaOut_$fileNameDate.log
        
        endRun=$(date +%s%3N) 

        deltaRunTime=$(expr $endRun - $startRun)
        echo -e -n "$deltaRunTime, " >> ../timeTesting/results_timeData_bash.txt

    done

    # Remove the last comma
    truncate -s -1 ../timeTesting/results_timeData_java.txt  
    truncate -s -2 ../timeTesting/results_timeData_bash.txt
    truncate -s -1 ../timeTesting/results_shipCount.txt
done

endTime=$(date +%s%3N)
deltaTotalTime=$(expr $endTime - $startTime)
echo "Tests Complete after $deltaTotalTime ms"

cd ../timeTesting
python3 timePlots.py
