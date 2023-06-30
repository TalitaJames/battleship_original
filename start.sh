#!/bin/bash

cd ./src/
rm *.class
javac *.java 

echo "Java Compiled!"
java Runner

cd ..
python3 ./py_plotting/heatmap.py