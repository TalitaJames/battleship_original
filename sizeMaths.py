BOARD_LENGTH = 5
SHIP_LENGTH = [2,3]
# SHIP_LENGTH = [5,4,3,3,2] # standard ships for 10x10 grid

totalArangments=1
# for each ship, how many different spots can it start on the board 
# assuming they can only go down/right
for ship in SHIP_LENGTH:
    placments=2*BOARD_LENGTH*(BOARD_LENGTH-ship+1)
    totalArangments*=placments
    print(f"\t{ship} has {placments}\tcumulative total: {totalArangments}")
#multiply all the ways of arranging each one to get the combos 

print(f"{totalArangments} ways to arange a battleship board (including overlaps)")