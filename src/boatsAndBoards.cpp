#include "boatsAndBoards.h"

using namespace std::chrono;

std::map<coordinateChooser, std::string> coordinateChooserNames{ 
    {USER_INPUT, "USER-INPUT"},
    {RND, "RND"},
    {RND_W_PROB, "RND-W-PROB"},
    {P_MAX, "P-MAX"},
    {P_RND, "P-RND"},
    {INFOGAIN, "INFOGAIN"},
    {DIAGONAL, "DIAGONAL"},
    {FLEXI, "FLEXI"}
};

int threadCount = 8;
bool verbose = false;
std::string codeVersion = "vERROR";

std::random_device rdDev;
std::mt19937 rng(rdDev());

std::mutex saveFile_mutex;

// -- Board drawing and manipulation

// Returns an empty board
board initBlankBoard(){
    board b;
    wipeBoard(b);
    return b;
};

// Clears an existing board to empty
void wipeBoard(board &b){
    memset(b.board, BOARD_DEFAULT, sizeof(b.board));
    b.isEmpty=true;
    b.isValid=false;
};

/* Draws a list of ship positions onto a board
@param board the referenced board
@param shipPos a pointer to an array of ship Positions
*/
void drawBoard(board &board, shipPosition* shipPos){
    wipeBoard(board);
    board.isEmpty = false;
    
    for (size_t i = 0; i < FLEET_SIZE; i++){ // for each ship
        for (size_t j = 0; j < FLEET[i]; j++){ // for the length of each ship
            // early return if a ship already there or if it is out of bounds
            if (shipPos[i].dir){
                if (shipPos[i].x+j>= BOARD_SIZE ||shipPos[i].y>= BOARD_SIZE) { // out of horizonal bounds
                    board.isValid=false;
                    return;
                }
                else if (board.board[shipPos[i].x+j][shipPos[i].y] != BOARD_DEFAULT) { // intersection!
                    board.isValid=false;
                    return;
                }
                board.board[shipPos[i].x+j][shipPos[i].y] = i; // update board value
            }
            else{
                if (shipPos[i].x>= BOARD_SIZE ||shipPos[i].y+j>= BOARD_SIZE) { // out of vertical bounds
                    board.isValid=false;
                    return;
                }
                else if (board.board[shipPos[i].x][shipPos[i].y+j] != BOARD_DEFAULT) { // intersection!
                    board.isValid=false;
                    return;
                }
                board.board[shipPos[i].x][shipPos[i].y+j] = i; // update board value
            }
        }
    }
    board.shipPositionsInt = shipArrayToInt(shipPos);
    board.isValid = true;
};

/* Updates the hitmask with a hit at x&y given a board
@param b board with the positions clearly shown
@param h hitmask reference to record the shot
@param x coordinate for shot
@param y cordinate for shot
*/
void hitBoard(board b, hitmask &h, int x, int y){
    int cell = b.board[x][y]; // check what is at (x,y) at board
    h.hitmask[x][y] = (cell != BOARD_DEFAULT) ? HIT : MISS; //update the hitmask accordingly (hit/miss)

    // Check if the ship is sunk
    if (h.hitmask[x][y] == HIT){ // if it was a hit
        bool isSunk = true;

        // make an array of ship positions from the board
        shipPosition shipPos[FLEET_SIZE];
        intToShipArray(b.shipPositionsInt, shipPos);

        int checkX,checkY=0;
        for (size_t i = 0; i < FLEET[cell]; i++) { //for the length of the ship just hit
        // get the location of the next ship segment
        if (shipPos[cell].dir) {
            checkX = shipPos[cell].x + i;
            checkY = shipPos[cell].y;
        }
        else{
            checkX = shipPos[cell].x;
            checkY = shipPos[cell].y + i;
        }
        
        // if the cell isn't recorded as hit or sunk, then the whole boat hasn't been explored yet, so stop checking the rest of the boats
        if (!(h.hitmask[checkX][checkY] == cellStatus::HIT || h.hitmask[checkX][checkY] == cellStatus::SUNK)){
            isSunk = false;
            break;
        }
        }
        
        // update the hitmask if sunk
        if (isSunk) {
        h.shipSunk[cell] = true; // the ship itself has been sunk
        for (size_t i = 0; i < FLEET[cell]; i++) { // for the length of the ship just hit
            // get the location of the next ship segment
            if (shipPos[cell].dir) {
            checkX = shipPos[cell].x + i;
            checkY = shipPos[cell].y;
            }
            else{
            checkX = shipPos[cell].x;
            checkY = shipPos[cell].y + i;
            }

            // update that segment to be sunk
            h.hitmask[checkX][checkY] = cellStatus::SUNK;
        }
        }
    }
};

/* Checks if a hitmask (h) is compatible with a board (b) 
@param b board
@param h hitmask
@return bool true if compatable, else false
*/
bool checkCompatible(board b,hitmask h){
    for (int y = 0; y < BOARD_SIZE; y++){
        for (int x = 0; x < BOARD_SIZE; x++){
            if (h.hitmask[x][y] != UNKNOWN){ // if the spot isn't unknown (ie a miss, hit ect)
                if (h.hitmask[x][y]==MISS && b.board[x][y]!=BOARD_DEFAULT) return false; // if hitmask is a miss, and board isn't 
                else if (h.hitmask[x][y]==HIT && b.board[x][y]==BOARD_DEFAULT)  return false; // is board empty and hitmask isn't
                else if ((h.hitmask[x][y]==SUNK && (!h.shipSunk[b.board[x][y]]))) return false; // if the ship is sunk, and the boat it claims to be isn't sunk
            }
        }
    }
    return true;
};

/* Checks if two hitmasks match
@param A first hitmask
@param B second hitmask
@return bool true if equal
*/
bool operator==(const struct hitmask &A,  const struct hitmask &B){

    for (int y = 0; y < BOARD_SIZE; y++){
        for (int x = 0; x < BOARD_SIZE; x++){
            // std::cout << "(" << x <<","<< y << ") match " << (A.hitmask[x][y] == B.hitmask[x][y]) << std::endl;
            if (A.hitmask[x][y] != B.hitmask[x][y]) return false; // if they don't equal in the grid, not same
        }
    }

    for (int i = 0; i < FLEET_SIZE; i++){
        // std::cout << "(" << i << ") match " << (A.shipSunk[i] == B.shipSunk[i]) << std::endl;
        if (A.shipSunk[i] != B.shipSunk[i]) return false; // one ship is sunk and other isn't, then not equal
    }
    
    return true;
};


// -- Random functions

