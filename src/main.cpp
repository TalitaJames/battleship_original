#include <iostream> 

#include "boatsAndBoards.h"
#include "json/json.h"
#include "mcTreesearch.h"


int main() {
  verbose = true;
  codeVersion = "v2.0";
  std::cout<<"\nCode Running Version: "<< codeVersion <<"\nBoard Len: "<< BOARD_SIZE<<"\tFleet size: "<< FLEET_SIZE<<"\tthreadCount: "<<threadCount<<"\tverbose: "<<verbose<<std::endl;
  
  // repeatGames({P_MAX}, 1, false);

  board b = rndBoard();

  MCTS_tree tree(b);
  if (verbose) printBoard(tree.getBoard());
  tree.getRootNodePtr() -> expand();
  // tree.advanceTree();
  // std::cout<<"\nroot size "<<tree.getRootNodePtr() -> getSize() <<"\n";
  // std::cout<<"\ntree size "<<tree.getSize() <<"\n";

  return 0;
};

