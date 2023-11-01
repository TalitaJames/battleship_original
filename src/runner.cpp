#include <iostream> 
#include <cstring> 
#include <random>
#include "runner.h"

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
        
        if (pos[i].x+j>= BOARD_SIZE ||pos[i].y>= BOARD_SIZE) {b.isValid=false; throw(3);} // out of horizonal bounds
        else if (b.board[pos[i].x+j][pos[i].y] != BOARD_DEFAULT) {b.isValid=false; throw(1);} // intersection!
        
        b.board[pos[i].x+j][pos[i].y] = i;
      } else{
        // std::cout <<"\t("<<pos[i].x+j<<","<<pos[i].y<<") @ "<< b.board[pos[i].x+j][pos[i].y] <<"\n";
      
        if (pos[i].x>= BOARD_SIZE ||pos[i].y+j>= BOARD_SIZE) {b.isValid=false; throw(4);} // out of vertical bounds
        else if (b.board[pos[i].x][pos[i].y+j] != BOARD_DEFAULT) {b.isValid=false; throw(2);}  // intersection!
      
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
        
        if (pos[i].x+j>= BOARD_SIZE ||pos[i].y>= BOARD_SIZE) {b.isValid=false; throw(10);} // out of horizonal bounds
        else if (b.board[pos[i].x+j][pos[i].y] != BOARD_DEFAULT) {b.isValid=false; throw(1);} // intersection!
        
        b.board[pos[i].x+j][pos[i].y] = i;
      } else{
        // std::cout <<"\t("<<pos[i].x+j<<","<<pos[i].y<<") @ "<< b.board[pos[i].x+j][pos[i].y] <<"\n";
      
        if (pos[i].x>= BOARD_SIZE ||pos[i].y+j>= BOARD_SIZE) {b.isValid=false; throw(20);} // out of vertical bounds
        else if (b.board[pos[i].x][pos[i].y+j] != BOARD_DEFAULT) {b.isValid=false; throw(2);}  // intersection!
      
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
  std::cout << "---\n";
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



int main() {
  shipPosition pos_rng[FLEET_SIZE];

  int repeats = 1e6;
  int good = 0;

  std::uniform_int_distribution<std::mt19937::result_type> udist(0,BOARD_SIZE); // distribution in range [1, 6]
  board b = initBlankBoard();

  for (size_t i = 0; i < repeats; i++){
    for (size_t i = 0; i < FLEET_SIZE; i++) pos_rng[i] = randShipPos();
    try{
      drawBoard(b,pos_rng);
      // printBoard(b);
      good++;
    } 
    catch(int e)  {
      // std::cerr <<"ERROR "<< e << '\t';
      // for (size_t j = 0; j < FLEET_SIZE; j++) std::cout << "(" << pos_rng[j].x << ", " << pos_rng[j].y << ", " << pos_rng[j].dir << ") ";
      // std::cout << std::endl;
    }

  }
  std::cout << good << "/"<<repeats<<  good/repeats <<"\n";

  shipPosition fleetPos_good[FLEET_SIZE] = {{0,1,false}, {2,2,true}};
  shipPosition fleetPos_goodTwo[FLEET_SIZE] = {{2,4,true}, {0,1,false}};

  
  

  return 0;
};