/* Generates a random position for a ship 
@param len a length of the shipr
@return shipPosition a (semi) random shipPosition
*/
shipPosition rndShipPos(ship len){
    std::uniform_int_distribution<std::mt19937::result_type> udist(0,BOARD_SIZE-len);
    
    shipPosition pos;
    pos.x = udist(rng);
    pos.y = udist(rng);
    pos.dir = rand() % 2;

    return pos;
};

/* Generates a random board
@return board a (pseudo) random and guaranteed valid board
*/
board rndBoard(){
    board b = initBlankBoard();
    shipPosition boardPositions[FLEET_SIZE];
    while (!b.isValid){
        for (size_t i = 0; i < FLEET_SIZE; i++) boardPositions[i]=rndShipPos(FLEET[i]);
        drawBoard(b, boardPositions);
    }
    return b;
};


// -- Ship Position Manipulation

/* Compares two ship positions and determines which is "greater".
The "greatest" ship pos is horizontal left topmost, prioritized in that order
@param pA position A
@param pB position B
@return int comparing the sizes, ie -1 if A>B, 0 if A=B, 1 if A<B
*/
int compareShipPositions(shipPosition pA, shipPosition pB){
    if (pA.dir != pB.dir ) return pB.dir - pA.dir;
    else if (pA.x != pB.x ) return (pB.x - pA.x)/abs(pB.x - pA.x);
    else if (pA.y != pB.y ) return (pB.y - pA.y)/abs(pB.y - pA.y);
    return 0;
};

/* Compares two shipPosition arrays and determines which is "greater".
The "greatest" array is the one with a greater ship, compared from left to right
@param pA position A
@param pB position B
@return int comparing the sizes, ie -1 if A>B, 0 if A=B, 1 if A<B
*/
int compareShipArray(shipPosition *pA, shipPosition *pB){
    int compare=0;
    size_t i = 0;
    while (compare==0 && i<FLEET_SIZE){
        compare = compareShipPositions(pA[i],pB[i]);
        i++;
    }
    return compare;
};


// -- Ship Position <-> numbers

/* Convert a shipPosition to a number
@param p shipPosition to be converted
@return positive int describing the position
*/
unsigned long shipPosToInt(shipPosition p){
    return p.dir*pow(BOARD_SIZE,2)+p.x*(BOARD_SIZE)+p.y;
};

/* Convert a shipPosition array to a number
@param p pointer to an array of ship positions to be converted
@return positive int describing the positions
*/
unsigned long shipArrayToInt(shipPosition *p){
    unsigned long i=0;
    unsigned long adr=FLEET_SIZE-1;

    unsigned long result=0;
    while (i<FLEET_SIZE){
        result+=shipPosToInt(p[i])*pow(pow(BOARD_SIZE,2)*2,adr);
        i++;
        adr--;
    }
    return result;
};

/* Convert a number to a shipPosition
@param input the number detailing the position
@param p shipPosition reference to be updated
*/
void intToShipPos(unsigned long input,shipPosition &p){
    p.dir = 0, p.x = 0, p.y= 0;

    p.dir=floor(input/pow(BOARD_SIZE,2));
    if (p.dir) input-=pow(BOARD_SIZE,2);
    p.x = input/BOARD_SIZE;
    p.y = input%BOARD_SIZE;

    if (input>=pow(BOARD_SIZE,2)) p.x= BOARD_SIZE-1, p.y = BOARD_SIZE-1;
};

/* Convert a number to a shipPosition
@param input the number detailing the position
@param p the shipPosition array as a reference to be updated
*/
void intToShipArray(unsigned long input, shipPosition *p){
    int i=0;
    int j=FLEET_SIZE-1;
    unsigned long radix = std::pow(BOARD_SIZE,2)*2;
    
    if (input>=pow(radix,FLEET_SIZE)){ // if too big, make max value instead
        setEndArray(p);
        return;
    }

    setStartArray(p); //Back to 0s, clears previous values

    while (j>=0) {
        if (0>input) break;

        if (input<pow(radix,j)){ // the value isn't big enough for this spot in the array
        i++; j--;
        continue;
        }

        unsigned long baseModInput = floor(input/pow(radix,j));

        intToShipPos(baseModInput,p[i]);
        input-=baseModInput*pow(radix,j);
        i++; j--;
    }
};

/* Convert a number to a board
@param input the number detailing the position
@return the board with a matching shiip position as input
*/
board intToBoard(unsigned long input){
    board b = initBlankBoard();
    shipPosition pos[FLEET_SIZE];

    intToShipArray(input, pos);
    drawBoard(b, pos);

    return b;
};


// -- Itterate positions

/* Given a ship position and a ship, generate the next one in sequence.
Assuming the ship has a length of one, and will thus fit in any cell 
@param p ship position, as a reference such that it gets updated
*/
void nextShipPosition(shipPosition &p){
    nextShipPosition(p,1);
};

/* Given a ship position and a ship, generate the next one in sequence
@param p ship position, as a reference such that it gets updated
@param s ship size, to avoid generating positions where a ship of length s doesn't fit
*/
void nextShipPosition(shipPosition &p, const ship s){ 
    p.y++;
    
    if (p.dir && p.y >= BOARD_SIZE){
        p.y=0;
        p.x++;
    } else if (!p.dir && p.y > BOARD_SIZE-s){
        p.y=0;
        p.x++;
    }

    if (p.dir && p.x > BOARD_SIZE-s){
        p.x=0;
        p.y=0;
        p.dir = !p.dir;
    } else if (!p.dir && p.x >= BOARD_SIZE){
        p.x=0;
        p.y=0;
        p.dir = !p.dir;
    }
};

/* Generate the next ship position array, from the current position
@param p a pointer to the ship position array
@param s a pointer to an array of ship sizes, to avoid generating positions where a ship of length s doesn't fit
*/
void nextShipPosArray(shipPosition* p, const ship *s){
    for (int i = FLEET_SIZE-1; i >= 0; i--){
        nextShipPosition(p[i],s[i]);
        if (!isStartPos(p[i])){
        return;
        }
    }
};


// -- Checking & setting array values

/* Checks if the ship is at the starting position
@param p a ship position
@return bool true if at the start, else false
*/
bool isStartPos(shipPosition p){
    return p.x == 0 && p.y == 0 && p.dir == 0;
};

/* Checks if the ships in an array are all at the starting position
@param p a pointer to an array of ship positions
@return bool true if all at the start, else false
*/
bool isStartArray(shipPosition *p){
    for (size_t i=0; i<FLEET_SIZE; i++) if (!isStartPos(p[i])) return false;
    return true;
};

