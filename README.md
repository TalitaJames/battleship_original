# Battleship!
(Best if ships have a distinct symbol, but doesn't break anything)

## Change log?
- tidied up time testing
    - what is up with negatives?
    - 




## Todo
- [ ] refactor [heatmap](./py/heatmap.py) to account for these changes
- [x] encode each board as a Byte
    - [ ] the range thing

## Encoding & Decoding the data:
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








