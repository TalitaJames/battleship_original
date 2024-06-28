#ifndef MCTS_H
#define MCTS_H

#include <vector>
#include <algorithm>
#include <limits>
#include <math.h>


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

    MCTS_tree *tree;
    MCTS_node *parentNode;
    std::vector<MCTS_node> childrenNodes;
    std::vector<MCTS_node *> childrenNodesPtr;
    // std::vector<hitmask> unexploredMoves;
    // std::vector<hitmask> unexploredMoves;

    // void generateUnexploredMoves();

    MCTS_node(MCTS_node *parentNode, MCTS_tree *tree, bool headNode);

  public:
    MCTS_node();
    MCTS_node(MCTS_tree *tree);
    MCTS_node(MCTS_node *parentNode, MCTS_tree *tree);
    ~MCTS_node();

    int getSize();
    int getVisitCount(); //TODO why is this private when visitCount is also private?
    void backpropagate(unsigned int score);
    double getUCBScore();
    virtual void rollout(); //TODO this is a pure virtual class right?
    // void expand();
    // MCTS_node* selectBestChild(); // pick the best move 

    void debug();

};

class MCTS_tree {
  private:
    MCTS_node *rootNodePtr;
    MCTS_node *currentNode;

  public:
    MCTS_tree();
    void advanceTree();
    int getSize();
    MCTS_node getRootNode();

    void debug();
};

#endif //MCTS_H
