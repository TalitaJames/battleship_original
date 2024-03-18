#include <iostream> 
#include <fstream>
#include <cstring> 
#include <random>
#include <chrono>
#include <thread>
#include <string>
#include <limits>
#include <algorithm>

#include "boatsAndBoards.h"

using namespace std::chrono;

std::map<coordinateChooser, std::string> gameStateNames{ 
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

// -- Board drawing and manipulation
board initBlankBoard(){
  board b;
  wipeBoard(b);
  return b;
};

void wipeBoard(board &b){
  memset(b.board, BOARD_DEFAULT, sizeof(b.board));
  b.isEmpty=true;
  b.isValid=false;
};

void drawBoard(board &b, shipPosition* pos){
  wipeBoard(b);
  b.isEmpty = false;
  
  for (size_t i = 0; i < FLEET_SIZE; i++){ // for each ship
    for (size_t j = 0; j < FLEET[i]; j++){ // for the length of each ship
    // check for a ship already there, if yes, early return
      if (pos[i].dir){  
        if (pos[i].x+j>= BOARD_SIZE ||pos[i].y>= BOARD_SIZE) {b.isValid=false; return;} // out of horizonal bounds
        else if (b.board[pos[i].x+j][pos[i].y] != BOARD_DEFAULT) {b.isValid=false; return;} // intersection!
        
        b.board[pos[i].x+j][pos[i].y] = i; // update board value
      }
      else{
        if (pos[i].x>= BOARD_SIZE ||pos[i].y+j>= BOARD_SIZE) {b.isValid=false; return;} // out of vertical bounds
        else if (b.board[pos[i].x][pos[i].y+j] != BOARD_DEFAULT) {b.isValid=false; return;}  // intersection!
      
        b.board[pos[i].x][pos[i].y+j] = i; // update board value
      }
    }
  }
  b.shipPositionsInt = shipArrayToInt(pos);
  b.isValid = true;
};

void hitBoard(board b, hitmask &h, int x, int y){
  int cell = b.board[x][y]; // check what is at (x,y) at board
  h.hitmask[x][y] = (cell != BOARD_DEFAULT) ? HIT : MISS; //update the hitmask accordingly (hit/miss)


  // Check if the ship is sunk
  if (h.hitmask[x][y] == HIT){ // if it was a hit
    bool isSunk = true;

    shipPosition shipPos[FLEET_SIZE];
    intToShipArray(b.shipPositionsInt, shipPos);

    int checkX,checkY=0;
    for (size_t i = 0; i < FLEET[cell]; i++) {
      if (shipPos[cell].dir) {
        checkX = shipPos[cell].x + i;
        checkY = shipPos[cell].y;
      }
      else{
        checkX = shipPos[cell].x;
        checkY = shipPos[cell].y + i;
      }

      if (!(h.hitmask[checkX][checkY] == cellStatus::HIT || h.hitmask[checkX][checkY] == cellStatus::SUNK)){
        isSunk = false;
        break;
      }
    }
    
    // update the hitmask if sunk
    if (isSunk) {
      for (size_t i = 0; i < FLEET[cell]; i++) {
        if (shipPos[cell].dir) {
          checkX = shipPos[cell].x + i;
          checkY = shipPos[cell].y;
        }
        else{
          checkX = shipPos[cell].x;
          checkY = shipPos[cell].y + i;
        }

        h.hitmask[checkX][checkY] = cellStatus::SUNK;
        h.shipSunk[cell] = true;

      }
    }
  }
};

bool checkCompatible(board b,hitmask h){
  // Check a board and hitmask are compatible
  for (int y = 0; y < BOARD_SIZE; y++){
    for (int x = 0; x < BOARD_SIZE; x++){

      if (h.hitmask[x][y] != UNKNOWN){
        if (h.hitmask[x][y]==MISS && b.board[x][y]!=BOARD_DEFAULT) return false; // if hitmask is a miss, and board isn't 
        else if (h.hitmask[x][y]==HIT && b.board[x][y]==BOARD_DEFAULT)  return false; // is board empty and hitmask isn't
        else if ((h.hitmask[x][y]==SUNK && (!h.shipSunk[b.board[x][y]]))) return false; // if the ship is sunk, and the boat it claims to be isn't sunk
      }
    }
  }
  return true;
};


// -- Random functions
shipPosition rndShipPos(ship len){
  std::uniform_int_distribution<std::mt19937::result_type> udist(0,BOARD_SIZE-len);
  
  shipPosition pos;
  pos.x = udist(rng);
  pos.y = udist(rng);
  pos.dir = rand() % 2;

  return pos;
};

board rndBoard(){
  board b = initBlankBoard();
  shipPosition bPos[FLEET_SIZE];
  while (!b.isValid){
    for (size_t i = 0; i < FLEET_SIZE; i++) bPos[i]=rndShipPos(FLEET[i]);
    drawBoard(b, bPos);
  }
  return b;
};


// -- Ship Position Manipulation
int compareShipPositions(shipPosition pA, shipPosition pB){
  // -1 if A>B
  //  0 if A=B
  //  1 if A<B

  if (pA.dir != pB.dir ) return pB.dir - pA.dir;
  else if (pA.x != pB.x ) return (pB.x - pA.x)/abs(pB.x - pA.x);
  else if (pA.y != pB.y ) return (pB.y - pA.y)/abs(pB.y - pA.y);
  return 0;
};

int compareShipArray(shipPosition *pA, shipPosition *pB){
  // -1 if A>B
  //  0 if A=B
  //  1 if A<B

  int compare=0;
  size_t i = 0;
  while (compare==0 && i<FLEET_SIZE){
    compare = compareShipPositions(pA[i],pB[i]);
    i++;
  }
  return compare;
};


// -- Ship Position <-> numbers
unsigned long shipPosToInt(shipPosition p){
  return p.dir*pow(BOARD_SIZE,2)+p.x*(BOARD_SIZE)+p.y;
};

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

void intToShipPos(unsigned long input,shipPosition &p){
  p.dir = 0, p.x = 0, p.y= 0;

  p.dir=floor(input/pow(BOARD_SIZE,2));
  if (p.dir) input-=pow(BOARD_SIZE,2);
  p.x = input/BOARD_SIZE;
  p.y = input%BOARD_SIZE;

  if (input>=pow(BOARD_SIZE,2)) p.x= BOARD_SIZE-1, p.y = BOARD_SIZE-1;
};

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

board intToBoard(unsigned long input){
  board b = initBlankBoard();
  shipPosition pos[FLEET_SIZE];

  intToShipArray(input, pos);
  drawBoard(b, pos);

  return b;
};

// -- Itterate positions
void nextShipPosition(shipPosition &p){
  nextShipPosition(p,1);
};

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

void nextShipPosArray(shipPosition* p, const ship *s){
  for (int i = FLEET_SIZE-1; i >= 0; i--){
    nextShipPosition(p[i],s[i]);
    if (!isStartPos(p[i])){
      return;
    }
  }
};


// -- Checking & setting array values
bool isStartPos(shipPosition p){
  return p.x == 0 && p.y == 0 && p.dir == 0;
};

bool isStartArray(shipPosition *p){
  for (size_t i=0; i<FLEET_SIZE; i++) if (!isStartPos(p[i])) return false;
  return true;
};

void setEndArray(shipPosition *p){
  for (size_t i = 0; i < FLEET_SIZE; i++){
    p[i].x=BOARD_SIZE-FLEET[i];
    p[i].y= BOARD_SIZE-1;
    p[i].dir=1; // true (->) is the last value
  }
}

void setStartArray(shipPosition *p){
  for (size_t i = 0; i < FLEET_SIZE; i++){
    p[i].x=0;
    p[i].y=0;   
    p[i].dir=0; 
  }
}

bool isHitmaskSolved(hitmask h){
  int fleetPositionCount=0;
  for (size_t i = 0; i < FLEET_SIZE; i++) fleetPositionCount += FLEET[i];

  int numShipPos=fleetPositionCount; // get the total expected hits and shots 
  
  for (int y = 0; y < BOARD_SIZE; y++){
    for (int x = 0; x < BOARD_SIZE; x++){
      if (h.hitmask[x][y] == HIT || h.hitmask[x][y] == SUNK) numShipPos--;
      if(numShipPos<=0) return true;
    }
  }
  return false;
};

bool isHit(hitmask h, int x, int y){
  return h.hitmask[x][y] != UNKNOWN;
};


// -- Output functions
void printBoard(board b){
  std::cout << "--- empty:"<<b.isEmpty<<" valid: "<<b.isValid<<" ---\n";
  for (int y = 0; y < BOARD_SIZE; y++){
    std::cout << "[";
    for (int x = 0; x < BOARD_SIZE; x++){
      if (b.board[x][y] == BOARD_DEFAULT) std::cout << " , ";
      else std::cout << b.board[x][y] << ", ";
    }
    std::cout << "]\n";
  }
  std::cout << std::flush;
};

void printHitmask(hitmask h){
  std::cout << "--- hitmask ---\nBoats: ";
  for (size_t i = 0; i < FLEET_SIZE; i++) std::cout << h.shipSunk[i] << ", ";
  std::cout << "\n";

  for (int y = 0; y < BOARD_SIZE; y++){
    std::cout << "[";
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
      std::cout <<rep << ", ";
    }
      std::cout << "]\n";
  }
  std::cout << std::flush;
};

