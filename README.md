# Battleship!
(Best if ships have a distinct symbol, but doesn't break anything)

## Change log?
- Added command line args (i preffer the style of .json, but i agree it was quicker & simpler)
- Performance testing 
    <!-- ![](timeTesting/results_timePlot.png) -->



## Todo
- [ ] implement this
    - [x] next byte method
```
for all Byte arrays:
    turn into Board
    check conflict (with existance of byte as a board, and current moves)
    if conflict:
        add to heatmap
    forget byte (and board) and move on
```
- [ ] refactor [heatmap](./py/heatmap.py) to account for these changes
- [x] encode each board as a Byte
    - [ ] Could that be generatable rather than making boards & converting?
    - [ ] fix the decoding error
    - [ ] the range thing
- [x] fix the copy issue ~~(or design a better way around it)~~
    - [ ] not have it save files as it coppies (work out what `deepCopy()` is actuall doing and not have it save the file names)
- [ ] Not a fan of how coords are being stored, would preffer cpp `pair` or py `tuple` style

## Encoding & Decoding the data:

- [ ] should export in the same order specified in command line args

The remaining data (x,y) and direction is encoded for each ship as follows:
position $p=x\times 10 + y$, then bitshifted left and final bit is the direction (`1` for horizontal $\rightarrow$)

## Input Args
`java Runner <BOARD_SIZE> <BOAT_STR> [turns]`
- turns is only for [heatmap](./py/heatmap.py)
- but all follow this standard


## Ships know what?
There should be a way to distinguish which ship is which in the return value, such that once a ship is identified 

eg if it knows that '2' is in (0,2 and 0,3), then it can't be the second ship in this list

```
[. . 2 2 3 ]
[. . . . 3 ]
[. . . . 3 ]
[. . . . . ]
[. . . . . ]

[. . 3 3 3 ]
[. . . . 2 ]
[. . . . 2 ]
[. . . . . ]
[. . . . . ]
```


## links & things
[Data genetics battleship blog](http://www.datagenetics.com/blog/december32011/), 
[Yuval's Python notebook](https://colab.research.google.com/drive/1NlMnu8ftS8EpXtlaJMqdnYkbQQUpWEXm#scrollTo=q3jCF0oooxSv)








