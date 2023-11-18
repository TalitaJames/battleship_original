#include <iostream> 
#include <cstring> 
#include <random>
#include <chrono>
#include <thread>
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

struct board{
  int board[BOARD_SIZE][BOARD_SIZE] {BOARD_DEFAULT};
  bool isEmpty = true;
  bool isValid = false;
};

struct worker{
  shipPosition start[FLEET_SIZE] = {0,0,0};
  shipPosition end[FLEET_SIZE] = {0,0,0};

  unsigned long goodBoards = 0;
  unsigned long heatmap[BOARD_SIZE][BOARD_SIZE] {0}; // an "bool" representive of the board, 
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
  h.hitmask[x][y] = (cell != 0) ? HIT : MISS; //update the hitmask accordingly (hit/miss)
  // Note: this only accounts for hit/miss and doesn't convert to sunk
}

bool checkCompatible(board b,hitmask h){
  // Check a board and hitmask are compatible
  for (int y = 0; y < BOARD_SIZE; y++){
    for (int x = 0; x < BOARD_SIZE; x++){
      if (h.hitmask[x][y] != UNKNOWN){
        if (h.hitmask[x][y]==MISS && b.board[x][y]!=0) return false; // if hitmask is a miss, and board isn't 
        else if ((h.hitmask[x][y]==HIT || h.hitmask[x][y]==SUNK) && b.board[x][y]==0) return false; // is board empty and hitmask isn't
      }
    }
  }
  return true;
};

void flattenBoardToHeatmap(board b,worker &w){
  if (!b.isValid) return;

  for (int y = 0; y < BOARD_SIZE; y++){
    for (int x = 0; x < BOARD_SIZE; x++){
      if (b.board[x][y] != BOARD_DEFAULT) w.heatmap[x][y]++;
    }
  }
};


// return a random ship possition (in the bounds of the board)
shipPosition randShipPos(ship len){
  std::uniform_int_distribution<std::mt19937::result_type> udist(0,BOARD_SIZE-len);
  
  shipPosition pos;
  pos.x = udist(rng);
  pos.y = udist(rng);
  pos.dir = rand() % 2;

  return pos;
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
}

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
}


// -- Ship Position <-> numbers
unsigned long shipPosToInt(shipPosition p){
  return p.dir*pow(BOARD_SIZE,2)+p.x*(BOARD_SIZE)+p.y;
}

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
}

void intToShipPos(unsigned long input,shipPosition &p){
  p.dir = 0, p.x = 0, p.y= 0;

  p.dir=floor(input/pow(BOARD_SIZE,2));
  if(p.dir) input-=pow(BOARD_SIZE,2);
  p.x = input/BOARD_SIZE;
  p.y = input%BOARD_SIZE;

  if(input>=pow(BOARD_SIZE,2)) p.x= BOARD_SIZE-1, p.y = BOARD_SIZE-1;
}

void intToShipArray(unsigned long input, shipPosition *p){

  int i=0;
  int j=FLEET_SIZE-1;
  unsigned long radix = std::pow(BOARD_SIZE,2)*2;
  
  if(input>=pow(radix,FLEET_SIZE)){ // if too big, make max value instead
    setEndArray(p);
    return;
  }

  setStartArray(p); //Back to 0s, clears previous values

 while(j>=0){
    if(0>input) break;

    if(input<pow(radix,j)){ // the value isn't big enough for this spot in the array
      i++; j--;
      continue;
    }

    unsigned long baseModInput = floor(input/pow(radix,j)); 

    intToShipPos(baseModInput,p[i]);
    input-=baseModInput*pow(radix,j);
    i++; j--;

  }
}


// -- Itterate positions
void nextShipPosition(shipPosition &p){
  nextShipPosition(p,1);
}

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
  std::cout << "---\n";
};

void printHitmask(hitmask h){
  std::cout << "---\n";
  for (int y = 0; y < BOARD_SIZE; y++){
    std::cout << "[";
    for (int x = 0; x < BOARD_SIZE; x++){
      char rep;
      switch (h.hitmask[x][y]){
        case UNKNOWN: 
          rep = '?';
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
  std::cout << "---\n";
};

void printHeatmap(unsigned long heatmap[BOARD_SIZE][BOARD_SIZE]){
  for (int y = 0; y < BOARD_SIZE; y++){
    std::cout << "[";
    for (int x = 0; x < BOARD_SIZE; x++){
      std::cout << heatmap[x][y] << ", "; 
    }
    std::cout << "]\n";
  }
  std::cout << "---\n";
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
}


// -- Thread and bulk bits
void checkBoards(worker &w, char threadID){
  std::cout << "\t" << threadID <<") START " << w.goodBoards<<"\n";
  
  board b = initBlankBoard();
  shipPosition pA[FLEET_SIZE]; // position array
  std::copy(w.start, w.start+FLEET_SIZE, std::begin(pA));

  unsigned long allBoards = 0;
  
  do{ // check all the boards from a workers start to end
    if (++allBoards % 50000000 == 0){
      float progress = (static_cast<float>(shipArrayToInt(pA)-shipArrayToInt(w.start)) / static_cast<float>(shipArrayToInt(w.end)-shipArrayToInt(w.start))*100);
      std::cout << "\t" << threadID << ") " << (int)progress << "%\n";
    } 
    drawBoard(b,pA);
    if (b.isValid){
      w.goodBoards++;
      flattenBoardToHeatmap(b,w);
    } 
    nextShipPosArray(pA, FLEET);
  }while (compareShipArray(pA,w.end)==1); //while the current pos array is behind the end

  std::cout << "\t" << threadID <<") DONE " << w.goodBoards<<"\n";
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
    
    worker foo;
    std::copy(pS, pS+FLEET_SIZE, std::begin(foo.start));
    std::copy(pE, pE+FLEET_SIZE, std::begin(foo.end));
    w.push_back(foo);
  }
}

void runThreads(bool verbose){

  // Make and split a vector of workers
  std::vector<worker> sweatshop;
  dividePositions(threadCount,sweatshop);
  if(verbose) printWorkers(sweatshop);
  
  auto start = high_resolution_clock::now();
  
  // Start all the threads
  std::vector<std::thread> sweatshopThreads;
  char threadID = 'A';
  for (auto &w : sweatshop){
    std::thread thr(checkBoards, std::ref(w), threadID++);
    sweatshopThreads.push_back(std::move(thr));
  }
  std::cout<<"made all " << sweatshop.size()<<" threads\n";
  
  // Wait for all the threads to be finished
  for (std::thread & th : sweatshopThreads){
    if (th.joinable())
      th.join();
  }

  auto stop = high_resolution_clock::now();
  auto runTime = duration_cast<seconds>(stop - start);

  // Sum it up and get time
  unsigned long totalGoodBoards=0;
  unsigned long heatmap[BOARD_SIZE][BOARD_SIZE] {0}; // an "bool" representive of the board, 

  for (auto &w : sweatshop){
    totalGoodBoards += w.goodBoards;
    for (int y = 0; y < BOARD_SIZE; y++){
      for (int x = 0; x < BOARD_SIZE; x++){
        heatmap[x][y]+=w.heatmap[x][y]; 
      }
    }
  }

  std::cout << "---\n";
  std::cout <<"Total: " << totalGoodBoards << " in " << runTime.count() <<" seconds\n" ;
  printHeatmap(heatmap);
}

int main() {
  std::cout<<"Board Len: "<< BOARD_SIZE<<"\tFleet size: "<< FLEET_SIZE<<"\tthreadCount: "<<threadCount<<"\n";
  
  runThreads(true);

  return 0;
};