# Starting script to puppet the battleship game
# Made by Talita James, on 2024-07-2

import os
import argparse
import time
import json

import helperFunctions

# given a board size (int), fleet (list of ints), and threadcount (int)  edit the sourcecode to match
def updateGameSettings(boardSize: int, fleet: list, threadCount = 8):
    checkDataValidity(args.size, fleet)
    fleetString = convertToCArray(fleet)
    
    # Change the header file to the new input args
    os.system(f'sed -r -i -E  "s/^\#define BOARD_SIZE .*$/\#define BOARD_SIZE {boardSize}/" src/boatsAndBoards.h')
    os.system(f'sed -r -i  "s/^const ship FLEET\[\] =.*;/const ship FLEET[] = {fleetString};/" src/boatsAndBoards.h')
    os.system(f'sed -r -i  "s/^int threadCount = .*;/int threadCount = {threadCount};/" src/boatsAndBoards.cpp')
    
    print(f"game with boardSize={boardSize} and fleet={fleet}, running threadCount={threadCount}\n-----------")


def convertToCArray(list: list) -> str:
    return "{" + ", ".join([str(x) for x in list]) + "}"


def build(clean = False):
    if clean:
        os.system("make clean")
    os.system("make")


def runTests():
    updateGameSettings(6, [3], args.threads)

    os.system("make clean")
    os.system("make tests")
    os.system("./build/unitTests.out")
    exit()

def checkDataValidity(boardSize, fleet):
    if len(fleet) == 1:
        print(f"There must be more than one ship in a fleet, but there are {helperFunctions.calculateTotalBoards(boardSize, fleet)} boards for the single ship f{fleet}")
        exit()

    for ship in fleet:
        if ship > boardSize or ship == 1:
            print("Each ship must be greater than size 1 and smaller than the board")
            exit()

if __name__ == "__main__":
    defaultSettings = json.load("defaultSettings.json")

    #region parse input args
    parser = argparse.ArgumentParser(description="Settings to change the running of the battleship computation code")
    parser.add_argument('-t', '--threads', type=int, help="num of threads", default=defaultSettings["threadCount"])
    parser.add_argument('-s', '--size', type=int,  default=defaultSettings["boardSize"], help="The board size")
    parser.add_argument('-f', '--fleet', type=str,  default=defaultSettings["fleet"])

    parser.add_argument('-c', '--clean', action='store_true', help="Will the build files get cleaned?")
    parser.add_argument('--utest', action='store_true', help="run the tests")
    args = parser.parse_args()
    #endregion

    if args.utest:
        runTests()

    fleet = [int(x) for x in args.fleet.split(",")] #turn "2,3" into [2,3]
    
    updateGameSettings(args.size, fleet, args.threads)
    build(args.clean)

    # run the game
    timestamp = time.strftime("%Y%m%d-%H%M%S",time.localtime())
    logFilename = f"./out/logs/{timestamp}_{defaultSettings["computerName"]}.log"
    returnVal = os.system(f"./build/runner.out 2>&1 | tee {logFilename}")

    if (returnVal != 0):
        print(f"\nERROR {returnVal}")
    else:
        print(f"\nDone! Logged in {logFilename}")