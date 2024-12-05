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
    // if (verboseMCTS) std::cout<<"Constructed Node " << this << std::endl;
};

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
                newHitmask.hitmask[x][y] = cellStatus::TURN;

                unexploredMoves.push_back(newHitmask);
            }
        }
    }
};

/* From the perspective of a parent making children nodes (this is called in expand)
check your siblings to see if any of their children (your niblings)
are identical to your hitmask, then return a pointer to them
@param hitmask used to compare with each nibling
@return a pointer to the nibling node
*/
MCTS_node* MCTS_node::findCousin(struct hitmask hitmask){
    // if you are the head node, you don't have siblings thus can't have niblings
    if(this -> isHeadNode()) return nullptr;
    MCTS_node* grandparent = this -> getParent(); // granparent for cousins is your parent

    std::vector<MCTS_node*> parentsSiblings = grandparent -> getAllChildren(); // the matching cousin must be a child of the parents siblings (if it exists)

    for(auto aunt : parentsSiblings){
        if (aunt == parentNode) break; // don't check own siblings

        for(auto cousin : aunt -> getAllChildren()){
            // check for the same hitmask
            if (cousin -> matchingHitmask(hitmask)){
                return cousin;
            }
        }
    }
   return nullptr; // if you've gotten thus far, no matching cousin exists
};

// Compares a hitmask to the nodes hitmask, true if they match
bool MCTS_node::matchingHitmask(struct hitmask outsideHitmask){
    return hitmask == outsideHitmask;
};

/* gets the size of the nodes children and granchildren, presumes you haven't yet seen any nodes
@return int, size of the children and any granchildren the node has
*/
int MCTS_node::getSize(){
    std::set<MCTS_node*> nodesSeen;
    return getSize(nodesSeen);
};

