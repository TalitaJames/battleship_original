#!/bin/bash

cd ./src/
rm *.class
javac *.java 
echo "Java Compiled!"

java Runner

cd ..
echo "How many turns?"
read turnCount
python3 ./py_plotting/heatmap.py $turnCount