/* Sets every value of a ship position array to the end
@param p a pointer to a ship posion array
*/
    void setEndArray(shipPosition *p){
    for (size_t i = 0; i < FLEET_SIZE; i++){
        p[i].x=BOARD_SIZE-FLEET[i];
        p[i].y= BOARD_SIZE-1;
        p[i].dir=1; // true (->) is the last value
    }
}

/* Sets every value of a ship position array to the start
@param p a pointer to a ship posion array
*/
void setStartArray(shipPosition *p){
    for (size_t i = 0; i < FLEET_SIZE; i++){
        p[i].x=0;
        p[i].y=0;   
        p[i].dir=0;
    }
};

/* Checks if a hitmask has finished the game (sunk all ships)
@param h a hitmask with shots
@return bool true if the hitmask has sunk all ships, else false
*/
bool isHitmaskSolved(hitmask h){
    int fleetPositionCount=0; //FIXME could change now to looking thru the shipSunk array for false/true values (all true then solved)
    for (size_t i = 0; i < FLEET_SIZE; i++) fleetPositionCount += FLEET[i];

    int numShipPos=fleetPositionCount; // get the total expected hits and shots 
    
    for (int y = 0; y < BOARD_SIZE; y++){
        for (int x = 0; x < BOARD_SIZE; x++){
        if (h.hitmask[x][y] == HIT || h.hitmask[x][y] == SUNK) numShipPos--;
        if (numShipPos <= 0) return true;
        }
    }
    return false;
};

/* Checks if coordinate (x,y) has been hit
@param h a hitmask with shots
@param x coordinate for shot
@param y cordinate for shot
@return bool true if hit, else false
*/
bool isHit(hitmask h, int x, int y){
    return h.hitmask[x][y] != UNKNOWN;
};


// -- Output functions

// FIXME actually implement more template methods to reduce the overloads?
template <class Type>

Json::Value jsonArrayAdderTEST(std::vector<Type> inVector) {
    Json::Value resultArray(Json::arrayValue);

    for (Type val : inVector){
        resultArray.append(val);
    }

    return resultArray;
};

Json::Value jsonArrayAdder(long unsigned int inputArray[][BOARD_SIZE]){
    Json::Value resultArray(Json::arrayValue);

    for (int y = 0; y < BOARD_SIZE; y++){
        Json::Value resultArray_row(Json::arrayValue);
        for (int x = 0; x < BOARD_SIZE; x++){
            resultArray_row.append(inputArray[x][y]);
        }
        resultArray.append(resultArray_row);
    }

    return resultArray;
};

Json::Value jsonArrayAdder(int inputArray[][BOARD_SIZE]){
    Json::Value resultArray(Json::arrayValue);

    for (int y = 0; y < BOARD_SIZE; y++){
        Json::Value resultArray_row(Json::arrayValue);
        for (int x = 0; x < BOARD_SIZE; x++){
            resultArray_row.append(inputArray[x][y]);
        }
        resultArray.append(resultArray_row);
    }

    return resultArray;
};

Json::Value jsonArrayAdder(double inputArray[][BOARD_SIZE]){
    Json::Value resultArray(Json::arrayValue);

    for (int y = 0; y < BOARD_SIZE; y++){
        Json::Value resultArray_row(Json::arrayValue);
        for (int x = 0; x < BOARD_SIZE; x++){
            resultArray_row.append(inputArray[x][y]);
        }
        resultArray.append(resultArray_row);
    }

    return resultArray;
};

Json::Value jsonArrayAdder(const int inputArray[], const size_t size){
    Json::Value resultArray(Json::arrayValue);

    for (int i = 0; i < size; i++){
        resultArray.append(inputArray[i]);
    }

    return resultArray;
};

void jsonFileoutput(std::string filename, Json::Value jsonOut){
    std::ofstream outfile;
    outfile.open(filename);
    Json::StreamWriterBuilder builder;
    std::string json_file = Json::writeString(builder, jsonOut);
    outfile << json_file << std::flush;
    outfile.close();
};

// toString for board
std::ostream& operator<<(std::ostream& os, board& b){
    os << "--- empty:"<<b.isEmpty<<" valid: "<<b.isValid<<" ---\n";
    for (int y = 0; y < BOARD_SIZE; y++){
        os << "[";
        for (int x = 0; x < BOARD_SIZE; x++){
            if (b.board[x][y] == BOARD_DEFAULT) os << " , ";
            else os << b.board[x][y] << ", ";
        }
        os << "]\n";
    }
  return os;
};

// toString for hitmask
std::ostream& operator<<(std::ostream& os, hitmask& h){
    os << "--- hitmask ---\nBoats: ";
    for (size_t i = 0; i < FLEET_SIZE; i++) os << h.shipSunk[i] << ", ";
    os << "\n";

    for (int y = 0; y < BOARD_SIZE; y++){
        os << "[";
        for (int x = 0; x < BOARD_SIZE; x++){
        char rep;
        switch (h.hitmask[x][y]){
            case UNKNOWN: 
            rep = ' '; //'?';
            break;
            case MISS:
            rep='O';
            break;
            case HIT:
            rep='X';
            break;
            case SUNK:
            rep='S';
            break;
        }
        os <<rep << ", ";
        }
        os << "]\n";
    }
    os << std::flush;

    return os;
};

// toString for probability grid
std::ostream& operator<<(std::ostream& os, probabilityGrid& p){
    os << "--- total:" << p.totalGoodBoards << " ---\n";

    for (int y = 0; y < BOARD_SIZE; y++){
        os << "[";
        for (int x = 0; x < BOARD_SIZE; x++){
        os << p.shipGrid[x][y] << ", ";
        }
        os << "]\n";
    }
    return os;
};

// toString for a vector of workers
std::ostream& operator<<(std::ostream& os, std::vector<worker>& wrks){
    os <<"workers " << wrks.size()<<'\n';

    for (auto &w :wrks){
        os <<"Worker: checking "<<shipArrayToInt(w.end)-shipArrayToInt(w.start)<<" boards\n\t";
        for (size_t j = 0; j < FLEET_SIZE; j++)
        os  << "("<< w.start[j].x << ", " << w.start[j].y << ", " << w.start[j].dir << ")\t";

        os <<"\n\t";
        for (size_t j = 0; j < FLEET_SIZE; j++)
        os  << "("<< w.end[j].x << ", " << w.end[j].y << ", " << w.end[j].dir << ")\t";
        os <<'\n';
    }
    return os;
};

// toString for a worker
std::ostream& operator<<(std::ostream& os, worker& worker){
    os << shipArrayToInt(worker.start) << "," << shipArrayToInt(worker.end);
    return os;
};

