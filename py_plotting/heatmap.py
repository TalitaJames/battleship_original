import json
import re
import numpy as np
import matplotlib.pyplot as plt

global gameSettings 

def setup():
    # loads the game settings
    with open('gameSettings.json') as f:
        global gameSettings 
        gameSettings = json.load(f)
    
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
        boardList = []
        for shipByte in boardByte:
            direction = shipByte % 2 == 1
            xyInfo = shipByte >> 1
            x = xyInfo // 10
            y = xyInfo  % 10
            # print(f"{shipByte}:  ({x},{y}) {'→' if direction else '↓'}")
            boardList.append((x,y,direction))
        
        newBoard = byteToBoard(boardList)
        displayBoard(newBoard)
        boardList.append(newBoard)



def byteToBoard(byte) -> np.ndarray:
    size = gameSettings['SIZE']
    ship = gameSettings['SHIPS']
    
    assert len(ship) == len(byte), "The number of ships and the number of bytes should match"


    singleBoard = np.zeros((size, size), dtype=bool)

    # place the ships on the board
    for i in range(len(ship)):
        length = ship[i]['size']
        x,y,direction = byte[i]
        # print(f"{length} {'→' if direction else '↓'} ({x},{y})")
        if direction:
            singleBoard[y,x:x+length] = True
            # singleBoard[x:x+length,y] = True
        else:
            # singleBoard[x,y:y+length] = True
            singleBoard[y:y+length,x] = True
    
    return singleBoard

    
def displayBoard(board):
    print('-'*13)
    for col in board:
        print("[", end=" ")
        for row in col:
            print("X" if row else ".", end=" ")
        print("]", end="\n")


if __name__ == "__main__":
    setup()
    boardList = readFiletoBoards("./out/smallBoards_5_byte.txt")
    # byte = [(0,1,False),(1,3,True)]
    
    # board = byteToBoard(byte)

