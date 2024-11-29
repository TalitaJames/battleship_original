import json
import matplotlib.pyplot as plt
import glob
import pandas as pd
import numpy as np
import pprint as pp
import math
import sys

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
        # print(f"{key} has len {len(value)}")
        value += [None] * (maxTurnValue - len(value))

    # make sure all are float (because float turns None into NaN)
    gameDataFrame = pd.DataFrame(data=gameDataDict, dtype='float64')

    # get rid of NaN values (turns that were not taken)

    return gameDataFrame

def gameDataToCompareData(gameDataFrame):
    for series_name, series in gameDataFrame.items():
        turnsPMAX = series[0]
        for x in range(len(series)):
            series[x] = series[x] - turnsPMAX

    return gameDataFrame


def dataFrameIntoBoxPlot(gameDataFrame, title="Effect of taking InfoGain Shots on Turns Taken"):
    # transpose the data, so each column is the value of the box plot at turn
    # gameDataFrame = gameDataFrame.T #FIXME needed for boxplots, not for plot
    gameDataNP = gameDataFrame.to_numpy()

    # FIXME the NaNs are not being ignored, rather causing the whole collumn to be ignored

    fig, ax = plt.subplots(1,1)
    # ax.boxplot(gameDataNP)
    ax.plot(gameDataNP,'-')

    plt.xlabel("Number of InfoGain Shots (TODO this should start from 0)")
    plt.ylabel("Turns Taken (as compared to pure P-MAX)")
    plt.title(title)
    plt.show()

if __name__ == "__main__":
    # Step 1 get the files and group the data
    filenameDir = "out/gamePlay/INFOGAIN_CHANGES_"

    # 1b) group the files
    filenamesGrouped = groupFileNames(filenameDir)
    # 1a) get the data
    gameType = sys.argv[1] if len(sys.argv)>=2 else "8_3"
    gameDataFrame = getGameTurnDataframe(filenamesGrouped[gameType], filenameDir)

    gameDataToCompareData(gameDataFrame)

    dataFrameIntoBoxPlot(gameDataFrame, f"Effect of taking InfoGain Shots on Turns Taken for {gameType}")

    pass
