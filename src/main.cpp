#include <iostream> 

#include "boatsAndBoards.h"
#include "json/json.h"
#include "mcTreesearch.h"


int main() {
  verbose = false;
  codeVersion = "v2.2";
  std::cout<<"Board Len: "<< BOARD_SIZE<<"\tFleet size: "<< FLEET_SIZE<<"\tthreadCount: "<<threadCount<<"\tverbose: "<<verbose<<std::endl;


  repeatIGRange(1);

  return 0;
};