// toString for runWorker state
std::ostream& operator<<(std::ostream& os, runWorkerState& w){
    os << "\tSave (" << w.saveFileBool << ") to " << w.saveFilename;
    os << "\tRead (" << w.readFileBool << ") from " << w.readFilename << std::endl;
    return os;
};

// input from stdIO for a worker
std::istream& operator>>(std::istream& is, worker& worker){ //FIXME later - would be nicer to use this rather than inputWorker
    unsigned long numStart, numEnd;
    is >> numStart;
    is >> numEnd;
    intToShipArray(numStart, worker.start);
    intToShipArray(numEnd, worker.end);

    // std::string start, end;
    // getline(is,start,',');
    // getline(is,  end,',');

    // intToShipArray((unsigned long)(start), worker.start);
    return is;
};

worker inputWorker(std::string inLine){
    worker w;

    unsigned long startI, endI;
    // an int = string to int (sub string(start, to end of comma))
    startI = std::stoi(inLine.substr(0,inLine.find(",")));
    endI = std::stoi(inLine.substr(inLine.find(",")+1,inLine.size()));

    intToShipArray(startI, w.start);
    intToShipArray(endI, w.end);
    // printWorkers({w});

    return w;

};


// -- ProbabilityGrid functions

/* Collates a vector of workers into a single probability grid
@param p reference to a probability grid
@param sweatshop a vector of workers
*/
void gatherProbabilityFromWorkers(probabilityGrid &p, std::vector<worker> sweatshop){
    // reset all values to 0
    p.totalGoodBoards = 0;
    memset(p.shipGrid, 0, sizeof(p.shipGrid));
    memset(p.shipProb, 0, sizeof(p.shipProb));
    memset(p.pChange, 0, sizeof(p.pChange));
    
    // sum the worker probability data
    for (auto &w : sweatshop){ 
        appendWorkerToProbGrid(p,w);
    }
    calcProbabilityGrid(p);
};

/* Appends data from a workers probability grid, to a comunal probability grid.
@param p reference to a probability grid
@param w a worker
*/
void appendWorkerToProbGrid(probabilityGrid &p, worker w){
    p.totalGoodBoards += w.sub_probGrid.totalGoodBoards;
    for (int y = 0; y < BOARD_SIZE; y++){
        for (int x = 0; x < BOARD_SIZE; x++){
        p.shipGrid[x][y]+=w.sub_probGrid.shipGrid[x][y];  // FIXME i could use memcopy more efficently here
        p.infoGain[x][y]+=w.sub_probGrid.infoGain[x][y];
        }
    }
};

/* Calculates various probabilities attached to a probability grid using the data in p.shipGrid
@param p reference to a probability grid
*/
void calcProbabilityGrid(probabilityGrid &p){
    for (int y = 0; y < BOARD_SIZE; y++){
        for (int x = 0; x < BOARD_SIZE; x++){
        p.shipProb[x][y] = static_cast<double>(p.shipGrid[x][y])/static_cast<double>(p.totalGoodBoards);
        p.pChange[x][y] = pow(p.shipProb[x][y],2)+pow(1-p.shipProb[x][y],2); // x^2+(1-x)^2 (where x=shipProb as above)
        }
    }
};

/* Given a board, if there is a ship in each cell, update the corresponding probability data
@param b a board to gather data from
@param p reference to a probability grid
*/
void flattenBoardToProbabilityGrid(board b,probabilityGrid &pG){
    if (!b.isValid) return;

    for (int y = 0; y < BOARD_SIZE; y++){
        for (int x = 0; x < BOARD_SIZE; x++){
        if (b.board[x][y] != BOARD_DEFAULT) pG.shipGrid[x][y]++;
        }
    }
};


// -- Thread and bulk bits

/* Checks if a section of boards matches a hitmask, given a workers start stop information
@param w a section of boards to check
@param hitM the hitmask to compare the boards too
@param threadID an ID number for debugging
*/
void checkBoards(worker &w, hitmask hitM, int threadID){
    runWorkerState noReadWrite;
    std::ofstream nullFile;
    checkBoardsSaveFile(w, hitM, threadID, noReadWrite, nullFile);
};

/* Checks if a section of boards matches a hitmask, given a workers start stop information
@param w a section of boards to check
@param hitM the hitmask to compare the boards too
@param threadID an ID number for debugging
@param saveSettings information on if to save and recall the worker changes
@param outfileWorker the output file for the worker changed to save // FIXME should be internal to the runWorkerState struct
*/
void checkBoardsSaveFile(worker &w, hitmask hitM, int threadID, runWorkerState saveSettings, std::ofstream &outfileWorker){
    board b = initBlankBoard();
    shipPosition pA[FLEET_SIZE]; // position array
    std::copy(w.start, w.start+FLEET_SIZE, std::begin(pA));
    
    bool previousState = false;
    shipPosition previousStateShipPos[FLEET_SIZE];
    std::copy(previousStateShipPos, previousStateShipPos+FLEET_SIZE, std::begin(pA));
    intToShipArray(b.shipPositionsInt, previousStateShipPos);
    // if (verbose) std::cout << "CheckBoards in " << threadID << saveSettings;
    
    if (saveSettings.saveFileBool){
        if (!outfileWorker.is_open()){
        std::cout << "ERROR! in " << threadID <<" Unable to open save file \"" << saveSettings.saveFilename << "\"" << std::endl;
        abort();
        }
    }

    do{ // check all the boards from a workers start to end
        drawBoard(b,pA);
        if (b.isValid && checkCompatible(b,hitM)){ // if the board is a good board
        w.sub_probGrid.totalGoodBoards++; // update the workers probability grid 
        flattenBoardToProbabilityGrid(b,w.sub_probGrid);
        } 

        // If theres a change in validity (and if the chunks of valid board are being recorded)
        if (saveSettings.saveFileBool && previousState != b.isValid){
        if (previousState){ //if the previous state was valid, then save it in a new worker
            worker newSubWorker;
            std::copy(previousStateShipPos, previousStateShipPos+FLEET_SIZE, std::begin(newSubWorker.start));
            intToShipArray(b.shipPositionsInt, previousStateShipPos); //update the previous ship pos to current
            std::copy(previousStateShipPos, previousStateShipPos+FLEET_SIZE, std::begin(newSubWorker.end));

            saveFile_mutex.lock();
            outfileWorker << newSubWorker << std::endl;
            saveFile_mutex.unlock();
        }
        else {
            intToShipArray(b.shipPositionsInt, previousStateShipPos); //update the previous ship pos to current
        }
        previousState = b.isValid; //set the previous state to the current state
        }

        nextShipPosArray(pA, FLEET);
    } while (compareShipArray(pA,w.end)==1); //while the current pos array is behind the end

    // if (verbose) std::cout << "CheckBoards " << threadID << " done" << std::endl;
};

