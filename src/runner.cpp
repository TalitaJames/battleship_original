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
  int board[BOARD_SIZE][BOARD_SIZE] {-1};
  bool isEmpty = true;
  bool isValid = false;
};

struct worker{
  shipPosition start[FLEET_SIZE] = {0,0,0};
  shipPosition end[FLEET_SIZE] = {0,0,0};

  unsigned long goodBoards = 0;
};


void initSystem(int argc, char *argv[], bool verbose){

  // for (size_t i = 0; i < argc; i++){
  //   if (verbose) std::cout << "Arg "<< i <<": "<< argv[i] << '\n';
  // }

  // BOARD_SIZE = strtol(argv[1], NULL, 10); //FIXME: null replace?
  // if (verbose) std::cout << "boardSize: "<< BOARD_SIZE << '\n';
  
  if (verbose){
    std::cout<<"Args: "<< argc <<'\n';
    std::cout<<"Fleet size: "<< FLEET_SIZE<<"\tBoard Len: "<< BOARD_SIZE<<'\n';
  }
};



void wipeBoard(board &b){
  memset(b.board, BOARD_DEFAULT, sizeof(b.board));
  b.isEmpty=true;
  b.isValid=false;
};

board initBlankBoard(){
  board b;
  wipeBoard(b);
  return b;
};

// play a board


void drawBoard(board &b, shipPosition* pos){
  wipeBoard(b);
  b.isEmpty = false;

  
  for (size_t i = 0; i < FLEET_SIZE; i++){ // for each ship
    // std::cout << "ship: " << FLEET[i] << " at (" << pos[i].x << ", " << pos[i].y << ", " << pos[i].dir << ")\n";

    for (size_t j = 0; j < FLEET[i]; j++){ // for the length of each ship
      
      // check for a ship already there, if yes, throw error
      if(pos[i].dir){ 
        // std::cout <<"\t("<<pos[i].x+j<<","<<pos[i].y<<") @ "<< b.board[pos[i].x+j][pos[i].y] <<"\n";
        
        if (pos[i].x+j>= BOARD_SIZE ||pos[i].y>= BOARD_SIZE) {b.isValid=false; return;} // out of horizonal bounds
        else if (b.board[pos[i].x+j][pos[i].y] != BOARD_DEFAULT) {b.isValid=false; return;} // intersection!
        
        b.board[pos[i].x+j][pos[i].y] = i;
      } else{
        // std::cout <<"\t("<<pos[i].x+j<<","<<pos[i].y<<") @ "<< b.board[pos[i].x+j][pos[i].y] <<"\n";
      
        if (pos[i].x>= BOARD_SIZE ||pos[i].y+j>= BOARD_SIZE) {b.isValid=false; return;} // out of vertical bounds
        else if (b.board[pos[i].x][pos[i].y+j] != BOARD_DEFAULT) {b.isValid=false; return;}  // intersection!
      
        b.board[pos[i].x][pos[i].y+j] = i;
      }
    }
  }

  b.isValid = true;
};



// Make a board
board makeBoard(shipPosition* pos){ 
  board b = initBlankBoard();

  for (size_t i = 0; i < FLEET_SIZE; i++){ // for each ship
    // std::cout << "ship: " << FLEET[i] << " at (" << pos[i].x << ", " << pos[i].y << ", " << pos[i].dir << ")\n";

    for (size_t j = 0; j < FLEET[i]; j++){ // for the length of each ship
      
      // check for a ship already there, if yes, throw error
      if(pos[i].dir){ 
        // std::cout <<"\t("<<pos[i].x+j<<","<<pos[i].y<<") @ "<< b.board[pos[i].x+j][pos[i].y] <<"\n";
        
        if (pos[i].x+j>= BOARD_SIZE ||pos[i].y>= BOARD_SIZE) {b.isValid=false; return b;} // out of horizonal bounds
        else if (b.board[pos[i].x+j][pos[i].y] != BOARD_DEFAULT) {b.isValid=false; return b;} // intersection!
        
        b.board[pos[i].x+j][pos[i].y] = i;
      } else{
        // std::cout <<"\t("<<pos[i].x+j<<","<<pos[i].y<<") @ "<< b.board[pos[i].x+j][pos[i].y] <<"\n";
      
        if (pos[i].x>= BOARD_SIZE ||pos[i].y+j>= BOARD_SIZE) {b.isValid=false; return b;} // out of vertical bounds
        else if (b.board[pos[i].x][pos[i].y+j] != BOARD_DEFAULT) {b.isValid=false; return b;}  // intersection!
      
        b.board[pos[i].x][pos[i].y+j] = i;
      }
    }
  }

  b.isValid = true;
  b.isEmpty = false;
  return b;
};

