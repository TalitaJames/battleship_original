# Monte carlo battleship implementation
Notes for the future extention of MCTS into battlships

```cpp
void MCTS_node::generateUnexploredMoves(){
    // for each possible move, add it to the unexploredMoves vector
    std::vector<cellStatus> possibleOutcomes = {HIT, MISS};
    
    for(int x=0; x<BOARD_SIZE; x++){
        for(int y=0; y<BOARD_SIZE; y++){

            cellStatus cell = hitmask.hitmask[x][y];

            if (cell == cellStatus::UNKNOWN){
                struct shipPosition tmpPos = {(short unsigned int) x, (short unsigned int) y,false};
                unexploredMoves.push_back(tmpPos);
            }
        }
    }
};

void MCTS_node::rollout(coordinateChooser playStrategy){
    unsigned int score = playGame(playStrategy, tree->getBoard());
    backpropagate(score);
};


struct board MCTS_tree::getBoard(){
    return board;
};
```


```cpp

void MCTS_node::expand(){
    // create child nodes off this tree from unexploredmoves
    int childrenToMake = 10;
    
    if (verbose) std::cout << "STARTING EXPANSION, currently " << this->childrenNodesPtr.size() << " children\n";
    for (size_t i = 0; i < childrenToMake; i++) {
        if (verbose) std::cout << "\nexpanding " << i << " of " << childrenToMake;

        int rndMove = rand() % unexploredMoves.size();
        std::swap(unexploredMoves[rndMove], unexploredMoves.back()); // swap the random move to the back
        shipPosition newMove = unexploredMoves.back(); // get the random move
        unexploredMoves.pop_back(); // get rid of the move (its been done)
        if (verbose) std::cout << " - got a move!\n";


        struct hitmask newHitmask = hitmask; //create a new hitmask and take the shot
        hitBoard(tree->getBoard(), newHitmask, newMove.x, newMove.y);

        // MCTS_node newChild = MCTS_node(this, newHitmask, this->tree); //make the child and add it to the vector of children
        MCTS_node* newChildPtr = new MCTS_node(this, newHitmask, this->tree); //make the child and add it to the vector of children
        
        if (verbose) std::cout << " made a child node!\t UCB: " <<  newChildPtr -> getUCBScore()<< "\t";
        
        // childrenNodes.insert(, newChild);
        // this -> childrenNodes.push_back(newChild);
        this -> childrenNodesPtr.push_back(newChildPtr);


        if (verbose) std::cout << "- added, total of: " << this -> childrenNodesPtr.size() << "children\n";
    }
    if (verbose) std::cout << "DONE EXPANSIION, currently " << this->childrenNodesPtr.size() << " children (pointing)\n";
    
};
```