/* Splits the board sections into threadCount number of workers
@param threadCount the number of sections to create
@param w vector of workers as a refference (they get updated)
*/
void dividePositions(int threadCount,std::vector<worker> &w){
    w.clear();
    w.reserve(threadCount);

    shipPosition pS[FLEET_SIZE]; //position Start
    shipPosition pE[FLEET_SIZE]; //position End

    // bounds & divisions of the position arrays
    unsigned long radix = std::pow(BOARD_SIZE,2)*2;
    setEndArray(pE);
    unsigned long maxSegValue = shipArrayToInt(pE);
    unsigned long segmentSize = maxSegValue/threadCount;

    for (size_t i = 1; i < threadCount+1; i++){
        intToShipArray(segmentSize*(i-1), pS);
        intToShipArray(segmentSize*i, pE);
        
        worker newWorker;
        std::copy(pS, pS+FLEET_SIZE, std::begin(newWorker.start));
        std::copy(pE, pE+FLEET_SIZE, std::begin(newWorker.end));
        w.push_back(newWorker);
    }
}

void runThreads(hitmask hitM, probabilityGrid &probGrid, int threadCount){
    runWorkerState noReadWrite;
    runThreads(hitM, probGrid, threadCount, noReadWrite);
};

void runThreads(hitmask hitM, probabilityGrid &probGrid, int threadCount, runWorkerState workerSettings){
    if (verbose) std::cout << "Starting " << threadCount << " threads and saving the workers to " << workerSettings.saveFilename << std::endl;
    std::vector<worker> sweatshop;
    std::vector<std::thread> sweatshopThreads;

    auto start = high_resolution_clock::now();
    int threadID = 0;
    
    // if (verbose) std::cout << workerSettings;
    if (workerSettings.readFileBool){
        runThreadsRead(hitM, probGrid, threadCount, workerSettings);
        return;
    }

    if (workerSettings.readFileBool){
        // read all the lines from a file and store each one as a worker
        std::string fileLine;
        std::ifstream myfile (workerSettings.readFilename);
        if (myfile.is_open()) {
        while (getline (myfile,fileLine)) {
            sweatshop.push_back(inputWorker(fileLine));
        }
        myfile.close();
        } else{
        std::cout << "ERROR! Unable to open read file \"" << workerSettings.readFilename << "\""<< std::endl;
        }
        if (verbose) std::cout << "Made " << sweatshop.size() << " threads" << std::endl;
    }
    else{
        // Make and split a vector of workers
        dividePositions(threadCount,sweatshop);
    }

    std::ofstream outfileWorker;
    if (workerSettings.saveFileBool){
        outfileWorker.open(workerSettings.saveFilename);
    }


    // Start all the threads
    if (verbose) std::cout << "Made " << sweatshop.size() << " workers and am about to start threads" << std::endl;
    for (auto &w : sweatshop){
        if (workerSettings.saveFileBool){
        std::thread thr(checkBoardsSaveFile, std::ref(w), hitM, threadID++, workerSettings, std::ref(outfileWorker));
        sweatshopThreads.push_back(std::move(thr));
        } else {
        std::thread thr(checkBoards, std::ref(w), hitM, threadID++);
        sweatshopThreads.push_back(std::move(thr));
        }
    }
    
    // Wait for all the threads to be finished
    for (std::thread & th : sweatshopThreads){
        if (th.joinable())
        th.join();
    }


    if (workerSettings.saveFileBool){
        std::ofstream outfileWorker;
        outfileWorker.close();
    }

    // Sum it up and get time
    gatherProbabilityFromWorkers(probGrid, sweatshop);

    auto stop = high_resolution_clock::now();
    auto runTime = duration_cast<seconds>(stop - start);
    // if (verbose) std::cout << probGrid.totalGoodBoards << " boards found in " << runTime.count() <<" seconds\n" ;
};

void runThreadsRead(hitmask hitM, probabilityGrid &probGrid, int threadCount, runWorkerState workerSettings){
    // BUG something in the reading means the threads never finish reading the same file
    if (!workerSettings.readFileBool){
        std::cerr <<"Code called \"runThreadsRead\" with a worker that cannot read! Aborting" << std::endl;
        abort();
    }

    if (verbose) std::cout << "READ MODE: Starting " << threadCount << " threads and saving the workers to " << workerSettings.saveFilename << std::endl;
    std::vector<worker> sweatshop;
    std::vector<std::thread> sweatshopThreads;

    const int MAX_WORKERS = 500; // Total number of threads going at one time
    auto start = high_resolution_clock::now();
    int threadID = 0;

    /*
    Open file
    launch a pool of size x
    while lines to read

    wait for pool to finish
    */

    // Read & Save file data
    std::ofstream outfileWorker;
    if (workerSettings.saveFileBool){
        outfileWorker.open(workerSettings.saveFilename);
    }
    std::string inFileLine;
    std::ifstream inFile (workerSettings.readFilename);

    // READING IN


    // read all the lines from a file and store each one as a worker
    if (inFile.is_open()) {
        int i = 0;
        while (getline (inFile,inFileLine)) {
            while (sweatshop.size()<MAX_WORKERS) {
            sweatshop.push_back(inputWorker(inFileLine));
            }


            // Start all the threads
            if (verbose) std::cout << i << ") Made " << sweatshop.size() << " workers and am about to start threads" << std::endl;
            for (auto &w : sweatshop){
            if (workerSettings.saveFileBool){
                std::thread thr(checkBoardsSaveFile, std::ref(w), hitM, threadID++, workerSettings, std::ref(outfileWorker));
                sweatshopThreads.push_back(std::move(thr));
            } else {
                std::thread thr(checkBoards, std::ref(w), hitM, threadID++);
                sweatshopThreads.push_back(std::move(thr));
            }
            }

            // Wait for all the threads to be finished
            for (std::thread & th : sweatshopThreads){
            if (th.joinable())
                th.join();
            }

            while (sweatshop.size()>0){
            worker lovelace = sweatshop.back();
            appendWorkerToProbGrid(probGrid, lovelace);
            sweatshop.pop_back();
            }
            i++;
        }
        inFile.close();
    } else{
        std::cout << "ERROR! Unable to open read file \"" << workerSettings.readFilename << "\""<< std::endl;
    }


    // Close the save file
    if (workerSettings.saveFileBool){
        std::ofstream outfileWorker;
        outfileWorker.close();
    }

    // Sum it up and get time
    gatherProbabilityFromWorkers(probGrid, sweatshop);

    auto stop = high_resolution_clock::now();
    auto runTime = duration_cast<seconds>(stop - start);
    // if (verbose) std::cout << probGrid.totalGoodBoards << " boards found in " << runTime.count() <<" seconds\n" ;
};

