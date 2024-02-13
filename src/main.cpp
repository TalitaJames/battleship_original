#include <iostream> 

#include "boatsAndBoards.h"
#include "json/json.h"
#include "mcTreesearch.h"


int main() {
  verbose = true;
  codeVersion = "v2.1";
  std::cout<<"Board Len: "<< BOARD_SIZE<<"\tFleet size: "<< FLEET_SIZE<<"\tthreadCount: "<<threadCount<<"\tverbose: "<<verbose<<std::endl;
  
  // repeatGames({P_MAX}, 15, false);
  board b = rndBoard();
  // playGame(INFOGAIN, b);
  repeatGames({INFOGAIN}, 2, false);

  return 0;
};

