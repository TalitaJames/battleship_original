# Starting script to puppet the battleship game
# Made by Talita James, on 2024-07-2
#
import os
import argparse
import time
# import sys
# sys.path.insert(1, 'home/talita/code/battleship/src')
# print(sys.path)

import helperFunctions

# given a board size (int), fleet (list of ints), and threadcount (int)  edit the sourcecode to match
def updateGameSettings(boardSize, fleet, threadCount):
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
        print("There must be more than one ship in a fleet")
        exit()
        
    
    for ship in fleet:
        if ship > boardSize or ship == 1:
            print("Each ship must be greater than size 1 and smaller than the board")
            exit()

if __name__ == "__main__":
    #region parse input args
    parser = argparse.ArgumentParser(description="Settings to change the running of the battleship computation code")
    parser.add_argument('-t', '--threads', type=int, help="num of threads", default=8)
    parser.add_argument('-c', '--clean', action='store_true', help="Will the build files get cleaned?")
    parser.add_argument('-s', '--boardsize', type=int,  default=8)
    parser.add_argument('-f', '--fleet', type=str,  default="2,3")
    parser.add_argument('--utest', action='store_true', help="run the tests")
    args = parser.parse_args()
    #endregion

    if args.utest:
        runTests()

    fleetStr=args.fleet.split(",")
    fleet = [int(x) for x in fleetStr]
    
    checkDataValidity(args.boardsize, fleet)
    updateGameSettings(args.boardsize, fleet, args.threads)
    build(args.clean)
    timestamp = time.strftime("%y%m%d-%H%M%S",time.localtime())
    # os.system(f"./build/runner.out |& tee ./out/logs/{timestamp}.log")
    os.system(f"./build/runner.out")
    