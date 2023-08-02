# Battleship!
(Best if ships have a distinct symbol, but doesn't break anything)

## Change log?
- tidied up time testing
- Ran len 2-10 (ships 2-4)
    - ship size is the contributing factor for runtime now (not board len)

|Ship count -> |1|2|3|4|5|
|--------------|-|-|-|-|-|
|Avg time(ms)|27.89|427.33|48853.89|13193912.44||
|"Real" Time|0s|0.4s|48s|3.6hr|~37 days|
|BPS|9.14|152.16|339.41|320.47|330.00?|
(BPS is Bytes per sec)


Getting rid of bytes

For any number of ships $n$, and any known illegal byte $x$ (where $r$ is the range each byte itterates to) the number of bytes $x$ is mentioned i think is;
$$x_{\#}=nr-(2^{n}-(n+1))$$
for $l=5$, (5 ships, full length) this is only 1.2k (for each board, there start with 4*length, and increase as smaller ships get eliminated)


## Todo
- [ ] A new classes for threading things
- [ ] do it the java way



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








