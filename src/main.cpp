#include <iostream> 

#include "boatsAndBoards.h"
#include "json/json.h"
#include "mcTreesearch.h"


int main() {
  std::cout<<"Board Len: "<< BOARD_SIZE<<"\tFleet size: "<< FLEET_SIZE<<"\tthreadCount: "<<threadCount<<"\tverbose: "<<verbose<<std::endl;
  verbose = true;
  
  repeatGames({RND_W_PROB, P_MAX}, 2, false);

  return 0;
};

