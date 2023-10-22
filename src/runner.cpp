#include "runner.h"
#include <iostream> 

struct shipPosition{
  unsigned short x;
  unsigned short y;
  bool dir;
};


struct hitmask{
  cellStatus status[BOARD_SIZE][BOARD_SIZE]; //FIXME: make all the global consts
};

struct board{
  int board[BOARD_SIZE][BOARD_SIZE] {0};
  //FIXME: When this wasn't {0} it hadn't initialised and picked random values
  // with {-1} the first value of the array was -1 and the rest were 0
  // 0 has them all as zero
};



int main() { //main function

  ship fleet[] = {2,3,3,4,5};

  shipPosition fleetPos[sizeof(fleet)/sizeof(fleet[0])];

  makeBoard(fleet, fleetPos, sizeof(fleet)/sizeof(fleet[0]));


  

  return 0;
};

// Make a board



board makeBoard(ship* fleet, shipPosition* pos, short fleetSize){
  // for (size_t i = 0; i < sizeof(fleet)/sizeof(fleet[0]); i++){
    // std::cout << "ship: " << fleet[i] << " at (" << pos[i].x << ", " << pos[i].y << ", " << pos[i].dir << ")";
  // }
  board foo;
  
  for (int x = 0; x < BOARD_SIZE; x++){
    std::cout << "[";
    for (int y = 0; y < BOARD_SIZE; y++){
      std::cout << foo.board[x][y] << ", ";
    }
      std::cout << "]\n";
  }



  return foo;
};



// Hit and update hitmask

// Check a board and hitmask are compatible

