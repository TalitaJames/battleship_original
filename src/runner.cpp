#include <iostream> 
#include <cstring> 
#include <random>
#include <chrono>
#include "runner.h"

using namespace std::chrono;

struct shipPosition{
  unsigned short x=0;
  unsigned short y=0;
  bool dir=0;
};


struct hitmask{
  cellStatus hitmask[BOARD_SIZE][BOARD_SIZE] {UNKNOWN};
};

struct board{
  int board[BOARD_SIZE][BOARD_SIZE] {-1};
  bool isEmpty = true;
  bool isValid = false;
};



// Draw board

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
int compareShipPositions(shipPosition pA, shipPosition pB){ //TODO: testing
  if (pA.dir != pB.dir ) return pB.dir - pA.dir;
  else if (pA.x != pB.x ) return (pB.x - pA.x)/abs(pB.x - pA.x);
  else if (pA.y != pB.y ) return (pB.y - pA.y)/abs(pB.y - pA.y);
  return 0;
}


void nextShipPosition(shipPosition &p){
  p.y++;
  if (p.y >= BOARD_SIZE){
    p.y=0;
    p.x++;
  }
  if (p.x >= BOARD_SIZE){
    p.x=0;
    p.y=0;
    p.dir = !p.dir;
  }
};

void nextShipPosArray(shipPosition* p){
  for (int i = FLEET_SIZE-1; i >= 0; i--){
    nextShipPosition(p[i]);
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


void initSystem(bool verbose){
  if (verbose){
    std::cout<<"Fleet size: "<< FLEET_SIZE<<"\tBoard Len: "<< BOARD_SIZE<<'\n';
  }
};

int main() {
  shipPosition  pA[FLEET_SIZE];
  board b = initBlankBoard();
  initSystem(true);

  unsigned long goodBoards = 0;
  auto start = high_resolution_clock::now();
  do{
    // for (size_t i = 0; i < FLEET_SIZE; i++) std::cout << "("<< pA[i].x << ", " << pA[i].y << ", " << pA[i].dir << ")\t";
    // std::cout << "\n";
    // printBoard(b);
    drawBoard(b,pA);
    if(b.isValid) goodBoards++;
    nextShipPosArray(pA);
  }while(!isStartArray(pA));
  auto stop = high_resolution_clock::now();
  auto runTime = duration_cast<seconds>(stop - start);


  std::cout << "eof "<<goodBoards<<" in "<<runTime.count()<<"seconds \n";
  return 0;
};