/* gets the size of the nodes children and granchildren, includes a set of already visited nodes. it adds to the visitedcousin
@return int, size of the children and any granchildren the node has
*/
int MCTS_node::getSize(std::set<MCTS_node*> &nodesSeen){
    int size = childrenNodesPtr.size();

    for (auto &&child : childrenNodesPtr) {
        if(nodesSeen.find(child) == nodesSeen.end()){ //if the child isn't in the set of nodes seen already, get all its children
            size += child->getSize(nodesSeen);
            nodesSeen.insert(child);
        }
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

/* @return internal hitmask
*/
struct hitmask MCTS_node::getHitmask(){
    return hitmask;
};


/* Adds a score and updates visit count
@param score the results from a rollout
*/
void MCTS_node::addResults(int score){
    visitCount++;
    scoreTotal += score;
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

// return a vector of pointers for each child the node is attached to
std::vector<MCTS_node *> MCTS_node::getAllChildren(){
    return childrenNodesPtr;
};

/* Simulates a game from the nodes play state, using the number
of turns taken to aproximate the efficency of this node
@return score
*/
int MCTS_node::rollout(board board){
    // Play the game given a board and hitmask
    // but the hitmask has to match a board state and not just an all "hit" one

    if (verboseMCTS) std::cout << "ROllOUT for " << this << std::endl;;
    int turnsTaken;

    //FIXME For testing, assume a random number of turns taken, from low, to the maximum of the board
    // std::random_device rdDev;
    // std::mt19937 rng(rdDev());
    // std::uniform_int_distribution<std::mt19937::result_type> udist(FLEET_SIZE*3,std::pow(BOARD_SIZE,2));
    // turnsTaken = udist(rng);

    turnsTaken = playGame_fromHitmask(coordinateChooser::P_MAX,board, this -> getHitmask());
    int score = std::pow(BOARD_SIZE,2) - turnsTaken; // invert, because a high score is good, but high num of turns is not.

    if (verboseMCTS) std::cout << hitmask <<  "--- DONE ROLLOUT took " << turnsTaken << " turns, thus score is " << score << "---\n\n" << std::endl;
    return score;
};

// convert the unexplored moves into children nodes
void MCTS_node::expand(){
    for (auto hitmask : unexploredMoves){

        MCTS_node *newChild = findCousin(hitmask);
        if(nullptr == newChild) newChild = new MCTS_node(hitmask, this); // if the cousin doesn't exist, make a new child
        childrenNodesPtr.push_back(newChild);
    }
    unexploredMoves.clear();
};

// provides debuging info for the internal of the node
void MCTS_node::debug(){
    std::cout <<"Debug Node: " << this << " isHead: " << isHeadNode() << " visitCount: " << visitCount << " scoreTotal: " << scoreTotal << " ";
    std::cout << "UCB: " << getUCBScore() << " children: " << childrenNodesPtr.size() << " unexplored:" << unexploredMoves.size() << std::endl;
    std::cout << hitmask << std::endl;

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
@return best child from the head node (ie best next move)
*/
MCTS_node* treeTraversal(MCTS_node* headNode, board board, int iterations){
    MCTS_node* currentNode = headNode;
    int i = 0;
    std::vector<MCTS_node*> visitedPath;
    visitedPath.push_back(currentNode);

    while (i<iterations) {
        if(currentNode -> isLeafNode()){

            if(0 == currentNode -> getVisitCount()){ // if the node hasn't been visited yet
                int score = currentNode -> rollout(board);
                backpropagate(score, visitedPath);
                visitedPath.clear();

                currentNode = headNode;
            }
            else{
                if(verboseMCTS) std::cout<< "Expanding from " << currentNode << std::endl;
                currentNode -> expand();
            }
        }
        else{
            if(verboseMCTS) std::cout<< "Get best child " << currentNode << std::endl;
            currentNode = currentNode -> getBestChild();
        }

        // if the last turn didn't end here, add it to the path
        if (visitedPath.back() != currentNode) visitedPath.push_back(currentNode);

        i++;
    }

    return headNode -> getBestChild();
};

/* adds a score to this node and to its parents
@param score the score to add to the node
*/
void backpropagate(int score, std::vector<MCTS_node*> visitedPath){

    for(auto node : visitedPath){
        node -> addResults(score);
    }
};

/* given a board, create a monte carlo tree search and save data as you simulate the game
@param board the board for the game
@param iterations number of times to traverse the tree each new move
*/
void saveGameMCTS(board board, int iterations){
    std::string filename = std::tmpnam(nullptr);

    // filename in the form: boardSize_fleetSize_boardID_playStyle_version_randomChars.json
    filename = "./out/gamePlay/"+std::to_string(BOARD_SIZE)+"_"+std::to_string(FLEET_SIZE)+"_"
                            +std::to_string(board.shipPositionsInt)+"_MCTS_"
                            +codeVersion+"_"+filename.substr(9, filename.length())+".json";

    Json::Value gamePlayHistory; //FIXME this could be in its own method, but i think it only needs to happen once here
    gamePlayHistory["FLEET_SIZE"] = FLEET_SIZE;
    // gamePlayHistory["FLEET"] = jsonArrayAdder(FLEET, FLEET_SIZE); //BUG why is this erroring here but fine in boatsAndBoards?k
    gamePlayHistory["BOARD_SIZE"] = BOARD_SIZE;
    gamePlayHistory["iterations"] = iterations;
    gamePlayHistory["board"] = jsonArrayAdder(board.board);
    gamePlayHistory["version"] = codeVersion;
    gamePlayHistory["shotMethod"] = "MCTS";

    simulateGameMCTS(board, iterations, gamePlayHistory);

    jsonFileoutput(filename, gamePlayHistory);
    std::cout<< "\tsaving to " << filename << "\n" << std::endl;
}

/* given a board, create a monte carlo tree search and simulate the game, taking turns each time
@param board the board for the game
@param iterations number of times to traverse the tree each new move
*/
void simulateGameMCTS(board board, int iterations){
    Json::Value rubishJSON;
    simulateGameMCTS(board, iterations, rubishJSON);
}

/* given a board, create a monte carlo tree search and simulate the game, taking turns each time
@param board the board for the game
@param iterations number of times to traverse the tree each new move
@param gamePlayHistory the json data for the board
*/
void simulateGameMCTS(board board, int iterations,  Json::Value &gamePlayHistory){
    hitmask gameHitmask;
    probabilityGrid pGrid;
    MCTS_node* headNode = new MCTS_node();
    MCTS_node* currentNode = headNode;

    int x = 0;
    int y = 0;

    Json::Value shotRecordJson = gamePlayHistory["shotRecord"];
    Json::Value treeRecord = gamePlayHistory["treeRecord"];
    Json::Value probabilityGridJson = gamePlayHistory["probabilityGrid"];


    int turnNumber = 1;

    while(!isHitmaskSolved(gameHitmask)){
        MCTS_node* nextMove = treeTraversal(currentNode, board, iterations);
        findHitmaskDifference(currentNode->getHitmask(), nextMove->getHitmask(), x, y); // work out the x/y coord to shoot

        std::cout << "-------- Taking turn " << turnNumber << " at " << x << ", " << y << std::endl;

        hitBoard(board, gameHitmask, x, y); // hit the board
        currentNode = nextMove; // start from the next move
        if(verboseMCTS) std::cout << gameHitmask << std::endl;
        turnNumber++;

        // Update JSON data
        Json::Value currentCoords(Json::arrayValue);
        currentCoords.append(x);
        currentCoords.append(y);
        shotRecordJson.append(currentCoords);

        runThreads(gameHitmask, pGrid, threadCount);
        probabilityGridJson.append(jsonArrayAdder(pGrid.shipGrid));

        std::string* mermaidChart = new std::string();
        visualiseTree(headNode, mermaidChart);
        treeRecord.append(*mermaidChart);
        std::cout<< "end of turn update: "<<  pGrid << std::endl;

    }

    // End of game JSON data
    gamePlayHistory["shotRecord"] = shotRecordJson;
    gamePlayHistory["probabilityGrid"] = probabilityGridJson;
    gamePlayHistory["treeRecord"] = treeRecord;

    // std::cout << "\n---- mermaid ----\n" << *mermaidChart << std::endl;
};

/* Generate a text based depiction of the graph for mermaid live
@param currentNode the node to start the listing of its children at
@param allNodesStr a string that gets recursivly appended to
*/
void visualiseTree(MCTS_node* currentNode, std::string* allNodesStr){
    std::set<MCTS_node*> newSet;
    visualiseTree(currentNode, allNodesStr, newSet);
};

/* Generate a text based depiction of the graph for mermaid live
@param currentNode the node to start the listing of its children at
@param allNodesStr a string that gets recursivly appended to
@param nodesSeen a set with the nodes that have already been added, preventing duplicates
*/
void visualiseTree(MCTS_node* currentNode, std::string* allNodesStr, std::set<MCTS_node*> &nodesSeen){

    for(auto child: currentNode -> getAllChildren()){
        std::ostringstream currentAddressOStringStream;
        currentAddressOStringStream << currentNode;
        std::string currentAddressStr =  currentAddressOStringStream.str();

        std::ostringstream childAddressOStringStream;
        childAddressOStringStream << child;
        std::string childAddressStr =  childAddressOStringStream.str();

        std::string thisNodeArrow = currentAddressStr + " --> " + childAddressStr;
        allNodesStr -> append(thisNodeArrow+"\n");
        if(nodesSeen.find(child) == nodesSeen.end()){ //if the child isn't in the set of nodes seen already, get all its children
            visualiseTree(child, allNodesStr, nodesSeen);
            nodesSeen.insert(child);
        }
    }
};

/*
int maxDepth(MCTS_node* headNode){
    int maxDepth = 0;

    std::vector<MCTS_node*>  nodesToVisit;
    nodesToVisit.push_back(headNode);

    while(nodesToVisit.size() > 0){
        // FIXME This is currently a tree, so no double visit worries, but should check when turning this into a DAG

        // get current node (pop front)
        MCTS_node* currentNode = nodesToVisit[0]; // get the first node
        nodesToVisit.erase(nodesToVisit.begin()); // remove it from the list

        // add all of childrens nodes to the back
        if(verboseMCTS) std::cout << "\n max depth of " << maxDepth << " and checking " << nodesToVisit.size() << " more after adding to be ";

        MCTS_node* firstChild = currentNode -> getAllChildren().front();
        MCTS_node* lastChild = currentNode -> getAllChildren().back();

        // BUG: terminate called after throwing an instance of 'std::length_error'
        //     what():  vector::_M_range_insert
        nodesToVisit.insert (nodesToVisit.end(),firstChild,lastChild);

        if(verboseMCTS) std::cout << nodesToVisit.size() << " big" << std::endl;

        // check if depth is greater or less than max
        if (currentNode -> getDepth() > maxDepth){
            maxDepth = currentNode -> getDepth();
        }
    }
    return maxDepth;
};
*/

