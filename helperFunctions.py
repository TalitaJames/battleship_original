# Misc helper functions used by a range of python scripts that do usefull things
# Made by Talita James on 2024-07-24

# given an int (n) return a tupple (boardSize, fleet)
# these parameters are from Yuvals initial quantum paper
def getBoardState(n):
    boardSize = 2*n
    fleet = []
    
    # make a boat for each k value (1, 2 ... n) following the formula
    # n - k + 1 + (2*k > n + 1)
    fleet = [n - k + 1 + (2*k > n + 1) for k in range(1,n+1)]
    
    return (boardSize, fleet)

# Calculates how many
def calculateTotalBoards(boardSize: int, fleet: list) -> int:
    allGoodBoards = 0
    
    for boat in fleet: 
        row = boardSize-boat
        allGoodBoards += row * boardSize * 2
        
    # calculate the total number of boards
    return allGoodBoards