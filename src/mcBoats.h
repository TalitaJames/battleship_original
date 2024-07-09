#ifndef MCTS_BOATS_H
#define MCTS_BOATS_H

#include <vector>
#include <algorithm>
#include <limits>
#include <math.h>

// #include "mcTreesearch.h"
#include "boatsAndBoards.h"

#define verboseMCTS true
#define explorationConst 2 //this is the 'c' const for the UCB calculations

class MCTC_node;

class MCTS_node {
  private:
    // bool terminal; // is this node the final possible one? (end of game)
    int visitCount; // how many times has it been visited?
    unsigned int scoreTotal;

    struct hitmask hitmask;

    MCTS_node *parentNode;
    std::vector<MCTS_node *> childrenNodesPtr;
    std::vector<struct hitmask> unexploredMoves;

    void generateUnexploredMoves();
    void backpropagate(unsigned int score);
    MCTS_node* findCousin(struct hitmask hitmask); //TODO working on this
    bool matchingHitmask(struct hitmask);

  public:
    MCTS_node();
    MCTS_node(struct hitmask hitmask);
    MCTS_node(struct hitmask hitmask, MCTS_node *parentNode);
    ~MCTS_node();

    int getSize();
    int getVisitCount();
    int getDepth();
    bool isLeafNode();
    bool isHeadNode();
    double getUCBScore();
    MCTS_node* getParent();
    MCTS_node* getBestChild();
    std::vector<MCTS_node *> getAllChildren();
    void rollout();
    void expand();

    void debug();

};  


// Tree things
void treeTraversal(MCTS_node*, int);
void visualiseTree(MCTS_node*, std::string*);
int maxDepth(MCTS_node*);


#endif //MCTS_BOATS_H
