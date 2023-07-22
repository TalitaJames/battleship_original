import matplotlib.pyplot as plt
import numpy as np
import sys

#TODO the way i'm encoding the data here isn't great
def readFile(filename):
    with open(filename, "r") as f:
        lines = f.read().split('\n')
    lines = [x.split(": ") for x in lines[1:]]
    
    # list comprehension!
    data=[]
    legend=[]
    for l in lines:
        legend.append(f"Len {int(l[0])}")

        intNew = [int(x) for x in l[1].split(",")]
        lblsNew=[x+1 for x in range(len(intNew))]
        
        data.append([lblsNew,intNew])
 
    
    return data,legend

# and thus the plotting is bad bc the data is bad
def plot(data, legend, filename):
    plt.clf()
    
    mark = (',', '+', 'o', 'x', '*', 'v', '1','s', 'D')
    print(mark)
    for n,time  in enumerate(data):
        plt.scatter(time[0], time[1], marker=mark[n])
    plt.legend(legend)
    
    plt.title('Time taken to generate a grid of ships')
    
    plt.yscale('log')
    plt.ylabel('time (ms)')
    bottom, top = plt.ylim()  # return the current ylim
    plt.ylim((1e0, top))   # set the ylim to bottom, top*10
    
    plt.xticks(data[0][0])
    plt.xlabel('Ship count')
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
    data, legend = readFile("./results_timeData_java.txt")
    plot(data, legend, "results_timePlot.png")
    
    

    # total time
    individualSum = [sum(x[1]) for x in data]
    print(f"Total Run: {(sum(individualSum))/(60000):.2f} mins")
    print(f"\tNOTE: This time doesn't accont for time taken to put ships on board that crashed before completion")