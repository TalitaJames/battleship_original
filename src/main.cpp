#include <iostream>
#include <fstream>

#include "boatsAndBoards.h"
#include "json/json.h"
#include "mcBoats.h"

int main() {
    verbose = true;
    codeVersion = "v4";
    std::cout<<"Board Len: "<< BOARD_SIZE <<"\tFleet size: "<< FLEET_SIZE <<"\tthreadCount: "<<threadCount<<"\tverbose: "<<verbose<<std::endl<<"Fleet: {";
    for (size_t i = 0; i < FLEET_SIZE; i++)
        std::cout << ", " << FLEET[i];
    std::cout<< "}" << std::endl;


    // -------   Standard  -------
    // repeatGames(coordinateChooser::INFOGAIN, 2);


    // -------   Compare Turn Count  -------

    std::ofstream outfile;
    std::string filename = "out/boardTurns/" + std::to_string(BOARD_SIZE) + "_" + std::to_string(FLEET_SIZE);
    outfile.open(filename, std::ios_base::app); //open and append to the file

    std::map<int, uint> boardTurns{}; // map int to turns taken

    for (size_t i = 0; i < 200; i++){
        board board = rndBoard();

        try {
            int turnsTaken = boardTurns.at(board.shipPositionsInt);
        }
        catch (const std::out_of_range& e) {
            // Block of code to handle errors
            uint turnsTaken = saveGame(coordinateChooser::P_MAX, board);
            boardTurns.insert({board.shipPositionsInt, turnsTaken});
            outfile << board.shipPositionsInt << ", " << turnsTaken << std::endl;
        }
    }

    outfile.close();


    // ------- Tree things -------

    // for(int i = 0; i<10; i++){
    //     board board = rndBoard();
    //     saveGameMCTS(board, 100);
    // }
    // simulateGameMCTS(board, 12);

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

