import matplotlib.pyplot as plt
import sys

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
        divs=len(intNew)
        lblsNew=[x for x in range(len(intNew))]
        
        data.append([lblsNew,intNew])
 
    
    return data,legend

def plot(data, legend):
    plt.clf()
    for i in data:
        plt.scatter(i[0], i[1])
    plt.legend(legend)
    
    plt.title('Time taken to generate a grid of ships')
    
    plt.yscale('log')
    plt.ylabel('time (ms)')

    plt.xlabel('Ship count')
    plt.savefig("results_timePlot.png", bbox_inches='tight')


if __name__ == "__main__":
    data, legend = readFile("./results_timeData.txt")
    plot(data, legend)

    # total time
    individualSum = [sum(x[1]) for x in data]
    print(f"Total Run: {sum(individualSum):.2f}")