void printProbabilityGrid(probabilityGrid p){
  std::cout << "--- total:" << p.totalGoodBoards << " ---\n";

  for (int y = 0; y < BOARD_SIZE; y++){
    std::cout << "[";
    for (int x = 0; x < BOARD_SIZE; x++){
      std::cout << p.infoGain[x][y] << ", "; 
    }
    std::cout << "]\n";
  }
  std::cout << std::flush;
};

void printWorkers(std::vector<worker> wrks){
  std::cout<<"workers " << wrks.size()<<'\n';
  for (auto &w :wrks){
    std::cout<<"Worker: checking "<<shipArrayToInt(w.end)-shipArrayToInt(w.start)<<" boards\n\t";
    for (size_t j = 0; j < FLEET_SIZE; j++) std::cout << "("<< w.start[j].x << ", " << w.start[j].y << ", " << w.start[j].dir << ")\t";
    std::cout<<"\n\t";
    for (size_t j = 0; j < FLEET_SIZE; j++) std::cout << "("<< w.end[j].x << ", " << w.end[j].y << ", " << w.end[j].dir << ")\t";
    std::cout<<'\n';
  }
  std::cout << std::flush;
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

std::ostream& operator<<(std::ostream& os, worker& worker){
  os << shipArrayToInt(worker.start) << "," << shipArrayToInt(worker.end);
  return os;
};

std::istream& operator>>(std::istream& is, worker& worker){ //TODO fixme
  unsigned long numStart, numEnd;
  is >> numStart;
  is >> numEnd;
  intToShipArray(numStart, worker.start);
  intToShipArray(numEnd, worker.end);

  // std::string start, end;
  // getline(is,start,',');
  // getline(is,  end,',');

  // intToShipArray((unsigned long)(start), worker.start);
  // in D.feet >> D.inches;
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
void gatherProbabilityFromWorkers(probabilityGrid &p, hitmask h, std::vector<worker> sweatshop){
  // reset all values to 0
  p.totalGoodBoards = 0;
  memset(p.shipGrid, 0, sizeof(p.shipGrid));
  memset(p.shipProb, 0, sizeof(p.shipProb));
  memset(p.pChange, 0, sizeof(p.pChange));
  
  // sum the worker probability data
  for (auto &w : sweatshop){ 
    p.totalGoodBoards += w.goodBoards;
    for (int y = 0; y < BOARD_SIZE; y++){
      for (int x = 0; x < BOARD_SIZE; x++){
        p.shipGrid[x][y]+=w.sub_probGrid.shipGrid[x][y]; 
      }
    }
  }
  calcProbabilityGrid(p, h);
};

void calcProbabilityGrid(probabilityGrid &p, hitmask hitM){
  for (int y = 0; y < BOARD_SIZE; y++){
    for (int x = 0; x < BOARD_SIZE; x++){
      p.shipProb[x][y] = static_cast<double>(p.shipGrid[x][y])/static_cast<double>(p.totalGoodBoards);
      p.pChange[x][y] = pow(p.shipProb[x][y],2)+pow(1-p.shipProb[x][y],2); // p^2+(1-p)^2
    }
  }
};

void flattenBoardToProbabilityGrid(board b,probabilityGrid &pG){
  if (!b.isValid) return;

  for (int y = 0; y < BOARD_SIZE; y++){
    for (int x = 0; x < BOARD_SIZE; x++){
      if (b.board[x][y] != BOARD_DEFAULT) pG.shipGrid[x][y]++;
    }
  }
};


// -- Thread and bulk bits
void checkBoards(worker &w, hitmask hitM, char threadID){
  board b = initBlankBoard();
  shipPosition pA[FLEET_SIZE]; // position array
  std::copy(w.start, w.start+FLEET_SIZE, std::begin(pA));
  
  /* TODO speed updates: this is the changy code
  bool previousState = false;
  shipPosition previousStateShipPos[FLEET_SIZE];
  std::copy(previousStateShipPos, previousStateShipPos+FLEET_SIZE, std::begin(pA));
  intToShipArray(b.shipPositionsInt, previousStateShipPos);
  std::ofstream outfileWorker;
  outfileWorker.open("../out/workerSerialisation/test.txt");
  */

  do{ // check all the boards from a workers start to end
    drawBoard(b,pA);
    if (b.isValid && checkCompatible(b,hitM)){
      w.goodBoards++;
      flattenBoardToProbabilityGrid(b,w.sub_probGrid);
    } 

    /* TODO speed updates: this is the changy code
    // If theres a change in validity
    if(previousState != b.isValid){
      if(previousState){ //if the previous state was valid, then save it in a new worker
        worker newSubWorker;
        std::copy(previousStateShipPos, previousStateShipPos+FLEET_SIZE, std::begin(newSubWorker.start));
        intToShipArray(b.shipPositionsInt, previousStateShipPos); //update the previous ship pos to current
        std::copy(previousStateShipPos, previousStateShipPos+FLEET_SIZE, std::begin(newSubWorker.end));

        // std::unique_lock<std::mutex> lck(mutex_workerSerializer);
        outfileWorker << newSubWorker << std::endl; //TODO (append to variable later) //TODO will this need mutexing?
      }
      else {
        intToShipArray(b.shipPositionsInt, previousStateShipPos); //update the previous ship pos to current
      }
      previousState = b.isValid; //set the previous state to the current state
    }
    */

    nextShipPosArray(pA, FLEET);
  } while (compareShipArray(pA,w.end)==1); //while the current pos array is behind the end

  // outfileWorker.close(); //close the file //TODO speed updates
};

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

  // Make and split a vector of workers
  std::vector<worker> sweatshop;
  dividePositions(threadCount,sweatshop);
  auto start = high_resolution_clock::now();
  
  // Start all the threads
  std::vector<std::thread> sweatshopThreads;
  char threadID = 'A';
  for (auto &w : sweatshop){
    std::thread thr(checkBoards, std::ref(w), hitM, threadID++);
    sweatshopThreads.push_back(std::move(thr));
  }
  
  // Wait for all the threads to be finished
  for (std::thread & th : sweatshopThreads){
    if (th.joinable())
      th.join();
  }

  auto stop = high_resolution_clock::now();
  auto runTime = duration_cast<seconds>(stop - start);

  // Sum it up and get time
  
  gatherProbabilityFromWorkers(probGrid, hitM, sweatshop);
  // if(verbose) std::cout << probGrid.totalGoodBoards << " boards found in " << runTime.count() <<" seconds\n" ;
};


// -- Game Play (and position deciding)
unsigned int playGame(coordinateChooser playStyle, board b){
  Json::Value rubishJSON;
  return playGame(playStyle, b, rubishJSON, std::pow(BOARD_SIZE,2)+1);
}

unsigned int playGame(coordinateChooser playStyle, board b, Json::Value &gamePlayHistory){
  return playGame(playStyle, b, gamePlayHistory, std::pow(BOARD_SIZE,2)+1);
}

unsigned int playGame(coordinateChooser playStyle, board b, Json::Value &gamePlayHistory, int playStyleTurnCount){
  if(verbose) printBoard(b);
  
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


  if(playStyle != RND) runThreads(hitM, probGrid, threadCount);
  probabilityGridJson.append(jsonArrayAdder(probGrid.shipGrid));
  infoGainGridJson.append(jsonArrayAdder(probGrid.infoGain));


  while (!isHitmaskSolved(hitM)){ //TODO turn this into a method? playTurn?
    std::cout << "TURN " <<turns <<"\t";

    // while the hit is valid (ie not yet hit)
    int x, y = 0;
    do{ // decide where to shoot
      if (playStyleTurnCount<=0){
        playStyle = P_MAX;
      }
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
          if (totalIG < 0.0001){
            if (verbose) std::cout <<"pMax now!\n";
            coordinate_pMax(x,y,probGrid,hitM);
          }
          break;
        case USER_INPUT:
        default:
          coordinate_userInput(x,y);

      }

      if (isHit(hitM, x, y)){
        if(verbose) std::cout << "You already hit (" << x << ", " << y << ")\n";
        calcProbabilityGrid(probGrid, hitM);
      }
      std::cout << "You entered (" << x << ", " << y << ") using " << gameStateNames[playStyle] << std::endl;
      playStyleTurnCount--;
      std::cout << std::flush;
    } while (isHit(hitM, x, y));
    
    // Take the shot
    hitBoard(b,hitM,x,y);

    // After the shot has been done gather information again 
    if(playStyle != RND) runThreads(hitM, probGrid, threadCount);
    if (verbose){
      std::cout << "\nPROBABILITY GRID:\n";
      printProbabilityGrid(probGrid);
      std::cout << "\nHITMASK:\n";
      printHitmask(hitM);
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
  }
  
  gamePlayHistory["shotRecord"] = shotRecordJson;
  gamePlayHistory["probabilityGrid"] = probabilityGridJson;
  gamePlayHistory["infoGainGrid"] = infoGainGridJson;
  gamePlayHistory["turnsTaken"] = turns;
  gamePlayHistory["playStyles"] = gameStateNames[playStyle];

  auto stop = high_resolution_clock::now();
  auto runTime = duration_cast<seconds>(stop - start);

  int fleetPositionCount = 0;
  for (size_t i = 0; i < FLEET_SIZE; i++) fleetPositionCount += FLEET[i];

  std::cout << "\n\nGAME OVER! you took a total of " << turns << " turns in " << runTime.count() <<" seconds.\n\t You have a " << (double)(fleetPositionCount)/(double)(turns) << " shot success rate\n";

  return turns;
};

void saveGame(coordinateChooser gameState, board b, int gameStateTurnCount){
  saveGame(gameState, b, gameStateTurnCount,"");
};

void saveGame(coordinateChooser gameState, board b, int gameStateTurnCount, std::string igExtra){
  std::string filename = std::tmpnam(nullptr);

  // filename in the form: boardSize_fleetSize_boardID_gameState_randomChars.json
  filename = "../out/gamePlay/"+std::to_string(BOARD_SIZE)+"_"+std::to_string(FLEET_SIZE)+"_"
                          +std::to_string(b.shipPositionsInt)+"_"+gameStateNames[gameState]+igExtra+"_"
                          +codeVersion+"_"+filename.substr(9, filename.length())+".json";

  Json::Value gamePlayHistory;
  gamePlayHistory["FLEET_SIZE"] = FLEET_SIZE;
  gamePlayHistory["FLEET"] = jsonArrayAdder(FLEET, FLEET_SIZE);
  gamePlayHistory["BOARD_SIZE"] = BOARD_SIZE;
  gamePlayHistory["BOARD_SIZE"] = BOARD_SIZE;
  gamePlayHistory["board"] = jsonArrayAdder(b.board);
  gamePlayHistory["version"] = codeVersion;
  if (gameState == INFOGAIN) gamePlayHistory["infoGainTurns"] = gameStateTurnCount;


  int turnCounter = playGame(gameState, b, gamePlayHistory, gameStateTurnCount);

  // File IO
  std::ofstream outfile;
  outfile.open(filename);
  Json::StreamWriterBuilder builder;
  std::string json_file = Json::writeString(builder, gamePlayHistory);
  outfile << json_file << std::flush;
  outfile.close();

  std::cout<< "\tsaving to " << filename << "\n" << std::endl;

};

void repeatGames(std::vector<coordinateChooser> playStyles, int repeats, bool sameBoard){
  board b = rndBoard();
  repeatGames(playStyles, repeats, false, b);
};

void repeatGames(std::vector<coordinateChooser> playStyles, int repeats, bool sameBoard, board b){
  for(auto gameState : playStyles){
    for (size_t i = 0; i < repeats; i++){
      if (!sameBoard){
        b = rndBoard();
      }
      saveGame(gameState, b, 100);

    }
  }
};

void repeatIGRange(std::vector<board> repeats){
  for (board b: repeats){
    printBoard(b);
    if (b.isValid == false){ //if board isn't valid, stop board
      std::cout << "ERROR! INVALID BOARD -- program terminating" << std::endl;
      abort();
    }

    for (int igCount = 0; igCount <= std::pow(BOARD_SIZE,2); igCount++){
      saveGame(INFOGAIN, b, igCount, "-shots"+std::to_string(igCount));
    }
  }
};

void coordinate_userInput(int &x, int &y){
  std::cout << "X: ";
  std::cin >> x;

  std::cout << "Y: ";
  std::cin >> y;
};

void coordinate_rnd(int &x, int &y, hitmask hitM){
  std::uniform_int_distribution<std::mt19937::result_type> udist(0,BOARD_SIZE-1);
  
  do {
    x = udist(rng);
    y = udist(rng);
  } while (isHit(hitM,x,y)); 
};

void coordinate_rndWProb(int &x, int &y, probabilityGrid pG, hitmask hitM){

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
    x = place1D/BOARD_SIZE;
    y = place1D % BOARD_SIZE;
    // std::cout<< place1D << ", (" << x << ", " << y << ")\n";
  } while (isHit(hitM,x,y)); 


};

