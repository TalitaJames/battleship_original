# Battleships: The C++ branch

## Todo:
- [ ] a python script that takes an input and plots the heatmap (like the spreadsheets i've made)

- alt runing methods:
    - random weighted by probablily to hit
    - the info gain huristic (run 200 times)

- [ ] Clean up code
- [ ] Implement file i/o to better store data
- [ ] implement the heuristics below

|  | max | weighted random |
|---|---|---|
| $p$ | DONE | TODO |
| $p^2+(1-p)^2$ | NEXT | TODO |

Relative entropy
- 

### Future discussions
- Open AI/machine learning things
- what do we do with gecco? what makes (or what will make) this a genetic algorithm? 


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