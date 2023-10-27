#include "runner.h"
#include <iostream> 
#include <cstring> 

struct shipPosition{
  unsigned short x;
  unsigned short y;
  bool dir;
};


struct hitmask{
  cellStatus hitmask[BOARD_SIZE][BOARD_SIZE] {UNKNOWN};
};

struct board{
  int board[BOARD_SIZE][BOARD_SIZE] {-1};
  bool isEmpty = true;
  bool isValid = false;
};



int main() {

  ship fleet[] = {2,3}; //3,4,5};
  short fleetSize = sizeof(fleet)/sizeof(fleet[0]);
  int repeats = 1e9;

  shipPosition fleetPos_good[fleetSize] = {{0,1,false}, {2,1,true}};
  shipPosition fleetPos_intersect[fleetSize] = {{1,0,false}, {1,1,true}};
  shipPosition fleetPos_outOfBounds[fleetSize] = {{1,0,false}, {3,4,true}};

  for (size_t i = 0; i < repeats; i++){
    try{
      makeBoard(fleet, fleetPos_good, fleetSize);
      // makeBoard(fleet, fleetPos_intersect, fleetSize);
      // makeBoard(fleet, fleetPos_outOfBounds, fleetSize);

    }  catch(int e){
      std::cerr << "bad board! error code " << e << '\n';
    }
  }

  std::cout << "done" << '\n';
  return 0;
};

// Draw board
// Wipe board
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