#include <iostream>
#include <fstream>

#include "boatsAndBoards.h"
#include "json/json.h"
#include "mcBoats.h"

int main() {
    verbose = true;
    codeVersion = "v3";
    std::cout<<"Board Len: "<< BOARD_SIZE<<"\tFleet size: "<< FLEET_SIZE<<"\tthreadCount: "<<threadCount<<"\tverbose: "<<verbose<<std::endl;

    board board = rndBoard();
    if(verbose) std::cout << board << std::endl;

    // -------   Standard  -------
    // saveGame(coordinateChooser::P_MAX, board, 1000);

    // // Repeat testing
    // std::vector<board> testBoards;
    // for (size_t i = 0; i < 2; i++){
    //   board b = rndBoard();
    //   testBoards.push_back(b);
    // }
    // repeatIGRange(testBoards);

    // ------- Tree things -------


    MCTS_node * headNode = new MCTS_node();
    treeTraversal(headNode,100);

    std::string* mermaidChart = new std::string();
    visualiseTree(headNode, mermaidChart);

    std::cout << *mermaidChart << std::endl;

    std::cout << "headNode has " << headNode -> getSize() << " (gran)children"  << std::endl;
    // std::cout << "headNode is " << maxDepth(headNode) << " deep"  << std::endl;


    std::cout << "\n\nEND CODE" << std::endl;
    return 0;
};

