#!/bin/bash

# if [ "$#" -ne 3 ]; then
#     size=7
#     ships="[2:2,3:3,3:a]"
#     threads=1
# else
    # size=$1
    # ships=$2
    # threads=$3
# fi


cd src
rm runner.out
make
./runner.out #$size $ships $threads

