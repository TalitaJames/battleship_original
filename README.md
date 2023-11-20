# Battleships: The C++ branch

113 mins (avg between 2 runs at 10 w/ 5 fleet, 1 thread)
## Todo:
- From checking an individual board, return a boolean flattened int array (1,0)s of the ship positions
- for each worker, make a heatmap (summated collection of the flattened int array)
- summate them at the end (between all workers)
- divide by the total number of boards for that hitmask (x) to get a % that any square in a heatmap will have a ship there


### Bugs:
- Board len 5, fleet 2 returns 953 (should be 956)


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