// Hit and update hitmask
void hitBoard(board b, hitmask &h, int x, int y){
  int cell = b.board[x][y]; // check what is at (x,y) at board
  h.hitmask[x][y] = cell != 0 ? HIT : MISS; //update the hitmask accordingly (hit/miss)
  
  // step 3: update if sunk
}


// Check a board and hitmask are compatible
bool checkCompatible(board b,hitmask h){
  for (int y = 0; y < BOARD_SIZE; y++){
    for (int x = 0; x < BOARD_SIZE; x++){
      if (h.hitmask[x][y] != UNKNOWN){
        if (h.hitmask[x][y]==MISS && b.board[x][y]!=0) return false;
        else if ((h.hitmask[x][y]==HIT || h.hitmask[x][y]==SUNK) && b.board[x][y]==0) return false;
      }
    }
  }
  return true;
};


shipPosition randShipPos(){
  std::uniform_int_distribution<std::mt19937::result_type> udist(0,BOARD_SIZE-1);
  
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

void nextShipPosition(shipPosition &p, const ship s){ 
  p.y++;
  
  if (p.dir && p.y >= BOARD_SIZE){
    p.y=0;
    p.x++;
  } else if(!p.dir && p.y > BOARD_SIZE-s){
    p.y=0;
    p.x++;
  }

  if (p.dir && p.x > BOARD_SIZE-s){
    p.x=0;
    p.y=0;
    p.dir = !p.dir;
  } else if(!p.dir && p.x >= BOARD_SIZE){
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

// -- Output functions

// print the board as a grid
void printBoard(board b){
  std::cout << "--- empty:"<<b.isEmpty<<" valid: "<<b.isValid<<" ---\n";
  for (int y = 0; y < BOARD_SIZE; y++){
    std::cout << "[";
    for (int x = 0; x < BOARD_SIZE; x++){
      if (b.board[x][y] == -1) std::cout << " , ";
      else std::cout << b.board[x][y] << ", ";
    }
    std::cout << "]\n";
  }
  std::cout << "---\n";
};

// print a representation of the hitmask
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


bool isStartPos(shipPosition p){
  return p.x == 0 && p.y == 0 && p.dir == 0;
};

bool isStartArray(shipPosition *p){
  for(size_t i=0; i<FLEET_SIZE; i++) if(!isStartPos(p[i])) return false;
  return true;
};

void setEndArray(shipPosition *p){
  for (size_t i = 0; i < FLEET_SIZE; i++){
    p[i].x=BOARD_SIZE-FLEET[i];
    p[i].y= BOARD_SIZE-1;
    p[i].dir=1; // true (->) is the last value
  }
}

void checkBoards(worker &w){
  
  board b = initBlankBoard();
  shipPosition pA[FLEET_SIZE]; // position array
  std::copy(w.start, w.start+FLEET_SIZE, std::begin(pA));

  unsigned long allBoards = 0;
  
  auto start = high_resolution_clock::now();
  do{
    // for (size_t i = 0; i < FLEET_SIZE; i++) std::cout << "("<< pA[i].x << ", " << pA[i].y << ", " << pA[i].dir << ")\t";
    // std::cout << "\n";
    // printBoard(b);

    if (++allBoards % 100000000 == 0) std::cout << w.goodBoards << "\n";
    drawBoard(b,pA);
    if(b.isValid) w.goodBoards++;

    nextShipPosArray(pA, FLEET);

  }while(compareShipArray(pA,w.end)==1); //while the current pos array is behind the end

  auto stop = high_resolution_clock::now();
  auto runTime = duration_cast<microseconds>(stop - start);

  
  std::cout << "found "<< w.goodBoards<<" in "<<runTime.count()<<" microseconds\n";

};

vector<shipPosition*> dividePositions(int threads){
  vector<shipPosition*> pos;
  
  int radix = std::pow(BOARD_SIZE,2)*2;
  int maxSegValue = radix * FLEET_SIZE;
  int segmentSize = maxSegValue/threads;

  std::cout<<"radix: "<< radix << " maxSegValue: " << maxSegValue << " segmentSize: "<< segmentSize <<'\n';


  for (size_t i = 0; i < threads; i++){
    int subSeg = segmentSize*i;
    shipPosition sP[FLEET_SIZE];
    int k = FLEET_SIZE-1;
    
    while(subSeg>radix){
      sP[k].x=BOARD_SIZE-1;
      sP[k].y=BOARD_SIZE-1;
      sP[k].dir=true;
      k--;
      subSeg-=radix;
    }

    std::cout<<" subSize: "<< subSeg <<'\t';
    

    sP[k].dir = (subSeg >= radix/2);
    if (subSeg >= radix/2) subSeg -= radix/2;
    std::cout<<" subSize: "<< subSeg <<'\t';

    sP[k].x = subSeg/BOARD_SIZE;
    sP[k].y = subSeg % BOARD_SIZE;

    pos.push_back(sP);

    for (size_t j = 0; j < FLEET_SIZE; j++) std::cout << "("<< sP[j].x << ", " << sP[j].y << ", " << sP[j].dir << ")\t";
    std::cout<<'\n';
  }

  shipPosition lastPos[FLEET_SIZE];
  setEndArray(lastPos);
  pos.push_back(lastPos);
  
  std::cout<<'\n';
  for(auto &v :pos){
    for (size_t j = 0; j < FLEET_SIZE; j++) std::cout << "("<< v->x << ", " << v->y << ", " << v->dir << ")\t";
    std::cout<<'\n';
  }


  return pos;
}

int main() {
  std::cout<<"Fleet size: "<< FLEET_SIZE<<"\tBoard Len: "<< BOARD_SIZE<<'\n';
  
  // Start and split the threads
  int threads = 3;
  vector<shipPosition*> segmentBorders = dividePositions(threads);

  std::cout<<'\n';
  for(auto &v :segmentBorders){
    for (size_t j = 0; j < FLEET_SIZE; j++) std::cout << "("<< v->x << ", " << v->y << ", " << v->dir << ")\t";
    std::cout<<'\n';
  }


  // Make a vector of workers
  shipPosition mid[FLEET_SIZE];
  for (size_t i = 0; i < FLEET_SIZE; i++) { mid[i].x=0; mid[i].y=0; mid[i].dir=1;}

  worker w0, w1;

  std::copy(mid, mid+FLEET_SIZE, std::begin(w0.end));
  std::copy(mid, mid+FLEET_SIZE, std::begin(w1.start));
  setEndArray(w1.end);

  std::vector<worker> sweatshop;
  sweatshop.push_back(w0);
  sweatshop.push_back(w1);
  
  /*
  // print the start and end array for each worker
  for (auto &w : sweatshop){
    std::cout << "Start: ";
    for (size_t j = 0; j < FLEET_SIZE; j++) std::cout << "("<< w.start[j].x << ", " << w.start[j].y << ", " << w.start[j].dir << ")\t";
    std::cout << "End: ";
    for (size_t j = 0; j < FLEET_SIZE; j++) std::cout << "("<< w.end[j].x << ", " << w.end[j].y << ", " << w.end[j].dir << ")\t";
    std::cout << "brd: " << w.goodBoards << "\n";
  }


  // Start all the threads
  std::vector<std::thread> sweatshopThreads;
  for (auto &w : sweatshop){
    std::thread thr(checkBoards, std::ref(w));
    sweatshopThreads.push_back(std::move(thr));
  }
  
  // Wait for all the threads to be finished
  for (std::thread & th : sweatshopThreads){
    if (th.joinable())
      th.join();
  }


  // Sum it up and get time
  unsigned long totalGoodBoards=0;

  for (auto &w : sweatshop){
    totalGoodBoards += w.goodBoards;
  }
  std::cout<<totalGoodBoards<<"\n";


  */
  
  
  return 0;
};

