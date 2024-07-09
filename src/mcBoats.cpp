#include "mcBoats.h"

// *********************************
// **            NODE             **
// *********************************

/* Constructs an empty parent node */
MCTS_node::MCTS_node(){
    struct hitmask emptyHitmask;
    new (this) MCTS_node(emptyHitmask);
};

/* Constructs a parent node with given hitmask
@param hitmask the starting hitmask of the game
*/
MCTS_node::MCTS_node(struct hitmask hitmask){
    new (this) MCTS_node(hitmask, nullptr);
    //TODO why does (this) need to be here?
    // or more specificly, what does it do and why did i probaly add it?

};

/* Constructs a node with given hitmask and parent
@param hitmask the starting hitmask of the game
@param parentNode a pointer to the parent node
*/
MCTS_node::MCTS_node(struct hitmask hitmask, MCTS_node* parentNode):
    parentNode(parentNode), hitmask(hitmask),
    visitCount(0),scoreTotal(0){

    generateUnexploredMoves();
    if (isHeadNode()) expand();
    if (verboseMCTS) std::cout<<"Constructed Node " << this << std::endl;
}

/* destruct node */
MCTS_node::~MCTS_node(){
    for ( auto child : childrenNodesPtr ){
        delete child;
    }
    if(verboseMCTS) std::cout << ":( Deleted Node " << this << std::endl;
};

