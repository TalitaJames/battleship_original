#!/bin/bash

# Compile the code
cd ./src/
rm *.class
javac *.java 
echo "Java Compiled"

# clear the file
echo -e -n "" > ../timeTesting/results_timeData_java.txt
echo -e -n "" > ../timeTesting/results_shipCount.txt


startTime=$(date +%s%3N)

threadCount=(1 20 100 500 2000000)
shipCount=("[2:2]" "[2:2,3:3]" "[2:2,3:3,3:a]" "[2:2,3:3,3:a,4:4]" "[2:2,3:3,3:a,4:4,5:5]")



for size in {2..10}
do
    echo -e "\nSize $size"

    echo -e -n "\n$size: " >> ../timeTesting/results_timeData_java.txt
    echo -e -n "\n$size: " >> ../timeTesting/results_shipCount.txt

    for ship in {0..3}
    do
        startDisplay=$(date +"%T")
        echo -e "\t$startDisplay running test ${shipCount[ship]} (${threadCount[ship]} threads)"
        
        fileNameDate=$(date +%Y%m%d_%H%M)
        java -Xss128m Runner $size "${shipCount[ship]}" ${threadCount[ship]} | tee ../out/javaOut_$fileNameDate.log
        

        deltaRunTime=$(expr $endRun - $startRun)

    done

    # Remove the last comma
    truncate -s -1 ../timeTesting/results_timeData_java.txt  
    truncate -s -1 ../timeTesting/results_shipCount.txt
done

endTime=$(date +%s%3N)
deltaTotalTime_ms=$(expr $endTime - $startTime)
deltaTotalTime=$(expr $deltaTotalTime_ms / 3600000)
echo "Tests Complete after $deltaTotalTime hrs ($deltaTotalTime_ms ms)"

cd ../timeTesting
python3 timePlots.py
