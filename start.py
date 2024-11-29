# Starting script to puppet the battleship game
# Made by Talita James, on 2024-07-2

import os
import argparse
import time
import json


def updateGameSettings(boardSize: int, fleet: list, threadCount = 8):
    ''' Updates the C++ code to match the given parameters
        - boardSize (int)
        - fleet (list of ints) ie [2,3] each representing the length of the ships
        - threadcount (int) number of threads c++ will make
    '''
    checkDataValidity(args.size, fleet)
    fleetString = convertToCArray(fleet)

    # Change the header file to the new input args
    os.system(f'sed -r -i -E  "s/^\#define BOARD_SIZE .*$/\#define BOARD_SIZE {boardSize}/" src/boatsAndBoards.h')
    os.system(f'sed -r -i  "s/^const ship FLEET\[\] =.*;/const ship FLEET[] = {fleetString};/" src/boatsAndBoards.h')
    os.system(f'sed -r -i  "s/^int threadCount = .*;/int threadCount = {threadCount};/" src/boatsAndBoards.cpp')

    print(f"Game with boardSize={boardSize} and fleet={fleet}, running threadCount={threadCount}\n-----------")


def convertToCArray(listInput: list) -> str:
    '''Turns a list of numbers into the C style [2,3] becomes {2, 3}'''
    return "{" + ", ".join([str(x) for x in listInput]) + "}"


def build(clean = False):
    '''Compiles the code, with an optional initial cleaning (removes pre compiled files)'''
    if clean:
        os.system("make clean")
    os.system("make")


def runTests():
    '''Runs the unittests then exits'''
    updateGameSettings(6, [2,3], args.threads)

    os.system("make clean")
    os.system("make tests")
    os.system("./build/unitTests.out") #TODO read the return value and if errored respond differently?
    exit()


def checkDataValidity(boardSize, fleet):
    '''Ensures the game settings fall in the required parameters
        - More than one ship
        - No ships are size 1, or larger than the board
    '''
    if len(fleet) == 1:
        print(f"There must be more than one ship in a fleet to run the code, but there are {calculateTotalBoards(boardSize, fleet)} boards for the single ship f{fleet}")
        exit()

    for ship in fleet:
        if ship > boardSize or ship == 1:
            print("Each ship must be greater than size 1 and smaller than the board")
            exit()


def calculateTotalBoards(boardSize: int, fleet: list) -> int:
    ''' Counts the total number of starting permutations that may fit in a given board size
        Doesn't account for collisions.
    '''
    allGoodBoards = 0

    for boat in fleet:
        row = boardSize-boat # ie a boat len 4 in a 10x10 grid may fit 6 times when starting from the left going right
        allGoodBoards += row * boardSize * 2 # accounts for each board row (ie ten down) *2 to account for the columns

    # calculate the total number of boards
    return allGoodBoards


if __name__ == "__main__":
    with open("defaultSettings.json") as f:
        defaultSettings = json.load(f)

    # Initialise the command line arguments
    parser = argparse.ArgumentParser(description="Settings to change the running of the battleship computation code")
    parser.add_argument('-t', '--threads', type=int, help="num of threads", default=defaultSettings["threadCount"])
    parser.add_argument('-s', '--size', type=int,  default=defaultSettings["boardSize"], help="The board size")
    parser.add_argument('-f', '--fleet', type=str,  default=defaultSettings["fleet"])

    parser.add_argument('-c', '--clean', action='store_true', help="Will the build files get cleaned?")
    parser.add_argument('--utest', action='store_true', help="run the tests")
    args = parser.parse_args()
    #end command line arguments

    if args.utest:
        runTests()

    fleet = [int(x) for x in args.fleet.split(",")] #turn "2,3" into [2,3]

    updateGameSettings(args.size, fleet, args.threads)
    build(args.clean)

    # run the game
    timestamp = time.strftime("%Y%m%d-%H%M%S",time.localtime())
    logFilename = f"./out/logs/{timestamp}_{defaultSettings['computerName']}.log" #creates a log file named "YYYMMDD-HHMMSS_computername.log"
    returnVal = os.system(f"./build/runner.out 2>&1 | tee {logFilename}")

    if (returnVal != 0):
        print(f"\nERROR {returnVal}")
    else:
        print(f"\nDone! Logged in {logFilename}")