// -- Game Play (and position deciding)

/* Plays an entire game of battleship from the start
@param playStyle the method used to shoot at the board
@param b the specific board to be played
@return the number of turns the game takes to play
*/
unsigned int playGame(coordinateChooser playStyle, board b){
    Json::Value rubishJSON;
    return playGame(playStyle, b, rubishJSON);
}

unsigned int playGame(coordinateChooser playStyle, board b, Json::Value &gamePlayHistory){
    return playGame(playStyle, b, gamePlayHistory, std::pow(BOARD_SIZE,2)+1);
}

unsigned int playGame(coordinateChooser playStyle, board b, Json::Value &gamePlayHistory, int playStyleTurnCount){
    if(verbose) std::cout << b << std::endl;
    
    // init JSON
    Json::Value shotRecordJson = gamePlayHistory["shotRecord"];
    Json::Value probabilityGridJson = gamePlayHistory["probabilityGrid"];
    Json::Value infoGainGridJson = gamePlayHistory["infoGainGrid"];

    // init Hitmask & misc
    hitmask hitM;
    probabilityGrid probGrid;
    unsigned int turns = 0;
    auto start = high_resolution_clock::now(); //start timing
    std::vector<std::string> playStylePerTurn;
    
    runWorkerState storeData = {true, "./out/workerSerialisation/turn" + std::to_string(turns) + ".txt", false, "BLANK-FILE"};

    while (!isHitmaskSolved(hitM)){
        int x, y = 0;
        takeTurn(playStyle, b, hitM, probGrid, storeData, x,y);

        if (verbose){
            std::cout << "\nPROBABILITY GRID:\n" << probGrid << std::endl;
            std::cout << "\nHITMASK:\n" << hitM << std::endl;
        }

        // record information gathered
        turns++;
        Json::Value currentCoords(Json::arrayValue);
        currentCoords.append(x);
        currentCoords.append(y);
        shotRecordJson.append(currentCoords);
        probabilityGridJson.append(jsonArrayAdder(probGrid.shipGrid));
        infoGainGridJson.append(jsonArrayAdder(probGrid.infoGain));
        std::cout << std::flush;

        storeData.saveFilename =  "./out/workerSerialisation/turn" + std::to_string(turns);
    }
    //FIXME: surely theres a better way to do this
    gamePlayHistory["shotRecord"] = shotRecordJson;
    gamePlayHistory["probabilityGrid"] = probabilityGridJson;
    gamePlayHistory["infoGainGrid"] = infoGainGridJson;
    gamePlayHistory["turnsTaken"] = turns;

    auto stop = high_resolution_clock::now();
    auto runTime = duration_cast<seconds>(stop - start);

    int fleetPositionCount = 0;
    for (size_t i = 0; i < FLEET_SIZE; i++) fleetPositionCount += FLEET[i];

    std::cout << "\tGAME OVER! you took a total of " << turns << " turns in " << runTime.count() <<" seconds.\n\tshot success rate: " << (double)(fleetPositionCount)/(double)(turns) << std::endl;

    return turns;
};

/* Given a method of 
@param playStyle which method is used to pick the next shot
@param b game board with positions of all games
@param hitM hitmask of current game, to be used as reference when picking shot and updated after shot
@param probGrid grid that holds the game probabilities
@param storeData spefification of how to save/remember worker changes to speed up work 
@param x coordinate to shoot
@param y coordinate to shoot
*/
void takeTurn(coordinateChooser playStyle, board b, hitmask &hitM, probabilityGrid &probGrid, runWorkerState storeData, int &x, int &y){
    if(isHitmaskSolved(hitM)) return;
    
    // gather data
    if(playStyle != RND) runThreads(hitM, probGrid, threadCount, storeData);
    
    // int x, y = 0;
    do{ // decide where to shoot
        switch(playStyle){
        case RND:
            coordinate_rnd(x,y,hitM);
            break;
        case RND_W_PROB:
            coordinate_rndWProb(x,y,probGrid,hitM);
            break;
        case P_MAX:
            coordinate_pMax(x,y,probGrid,hitM);
            break;
        case P_RND:
            coordinate_pRnd(x,y,probGrid,hitM);
            break;
        case INFOGAIN:
            coordinate_infoGain(x,y,probGrid,hitM);
            break;
        case DIAGONAL:
            coordinate_diagonal(x,y,probGrid,hitM);
            break;
        case FLEXI:
            double totalIG;
            totalIG = coordinate_infoGain(x,y,probGrid,hitM);
            if (totalIG < 0.00001){
            // if (verbose) std::cout <<"pMax now!\n";
            coordinate_pMax(x,y,probGrid,hitM);
            }
            break;
        case USER_INPUT:
        default:
            coordinate_userInput(x,y);

        }

        if (isHit(hitM, x, y)){
            if(verbose) std::cout << "You already hit (" << x << ", " << y << ")\n";
        }
        if (verbose) std::cout << "You entered (" << x << ", " << y << ") using " << coordinateChooserNames[playStyle] << std::endl;
        std::cout << std::flush;
    } while (isHit(hitM, x, y)); // repeat until the hit is valid (ie cell isn't yet hit)
    
    // Take the shot
    hitBoard(b,hitM,x,y);

    // After the shot has been done gather information again 

    // record information gathered
    // Json::Value currentCoords(Json::arrayValue);
    // currentCoords.append(x);
    // currentCoords.append(y);
    // shotRecordJson.append(currentCoords);
    // probabilityGridJson.append(jsonArrayAdder(probGrid.shipGrid));
    // infoGainGridJson.append(jsonArrayAdder(probGrid.infoGain));
    // std::cout << std::flush;

};

unsigned int saveGame(coordinateChooser gameState, board b, int gameStateTurnCount){
    return saveGame(gameState, b, gameStateTurnCount,"");
};

