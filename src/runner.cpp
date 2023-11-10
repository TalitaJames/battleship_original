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
      if (pos[i].dir){ 
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

void checkBoards(worker &w, char threadID){
  std::cout << "\t" << threadID <<") START " << w.goodBoards<<"\n";
  
  board b = initBlankBoard();
  shipPosition pA[FLEET_SIZE]; // position array
  std::copy(w.start, w.start+FLEET_SIZE, std::begin(pA));

  unsigned long allBoards = 0;
  
  // auto start = high_resolution_clock::now();
  do{
    // for (size_t i = 0; i < FLEET_SIZE; i++) std::cout << "("<< pA[i].x << ", " << pA[i].y << ", " << pA[i].dir << ")\t";
    // std::cout << "\n";
    // printBoard(b);

    if (++allBoards % 50000000 == 0) std::cout << "\t" << threadID << ") " << w.goodBoards << "\n";
    drawBoard(b,pA);
    if (b.isValid) w.goodBoards++;

    nextShipPosArray(pA, FLEET);

  }while (compareShipArray(pA,w.end)==1); //while the current pos array is behind the end

  // auto stop = high_resolution_clock::now();
  // auto runTime = duration_cast<microseconds>(stop - start);

  
  std::cout << "\t" << threadID <<") DONE " << w.goodBoards<<"\n";
  // std::cout << "found "<< w.goodBoards<<" in "<<runTime.count()<<" microseconds\n";

};

void dividePositions(int threadCount,std::vector<worker> &w){
  w.reserve(threadCount);
  
  int radix = std::pow(BOARD_SIZE,2)*2;
  int maxSegValue = radix * FLEET_SIZE;
  int segmentSize = maxSegValue/threadCount;

  std::cout<<"radix: "<< radix << " maxSegValue: " << maxSegValue << " segmentSize: "<< segmentSize <<'\n';
  
  shipPosition sP[FLEET_SIZE]; //ship pos

  shipPosition pA[FLEET_SIZE]; //position A (end)
  shipPosition pB[FLEET_SIZE]; //position B sStart)

  for (size_t i = 0; i < threadCount+1; i++){
    int subSeg = segmentSize*i;

    // This section is creating the ship pos array value:
    if (i<threadCount){
      int k = FLEET_SIZE-1;
      
      
      while (subSeg>radix){ // if bigger than the single max, start making back values max until smaller than max
        sP[k].x=BOARD_SIZE-1;
        sP[k].y=BOARD_SIZE-1;
        sP[k].dir=true;
        k--;
        subSeg-=radix;
      }

      // std::cout<< subSeg <<", ";
      
      // if over half, set direction =1 and get rid of the half
      sP[k].dir = (subSeg >= radix/2);
      if (subSeg >= radix/2) subSeg -= radix/2;
      // std::cout<< subSeg <<'\t';

      // with the remaining values, set x & y
      sP[k].x = subSeg/BOARD_SIZE;
      sP[k].y = subSeg % BOARD_SIZE;
    }


    if (i>0)
      std::copy(pA, pA+FLEET_SIZE, std::begin(pB));

    std::copy(sP, sP+FLEET_SIZE, std::begin(pA));
    if (i==threadCount){
      shipPosition lastPos[FLEET_SIZE];
      setEndArray(lastPos);
      std::copy(lastPos, lastPos+FLEET_SIZE, std::begin(pA));
    }

    // std::cout<<"sP array "<<i<<"\n";
    // for (size_t j = 0; j < FLEET_SIZE; j++) std::cout << "sP("<< sP[j].x << ", " << sP[j].y << ", " << sP[j].dir << ")\t";
    // std::cout<<'\n';
    // for (size_t j = 0; j < FLEET_SIZE; j++) std::cout << "pA("<< pA[j].x << ", " << pA[j].y << ", " << pA[j].dir << ")\t";
    // std::cout<<'\n';
    // for (size_t j = 0; j < FLEET_SIZE; j++) std::cout << "pB("<< pB[j].x << ", " << pB[j].y << ", " << pB[j].dir << ")\t";
    // std::cout<<'\n';

    if (i>0){
      worker foo;
      std::copy(pB, pB+FLEET_SIZE, std::begin(foo.start));
      std::copy(pA, pA+FLEET_SIZE, std::begin(foo.end));
      w.push_back(foo);
    }

  }
}


int main() {
  std::cout<<"Board Len: "<< BOARD_SIZE<<"\tFleet size: "<< FLEET_SIZE<<"\tthreadCount: "<<threadCount<<"\n";
  
  // Start and pBlit the threads
  std::vector<worker> sweatshop;

  // Make and split a vector of workers
  dividePositions(threadCount,sweatshop);

  // Print worker start/ends
  std::cout<<"workers " << sweatshop.size()<<'\n';
  // for (auto &w :sweatshop){
  //   std::cout<<'\n';
  //   for (size_t j = 0; j < FLEET_SIZE; j++) std::cout << "("<< w.start[j].x << ", " << w.start[j].y << ", " << w.start[j].dir << ")\t";
  //   std::cout<<'\n';
  //   for (size_t j = 0; j < FLEET_SIZE; j++) std::cout << "("<< w.end[j].x << ", " << w.end[j].y << ", " << w.end[j].dir << ")\t";
  //   std::cout<<'\n';
  // }
  

  auto start = high_resolution_clock::now();

  // Start all the threads
  std::vector<std::thread> sweatshopThreads;
  char threadID = 'a';
  for (auto &w : sweatshop){
    std::thread thr(checkBoards, std::ref(w), threadID++);
    sweatshopThreads.push_back(std::move(thr));
  }
  std::cout<<"made all " << sweatshop.size()<<" threads\n";
  
  // Wait for all the threads to be finished
  for (std::thread & th : sweatshopThreads){
    if (th.joinable())
      std::cout<<"joining a thread \n";
      th.join();
  }

  std::cout<<"done soon?\n";
  auto stop = high_resolution_clock::now();
  auto runTime = duration_cast<seconds>(stop - start);

  // Sum it up and get time
  unsigned long totalGoodBoards=0;

  for (auto &w : sweatshop){
    totalGoodBoards += w.goodBoards;
  }
  std::cout << totalGoodBoards << " in " << runTime.count() <<" seconds\n" ;

  
  return 0;
};

