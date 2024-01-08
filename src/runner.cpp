#include <iostream> 
#include <fstream>
#include <cstring> 
#include <random>
#include <chrono>
#include <thread>
#include <string>
#include <limits>
#include <algorithm>

#include "json/json.h"
// #include "../_deps/glaze-src/include/glaze"

#include "runner.h"

using namespace std::chrono;
using namespace std;

struct shipPosition{
  unsigned short x=0;
  unsigned short y=0;
  bool dir=0; // 1 is horizontal
};

struct hitmask{
  cellStatus hitmask[BOARD_SIZE][BOARD_SIZE] {UNKNOWN};
};

struct probabilityGrid{
  unsigned long totalGoodBoards = 0;
  unsigned long shipGrid[BOARD_SIZE][BOARD_SIZE] {0}; // how many ships could be in this spot (from each possible good board)? 
  double shipProb[BOARD_SIZE][BOARD_SIZE] {0}; // shipGrid % scaled to total board count (probability of a ship, p)
  double infoGain[BOARD_SIZE][BOARD_SIZE] {0}; // p^2+(1-p)^2

  // Note, the min and max are the UNHIT min/max per a hitmask
  // unsigned short minX,minY = 0;
  // unsigned short maxX,maxY = 0;
};

struct board{
  int board[BOARD_SIZE][BOARD_SIZE] {BOARD_DEFAULT};
  bool isEmpty = true;
  bool isValid = false;
};

struct worker{
  shipPosition start[FLEET_SIZE] = {0,0,0};
  shipPosition end[FLEET_SIZE] = {0,0,0};

  unsigned long goodBoards = 0;
  probabilityGrid sub_probGrid; // an "bool" representive of the board, 
};

struct gamePlayHistory {
	int board[BOARD_SIZE][BOARD_SIZE];
  std::string shotMethod;
	std::vector<std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE>>  probabilityDist;
	std::vector<std::array<int,2>> shotRecord;
};


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
  b.isValid = true;
};

void hitBoard(board b, hitmask &h, int x, int y){
  // Hit and update hitmask
  int cell = b.board[x][y]; // check what is at (x,y) at board
  h.hitmask[x][y] = (cell != BOARD_DEFAULT) ? HIT : MISS; //update the hitmask accordingly (hit/miss)
  // Note: this only accounts for hit/miss and doesn't convert to sunk
};

