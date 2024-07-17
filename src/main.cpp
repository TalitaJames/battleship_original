#include <iostream>
#include <fstream>

#include "boatsAndBoards.h"
#include "json/json.h"
#include "mcBoats.h"

int main() {
    verbose = true;
    codeVersion = "v3";
    std::cout<<"Board Len: "<< BOARD_SIZE<<"\tFleet size: "<< FLEET_SIZE<<"\tthreadCount: "<<threadCount<<"\tverbose: "<<verbose<<std::endl;


    // -------   Standard  -------
    board board = initBlankBoard();
    shipPosition shipPositions[FLEET_SIZE];
    intToShipArray(62632, shipPositions);
    drawBoard(board, shipPositions);

    if(verbose) std::cout << board << std::endl;
    hitmask hitmaskTurns;
    hitmaskTurns.hitmask[0][4] = cellStatus::TURN;
    hitmaskTurns.hitmask[0][0] = cellStatus::TURN;
    hitmaskTurns.hitmask[0][1] = cellStatus::TURN;
    hitmaskTurns.hitmask[1][0] = cellStatus::TURN;
    hitmaskTurns.hitmask[3][3] = cellStatus::TURN;

    hitmask hitmaskShots = turnsToShotmask(board, hitmaskTurns);
    // if(verbose) std::cout << hitmaskTurns << hitmaskShots << std::endl;

    // playGame_fromHitmask(coordinateChooser::P_MAX, board, hitmaskShots);

    Json::Value rubishJSON;
    std::vector<coordinateChooser> playStyles;
    playStyles.reserve(25);
    
    for (size_t i = 0; i < 25; i++) {
        coordinateChooser c = (0 == i % 2) ? P_MAX : RND;
        playStyles.push_back(c);
    }
    
    std::cout << "Playstyles" << std::endl;
    for (auto c : playStyles) std::cout << c << " ";
    std::cout<<std::endl;

    saveGame(P_MAX, board);

    // saveGame(coordinateChooser::P_MAX, board, 100);

    // // Repeat testing
    // std::vector<board> testBoards;
    // for (size_t i = 0; i < 2; i++){
    //   board b = rndBoard();
    //   testBoards.push_back(b);
    // }
    // repeatIGRange(testBoards);

    // ------- Tree things -------


    // MCTS_node* headNode = new MCTS_node();

    // std::string* mermaidChart = new std::string();
    // visualiseTree(headNode, mermaidChart);
    // std::cout << "\n---- mermaid ----\n" << *mermaidChart << std::endl;

    // MCTS_node* nextMove = treeTraversal(headNode, 100);

    // // mermaidChart -> clear();
    // // visualiseTree(headNode, mermaidChart);
    // // std::cout << "\n---- mermaid ----\n" << *mermaidChart << std::endl;

    // std::cout << "headNode has " << headNode -> getSize() << " (gran)children"  << std::endl;
    // // std::cout << "headNode is " << maxDepth(headNode) << " deep"  << std::endl;

    std::cout << "\n\nEND CODE" << std::endl;
    return 0;
};

