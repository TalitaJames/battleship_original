# Battleship!

## next steps
5x5 (with a 2 & 3 ship)

user inputs a shot
- return heatmap

## links & things
[Data genetics battlehip blog](http://www.datagenetics.com/blog/december32011/), 
[Yuval's Python notebook](https://colab.research.google.com/drive/1NlMnu8ftS8EpXtlaJMqdnYkbQQUpWEXm#scrollTo=q3jCF0oooxSv)



## Approach (getting all positions for a single ship)
1. Make a horizontal list of each possible single row position
1. Copy each one of those to each row in the grid
1. Transpose & copy each of those to the list 
    - (first half will be horizontal, last are vertical)
