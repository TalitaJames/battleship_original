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
class MCTS_tree;


class MCTS_node {
  private:
    bool headNode; // is this node the head?
    // bool terminal; // is this node the final possible one? (end of game)
    int visitCount; // how many times has it been visited?
    unsigned int scoreTotal;
    std::string nodeID; // random ID for each node for identification in debuging

    struct hitmask hitmask;

    MCTS_tree *tree;
    MCTS_node *parentNode;
    std::vector<MCTS_node *> childrenNodesPtr;
    std::vector<struct hitmask> unexploredMoves;

    MCTS_node(struct hitmask hitmask, MCTS_tree *tree, MCTS_node *parentNode, bool headNode);

    void generateUnexploredMoves();
    void backpropagate(unsigned int score);

  public:
    MCTS_node(struct hitmask hitmask);
    MCTS_node(struct hitmask hitmask, MCTS_tree *tree);
    MCTS_node(struct hitmask hitmask, MCTS_tree *tree, MCTS_node *parentNode);
    ~MCTS_node();

    int getSize();
    int getVisitCount();
    double getUCBScore();
    MCTS_node* getBestChild();
    void rollout(); //TODO this is a pure virtual class right?
    void expand();
    // MCTS_node* selectBestChild(); // pick the best move 

    void debug();

};  

class MCTS_tree {
  private:
    MCTS_node *rootNodePtr;
    MCTS_node *currentNode;

    struct board board;

  public:
    MCTS_tree();
    MCTS_tree(struct board board);
    ~MCTS_tree();
    // void advanceTree();
    int getSize();
    MCTS_node* getRootNode();

    void debug();
};


#endif //MCTS_BOATS_H
