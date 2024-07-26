# Starting script to puppet the battleship game
# Made by Talita James, on 2024-07-2
#
import os
import argparse
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

    updateGameSettings(6, [2,3,2], args.threads)

    os.system("make clean")
    os.system("make tests")
    os.system("./build/unitTests.out")
    exit()



if __name__ == "__main__":
    #region parse input args
    parser = argparse.ArgumentParser(description="Settings to change the running of the battleship computation code")
    parser.add_argument('-t', '--threads', type=int, help="num of threads", default=8)
    parser.add_argument('-c', '--clean', action='store_true', help="Will the build files get cleaned?")
    parser.add_argument('--utest', action='store_true', help="run the tests")
    args = parser.parse_args()
    #endregion

    if args.utest:
        runTests()

    for n in range(1,8):
        boardSize, fleet = helperFunctions.getBoardState(n)
    
        updateGameSettings(boardSize, fleet, args.threads)
        build(args.clean)
        os.system("./build/runner.out")
    