# Battleship!
Best if ships have a distinct symbol, but doesn't break play

## next steps
5x5 (with a 2 & 3 ship)

user inputs a shot
- return heatmap

## todo
- [x] fix the copy issue ~~(or design a better way around it)~~
- [ ] java plotting things
    - [ ] heatmap given a list of `Board`
- [x] User input simple play

## Ship stats & code outputs
for 5x5 [2,3]:
- takes about 1 sec to generate board pos
- 956 different possitions max


As predicted, i ran out of memory on a complete 10x10 game (`Exception in thread "main" java.lang.OutOfMemoryError: Java heap space`)



## links & things
[JFreeChart for plotting ect](https://github.com/jfree/jfreechart/releases/tag/v1.5.2)

[Data genetics battlehip blog](http://www.datagenetics.com/blog/december32011/), 
[Yuval's Python notebook](https://colab.research.google.com/drive/1NlMnu8ftS8EpXtlaJMqdnYkbQQUpWEXm#scrollTo=q3jCF0oooxSv)

