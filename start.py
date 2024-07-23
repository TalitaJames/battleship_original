# Starting script to puppet the battleship game
# Made by Talita James, on 2024-07-2
#

import os
# import subprocess

# given an int (n) return a tupple (boardSize, fleet)
def getBoardState(n):
    boardSize = 2*n
    fleet = []
    
    # make a boat for each k value (1, 2 ... n) following the formula
    # n - k + 1 + (2*k > n + 1)
    fleet = [n - k + 1 + (2*k > n + 1) for k in range(1,n+1)]
    
    return (boardSize, fleet)

# given a board size (int), fleet (list of ints), and threadcount (int)  edit the sourcecode to match
def updateGameSettings(boardSize, fleet, threadCount):
    fleetString = convertToCArray(fleet)
    
    # # Change the header file to the new input args
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
    
if __name__ == "__main__":
    for n in range(1,8):
        boardSize, fleet = getBoardState(n)
    
        threadCount = 8
        
        updateGameSettings(boardSize, fleet, threadCount)
        build(clean=True)
        os.system("./build/runner.out")
    