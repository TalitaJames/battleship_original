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

def heatmap(boardProbabilities, shotBool, shotPosition = [-1,-1] ):
    plt.clf()
    plt.imshow(boardProbabilities) 
    if shotBool:
        plt.plot(shotPosition[0], shotPosition[1], 'o', ms=15, color='red')
    plt.colorbar()
    return plt


def make_gif(gamePlayHistoryJson, filenameOut):
    tempFolder = "out/tmpGamePics"
    shutil.rmtree(tempFolder, ignore_errors=True) 
    os.makedirs(tempFolder)
    
    shotMethod = gamePlayHistoryJson['shotMethod']
    
    # save each image from the game
    for num,turnProb in enumerate(gamePlayHistoryJson["probabilityGrid"]):
        # fills number with zeros for better sorting
        numZeroFilled = str(num).zfill((len(str(len(gamePlayHistoryJson['probabilityGrid'])))))
        pltTitle = f"Playing {shotMethod}: Turn {num+1} of {len(gamePlayHistoryJson['probabilityGrid'])}"
        
        plotHeatmap = heatmap(turnProb, False)
        plotHeatmap.title(pltTitle)
        plotHeatmap.savefig(f"{tempFolder}/{numZeroFilled}-0_turn.png")
    
        # there is one more probabilityGrid than there are shots (starting one)
        if num+1 != len(gamePlayHistoryJson['probabilityGrid']): 
            coord =gamePlayHistoryJson['shotRecord'][num] 
            plotHeatmap_Circle = heatmap(turnProb, True, coord)
            plotHeatmap_Circle.title(pltTitle)
            plotHeatmap_Circle.savefig(f"{tempFolder}/{numZeroFilled}-5_turn.png")
    
    # with each image now saved, create a giff
    frameNames = sorted([image for image in glob.glob(f"{tempFolder}/*.png")])
    frames = [Image.open(img) for img in frameNames]
    frame_one = frames[0]
    frame_one.save(f"{filenameOut}.gif", format="GIF", append_images=frames,
               save_all=True, duration=450, loop=0)
    
    # clean up mess at the end
    shutil.rmtree(tempFolder, ignore_errors=True) 
    

if __name__ == "__main__":
    filenameDir = "out/gamePlay/"
    filenames = [n for n in glob.glob(f"{filenameDir}/*.json")]
    
    # 
    for num,file in enumerate(filenames):
        gamePlayHistoryJson=readFileGameHistory(file)
        make_gif(gamePlayHistoryJson, file.rstrip(".json"))
        print(f" Progress {num/len(filenames)*100:.2f}%", end="\r")
    
    