void coordinate_pMax(int &maxX, int &maxY, probabilityGrid pG, hitmask hitM){
  unsigned long min = -1;
  unsigned long max = 0;
  int minX,minY=0; // min isn't yet used but no harm in finding them

  for (int y = 0; y < BOARD_SIZE; y++){
    for (int x = 0; x < BOARD_SIZE; x++){
      if (pG.shipGrid[x][y]< min && !isHit(hitM,x,y)){
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
};

void coordinate_pRnd(int &maxX, int &maxY, probabilityGrid pG, hitmask hitM){
  double min = std::numeric_limits<double>::max();
  double max = 0;
  int minX,minY=0; // min isn't yet used but no harm in finding them
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
};

double coordinate_infoGain(int &valX, int &valY, probabilityGrid &pG, hitmask hitM){
  /* For every ship, the 7 options (miss, hit, sink 2, sink 3, sink 3, sink 4, sink 5)
      info gain += (num of boards matching option * probability of option)
  */
  double max = 0;
  int maxX, maxY = 0;
  double infoGainSum = 0;

  std::vector<cellStatus> options = {MISS, HIT, SUNK};
  
  for (int y = 0; y < BOARD_SIZE; y++){
    for (int x = 0; x < BOARD_SIZE; x++){
      pG.infoGain[x][y] = 0;

      if (!isHit(hitM, x,y)){
        for (auto opt : options){
          // if testing sunk, and there aren't any surounding hits, don't bother
          if(opt == SUNK && !(((x-1) >= 0 && hitM.hitmask[x-1][y] == HIT )||((x+1<=BOARD_SIZE) && hitM.hitmask[x+1][y] == HIT)
                  || ((y-1)>= 0 && hitM.hitmask[x][y-1] == HIT )||((y+1<=BOARD_SIZE) && hitM.hitmask[x][y+1] == HIT ))) break;

          // if surounding is all miss or all sunk, don't check //TODO: test this *after* i get the code working
          if(((x-1) >= 0 && (hitM.hitmask[x-1][y] == MISS || hitM.hitmask[x-1][y] == SUNK)) && ((x+1<=BOARD_SIZE) && (hitM.hitmask[x+1][y] == MISS || hitM.hitmask[x+1][y] == SUNK)) &&
              ((y-1)>= 0 && (hitM.hitmask[x][y-1] == MISS || hitM.hitmask[x][y-1] == SUNK)) && ((y+1<=BOARD_SIZE) && (hitM.hitmask[x][y+1] == MISS || hitM.hitmask[x][y+1] == SUNK))) 
            break;

          hitmask infoHitmask = hitM;
          infoHitmask.hitmask[x][y] = opt;
          probabilityGrid infoPG;
          double infoGainPart = 0;

          for(int i=0; i<FLEET_SIZE; i++){
            if (opt == SUNK){ // if its a sunk ship, then get set the next ship as sunk
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

      if (pG.infoGain[x][y] >= max && !isHit(hitM, x,y)){
        max = pG.infoGain[x][y];
        maxX = x;
        maxY = y;
      }
    }
  }

  valX=maxX;
  valY=maxY;

  return infoGainSum;
};

void coordinate_diagonal(int &valX, int &valY, probabilityGrid pG, hitmask hitM){ //as with infogain above
  int largestShip = *std::max_element(FLEET , FLEET + FLEET_SIZE);
  
  int tempX,tempY = 0;
  coordinate_pMax(tempX, tempY, pG, hitM);
  if (pG.shipProb[tempX][tempY] == 1){
    valX = tempX;
    valY = tempY;    
    return; 
  } 
  
  while (largestShip>0) {
    int subBoxCount = BOARD_SIZE/largestShip;

    for (int y = 0; y < subBoxCount; y++){
      for (int x = 0; x < subBoxCount; x++){
        for (int i = 0; i < largestShip; i++){
          valX = i + x*largestShip;
          valY = i + y*largestShip;
          // std::cout << "(" << x << ", " << y << ") " << i << " " << isHit(hitM,i,i) << "-> " << "(" << valX << ", " << valY << ") \n";
          if (!isHit(hitM,valX,valY)) return;
        }
      }
    }
    largestShip--;
  }
};


