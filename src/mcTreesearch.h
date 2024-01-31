#ifndef MCTS_H
#define MCTS_H

#include <vector>
#include "boatsAndBoards.h"

#define explorationConst 2 //this is the 'c' const for the UCB calculations

class MCTC_node;
class MCTS_tree;


class MCTS_node {
  private:
    bool terminal; // is this node the final possible one? (end of game)
    int visitCount;
    unsigned int scoreTotal;

    MCTS_tree *tree;
    MCTS_node *parentNode;
    // std::vector<MCTS_node> childrenNodes;
    std::vector<MCTS_node *> childrenNodesPtr;
    // std::vector<hitmask> unexploredMoves;
    std::vector<shipPosition> unexploredMoves;

    // -- board state
    const struct hitmask hitmask;
    struct probabilityGrid probabilityGrid;

    void generateUnexploredMoves();
    int getVisitCount();

  public:
    MCTS_node(MCTS_node *parentNode, struct hitmask hitmask, MCTS_tree *tree);
    MCTS_node(struct hitmask hitmask, MCTS_tree *tree);
    ~MCTS_node();

    int getSize();
    void backpropagate(unsigned int score);
    double getUCBScore();
    void rollout(coordinateChooser);
    void expand();
    MCTS_node* selectBestChild(); // pick the best move 
};

class MCTS_tree {
  private:
    MCTS_node *rootNodePtr;
    MCTS_node *currentNode;

    struct board board;

  public:
    MCTS_tree(struct board b);
    struct board getBoard();
    void advanceTree();
    int getSize();
    MCTS_node getRootNode();
};

#endif //MCTS_H
