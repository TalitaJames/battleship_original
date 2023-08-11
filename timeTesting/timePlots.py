import matplotlib.pyplot as plt
import numpy as np
import sys

def readFile(filename):
    with open(filename, "r") as f:
        lines = f.read().split('\n')
    
    if lines[0]=="":
        lines = lines[1:]

    lines = [x.split(": ") for x in lines]
    boardLengths = [x[0] for x in lines]

    times = [[int(y) for y in x]
                for z in lines
                for x in [z[1].split(",")]]
    shipCounts = [(x+1) for x in range(len(times[1]))] # presumably the tests are in accending consecutive order

    timeData=np.array(times)
    # timeData=np.transpose(timeData) # the first order is now ship length then board size (comment for opposite)

    return timeData, (boardLengths, shipCounts)
    
def plot(data, gameInfo, filename):
    boardLengths, shipCounts = gameInfo

    plt.clf()
    
    col=["#0a8f67","#d68b00","#0306a0","#990077","#fb282c"]
    mark = (',', '+', 'o', 'x', '*', 'v', '1','s', 'D')
    for i in range(len(boardLengths)):
        for j in range(len(shipCounts)):
            plt.scatter(boardLengths[i], data[i][j],c=col[j], marker=mark[j])
    
    legend = [f"{x} ships" for x in shipCounts]
    plt.legend(legend,bbox_to_anchor=(1.05, 1), loc='upper left')
    
    plt.title('Time taken to generate a grid of ships')
    
    plt.yscale('log')
    plt.ylabel('time (ms)')
    bottom, top = plt.ylim()  # gets y axis lims
    plt.ylim((1e0, top*1.2))   # set new y axis lims
    
    plt.xlabel('Board Size')
    plt.savefig(filename, bbox_inches='tight')

def avgData(minF, maxF):
    fileCount=maxF-minF+1
    data, allGameInfo = readFile(f"./repeats/0{minF}_results_timeData_java.txt")

    allData = np.zeros((fileCount,*data.shape)) 
    allData[0]=data

    for i in range(1,fileCount):
        data, gameI = readFile(f"./repeats/0{minF+i}_results_timeData_java.txt")
        assert allGameInfo==gameI, f"Game state for test {minF+i} isn't consistant to other tests"
        
        allData[i]=data

    return allData.mean(axis=0)

def plotAvgData(minF,maxF,filename):
    aData=avgData(minF,maxF)  
    _, gameInfo = readFile(f"./repeats/0{minF}_results_timeData_java.txt")
    plot(aData, gameInfo, filename)

    



if __name__ == "__main__":
    filename="timeTesting/repeats/12_results_timeData_java.txt"
    timeData, gameInfo = readFile(filename)
    plot(timeData, gameInfo, "results_timePlot.png")
    

    # total time
    # timeTotal = sum([sum(x[1]) for x in data])
    # print(f"Total Run: {(timeTotal)/(60000):.2f} mins")
    # print(f"\tNOTE: This time doesn't accont for time taken to put ships on board that crashed before completion")
    pass