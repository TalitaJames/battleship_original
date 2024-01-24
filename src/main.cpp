#include <iostream> 

#include "boatsAndBoards.h"
#include "json/json.h"
#include "mcTreesearch.h"


int main() {
  for (size_t i = 0; i < FLEET_SIZE; i++) fleetPositionCount += FLEET[i];
  std::cout<<"Board Len: "<< BOARD_SIZE<<"\tFleet size: "<< FLEET_SIZE<<"\tthreadCount: "<<threadCount<<"\tverbose: "<<verbose<<"\tfleetPositionCount: "<<fleetPositionCount<<std::endl;
  
  repeatGames({RND_W_PROB, P_MAX}, 2, false);

  return 0;
};

