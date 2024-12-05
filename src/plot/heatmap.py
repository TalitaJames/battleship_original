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
    '''Load a json and recive the dictionary'''
    with open(filename) as f:
        gameHistory = json.load(f)
    return gameHistory

def heatmap(boardProbabilities, boardLayout, shotPosition = [], pastShotPositions = []):
    ''''''
    plt.clf()
    plt.imshow(boardProbabilities)
    if len(shotPosition) != 0:
        plt.plot(shotPosition[0], shotPosition[1], 'o', ms=15, color='red')

    if len(pastShotPositions) != 0:
        i=0
        for shotNumber, pastShot in enumerate(pastShotPositions):

            y,x=pastShot[0], pastShot[1]
            plt.plot(y,x, 'o', ms=15, color=(0.58, 0.58, 0.58, 0.5)) # a light grey
            plt.text(y-0.45, x-0.45, shotNumber+1, ha="left", va="top", fontsize=11, color="#484848")
            i+=1

            if (boardLayout[x][y] != -1):
                plt.text(y,x, boardLayout[x][y], ha="center", va="center", fontsize=12, color="black")
    plt.colorbar()
    return plt


def make_gif(gamePlayHistoryJson, filenameOut):
    ''' given the json data and filename, export a gif of progressive heatmaps'''

    # setup the temp directory for creating images
    tempFolder = "out/tmpGamePics"
    shutil.rmtree(tempFolder, ignore_errors=True)
    os.makedirs(tempFolder)
    boardLayout = gamePlayHistoryJson['board']

    previousCoords = [] #so far, no shots have been taken

    # save each image from the game
    for num, turnProb in enumerate(gamePlayHistoryJson["probabilityGrid"]):

        # fills number with zeros for sorting
        numZeroFilled = str(num).zfill((len(str(len(gamePlayHistoryJson['probabilityGrid'])))))
        filename = f"{tempFolder}/{numZeroFilled}"
        title = f"Playing {gamePlayHistoryJson['shotMethod']}: Turn {num+1} of {len(gamePlayHistoryJson['probabilityGrid'])}"
        if gamePlayHistoryJson['shotMethod']=="MCTS":
            title = title + f"(Tree iterations {gamePlayHistoryJson['iterations']})"

        plotHeatmap = heatmap(turnProb, boardLayout, pastShotPositions = previousCoords)
        plotHeatmap.title(title)
        plotHeatmap.savefig(f"{filename}-1_turn.png")

        # now take the same game and highlight the current shot position
        coord = gamePlayHistoryJson['shotRecord'][num]
        plotHeatmap_Circle = heatmap(turnProb, boardLayout, shotPosition = coord, pastShotPositions = previousCoords)
        plotHeatmap_Circle.title(title)
        plotHeatmap_Circle.savefig(f"{filename}-5_turn.png")
        previousCoords.append(coord)

    # with each image now saved, create a gif
    frameNames = sorted([image for image in glob.glob(f"{tempFolder}/*.png")])
    frameNames.append(frameNames[-1]) # append the last image a few times so it stays
    frameNames.append(frameNames[-1])
    frameNames.append(frameNames[-1])

    frames = [Image.open(img) for img in frameNames]
    frame_one = frames[0]
    frame_one.save(f"{filenameOut}.gif", format="GIF", append_images=frames,
               save_all=True, duration=450, loop=0, optimize=True)

    shutil.rmtree(tempFolder, ignore_errors=True) # clean up mess at the end (remove the temporary folder)


if __name__ == "__main__":
    # Get all json files in the directory, excluding any that already have a matching .gif filename
    filenameDir = "out/gamePlay/"
    filenames = {n for n in glob.glob(f"{filenameDir}/*.json")}

    for doneFile in glob.glob(f"{filenameDir}/*.gif"): # get rid of files w/ gifs
        filenames.discard(doneFile.replace(".gif", ".json"))

    print(f"START (making {len(filenames)} gif{'s' if len(filenames)>1 else ''})")

    for num,filename in enumerate(filenames):
        formatlessName = filename.rstrip(".json")
        if os.path.isfile(f"{formatlessName}.gif"): # don't remake old gifs
            continue
        try: # str.isNeumeric() would fix this
            newFilename = formatlessName.replace(filenameDir,'')
            int(newFilename[0])
        except ValueError:
            continue # don't make gifs of the IG colellated data

        gamePlayHistoryJson=readFileGameHistory(filename)
        # make_gif(gamePlayHistoryJson, formatlessName)
        make_gif(gamePlayHistoryJson, formatlessName)
        print(f"Created {filename} ({num+1}/{len(filenames)})\n")