bool checkCompatible(board b,hitmask h){
  // Check a board and hitmask are compatible
  for (int y = 0; y < BOARD_SIZE; y++){
    for (int x = 0; x < BOARD_SIZE; x++){
      if (h.hitmask[x][y] != UNKNOWN){
        if (h.hitmask[x][y]==MISS && b.board[x][y]!=BOARD_DEFAULT) return false; // if hitmask is a miss, and board isn't 
        else if ((h.hitmask[x][y]==HIT || h.hitmask[x][y]==SUNK) && b.board[x][y]==BOARD_DEFAULT)  return false; // is board empty and hitmask isn't
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

 while(j>=0){
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
          rep='D';
          break;
      }
      std::cout <<rep << ", ";
    }
      std::cout << "]\n";
  }
  std::cout << std::flush;
};

void printProbabilityGrid(probabilityGrid p){
  std::cout << "total:" << p.totalGoodBoards << "\n";

  for (int y = 0; y < BOARD_SIZE; y++){
    std::cout << "[";
    for (int x = 0; x < BOARD_SIZE; x++){
      std::cout << p.shipGrid[x][y] << ", "; 
      // std::cout << ((int)(p.shipProb[x][y]*100))/(double)100.0 << ", "; 
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


// -- ProbabilityGrid functions
void gatherProbabilityFromWorkers(probabilityGrid &p, hitmask h, std::vector<worker> sweatshop){
  // reset all values to 0
  p.totalGoodBoards = 0;
  memset(p.shipGrid, 0, sizeof(p.shipGrid));
  memset(p.shipProb, 0, sizeof(p.shipProb));
  memset(p.infoGain, 0, sizeof(p.infoGain));
  
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
      p.infoGain[x][y] = pow(p.shipProb[x][y],2)+pow(1-p.shipProb[x][y],2); // p^2+(1-p)^2
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
  // if (verbose) std::cout << "\t" << threadID <<") START " << w.goodBoards<<"\n";
  
  board b = initBlankBoard();
  shipPosition pA[FLEET_SIZE]; // position array
  std::copy(w.start, w.start+FLEET_SIZE, std::begin(pA));

  unsigned long allBoards = 0;
  
  do{ // check all the boards from a workers start to end
    drawBoard(b,pA);
    if (b.isValid && checkCompatible(b,hitM)){
      w.goodBoards++;
      flattenBoardToProbabilityGrid(b,w.sub_probGrid);
    } 
    nextShipPosArray(pA, FLEET);
  }while (compareShipArray(pA,w.end)==1); //while the current pos array is behind the end
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

void runThreads(int threadCount, hitmask hitM, probabilityGrid &probGrid){

  // Make and split a vector of workers
  std::vector<worker> sweatshop;
  dividePositions(threadCount,sweatshop);
  // if (verbose) printWorkers(sweatshop);
  
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
  if(verbose) std::cout << probGrid.totalGoodBoards << " boards found in " << runTime.count() <<" seconds\n" ;
};

// -- Game Play (and position deciding)
unsigned int playGame(coordinateChooser playStyle, board b, gamePlayHistory &gph){
  // board b = rndBoard();

  unsigned int turns = 0;
  auto start = high_resolution_clock::now();

  hitmask hitM;
  probabilityGrid probGrid;
  if(playStyle != RND) runThreads(threadCount, hitM, probGrid);
  long maxBoards = probGrid.totalGoodBoards;

  if(verbose){
    printBoard(b);
    printProbabilityGrid(probGrid);
  }



  while (!isHitmaskSolved(hitM)){
    if(verbose) std::cout << "\nNEW TURN ";

    // while the hit is a valid one (ie hasn't been hit yet)
    int x, y = 0;
    do{
      switch(playStyle){
        case RND:
          gph.shotMethod = "RND";
          coordinate_rnd(x,y,hitM);
          break;
        case RND_W_PROB:
          gph.shotMethod = "RND_W_PROB";
          coordinate_rndWProb(x,y,probGrid,hitM);
          break;
        case P_MAX:
          gph.shotMethod = "P_MAX";
          coordinate_pMax(x,y,probGrid,hitM);
          break;
        case P_RND:
          gph.shotMethod = "P_RND";
          coordinate_pRnd(x,y,probGrid,hitM);
          break;
        case infoGain_MAX:
          gph.shotMethod = "infoGain_MAX";
          coordinate_infoGain(x,y,probGrid,hitM);
          break;
        case infoGain_RND:
          gph.shotMethod = "infoGain_RND";
          coordinate_infoGainRnd(x,y,probGrid,hitM);
          break;
        case DIAGONAL:
          gph.shotMethod = "DIAGONAL";
          coordinate_diagonal(x,y,probGrid,hitM);
          break;
        case FLEXI:
          gph.shotMethod = "FLEXI";
          if (0.25 <= probGrid.totalGoodBoards/(double)maxBoards){
            coordinate_diagonal(x,y,probGrid,hitM);
          } else{
            coordinate_pMax(x,y,probGrid,hitM);
          }
          break;
        case USER_INPUT:
        default:
          gph.shotMethod = "USER_INPUT";
          coordinate_userInput(x,y);

      }

      if (isHit(hitM, x, y)){
        if(verbose) std::cout << "You already hit (" << x << ", " << y << ")\n";
        calcProbabilityGrid(probGrid, hitM);
      }
      else if (verbose) std::cout << "You entered (" << x << ", " << y << ")\n";

    } while (isHit(hitM, x, y));
    
    // Take the shot
    hitBoard(b,hitM,x,y);

    // After the shot has been done gather information again 
    if(playStyle != RND) runThreads(threadCount, hitM, probGrid);
    if (verbose){
      std::cout << "\nPROBABILITY GRID:\n";
      printProbabilityGrid(probGrid);

      std::cout << "\nHITMASK:\n";
      printHitmask(hitM);
    }

    // record information gathered
    turns++;
    gph.shotRecord.push_back({x,y});
    std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE> tempShipGrid;
    // unsigned long tempShipGrid[BOARD_SIZE][BOARD_SIZE] {0};
    for (int y = 0; y < BOARD_SIZE; y++){
      for (int x = 0; x < BOARD_SIZE; x++){
        tempShipGrid[x][y] = probGrid.shipProb[x][y];
      }
    }
    gph.probabilityDist.push_back(tempShipGrid);
  }
  
  auto stop = high_resolution_clock::now();
  auto runTime = duration_cast<seconds>(stop - start);

  if (verbose) std::cout << "Game over! you took a total of " << turns << " turns in " << runTime.count() <<" seconds.\n\t You have a " << (double)(fleetPositionCount)/(double)(turns) << " shot sucsess rate\n";
  return turns;
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
      // std::cout<< prob << ", ";
    }
  }
  std::discrete_distribution<int> distribution(flattened.begin(), flattened.end());
  // std::cout<< distribution <<"\n";
  // std::cout<< std::endl;

  std::random_device rd;
  std::mt19937 gen(rd());
  // std::cout << "\t" << distribution(gen) << std::endl;

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

void coordinate_infoGain(int &valX, int &valY, probabilityGrid pG, hitmask hitM){
  //TODO mode 
  unsigned long min = -1;
  unsigned long max = 0;
  int minX,minY=0; 
  int maxX,maxY=0; 

  for (int y = 0; y < BOARD_SIZE; y++){
    for (int x = 0; x < BOARD_SIZE; x++){
      if (pG.infoGain[x][y]< min && !isHit(hitM,x,y)){
        min = pG.infoGain[x][y];
        minX = x;
        minY = y;
      }

      if (pG.infoGain[x][y] > max && !isHit(hitM,x,y)){
        max = pG.infoGain[x][y];
        maxX = x;
        maxY = y;
      }
    }
  }

  // note: if there is a ship here (ie probGrid is 1, and the max value is 1 (and not yet hit)
  // checks probGrid because info gain of zeros is considered high too
  if (max==1 && pG.shipProb[maxX][maxY]==1 && !isHit(hitM,maxX,maxY)) { 
    valX=maxX;
    valY=maxY;
    return;
  }
  valX=minX;
  valY=minY;
};

void coordinate_infoGainRnd(int &valX, int &valY, probabilityGrid pG, hitmask hitM){
  double min = std::numeric_limits<double>::max();
  double max = 0;
  int minX,minY=0; // min isn't yet used but no harm in finding them
  int maxX,maxY=0; 

  std::uniform_real_distribution<> dis(0,1);

  for (int y = 0; y < BOARD_SIZE; y++){
    for (int x = 0; x < BOARD_SIZE; x++){
      double scaledProb = pG.infoGain[x][y]*dis(rng);
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

  valX=minX;
  valY=minY;
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


int main() {

  Json::Value root;
  Json::Value data;
  constexpr bool shouldUseOldWay = false;
  root["action"] = "run";
  data["number"] = 1;
  root["data"] = data;

  if (shouldUseOldWay) {
    Json::FastWriter writer;
    const std::string json_file = writer.write(root);
    std::cout << json_file << std::endl;
  } else {
    Json::StreamWriterBuilder builder;
    const std::string json_file = Json::writeString(builder, root);
    std::cout << json_file << std::endl;
  }
  return EXIT_SUCCESS;

  
//   verbose=false;
//   for (size_t i = 0; i < FLEET_SIZE; i++) fleetPositionCount += FLEET[i];
//   std::cout<<"Board Len: "<< BOARD_SIZE<<"\tFleet size: "<< FLEET_SIZE<<"\tthreadCount: "<<threadCount<<"\tverbose: "<<verbose<<"\tfleetPositionCount: "<<fleetPositionCount<<std::endl;

//   ofstream outfile;

//   int repeats = 1;
  
//   // std::vector<coordinateChooser> allGameStates = {RND, RND_W_PROB, P_MAX, P_RND, infoGain_MAX, infoGain_RND, DIAGONAL, FLEXI};
//   std::vector<coordinateChooser> allGameStates = {RND_W_PROB};

//   // clear the file to empty again
//   string filename = "../out/turnsTaken.out";
//   outfile.open(filename);
//   outfile.close();

//   outfile.open(filename, ios::app); // open and append to file

//   board b = rndBoard();
//   for(auto gameState : allGameStates){
//     for (size_t i = 0; i < repeats; i++){
//       //TODO copy board into new 'b' (i think it works regardless!)
//       gamePlayHistory gph;
//       // gph.board = b.board;

//       int turnCounter = playGame(gameState, b, gph);

//       // Status prints ect
//       std::cout << "Game style " << gph.shotMethod << " took " << turnCounter << " turns\n";
//       for(auto &shot : gph.shotRecord){
//         std::cout << "(" << shot[0] << ", " << shot[1] << ") ";
//       }
//       std::cout << "\n\n";
      
//       std::string buffer{};
//       // glaze::write_json(gph, buffer);
//       // glz::write_json(gph, buffer);

//       std::cout << buffer << std::endl;



//       // FILE IO
//       outfile << turnCounter << "," << std::flush;//endl;
//       if (i%1==0 & verbose) {
//         std::cout<< "GAME FINISHED: " << i << " of " << repeats << " --------- " << std::endl;
//       }
//     }
//     outfile << std::endl;
//     // std::cout<<"Done "<<gameState<<" of " << allGameStates.size() << std::endl;
//   }

//   return 0;
};