/* saves a game by playing it then exporting the info in a json file
@param playstyle the method used to shoot at the board
@param b the specific board to be played
@param playstyleTurnCount how many turns of that playStyle to use before defaulting to pMax
@param igExtra an extra string used for describing infoGain turns only
@return the number of turns the game takes to play
*/
unsigned int saveGame(coordinateChooser playStyle, board b, int playStyleTurnCount, std::string igExtra){
    std::string filename = std::tmpnam(nullptr);

    // filename in the form: boardSize_fleetSize_boardID_playStyle_randomChars.json
    filename = "./out/gamePlay/"+std::to_string(BOARD_SIZE)+"_"+std::to_string(FLEET_SIZE)+"_"
                            +std::to_string(b.shipPositionsInt)+"_"+coordinateChooserNames[playStyle]+igExtra+"_"
                            +codeVersion+"_"+filename.substr(9, filename.length())+".json";

    Json::Value gamePlayHistory;
    gamePlayHistory["FLEET_SIZE"] = FLEET_SIZE;
    gamePlayHistory["FLEET"] = jsonArrayAdder(FLEET, FLEET_SIZE);
    gamePlayHistory["BOARD_SIZE"] = BOARD_SIZE;
    gamePlayHistory["board"] = jsonArrayAdder(b.board);
    gamePlayHistory["version"] = codeVersion;
    gamePlayHistory["shotMethod"] = coordinateChooserNames[playStyle];
    if (playStyle == INFOGAIN) gamePlayHistory["infoGainTurns"] = playStyleTurnCount;


    int turnCounter = playGame(playStyle, b, gamePlayHistory, playStyleTurnCount);

    jsonFileoutput(filename, gamePlayHistory);
    std::cout<< "\tsaving to " << filename << "\n" << std::endl;

    return turnCounter;
};

/* Plays a number of games repeatedly
@param playStyles vector of which coordinate choosing methods should be chosen
@param repeats how many times it should repeat
*/
void repeatGames(std::vector<coordinateChooser> playStyles, int repeats){
    board b = rndBoard();
    repeatGames(playStyles, repeats, false, b);
};

/* Plays a number of games repeatedly
@param playStyles vector of which coordinate choosing methods should be chosen
@param repeats how many times it should repeat
@param sameBoard is the board the same per repettion
@param b the board to be used
*/
void repeatGames(std::vector<coordinateChooser> playStyles, int repeats, bool sameBoard, board b){
    for (auto gameState : playStyles){
        for (size_t i = 0; i < repeats; i++){
        if (!sameBoard){
            b = rndBoard();
        }
        saveGame(gameState, b, 100);
        }
    }
};

void repeatIGRange(std::vector<board> repeats){
std::vector<int> turnCounts;
for (board b: repeats){
    std::cout << b << std::endl;
    turnCounts.clear();
    if (b.isValid == false){ //if board isn't valid, stop board
    std::cout << "ERROR! INVALID BOARD -- program terminating" << std::endl;
    std::cout << b << std::endl;
    abort();
    }

    for (int igCount = 0; igCount <= std::pow(BOARD_SIZE,2); igCount++){
    int singleTurnCount = saveGame(INFOGAIN, b, igCount, "-shots"+std::to_string(igCount));
    turnCounts.push_back(singleTurnCount);

    if (singleTurnCount < igCount){
        std::cout << "Pointless tests. Ending early " << std::endl;
        break;
    }
    }

    std::string filename = std::tmpnam(nullptr);

    // filename in the form: boardSize_fleetSize_boardID_gameState_randomChars.json
    filename = "./out/gamePlay/INFOGAIN_CHANGES_"+std::to_string(BOARD_SIZE)+"_"+std::to_string(FLEET_SIZE)+"_"
                            +std::to_string(b.shipPositionsInt)+"_"
                            +codeVersion+"_"+filename.substr(9, filename.length())+".json";

    Json::Value infoGainHistory;
    infoGainHistory["FLEET_SIZE"] = FLEET_SIZE;
    infoGainHistory["FLEET"] = jsonArrayAdder(FLEET, FLEET_SIZE);
    infoGainHistory["BOARD_SIZE"] = BOARD_SIZE;
    infoGainHistory["board"] = jsonArrayAdder(b.board);
    infoGainHistory["version"] = codeVersion;
    infoGainHistory["TurnsTaken"] = jsonArrayAdderTEST(turnCounts);

    jsonFileoutput(filename, infoGainHistory);

    std::cout<< "INFOGAIN TOTAL: saving " << turnCounts.size() <<" turns to " << filename << std::endl;
}
};


// -- Coordinate choosing 

/* User chooses where to shoot
@param x coordinate for shot
@param y cordinate for shot
*/
void coordinate_userInput(int &x, int &y){
    std::cout << "X: ";
    std::cin >> x;

    std::cout << "Y: ";
    std::cin >> y;
};

/* Choses uniform random (x,y) to shoot
@param xReturn coordinate for shot
@param yReturn cordinate for shot
@param hitM hitmask to ensure shot hasn't been taken yet
*/
void coordinate_rnd(int &xReturn, int &yReturn, hitmask hitM){
    std::uniform_int_distribution<std::mt19937::result_type> udist(0,BOARD_SIZE-1);
    do {
        xReturn = udist(rng);
        yReturn = udist(rng);
    } while (isHit(hitM,xReturn,yReturn));
};

/* Choses a random (x,y) over a weighted distribution of shipGrid counts to shoot
@param xReturn coordinate for shot
@param yReturn cordinate for shot
@param pG probability grid for weighted distribution
@param hitM hitmask to ensure shot hasn't been taken yet
*/
void coordinate_rndWProb(int &xReturn, int &yReturn, probabilityGrid pG, hitmask hitM){

    std::vector<unsigned long> flattened; // 1d array because it works bettwer w/ weighted distribution
    
    for (auto & arrayProb : pG.shipGrid){
        for (auto & prob : arrayProb){
        flattened.push_back(prob);
        }
    }
    std::discrete_distribution<int> distribution(flattened.begin(), flattened.end());

    std::random_device rd;
    std::mt19937 gen(rd());

    do {
        int place1D = distribution(gen);
        xReturn = place1D/BOARD_SIZE;
        yReturn = place1D % BOARD_SIZE;
        // std::cout<< place1D << ", (" << xReturn << ", " << yReturn << ")\n";
    } while (isHit(hitM,xReturn,yReturn));


};

/* Choses the shot (x,y) that maximises the shipGrid
@param xReturn coordinate for shot
@param yReturn cordinate for shot
@param pG probability grid to find pMax value in the shipGrid array
@param hitM hitmask to ensure shot hasn't been taken yet
*/
void coordinate_pMax(int &xReturn, int &yReturn, probabilityGrid pG, hitmask hitM){
    unsigned long min = -1;
    unsigned long max = 0;
    int minX,minY=0;
    int maxX,maxY=0;

    for (int y = 0; y < BOARD_SIZE; y++){
        for (int x = 0; x < BOARD_SIZE; x++){
        if (pG.shipGrid[x][y] < min && !isHit(hitM,x,y)){
            min = pG.shipGrid[x][y];
            minX = x;
            minY = y;
        }

        if (pG.shipGrid[x][y] > max && !isHit(hitM,x,y)){
            max = pG.shipGrid[x][y];
            maxX = x;
            maxY = y;
        }
        }
    }
    xReturn = maxX;
    yReturn = maxY;
};

