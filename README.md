# Battleship!
(Best if ships have a distinct symbol, but doesn't break anything)

## TODOs, thoughts and goals
### Change log?
- Worker now exists
    - can serialise itself to save initial hit-null data 
    - *much* faster (unsupprising)
    - Haven't threaded yet (will work on that, expecting more speedup from it)

    - Memory problems
        - at hit-null (ie no hitmask) 7-5 is the first to run into a to many for the heap error
        - This will only increase with more ships*
            - but i imagine the hitmask will hopefully remove some of the small bits so it will get better as more hits



### Todo
- [ ] Worker plan
    - [ ] 


- [ ] count between `[S,E]` (Check if the last one is allways bad (i assume so) and remove it off the list?)
    - make previous byte method??
    - [ ] turn a `Byte[]` into an `int` (or `long`)

- [ ] ~~have the filenames for I/O be variable rather than fixed~~




### Long term goals: 




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

