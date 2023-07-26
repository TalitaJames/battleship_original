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
    shipCounts = [(x+2) for x in range(len(times[1]))] # presumably the tests are in accending consecutive order

    timeData=np.array(times)
    # timeData=np.transpose(timeData) # the first order is now ship length then board size (comment for opposite)

    return timeData, (boardLengths, shipCounts)
    

# and thus the plotting is bad bc the data is bad
def plot(data, gameInfo, filename):
    boardLengths, shipCounts = gameInfo

    plt.clf()
    
    col=["#0a8f67","#d68b00","#0306a0","#990077","#fb282c"]
    mark = (',', '+', 'o', 'x', '*', 'v', '1','s', 'D')
    for i in range(len(boardLengths)):
        for j in range(len(shipCounts)):
            plt.scatter(boardLengths[i], data[i][j],c=col[j], marker=mark[j])
    
    legend = [f"{x} ships" for x in shipCounts]
    plt.legend(legend)
    
    plt.title('Time taken to generate a grid of ships')
    
    plt.yscale('log')
    plt.ylabel('time (ms)')
    bottom, top = plt.ylim()  # gets y axis lims
    plt.ylim((1e0, top*1.2))   # set new y axis lims
    
    # plt.xticks(data[0][0])
    plt.xlabel('Board Size')
    plt.savefig(filename, bbox_inches='tight')

#FIXME not finished yet
def avgData():
    allData=[]
    for x in range(5,8):
        data, legend = readFile(f"./repeats/0{x}_results_timeData_java.txt")
        data=[x[1] for x in data]
        allData.append(data)

    my_array = np.array(allData)
    np.mean(allData, axis=1)

    print(my_array)
    print('-'*20)
    my_array.mean(axis=0)
    print(my_array)


if __name__ == "__main__":
    timeData, gameInfo = readFile("./results_timeData_java.txt")
    plot(timeData, gameInfo, "results_timePlot.png")
    
    

    # total time
    # individualSum = [sum(x[1]) for x in data]
    # print(f"Total Run: {(sum(individualSum))/(60000):.2f} mins")
    # print(f"\tNOTE: This time doesn't accont for time taken to put ships on board that crashed before completion")