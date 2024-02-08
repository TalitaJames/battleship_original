#include <iostream>
#include <algorithm>
#include <limits>

#include "mcTreesearch.h"




// *********************************
// **            NODE             **
// *********************************

MCTS_node::MCTS_node(MCTS_node *parent, struct hitmask hM, MCTS_tree *tree):
    tree(tree), hitmask(hM) {
        if (verbose) std::cout<<"Start constructor, " << childrenNodesPtr.size() << " children\n";

        parentNode = parent;
        // hitmask = hM;

        visitCount = 0;
        scoreTotal = 0;

        unexploredMoves = *(new std::vector<shipPosition> ());
        this->unexploredMoves.reserve(20);
        generateUnexploredMoves();
        
        // I have both childrenNodes and the Ptr version because i wasn't sure which was best & have been testing with both
        // childrenNodes = {}; 
        // childrenNodesPtr = {};
        this->childrenNodesPtr.reserve(10);
        
        runThreads(8, hitmask, probabilityGrid); //generate a probability grid for the setup
        if (verbose) std::cout<<"DONE constructor, " << childrenNodesPtr.size() << " children\n";
};

MCTS_node::MCTS_node(struct hitmask hM, MCTS_tree *tree){
    new (this) MCTS_node(nullptr, hM, tree); //TODO do i need to now delete this at the end?
};

MCTS_node::MCTS_node(struct hitmask hM): hitmask(hM) {
    //i've been playing around with different constructor inputs, i thought it was an object creation error @ first
    if (verbose) std::cout<<"DEBUG CONSTRUCTOR\n"; 
    // parentNode;
    // hitmask;
    // tree;

    visitCount = 0;
    scoreTotal = 0;
};

MCTS_node::~MCTS_node(){
    for ( auto child : childrenNodesPtr ){
        delete child;
    }
};


void MCTS_node::expand(){
    // create child nodes off this tree from unexploredmoves
    int childrenToMake = 10;

    // for(auto space : unexploredMoves){
    //     std::cout << "Position (" << space.x << ", " << space.y << ", " << space.dir << ")\n";
    // }
    
    // if (verbose) std::cout << "STARTING EXPANSION, currently " << this->childrenNodes.size() << " children\n";
    for (size_t i = 0; i < childrenToMake; i++) {
        if (verbose) std::cout << "\nexpanding " << i << " of " << childrenToMake << " Unexplored len: " << unexploredMoves.size()  ;

        // int rndMove = rand() % unexploredMoves.size();
        // std::swap(unexploredMoves[rndMove], unexploredMoves.back()); // swap the random move to the back
        shipPosition newMove = unexploredMoves.back(); // get the random move
        unexploredMoves.pop_back(); // get rid of the move for future
        // if (verbose) std::cout << " - got a move (" << newMove.x <<", "<< newMove.y<< ")\n";

        struct hitmask newHitmask = hitmask; //create a new hitmask and take the shot
        hitBoard(tree->getBoard(), newHitmask, newMove.x, newMove.y);

        // MCTS_node::MCTS_node(MCTS_node *parentNode, struct hitmask hitmask, MCTS_tree *tree):
        // MCTS_node newChild(newHitmask); //make the child and add it to the vector of children
        
        // if (verbose) std::cout << " made a child node!\t UCB: " <<  newChild.getUCBScore()<< "\t";
        
        // this->childrenNodes.push_back(newChild);


        // if (verbose) std::cout << "- added, total of: " << this->childrenNodes.size() << "children\n";
    }
    // if (verbose) std::cout << "\nDONE EXPANSIION, currently " << this->childrenNodes.size() << " children\n";
};

// Cant test - needs a working expand() function
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

void MCTS_node::backpropagate(unsigned int score){
    visitCount ++;
    scoreTotal += score;
    if (parentNode != nullptr) parentNode->backpropagate(score);
    
    if (verbose) std::cout << "backpropogated for the node with " << unexploredMoves.size() << " to explore\n";
};

int MCTS_node::getSize(){
    int size = childrenNodesPtr.size();

    for (auto &&child : childrenNodesPtr) { //
        size += child->getSize();
    }
    
    return size;
};

double MCTS_node::getUCBScore(){ 
    // if it hasn't been visited yet, it has a UCB of infinity (max in implementation)
    if (visitCount == 0) return std::numeric_limits<double>::max(); 

    double ucbScore = ((double) scoreTotal)/((double) visitCount); // exploit term
    ucbScore += explorationConst * std::sqrt(std::log((double)parentNode->getVisitCount())/((double) visitCount)); // explore term
    
    return ucbScore;
};

// Working Functions?
void MCTS_node::rollout(coordinateChooser playStrategy){
    unsigned int score = playGame(playStrategy, tree->getBoard());
    backpropagate(score);
};

void MCTS_node::generateUnexploredMoves(){
    // for each possible move, add it to the unexploredMoves vector
    
    for(int x=0; x<BOARD_SIZE; x++){
        for(int y=0; y<BOARD_SIZE; y++){

            cellStatus cell = hitmask.hitmask[x][y];

            if (cell == cellStatus::UNKNOWN){
                // TODO should me made with `new` and del @ end
                // Though i tested w/out adding that and it retains the data when printed elsewhere inside the class
                struct shipPosition tmpPos = {(short unsigned int) x, (short unsigned int) y,false}; 
                unexploredMoves.push_back(tmpPos);
            }
        }
    }
};

int MCTS_node::getVisitCount(){
    return visitCount;
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

MCTS_node* MCTS_tree::getRootNodePtr(){
    return rootNodePtr;
};