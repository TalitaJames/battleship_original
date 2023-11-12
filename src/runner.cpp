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

int shipPosToInt(shipPosition p){
  return p.dir*pow(BOARD_SIZE,2)+p.x*(BOARD_SIZE)+p.y;
}

int shipArrayToInt(shipPosition *p){
  int i=0;
  int adr=FLEET_SIZE-1;

  int result=0;
  while (i<FLEET_SIZE){
    result+=shipPosToInt(p[i])*pow(pow(BOARD_SIZE,2)*2,adr);
    i++;
    adr--;
  }
  return result;
}

void intToShipPos(int input,shipPosition &p){
  p.dir = 0, p.x = 0, p.y= 0;

  p.dir=floor(input/pow(BOARD_SIZE,2));
  if(p.dir) input-=pow(BOARD_SIZE,2);
  p.x = input/BOARD_SIZE;
  p.y = input%BOARD_SIZE;

  if(input>=pow(BOARD_SIZE,2)) p.x= BOARD_SIZE-1, p.y = BOARD_SIZE-1;
}

void intToShipArray(int input, shipPosition *p){

  int i=0;
  int j=FLEET_SIZE-1;
  int radix = std::pow(BOARD_SIZE,2)*2;
  
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

    int baseModInput = floor(input/pow(radix,j)); 

    intToShipPos(baseModInput,p[i]);
    input-=baseModInput*pow(radix,j);
    i++; j--;

  }
}


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
  w.clear();
  w.reserve(threadCount);

  int radix = std::pow(BOARD_SIZE,2)*2;
  int maxSegValue = pow(radix, FLEET_SIZE);
  int segmentSize = maxSegValue/threadCount;
  // std::cout<<"radix: "<< radix << " maxSegValue: " << maxSegValue << " segmentSize: "<< segmentSize <<'\n';

  shipPosition pS[FLEET_SIZE]; //position Start
  shipPosition pE[FLEET_SIZE]; //position End

  for (size_t i = 1; i < threadCount+1; i++){
    intToShipArray(segmentSize*(i-1), pS);
    intToShipArray(segmentSize*i, pE);
    
    // std::cout<<"\nData array "<<i<<", segments ("<< segmentSize*(i-1)<<", "<< segmentSize*i<<")\n";
    // for (size_t j = 0; j < FLEET_SIZE; j++) std::cout << "pS("<< pS[j].x << ", " << pS[j].y << ", " << pS[j].dir << ")\t";
    // std::cout<<'\n';
    // for (size_t j = 0; j < FLEET_SIZE; j++) std::cout << "pE("<< pE[j].x << ", " << pE[j].y << ", " << pE[j].dir << ")\t";
    // std::cout<<'\n';
     
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
  
  // if any are negative (ie end before start) then get rid and make sure the one before is set to propper end
  for (size_t i = 0; i < sweatshop.size(); i++){
    if (shipArrayToInt(sweatshop[i].end)-shipArrayToInt(sweatshop[i].start)<0){
      setEndArray(sweatshop[i-1].end); //FIXME: works, but not great solution?
      sweatshop.erase(sweatshop.begin()+i);
    }
  }
  
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
      // std::cout<<"joining a thread \n";
      th.join();
  }

  auto stop = high_resolution_clock::now();
  auto runTime = duration_cast<seconds>(stop - start);

  // Sum it up and get time
  unsigned long totalGoodBoards=0;

  for (auto &w : sweatshop){
    totalGoodBoards += w.goodBoards;
  }
  std::cout << totalGoodBoards << " in " << runTime.count() <<" seconds\n" ;
}

int main() {
  std::cout<<"Board Len: "<< BOARD_SIZE<<"\tFleet size: "<< FLEET_SIZE<<"\tthreadCount: "<<threadCount<<"\n";
  runThreads(true);

  return 0;
};

