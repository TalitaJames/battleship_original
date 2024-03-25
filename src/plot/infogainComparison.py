import json
import matplotlib.pyplot as plt
import glob
import pandas as pd
import numpy as np
import pprint as pp
import math

def findNumberedChar(char, msg, count = 1):
    allPos = [index for index, c in enumerate(msg) if c==char]
    return allPos[count-1]

def readFileGameHistory(filename) -> dict:
    with open(filename) as f:
        gameHistory = json.load(f)
    return gameHistory

def groupFileNames(fnameDir) -> dict:
    filenames = {n for n in glob.glob(f"{fnameDir}*.json")}
    groupedFileNames = {}
    for x in filenames:
        gameCode = x.replace(fnameDir, "")
        gameCode = gameCode[0:findNumberedChar('_', gameCode, 2)]
        if gameCode not in groupedFileNames:
            groupedFileNames[gameCode] = []
        groupedFileNames[gameCode].append(x)
        
    # returns a dict of the game code (boardSize_fleetSize) with a list of the associated filenames for each game
    return groupedFileNames 

def getGameTurnDataframe(filenames, fnameDir):
    gameDataDict = {}
    
    maxTurnValue = 0 
    for x in filenames:
        boardCode = x.replace(fnameDir, "")
        boardCode = boardCode[findNumberedChar('_', boardCode, 2)+1:findNumberedChar('_', boardCode, 3)]
        
        turns = readFileGameHistory(x)["TurnsTaken"]
        gameDataDict[boardCode] = turns
        maxTurnValue = len(turns) if len(turns) > maxTurnValue else maxTurnValue
    
    # ensure all values are the same length
    for (key,value) in gameDataDict.items():
        value += [None] * (maxTurnValue - len(value))

    gameDataFrame = pd.DataFrame(data=gameDataDict, dtype='float64')
    # make sure all are float (because float turns None into NaN)
    return gameDataFrame

def turnDataFrameIntoBoxPlot(gameDataFrame):
    
    # transpose the data, so each column is the value of the box plot at turn
    gameDataFrame = gameDataFrame.T 
    print(gameDataFrame)
    gameDataNP = gameDataFrame.to_numpy()
    print((gameDataNP))
    
    fig, ax = plt.subplots(1,1, figsize=(10, 10))
    ax.boxplot(gameDataNP)
    
    plt.xlabel("Number of InfoGain Shots")
    ax.set_xbound(0,len(gameDataNP[0]))
    ax.set_xticks(np.arange(0,len(gameDataNP[0]), 1))
    plt.ylabel("Turns Taken")
    plt.show()

if __name__ == "__main__":
    # # Step 1 get the files and group the data
    filenameDir = "out/gamePlay/INFOGAIN_CHANGES_"
    
    # # 1b) group the files
    filenamesGrouped = groupFileNames(filenameDir)
    # print(filenamesGrouped)
    # # 1a) get the data
    gameDataFrame = getGameTurnDataframe(filenamesGrouped["5_3"], filenameDir)

    # print(gameDataFrame)
    

    turnDataFrameIntoBoxPlot(gameDataFrame)
    
        
        
    pass
    