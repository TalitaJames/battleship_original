# Plot the ratio of valid to total boards
#
# Made by Talita James, on 2024-07-22
#
import pandas as pd
import matplotlib.pyplot as plt

def getData(filename):
    # open a csv and read it in as a Dataframe
    dataFrame = pd.read_csv(filename)
    
    
    nList = dataFrame["n"].to_list()
    goodBoards = dataFrame["goodBoards"].to_list()
    totalBoards = [calculateTotalBoards(*getBoardState(n)) for n in nList]
    
    return nList, goodBoards, totalBoards


if __name__ == "__main__":
    nList, goodBoards, totalBoards = getData("./out/n_sizeData.csv")
    # print(nList, goodBoards, totalBoards)
    
    ratioBoards = [goodBoards[x]/totalBoards[x] for x in range(len(totalBoards))]
    
    plt.plot(nList,ratioBoards, label='Ratio Boards')
    # plt.yscale("log")
    
    plt.ylabel("Ratio of good/total boards ")
    plt.xticks(nList) 
    plt.title("Good Boards to Total Boards Ratio")
    plt.legend()
    plt.show()
