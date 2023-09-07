# Battleship!
(Best if ships have a distinct symbol, but doesn't break anything)

## Change log?
- Better waiting in main thread

## Todo
- [ ] create a different `PrimitiveBoard` object for each thread, (ie `PrimitiveBoardAlpha`, `PrimitiveBoardBravo`,`PrimitiveBoardCharlie` ect )
- [ ] job queue for threads
    - refactor the `ByteItterator` to account for the job thing
    - protect it whilst other things are happening (so 2 don't take the same one)
    
### Much later: 
- [ ] refactor [heatmap](./py/heatmap.py) to account for these changes


## Encoding & Decoding the data:
The remaining data (x,y) and direction is encoded for each ship as follows:
position $p=x\times 10 + y$, then bitshifted left and final bit is the direction (`1` for horizontal $\rightarrow$)

### Max byte data for each board size
Note that the code generates a `uByteMax` (the max posible byte legal for this size board)
| Board Size | max byte (Unsigned)| Signed|
|-|-|-|
|2|23|23|
|3|45|45|
|4|67|67|
|5|89|89|
|6|111|111|
|7|133|-123|
|8|155|-101|
|9|177|-79|
|10|199|-57|

## Ships know what?
- [ ] later
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

