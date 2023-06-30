import json
import re
import sys
import numpy as np
import matplotlib.pyplot as plt

global gameSettings 
global fileDir

def setup():
    # loads the game settings
    with open('gameSettings.json') as f:
        global gameSettings 
        gameSettings = json.load(f)
    global fileDir
    fileDir = gameSettings['FILEDIR']
    print("Game setup done")
    

def readFiletoBoards(filename: str) -> list:
    
    # loads an encoded list of boards
    with open(filename) as f:
        gameBytes = f.read().split("\n")
    
    if gameBytes[-1] == "": # gets rid of the last empty line
        gameBytes = gameBytes[:-1]
    
    gameBytes = [re.sub("\[|\]","",i) for i in gameBytes] # gets rid of the [ brackets ]
    gameBytes = [[int(j) for j in i.split(",")] for i in gameBytes] # splits the string into a list of ints
    
    # next step is to decode each board into a list of boards (np arrays of bools)
    boardList = []

    for boardByte in gameBytes:
        singleBoardData = []

        for shipByte in boardByte:
            direction = shipByte % 2 == 1
            xyInfo = shipByte >> 1
            x = xyInfo // 10
            y = xyInfo  % 10
            # print(f"{shipByte}:  ({x},{y}) {'→' if direction else '↓'}\t({xyInfo})")
            
            singleBoardData.append((x,y,direction))
        
       
        newBoard = byteToBoard(singleBoardData)
        # displayBoard(newBoard)
        boardList.append(newBoard)
    
    return boardList


def byteToBoard(shipTuple) -> np.ndarray:
    size = gameSettings['SIZE']
    ship = gameSettings['SHIPS']
    
    assert len(ship) == len(shipTuple), "The number of ships and the number of bytes should match"


    singleBoard = np.zeros((size, size), dtype=bool)

    # place the ships on the board
    for i in range(len(ship)):
        length = ship[i]['size']
        x,y,direction = shipTuple[i]

        if direction:
            singleBoard[y,x:x+length] = True
            # singleBoard[x:x+length,y] = True
        else:
            # singleBoard[x,y:y+length] = True
            singleBoard[y:y+length,x] = True
    
    return singleBoard


# prints a singluar board in Board.toString() style
def displayBoard(board):
    print('-'*13)
    for col in board:
        print("[", end=" ")
        for row in col:
            print("X" if row else ".", end=" ")
        print("]", end="\n")

def heatmap(boardList):
    boardAvg = np.mean(boardList, axis=0)
    
    plt.imshow(boardAvg) 
    plt.colorbar()
    # plt.show()
    plt.savefig(f"./out/game0/{i}heatmap.png")
    plt.clf()



if __name__ == "__main__":
    setup()
    try:
        turnCount = sys.argv[1]
    except:
        turnCount=1

    for i in range(1,int(turnCount)+1):
        boardList = readFiletoBoards(f"{fileDir}{i}enc.txt")
        heatmap(boardList)
   

