#include "mcBoats.h"

// *********************************
// **            NODE             **
// *********************************
MCTS_node::MCTS_node(){
    struct hitmask emptyHitmask;
    new (this) MCTS_node(emptyHitmask);
};

MCTS_node::MCTS_node(struct hitmask hitmask){
    new (this) MCTS_node(hitmask, nullptr, nullptr, true);
};

MCTS_node::MCTS_node(struct hitmask hitmask, MCTS_tree *tree){
    new (this) MCTS_node(hitmask, tree, nullptr, true);
    //TODO is nullptr helpfull? or needs more caution around it?
};

MCTS_node::MCTS_node(struct hitmask hitmask, MCTS_tree *tree, MCTS_node *parentNode){
    new (this) MCTS_node(hitmask, tree, parentNode, false); //TODO do i need to now delete this at the end?
};

MCTS_node::MCTS_node(struct hitmask hitmask, MCTS_tree *tree, MCTS_node *parentNode, bool headNode):
    parentNode(parentNode), tree(tree), headNode(headNode),
    hitmask(hitmask), visitCount(0),scoreTotal(0){

    generateUnexploredMoves();
    this->childrenNodesPtr.reserve(10); //FIXME use a macro of default children size or similar?
    if (verboseMCTS) std::cout<<"Constructed Node " << this << std::endl;
}

MCTS_node::~MCTS_node(){
    for ( auto child : childrenNodesPtr ){
        delete child;
    }
    if(verboseMCTS) std::cout << ":( Deleted Node " << this << std::endl;
};

void MCTS_node::generateUnexploredMoves(){
    // for each possible move, add it to the unexploredMoves vector

    for(int x=0; x<BOARD_SIZE; x++){
        for(int y=0; y<BOARD_SIZE; y++){

            cellStatus cell = hitmask.hitmask[x][y];

            if (cell == cellStatus::UNKNOWN){
                // struct shipPosition tmpPos = {(short unsigned int) x, (short unsigned int) y,false};
                struct hitmask newHitmask = hitmask;
                newHitmask.hitmask[x][y] = cellStatus::HIT;

                // if (verboseMCTS) std::cout << newHitmask << std::endl;
                unexploredMoves.push_back(newHitmask);
            }
        }
    }
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
    // if it hasn't been visited yet or is the head node, it has a UCB of infinity
    if (0 == visitCount || headNode) return std::numeric_limits<double>::max();

    double exploit = ((double) scoreTotal)/((double) visitCount); // exploit term
    double explore = explorationConst * std::sqrt(std::log((double)parentNode->getVisitCount())/((double) visitCount)); // explore term
    double ucbScore = explore + exploit;

    // if (verboseMCTS) std::cout << this << " has exploit: " << exploit << ", explore: " << explore << "" <<std::endl;

    return ucbScore;
};

MCTS_node* MCTS_node::getBestChild(){
    MCTS_node *bestChild;
    bestChild = childrenNodesPtr.front();

    for(auto child : childrenNodesPtr){
        if (child->getUCBScore() > bestChild->getUCBScore()){
            bestChild = child;
        }
    }
    return bestChild;
};

void MCTS_node::rollout(){
    // Play the game given a board and hitmask
    // but the hitmask has to match a board state and not just an all "hit" one

    if (verboseMCTS) std::cout << " ROllOUT for " << this;

    // For testing, assume a random number of turns taken, from low, to the maximum of the board
    std::random_device rdDev;
    std::mt19937 rng(rdDev());
    std::uniform_int_distribution<std::mt19937::result_type> udist(FLEET_SIZE*3,std::pow(BOARD_SIZE,2));
    int turnsTaken = udist(rng);
    int score = std::pow(BOARD_SIZE,2) - turnsTaken;
    if (verboseMCTS) std::cout << " took " << turnsTaken << " turns, thus score is " << score << std::endl;
    backpropagate(score);
};

void MCTS_node::backpropagate(unsigned int score){
    visitCount ++;
    scoreTotal += score;
    // FIXME is this a better check than storing a bool `headNode`
    if (verboseMCTS) std::cout << "backpropogated from " << this << " to node " << parentNode << std::endl;
    if (nullptr != parentNode ) parentNode -> backpropagate(score);
};

void MCTS_node::expand(){
    // if(verboseMCTS) std::cout << "Node has " << childrenNodesPtr.size() << " children and " << unexploredMoves.size() << " future" << std::endl;
    for (auto hitmask : unexploredMoves){
        MCTS_node *newChild = new MCTS_node(hitmask, tree, this);
        childrenNodesPtr.push_back(newChild);
    }
    unexploredMoves.clear(); 
};

void MCTS_node::debug(){
    std::cout <<"Debug Node: " << this << " isHead: " << headNode << " visitCount: " << visitCount << " scoreTotal: " << scoreTotal << " ";
    std::cout << "UCB: " << getUCBScore() << " children: " << childrenNodesPtr.size() << " unexplored:" << unexploredMoves.size() << std::endl;
    std::cout << hitmask << std::endl;

    // for each child
    for(auto child : childrenNodesPtr){
        child -> debug();
    }
};


// *********************************
// **            TREE             **
// *********************************

MCTS_tree::MCTS_tree(){
    struct board randomBoard;
    randomBoard = rndBoard();
    new (this) MCTS_tree(randomBoard);
}

MCTS_tree::MCTS_tree(struct board board):
    board(board){
    struct hitmask emptyHitmask;
    

    // MCTS_node rootNode = MCTS_node(this, emptyHitmask); // TODO: this node gets deleted at the end of the scope
    // this->rootNodePtr = &rootNode;


    this->rootNodePtr = new MCTS_node(emptyHitmask, this);
    
    if(verboseMCTS) std::cout << "Made a tree\n"<< board << std::endl;
};

MCTS_tree::~MCTS_tree(){
    if(verboseMCTS) std::cout << ":( Deleted tree" << std::endl;
};

int MCTS_tree::getSize(){
    // size = number of children from the root +1 to count the root node
    return rootNodePtr->getSize() + 1;
};

// void MCTS_tree::advanceTree(){
//     if (verboseMCTS) std::cout<<"advancing tree\n";
//     rootNodePtr->expand();
// };

MCTS_node* MCTS_tree::getRootNode(){
    return rootNodePtr;
};

void MCTS_tree::debug(){
    std::cout << "You have a tree from board" << board;
    std::cout << "root node is " << &rootNodePtr << std::endl;
    std::cout << std::endl;
};
