#include <iostream>

#include "mcTreesearch.h"

// *********************************
// **            NODE             **
// *********************************
MCTS_node::MCTS_node(){
    new (this) MCTS_node(nullptr, nullptr, true);
};

MCTS_node::MCTS_node(MCTS_tree *tree){
    new (this) MCTS_node(nullptr, tree, false); //TODO do i need to now delete this at the end?
    //TODO is nullptr helpfull? or needs more caution around it?
};

MCTS_node::MCTS_node(MCTS_node *parentNode, MCTS_tree *tree){
    new (this) MCTS_node(parentNode, tree, false); //TODO do i need to now delete this at the end?
};

MCTS_node::MCTS_node(MCTS_node *parentNode, MCTS_tree *tree, bool headNode):
    parentNode(parentNode), tree(tree), headNode(headNode),
    visitCount(0),scoreTotal(0){

    if (verboseMCTS) std::cout << "NEW NODE" << std::endl;

    this->childrenNodesPtr.reserve(10); //FIXME use a macro of default children size or similar
    if (verboseMCTS) std::cout<<"Constructed" << std::endl;
}

MCTS_node::~MCTS_node(){
    // for ( auto child : childrenNodesPtr ){
    //     delete child;
    // }
    if(verboseMCTS) std::cout << ":( Deleted Node" << std::endl;
};

int MCTS_node::getSize(){
    int size = childrenNodesPtr.size();
    // if(verboseMCTS) std::cout << "(node has " << size << " children) ";

    for (auto &&child : childrenNodesPtr) { //
        size += child->getSize();
    }
    
    return size;
};

int MCTS_node::getVisitCount(){
    return visitCount;
};

double MCTS_node::getUCBScore(){
    // if it hasn't been visited yet, it has a UCB of infinity
    if (verboseMCTS) std::cout << "Checking UCB (tangent: " << this->childrenNodesPtr.size() << " childrnPtrs)";
    if (0 == visitCount || headNode) return std::numeric_limits<double>::max();

    double exploit = ((double) scoreTotal)/((double) visitCount); // exploit term
    double explore = explorationConst * std::sqrt(std::log((double)parentNode->getVisitCount())/((double) visitCount)); // explore term
    double ucbScore = explore + exploit;

    if (verboseMCTS) std::cout << "\t (exploit: " << exploit << ", explore: " << explore << ")" <<std::endl;

    return ucbScore;
};

// void MCTS_node::backpropagate(unsigned int score){
//     visitCount ++;
//     scoreTotal += score;
//     if (nullptr != parentNode ) parentNode -> backpropagate(score);

//     if (verboseMCTS) std::cout << "backpropogated for the node with " << unexploredMoves.size() << " to explore\n";
// };

// MCTS_node* MCTS_node::selectBestChild(){ // TODO this should be pointers?
//     if (childrenNodesPtr.size() == 0) {
//         return this;
//     }
//     // pick the best child
//     MCTS_node* bestChild = childrenNodesPtr[0];
//
//     for (auto &&child : childrenNodesPtr) {
//         if (child->getUCBScore() > bestChild->getUCBScore()) {
//             bestChild = child;
//         }
//     }
//     return bestChild;
// };

void MCTS_node::debug(){
    for (size_t i = 0; i < 10; i++) {
        MCTS_node* fooPtr = new MCTS_node();
        childrenNodesPtr.push_back(fooPtr);
    }
    visitCount++;

    std::cout << " --- Debug node" << std::endl;
}


// *********************************
// **            TREE             **
// *********************************

/*
MCTS_tree::MCTS_tree() {
    MCTS_node rootNode = MCTS_node(this);
    this->rootNodePtr = &rootNode;
};


int MCTS_tree::getSize(){
    // size = number of children from the root +1 to count the root node
    return rootNodePtr->getSize() + 1;
};


void MCTS_tree::advanceTree(){
    if (verboseMCTS) std::cout<<"advancing tree\n";

    rootNodePtr->expand();
};

MCTS_node MCTS_tree::getRootNode(){
    return *rootNodePtr;
};

void MCTS_tree::debug(){
    std::cout<<"You have a tree from board\n";
};
*/
