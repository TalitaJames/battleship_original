#ifndef MCTS_BOATS_H
#define MCTS_BOATS_H

#include "mcTreesearch.h"


class MCTS_boatNode : public MCTS_node {
    
};


class MCTS_boatTree {
  private:
    MCTS_node *rootNodePtr;
    MCTS_node *currentNode;

  public:
    MCTS_boatTree();
    void advanceTree();
    int getSize();
    MCTS_node getRootNode();

    void debug();
};



#endif //MCTS_BOATS_H
