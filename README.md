# Battleship!
(Best if ships have a distinct symbol, but doesn't break anything)

## Todo
- [ ] encode each board as a Byte
    - [ ] Could that be generatable rather than making boards & converting?
    - [ ] the range thing
- [ ] decode `.json` to have `gameData.json` acting as universal settings, rather than hardcoding game states
- [x] fix the copy issue ~~(or design a better way around it)~~
- [ ] Plotting things
    - [ ] heatmap given a list of `Board`
    - [ ] Make a python script that takes the java output & makes heatmaps
    - [ ] Maven? plotting in Java? (sounds terrible)
- [x] Range elimination as playing thru game
- [x] User input simple play
- [ ] Memory benchmarking
- [ ] Not a fan of how coords are being stored, would preffer cpp `pair` style


## Encoding & Decoding the data:
For a game, the data needed is:
- board size
- ship data
    - x,y position
    - direction 
    - length

Since board size, and length are fixed for all games, this information is fixed in `gameData.json` (TODO, for now hardcoded)

The remaining data (x,y) and direction is encoded for each ship as follows:
position = x*10+y, then bitshifted and the direction is orred on

Will move into & implement but i wrote this [externaly](https://edstem.org/au/courses/10667/workspaces/pRGmyfhqh8Zk77INwFpF5e3NzRRqsOLt) and am dumping it here for now

```java
public static byte encode(int x, int y, boolean dir) {
    int val = x*10+y;
    byte encoded = (byte) val;
    encoded <<= 1;

    int dirInt = dir ? 1 : 0;
    byte dirByte = (byte) dirInt;
    encoded |= dirByte;

    return encoded;
}


public static void decode(byte value){
    boolean dir = (value % 2 != 0); // if odd, then true true
    
    int uint = value & 0xff; // unsign it
    uint>>=1;
    System.out.println("byte "+value +" uint "+uint+" 0b"+Integer.toBinaryString(uint));

    int x= (int) Math.floor(uint/10);
    int y= uint % 10;
    
    System.out.println("("+x+","+y+") "+dir);
}
```


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


## Ship stats & code outputs
for 5x5 [2,3]:
- takes about 1 sec to generate board pos
- 956 different positions max


As predicted, I ran out of memory on a complete 10x10 game (`Exception in thread "main" java.lang.OutOfMemoryError: Java heap space`)



## links & things
[JFreeChart for plotting ect](https://github.com/jfree/jfreechart/releases/tag/v1.5.2)

[Data genetics battleship blog](http://www.datagenetics.com/blog/december32011/), 
[Yuval's Python notebook](https://colab.research.google.com/drive/1NlMnu8ftS8EpXtlaJMqdnYkbQQUpWEXm#scrollTo=q3jCF0oooxSv)
