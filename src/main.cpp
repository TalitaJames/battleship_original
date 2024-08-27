#include <iostream>
#include <fstream>

#include "boatsAndBoards.h"
#include "json/json.h"
#include "mcBoats.h"

int main() {
    verbose = false;
    codeVersion = "v3";
    std::cout<<"Board Len: "<< BOARD_SIZE <<"\tFleet size: "<< FLEET_SIZE <<"\tthreadCount: "<<threadCount<<"\tverbose: "<<verbose<<std::endl<<"Fleet: {";
    for (size_t i = 0; i < FLEET_SIZE; i++)
        std::cout << ", " << FLEET[i];
    std::cout<< "}" << std::endl;

    // -------   Standard  -------
    // std::cout<<std::endl;

    struct hitmask hitM;
    // struct probabilityGrid probGrid;

    // runThreads(hitM, probGrid, threadCount);
    board board = rndBoard();
    // playGame_fromStart(coordinateChooser::P_MAX, board );

    // if (verbose) std::cout << probGrid << std::endl;



    // ------- Tree things -------

    simulateGameMCTS(board);

    // MCTS_node* headNode = new MCTS_node();

    // std::string* mermaidChart = new std::string();
    // visualiseTree(headNode, mermaidChart);
    // std::cout << "\n---- mermaid ----\n" << *mermaidChart << std::endl;

    // MCTS_node* nextMove = treeTraversal(headNode, 100);

    // mermaidChart -> clear();
    // visualiseTree(headNode, mermaidChart);
    // std::cout << "\n---- mermaid ----\n" << *mermaidChart << std::endl;

    // std::cout << "headNode has " << headNode -> getSize() << " (gran)children"  << std::endl;
    // // std::cout << "headNode is " << maxDepth(headNode) << " deep"  << std::endl;

    std::cout << "\n\nEND CODE" << std::endl;
    return 0;
};