/* From the hitmask, generate a vector of potential moves */
void MCTS_node::generateUnexploredMoves(){
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

/* gets the size of the nodes children and granchildren
@return int, size of the children and any granchildren the node has
*/
int MCTS_node::getSize(){
    int size = childrenNodesPtr.size();
    // if(verboseMCTS) std::cout << "(node has " << size << " children) ";

    for (auto &&child : childrenNodesPtr) { //
        size += child->getSize();
    }
    
    return size;
};

/* Gets number of times node has visited
@return int, number of times visited
*/
int MCTS_node::getVisitCount(){
    return visitCount;
};

/* Gets the depth of the node, ie number of steps to the parent
Note that counting starts from 1 (ie parent has depth of one)
@return int, depth of node
*/
int MCTS_node::getDepth(){
    int depth = 0;
    MCTS_node* currentNode;
    while (!currentNode -> isHeadNode()) {
        depth++;
        currentNode = getParent();
    }
    return depth;
};

/* Check if the node is a leaf
@return boolean, true if node is a leaf
*/
bool MCTS_node::isLeafNode(){
    return 0 == childrenNodesPtr.size(); // is a leaf if there aren't any children nodes
};

/* Check if the node is the head node
@return boolean, true if the nodes parent doesn't exist (ie nullptr)
*/
bool MCTS_node::isHeadNode(){
    return nullptr == parentNode;
};

// Return the UCB1 score of the node
double MCTS_node::getUCBScore(){
    // if it hasn't been visited yet it has a UCB of infinity
    if (0 == visitCount) return std::numeric_limits<double>::max();

    double exploit = ((double) scoreTotal)/((double) visitCount); // exploit term
    double explore = explorationConst * std::sqrt(std::log((double)parentNode->getVisitCount())/((double) visitCount)); // explore term
    double ucbScore = explore + exploit;

    // if (verboseMCTS) std::cout << this << " has exploit: " << exploit << ", explore: " << explore << "" <<std::endl;

    return ucbScore;
};

MCTS_node* MCTS_node::getParent(){
    if (this -> isHeadNode()) return this;
    return parentNode;
};

// Returns the child node with the maximal UCB score
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

std::vector<MCTS_node *> MCTS_node::getAllChildren(){
    return childrenNodesPtr;
};

/* Simulates a game from the nodes play state, using the number
of turns taken to aproximate the efficency of this node
*/
void MCTS_node::rollout(){
    // Play the game given a board and hitmask
    // but the hitmask has to match a board state and not just an all "hit" one

    if (verboseMCTS) std::cout << "ROllOUT for " << this;

    //FIXME For testing, assume a random number of turns taken, from low, to the maximum of the board
    std::random_device rdDev;
    std::mt19937 rng(rdDev());
    std::uniform_int_distribution<std::mt19937::result_type> udist(FLEET_SIZE*3,std::pow(BOARD_SIZE,2));
    int turnsTaken = udist(rng);

    int score = std::pow(BOARD_SIZE,2) - turnsTaken; // invert, because a high score is good, but high num of turns is not.
    if (verboseMCTS) std::cout << " took " << turnsTaken << " turns, thus score is " << score << std::endl;
    backpropagate(score);
};

/* adds a score to this node and to its parents
@param score the score to add to the node
*/
void MCTS_node::backpropagate(unsigned int score){
    visitCount ++;
    scoreTotal += score;
    // FIXME is this a better check than storing a bool `headNode`
    if (verboseMCTS) std::cout << "backpropogated from " << this << " to node " << parentNode << std::endl;
    if (!this->isHeadNode()) parentNode -> backpropagate(score);
};

// convert the unexplored moves into children nodes
void MCTS_node::expand(){
    // if(verboseMCTS) std::cout << "Node has " << childrenNodesPtr.size() << " children and " << unexploredMoves.size() << " future" << std::endl;
    for (auto hitmask : unexploredMoves){
        MCTS_node *newChild = new MCTS_node(hitmask, this);
        childrenNodesPtr.push_back(newChild);
    }
    unexploredMoves.clear();
};

// provides debuging info for the internal of the node
void MCTS_node::debug(){
    std::cout <<"Debug Node: " << this << " isHead: " << isHeadNode() << " visitCount: " << visitCount << " scoreTotal: " << scoreTotal << " ";
    std::cout << "UCB: " << getUCBScore() << " children: " << childrenNodesPtr.size() << " unexplored:" << unexploredMoves.size() << std::endl;
    std::cout << hitmask << std::endl;

    // for each child
    // for(auto child : childrenNodesPtr){
    //     child -> debug();
    // }
};



// *********************************
// **            TREE             **
// *********************************

/* function to puppet the MCTS
@param headNode the node to start traversal at (Generaly the parent node)
@param itterations the number of times to run the search
*/
void treeTraversal(MCTS_node* headNode, int iterations){
    MCTS_node* currentNode = headNode;
    int i = 0;
    
    while (i<iterations) {
        if (verboseMCTS) std::cout << "\nTree Traversal itteration #" << i << " node is " << currentNode << std::endl;

        if(currentNode -> isLeafNode()){
            if (verboseMCTS) std::cout << "\tIS LEAF #" << i << std::endl;
            
            if(0 == currentNode -> getVisitCount()){ // if the node hasn't been visited yet
                if (verboseMCTS) std::cout << "\t\tROLLOUT #" << i << std::endl;
                currentNode -> rollout();
                currentNode = headNode;
            }
            else{
                if (verboseMCTS) std::cout << "\t\tEXPAND #" << i << std::endl;
                currentNode -> expand();
            }
        }
        else{
            if (verboseMCTS) std::cout << "\tFIND BEST #" << i << std::endl;
            currentNode = currentNode -> getBestChild();
        }
        if (verboseMCTS) std::cout << "\tEnd of traversal #" << i << "current node is " << currentNode << std::endl;
        i++;
    }
};

/* Generate a text based depiction of the graph for mermaid live
@param currentNode the node to start the listing of its children at
@param allNodesStr a string that gets recursivly appended too
*/
void visualiseTree(MCTS_node* currentNode, std::string* allNodesStr){
    for(auto child: currentNode -> getAllChildren()){
        std::ostringstream currentAddressOStringStream; 
        currentAddressOStringStream << currentNode;
        std::string currentAddressStr =  currentAddressOStringStream.str(); 

        std::ostringstream childAddressOStringStream; 
        childAddressOStringStream << child;
        std::string childAddressStr =  childAddressOStringStream.str(); 

        std::string thisNodeArrow = currentAddressStr + " --> " + childAddressStr;
        allNodesStr -> append(thisNodeArrow+"\n");
        visualiseTree(child, allNodesStr);
    }
};


// int maxDepth(MCTS_node* headNode){ 
//     int maxDepth = 0;
    
//     std::vector<MCTS_node*>  nodesToVisit;
//     nodesToVisit.push_back(headNode);

//     while(nodesToVisit.size() > 0){
//         // FIXME This is currently a tree, so no double visit worries, but should check when turning this into a DAG
        
//         // get current node (pop front)
//         MCTS_node* currentNode = nodesToVisit[0]; // get the first node
//         nodesToVisit.erase(nodesToVisit.begin()); // remove it from the list

//         // add all of childrens nodes to the back
//         if(verboseMCTS) std::cout << "\n max depth of " << maxDepth << " and checking " << nodesToVisit.size() << " more after adding to be ";

//         MCTS_node* firstChild = currentNode -> getAllChildren().front();
//         MCTS_node* lastChild = currentNode -> getAllChildren().back();

//         /*BUG: terminate called after throwing an instance of 'std::length_error'
//             what():  vector::_M_range_insert
//         */
//         nodesToVisit.insert (nodesToVisit.end(),firstChild,lastChild);

//         if(verboseMCTS) std::cout << nodesToVisit.size() << " big" << std::endl;

//         // check if depth is greater or less than max
//         if (currentNode -> getDepth() > maxDepth){
//             maxDepth = currentNode -> getDepth();
//         }
//     }
//     return maxDepth;
// };

