#include <iostream>
#include <fstream>

#include "boatsAndBoards.h"
#include "json/json.h"
#include "mcTreesearch.h"


int main() {
  verbose = true;
  codeVersion = "v3";
  std::cout<<"Board Len: "<< BOARD_SIZE<<"\tFleet size: "<< FLEET_SIZE<<"\tthreadCount: "<<threadCount<<"\tverbose: "<<verbose<<std::endl;

  board b = rndBoard();
  printBoard(b);
  playGame(P_MAX, b);

  // // Repeat testing
  // std::vector<board> testBoards;
  // for (size_t i = 0; i < 2; i++){
  //   board b = rndBoard();
  //   testBoards.push_back(b);
  // }
  // repeatIGRange(testBoards);

  std::cout << "END CODE" << std::endl;
  return 0;
};

