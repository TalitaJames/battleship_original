#include <iostream> 

#include "boatsAndBoards.h"
#include "json/json.h"
#include "mcTreesearch.h"


int main() {
  verbose = false;
  codeVersion = "v3";
  std::cout<<"Board Len: "<< BOARD_SIZE<<"\tFleet size: "<< FLEET_SIZE<<"\tthreadCount: "<<threadCount<<"\tverbose: "<<verbose<<std::endl;
  
  std::vector<board> testBoards;

  // intToBoard(498990377); // for 9_4
  testBoards.push_back(rndBoard());
  testBoards.push_back(rndBoard());
  testBoards.push_back(rndBoard());


  repeatIGRange(testBoards);

  return 0;
};

