import json
import os
import matplotlib.pyplot as plt
import glob
import sys
import pprint
import math
import shutil
from PIL import Image

def readFileGameHistory(filename) -> dict:
    with open(filename) as f:
        gameHistory = json.load(f)
    return gameHistory

def heatmap(boardProbabilities, shotPosition = [], pastShotPositions = []):
    plt.clf()
    plt.imshow(boardProbabilities) 
    if len(shotPosition) != 0:
        plt.plot(shotPosition[0], shotPosition[1], 'o', ms=15, color='red')
    if len(pastShotPositions) != 0:
        for pastShot in pastShotPositions:
            plt.plot(pastShot[0], pastShot[1], 'o', ms=15, color=(0.58, 0.58, 0.58, 0.5)) # a light grey
    plt.colorbar()
    return plt

def allShips(gamePlayHistoryJson):
    plt.clf()
    plt.xlim(0,gamePlayHistoryJson['BOARD_SIZE'])
    plt.ylim(0,gamePlayHistoryJson['BOARD_SIZE'])
    plt.title(f"Playing {gamePlayHistoryJson['shotMethod']}: All Ship Positions")


    for x, row in enumerate(gamePlayHistoryJson['board']):
        for y, position in enumerate(row):
            if position != -1:
                plt.plot(x+0.5, y+0.5, 'x', ms=25, color='red')
    return plt

def make_gif(gamePlayHistoryJson, filenameOut, includeAllShips = False):
    # setup the temp directory for creating images
    tempFolder = "out/tmpGamePics"
    shutil.rmtree(tempFolder, ignore_errors=True) 
    os.makedirs(tempFolder)

    if includeAllShips:
        filename = f"{tempFolder}/{'0'.zfill((len(str(len(gamePlayHistoryJson['probabilityGrid'])))))}"
        allShips(gamePlayHistoryJson).savefig(f"{filename}-0_shipPositions.png")

    previousCoords = []
    # save each image from the game
    for num,turnProb in enumerate(gamePlayHistoryJson["probabilityGrid"]):
        # fills number with zeros for better sorting
        numZeroFilled = str(num).zfill((len(str(len(gamePlayHistoryJson['probabilityGrid'])))))
        filename = f"{tempFolder}/{numZeroFilled}"
        title = f"Playing {gamePlayHistoryJson['shotMethod']}: Turn {num+1} of {len(gamePlayHistoryJson['probabilityGrid'])}"
        
        plotHeatmap = heatmap(turnProb, pastShotPositions = previousCoords)
        plotHeatmap.title(title)
        plotHeatmap.savefig(f"{filename}-1_turn.png")
    
        # there is one more probabilityGrid than there are shots (starting one)
        if num+1 != len(gamePlayHistoryJson['probabilityGrid']): 
            coord = gamePlayHistoryJson['shotRecord'][num] 
            plotHeatmap_Circle = heatmap(turnProb, coord, previousCoords)
            plotHeatmap_Circle.title(title)
            plotHeatmap_Circle.savefig(f"{filename}-5_turn.png")
            previousCoords.append(coord)
        
    # with each image now saved, create a gif
    frameNames = sorted([image for image in glob.glob(f"{tempFolder}/*.png")])
    frames = [Image.open(img) for img in frameNames]
    frame_one = frames[0]
    frame_one.save(f"{filenameOut}.gif", format="GIF", append_images=frames,
               save_all=True, duration=450, loop=0, optimize=True)
    
    # clean up mess at the end
    shutil.rmtree(tempFolder, ignore_errors=True) 
    

if __name__ == "__main__":
    filenameDir = "out/gamePlay/"
    filenames = [n for n in glob.glob(f"{filenameDir}/*.json")]
    
    for num,file in enumerate(filenames):
        formatlessName = file.rstrip(".json")
        print(f" Progress {num/len(filenames)*100:.2f}%", end="\r")
        if os.path.isfile(f"{formatlessName}.gif"): # don't remake old gifs
            continue
        gamePlayHistoryJson=readFileGameHistory(file)
        make_gif(gamePlayHistoryJson, formatlessName)
    
    
