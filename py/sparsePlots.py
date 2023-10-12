import matplotlib.pyplot as plt
import sys

def readFileFlat(filename):
    data = []
    with open(filename, 'r') as f:
        for line in f:
            # add some occasional print statments to update progress
            print(len(data)) if len(data) % 1e6 == 0 else None
                
            line = line.strip()
            line = line.replace('[', '').replace(']', '')
            line = line.split(':')
            line[0]=[int(x) for x in line[0].split(',')]
            line[1]=int(line[1].strip())
            data.append(line)
    return data
  
def readFileZsplit(filename):
    zMax=5
    data=[[] for i in range(zMax)]
    with open(filename, 'r') as f:
        for line in f:
            # add some occasional print statments to update progress
            print(len(data)) if len(data) % 1e6 == 0 else None
                
            line = line.strip()
            line = line.replace('[', '').replace(']', '')
            line = line.split(':')
            line[0]=[int(x) for x in line[0].split(',')]
            line[1]=int(line[1].strip())
            
            z=abs(line[1])
            data[z].append(line[0])
    
    # convert data in the form [[x0,y0],[x1,y1],[x2,y2]] to [[x0,x1,x2],[y0,y1,y2]]
    dataFlattened=[[[],[]] for i in range(len(data))]
    for n,values in enumerate(data):
        for i in values:
            dataFlattened[n][0].append(i[0])
            dataFlattened[n][1].append(i[1])
    
    print("Data parsed!")
    
    return dataFlattened

# make a dot plot of the data
def plotDataFlat(data,filepath):
    # get the x and y values
    x = [i[0][0] for i in data]
    y = [i[0][1] for i in data]
    
    # get the z values
    z = [i[1] for i in data]
    # plot the data
    col_Vals=['xkcd:green', 'xkcd:coral', 'xkcd:ocean blue', 'xkcd:tangerine', 'xkcd:red']
    cols = [col_Vals[abs(i)] for i in z]
    
    
    plt.scatter(x, y, c=cols)
    plt.savefig(filepath+".png")
    plt.show()
    return plt
    
    
# make a dot plot of the data
def plotDataZsplit(data, filepath):

    # plot the data
    col=['#44BC44', '#C41272', '#CC8503', '#0C83AB','#43DA8E' ]
    lbl=['Good', 'Intersect', 'Placment', 'Unknown error', 'FATAL']
    sym=['o' for i in range(len(lbl))] #['o', 'x', '>', '.', ',']
    
    # plt.scatter(x, y, c=z,f=symb)
    for n,values in enumerate(data):
        # print(n,values)
        plt.scatter(values[0], values[1], c=col[n], label=lbl[n],marker=sym[n])
    
    plt.legend(loc='best')
    plt.savefig(filepath+".png")
    plt.show()
    return plt
    
    
if __name__ == '__main__':
    filename=sys.argv[1] #f"../out/sparseData/{sys.argv[1]}"
    # data=readFileFlat(filename)
    filepath=filename.rsplit(".",1)[0]
    plotDataZsplit(readFileZsplit(filename),filepath)
    print("saved!")
    
    
    