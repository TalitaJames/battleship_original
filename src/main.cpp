#include <iostream> 

#include "boatsAndBoards.h"
#include "json/json.h"
#include "mcTreesearch.h"


int main() {
  std::cout<<"Board Len: "<< BOARD_SIZE<<"\tFleet size: "<< FLEET_SIZE<<"\tthreadCount: "<<threadCount<<"\tverbose: "<<verbose<<std::endl;
  verbose = true;
  
  repeatGames({P_MAX}, 15, false);

  // if (verbose) printBoard(b);
  // hitmask hM;

  // MCTS_tree tree(b);
  // tree.getRootNodePtr() -> expand();
  // tree.advanceTree();
  // std::cout<<"\nroot size "<<tree.getRootNodePtr() -> getSize() <<"\n";
  // std::cout<<"\ntree size "<<tree.getSize() <<"\n";

  return 0;
};

