
def maxShipArangemtnets(boardLen=5, shipLens=[2,3]):
    # shipLens = [5,4,3,3,2] # standard ships for 10x10 grid

    totalArangments=1
    # for each ship, how many different starting spots? assuming only down/right
    for ship in shipLens:
        placments=2*boardLen*(boardLen-ship+1)
        totalArangments*=placments
        # print(f"\t{ship} has {placments}\tcumulative total: {totalArangments}")
        #multiply all the ways of arranging each one to get the combos 
        
    # print(f"{totalArangments} ways to arange a battleship board (including overlaps)")
    return totalArangments



if __name__ == "__main__":
    for length in range(2,10+1):
        print("\n")
        ships=[]
        for j in [2,3]: #,3,4,5]:
            ships.append(j)
            print(f"Length {length} with {ships} has {maxShipArangemtnets(length,ships)} combos")