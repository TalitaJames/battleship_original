# Battleships: The C++ branch

## Todo:
- [ ] Monty Carlo Tree Search
- [ ] "sunk" implement properly now


### Bugs:
- Board len 5, fleet 2 returns 953 (should be 956)

### Future discussions
- Open AI/machine learning things
<!-- - what do we do with gecco? what makes (or what will make) this a genetic algorithm?  -->

## Filename versions in `out/gamePlay`
- `v1.0` working but ship doesn't track as solved
- `v2.0` ship knows when solved
- `v2.1`
- `v2.2` `json` file tracks extra info (Fleet contains and IG maps)
- `v3.0` `INFOGAIN` (IG) now takes a variable number of shots before switching to `P-MAX`


### Bugs:
- for any len, fleet 2, good boards are dependant on thread count (should return same regardless of thread count) 
    - originates in `dividePositions()` i *think*

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