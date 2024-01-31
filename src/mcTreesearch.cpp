#include <iostream>
#include <algorithm>
#include <limits>

#include "mcTreesearch.h"




// *********************************
// **            NODE             **
// *********************************


MCTS_node::MCTS_node(MCTS_node *parentNode, struct hitmask hitmask, MCTS_tree *tree):
    parentNode(parentNode),hitmask(hitmask), tree(tree),
    visitCount(0),scoreTotal(0){

        generateUnexploredMoves();
        runThreads(8, hitmask, probabilityGrid); //generate a probability grid for the setup
        this->childrenNodesPtr.reserve(10);
        if (verbose) std::cout<<"DONE constructor, " << childrenNodesPtr.size() << " children\n";

};

MCTS_node::MCTS_node(struct hitmask hitmask, MCTS_tree *tree){
    new (this) MCTS_node(nullptr, hitmask, tree); //TODO do i need to now delete this at the end?
};

MCTS_node::~MCTS_node(){
    for ( auto child : childrenNodesPtr ){
        delete child;
    }
};

int MCTS_node::getSize(){
    int size = childrenNodesPtr.size();
    if(verbose) std::cout << "(node has " << size << " children) ";

    for (auto &&child : childrenNodesPtr) { //
        size += child->getSize();
    }
    
    return size;
};

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

int MCTS_node::getVisitCount(){
    return visitCount;
};

double MCTS_node::getUCBScore(){
    // if it hasn't been visited yet, it has a UCB of infinity
    if (verbose) std::cout << "Checking UCB (tangent: " << this->childrenNodesPtr.size() << " childrnPtrs)\n";
    if (visitCount == 0) return std::numeric_limits<double>::max(); 

    double ucbScore = ((double) scoreTotal)/((double) visitCount); // exploit term
    ucbScore += explorationConst * std::sqrt(std::log((double)parentNode->getVisitCount())/((double) visitCount)); // explore term
    
    return ucbScore;
};

void MCTS_node::rollout(coordinateChooser playStrategy){
    unsigned int score = playGame(playStrategy, tree->getBoard());
    backpropagate(score);
};

void MCTS_node::backpropagate(unsigned int score){
    visitCount ++;
    scoreTotal += score;
    if (parentNode != nullptr) parentNode -> backpropagate(score);
    
    if (verbose) std::cout << "backpropogated for the node with " << unexploredMoves.size() << " to explore\n";
};

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

MCTS_node* MCTS_node::selectBestChild(){ // TODO this should be pointers?
    if (childrenNodesPtr.size() == 0) {
        return this;
    }
    // pick the best child
    MCTS_node* bestChild = childrenNodesPtr[0];

    for (auto &&child : childrenNodesPtr) {
        if (child->getUCBScore() > bestChild->getUCBScore()) {
            bestChild = child;
        }
    }
    return bestChild;
};




// *********************************
// **            TREE             **
// *********************************


MCTS_tree::MCTS_tree(struct board b) {
    this->board = b;
    struct hitmask hitmask;
    MCTS_node rootNode = MCTS_node(hitmask, this);
    this->rootNodePtr = &rootNode;

};


struct board MCTS_tree::getBoard(){
    return board;
};


int MCTS_tree::getSize(){
    return rootNodePtr->getSize() + 1; // +1 to count the root node
};


void MCTS_tree::advanceTree(){
    if (verbose) std::cout<<"advancing tree\n";

    rootNodePtr->expand();
};

MCTS_node MCTS_tree::getRootNode(){
    return *rootNodePtr;
};