/* Choses the shot (x,y) that maximises the shipGrid multipled by a random double
@param xReturn coordinate for shot
@param yReturn cordinate for shot
@param pG probability grid to find pMax with a random elelment
@param hitM hitmask to ensure shot hasn't been taken yet
*/
void coordinate_pRnd(int &xReturn, int &yReturn, probabilityGrid pG, hitmask hitM){
    double min = std::numeric_limits<double>::max();
    double max = 0;
    int minX,minY=0; // min isn't yet used but no harm in finding them
    int maxX,maxY=0;
    std::uniform_real_distribution<> dis(0,1);

    for (int y = 0; y < BOARD_SIZE; y++){
        for (int x = 0; x < BOARD_SIZE; x++){
        double scaledProb = pG.shipProb[x][y]*dis(rng);
        if (scaledProb < min && !isHit(hitM,x,y)){
            min = scaledProb;
            minX = x;
            minY = y;
        }

        if (scaledProb > max && !isHit(hitM,x,y)){
            max = scaledProb;
            maxX = x;
            maxY = y;
        }
        }
    }
    xReturn = maxX;
    yReturn = maxY;
};

/* Find the position that maximises the information gain
for every unknown cell in the hitmask, simulate each possible outcome (miss, hit and sink (for each possible boat))
and the information gain for that cell is equal to the sum of (num of boards matching option * probability of option) for each outcome
@param xReturn coordinate for shot
@param yReturn cordinate for shot
@param pG probability grid to record infoGain data
@param hitM hitmask to ensure shot hasn't been taken yet
*/
double coordinate_infoGain(int &xReturn, int &yReturn, probabilityGrid &pG, hitmask hitM){
    double max = 0;
    int maxX, maxY = 0;
    double infoGainSum = 0;

    std::vector<cellStatus> options = {MISS, HIT, SUNK};

    for (int y = 0; y < BOARD_SIZE; y++){
        for (int x = 0; x < BOARD_SIZE; x++){ // for each cell
        pG.infoGain[x][y] = 0;

        if (!isHit(hitM, x,y)){ // if the cell hasn't been hit yet
            for (auto opt : options){

            // if testing sunk, and there aren't any surounding hits, don't bother
            if(opt == SUNK && !(((x-1) >= 0 && hitM.hitmask[x-1][y] == HIT )||((x+1<=BOARD_SIZE) && hitM.hitmask[x+1][y] == HIT)
                    || ((y-1)>= 0 && hitM.hitmask[x][y-1] == HIT )||((y+1<=BOARD_SIZE) && hitM.hitmask[x][y+1] == HIT ))) break;

            // if surounding is all miss or all sunk, don't check (because it must be a miss)
            if (((x-1) >= 0 && (hitM.hitmask[x-1][y] == MISS || hitM.hitmask[x-1][y] == SUNK)) && ((x+1<=BOARD_SIZE) && (hitM.hitmask[x+1][y] == MISS || hitM.hitmask[x+1][y] == SUNK)) &&
                ((y-1)>= 0 && (hitM.hitmask[x][y-1] == MISS || hitM.hitmask[x][y-1] == SUNK)) && ((y+1<=BOARD_SIZE) && (hitM.hitmask[x][y+1] == MISS || hitM.hitmask[x][y+1] == SUNK))) 
                break;

            hitmask infoHitmask = hitM;
            infoHitmask.hitmask[x][y] = opt;
            probabilityGrid infoPG;
            double infoGainPart = 0;

            for (int i=0; i<FLEET_SIZE; i++){ // for each ship that could be sunk
                if (opt == SUNK){ // if testing sunk, set the next ship as sunk
                std::memset(infoHitmask.shipSunk, 0, FLEET_SIZE);
                infoHitmask.shipSunk[i]=1;
                }
                runThreads(infoHitmask, infoPG, threadCount);
                double probOptionIsTrue = ((double) infoPG.totalGoodBoards)/((double) pG.totalGoodBoards);
                infoGainPart += (1 - probOptionIsTrue) * probOptionIsTrue;
                
                if (opt != SUNK) break; // if not testing sunk, only do it once
            }

            pG.infoGain[x][y] += infoGainPart;
            }
            infoGainSum += pG.infoGain[x][y];
        } 

        if (pG.infoGain[x][y] >= max && !isHit(hitM, x,y)){ // if the IG is greater than the current max, point at the new cell
            max = pG.infoGain[x][y];
            maxX = x;
            maxY = y;
        }
        }
    }

    xReturn = maxX;
    yReturn = maxY;

    return infoGainSum;
};

/* Solves the board by findng largest unsolved ship and hiting in a diagonal following that pattern
@param xReturn coordinate for shot
@param yReturn cordinate for shot
@param pG probability grid for the backup (if all diagonals are done then do pMax as a backup)
@param hitM hitmask to ensure shot hasn't been taken yet
*/
void coordinate_diagonal(int &xReturn, int &yReturn, probabilityGrid pG, hitmask hitM){ //as with infogain above
    int largestShip = *std::max_element(FLEET , FLEET + FLEET_SIZE);
    
    int tempX,tempY = 0;
    coordinate_pMax(tempX, tempY, pG, hitM);
    //FIXME not sure this should be here?  (ie why is it before diagonal?)
    if (1 == pG.shipProb[tempX][tempY]){ // if a ship is definatly at that position (ie probability == 1) shoot it anyway
        xReturn = tempX;
        yReturn = tempY;    
        return;
    } 
    
    while (largestShip>0) {
        int subBoxCount = BOARD_SIZE/largestShip;

        for (int y = 0; y < subBoxCount; y++){
        for (int x = 0; x < subBoxCount; x++){
            for (int i = 0; i < largestShip; i++){
            xReturn = i + x*largestShip;
            yReturn = i + y*largestShip;
            // if (verbose) std::cout << "(" << x << ", " << y << ") " << i << " " << isHit(hitM,i,i) << "-> " << "(" << xReturn << ", " << yReturn << ") \n";
            if (!isHit(hitM,xReturn,yReturn)) return;
            }
        }
        }
        largestShip--;
    }
};
