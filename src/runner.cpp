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

// Make a board
board makeBoard(ship* fleet, shipPosition* pos, short fleetSize){ 
  /*this technicaly works, but the out of bounds things are a bit funky 
  because the board array is a pointer it doesn't know the size, 
  so when it goes out of bounds it just hopes that chunk of memory isn't a 0 (else it'll overwrite it)

  i'd still love the board "blanks" to be non zero (maybe -1?) so that the value at each ship directly matches the array address of the fleet
  */
  board boardNew;

  for (size_t i = 0; i < fleetSize; i++){ // for each ship
    // std::cout << "ship: " << fleet[i] << " at (" << pos[i].x << ", " << pos[i].y << ", " << pos[i].dir << ")\n";

    for (size_t j = 0; j < fleet[i]; j++){ // for the length of each ship
      // check for a ship already there, if yes, throw error?
      if(pos[i].dir){ 
        // std::cout <<"\t"<< boardNew.board[pos[i].x+j][pos[i].y] <<"\n";
        if (boardNew.board[pos[i].x+j][pos[i].y] != 0) throw(1);
        boardNew.board[pos[i].x+j][pos[i].y] = i+1;
      } else{
        // std::cout <<"\t"<< boardNew.board[pos[i].x][pos[i].y+j] <<"\n";
        if (boardNew.board[pos[i].x][pos[i].y+j] != 0) throw(2);
        boardNew.board[pos[i].x][pos[i].y+j] = i+1;
      }
    }
  }

  // print a rep of the board
  // for (int y = 0; y < BOARD_SIZE; y++){
  //   std::cout << "[";
  //   for (int x = 0; x < BOARD_SIZE; x++){
  //     std::cout << boardNew.board[x][y] << ", ";
  //   }
  //     std::cout << "]\n";
  // }



  return boardNew;
};



// Hit and update hitmask

// Check a board and hitmask are compatible

