# Battleships: The C++ branch

## Todo:
### Monte Carlo notes:
- [ ] BUG visualise tree - only plot each node once
- [ ] fix maxDepth
- [x] UCB calculations are based on parent visitations, which parent?
    - [ ] does the node have to even store the parent? (i think not!)

### Plotting:
- [ ] Make a plot with 1->n (x axis) and 0->1 ratio of good boards to all boards
    - [ ] can i find an algorithm that calculates the line in this algorithm for any $n$?

- [x] BUG heatmap json probability grid out
    - works in `playGame_fromHitmask` but not the variable play method
    - without updating those variables and reseting them at the end the json returns `null`

### Future:
- [ ] memory something to speed up runtime?
- [ ] turn ./start.sh into python script? to better manage all the misc input args and increasing required functions


## Filename versions in `out/gamePlay`
- `v1.0` working but ship doesn't track as solved
- `v2.0` ship knows when solved
- `v2.1`
- `v2.2` `json` file tracks extra info (Fleet contains and IG maps)
- `v3` `INFOGAIN` (IG) now takes a variable number of shots before switching to `P-MAX`


## All board counts
```
2: 4,0,0,0,0
3: 12,36,24,0,0
4: 24,264,1608,1368,0
5: 40,956,16000,92480,80848
6: 60,2472,80648,1266864,6687136
7: 84,5268,280176,8728400,124757096
8: 112,9896,773368,39998648,1142253520
9: 144,17004,1825760,140730720,6788392256
10: 180,27336,3848040,411770168,30093975536
```


## Credit
[MCTS inspired code](https://github.com/michaelbzms/MonteCarloTreeSearch)