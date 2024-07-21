#include "acutest.h"
#include "boatsAndBoards.h"
#include "json/json.h"
#include "mcBoats.h"

#include <exception>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

#include <iostream>


// Given a board, ensure that it is blank
void checkBoardBlank(struct board b){
    TEST_CHECK_(true == b.isEmpty, "Expected blankBoard to return b.isEmpty as true");

    TEST_CHECK_(false == b.isValid, "Expected blankBoard to return b.isValid as false");

    for (int y = 0; y < BOARD_SIZE; y++){
        for (int x = 0; x < BOARD_SIZE; x++){
            TEST_CHECK_(BOARD_DEFAULT == b.board[x][y], "Board at %i,%i expected %i and was %i", x, y, BOARD_DEFAULT, b.board[x][y]);
        }
    }
}

void test_initBlankBoard(void){ 
    board b = initBlankBoard();
    checkBoardBlank(b);
};

void test_wipeBoard(void){ 
    board b = initBlankBoard();
    b.shipPositionsInt = 3591;
    b.isEmpty = false;
    b.board[BOARD_SIZE-1][0] = 2;
    wipeBoard(b);
    checkBoardBlank(b);
};

void test_drawBoard(void){
    board b = initBlankBoard();
    shipPosition shipPos[FLEET_SIZE];

    for (size_t i = 0; i < FLEET_SIZE; i++) shipPos[i] = rndShipPos(FLEET[i]);

    drawBoard(b, shipPos);
    TEST_CHECK_(false == b.isEmpty, "Expected drawBoard to return b.isEmpty as false");

    TEST_CHECK_(true == b.isValid, "Expected drawBoard to return b.isValid as true");

    for (int i = 0; i < FLEET_SIZE; i++){
        TEST_CHECK_(b.board[shipPos[i].x][shipPos[i].y] == i, "Expected the cell (%i,%i) to return %i, instead returned %i", shipPos[i].x, shipPos[i].y, i, b.board[shipPos[i].x][shipPos[i].y]);
    }
};

// this doesn't feel particularly helpful but its nice to have a passing test
void test_compareShipData(void){ 
    shipPosition same1 = {2,4, true};
    shipPosition same2 = {2,4, true};
    shipPosition before1 = {1,4, false};

    TEST_CHECK(compareShipPositions(same1, same2) == 0);
    TEST_CHECK(compareShipPositions(same1, before1) == -1);
    TEST_CHECK(compareShipPositions(before1, same1) == 1);
};

void test_hitBoard(void){
    board board = rndBoard();
    struct hitmask hitmask;
    int x, y = 0;

    coordinate_rnd(x, y, hitmask);

    hitBoard(board, hitmask, x, y);
    
    TEST_CHECK_(hitmask.hitmask[x][y] != cellStatus::UNKNOWN, "Hitmask at (%i,%i) is equal to a cell status of %i, when the board is %i", x,y,hitmask.hitmask[x][y], board.board[x][y]);
};

void test_runThreads(void){
    hitmask hitM;
    probabilityGrid probGrid;

    std::string gameCode = std::to_string(BOARD_SIZE);
    gameCode.append("_");
    gameCode.append(std::to_string(FLEET_SIZE));
    
    std::map<std::string, long unsigned int> totalBoards;
    totalBoards["5_1"] = 40; // all the possible values for 
    totalBoards["5_2"] = 956; // a completly empty hitmask
    totalBoards["5_3"] = 16000; // given a board size and 
    totalBoards["5_4"] = 92480; // fleet length
    totalBoards["5_5"] = 80848;
    totalBoards["6_1"] = 60;
    totalBoards["6_2"] = 2472;
    totalBoards["6_3"] = 80648;
    totalBoards["6_4"] = 1266864;
    totalBoards["6_5"] = 6687136;
    totalBoards["7_1"] = 84;
    totalBoards["7_2"] = 5268;
    totalBoards["7_3"] = 280176;
    totalBoards["7_4"] = 8728400;
    totalBoards["7_5"] = 124757096;
    totalBoards["8_1"] = 112;
    totalBoards["8_2"] = 9896;
    totalBoards["8_3"] = 773368;
    totalBoards["8_4"] = 39998648;
    totalBoards["8_5"] = 1142253520;
    totalBoards["9_1"] = 144;
    totalBoards["9_2"] = 17004;
    totalBoards["9_3"] = 1825760;
    totalBoards["9_4"] = 140730720;
    totalBoards["9_5"] = 6788392256;
    totalBoards["10_1"] = 180;
    totalBoards["10_2"] = 27336;
    totalBoards["10_3"] = 3848040;
    totalBoards["10_4"] = 411770168;
    totalBoards["10_5"] = 30093975536;


    long unsigned int expected = totalBoards[gameCode];
    runThreads(hitM, probGrid, threadCount);

    TEST_CHECK_(probGrid.totalGoodBoards == expected,
        "Expected %ld, produced %ld for game %s",expected, probGrid.totalGoodBoards,  gameCode.c_str());
};

void test_appendWorker(void){
    probabilityGrid p_wOne{30,{2}};
    worker wOne;
    wOne.sub_probGrid = p_wOne;
    p_wOne.totalGoodBoards = 50;
    worker wTwo;
    wTwo.sub_probGrid = p_wOne;

    std::cout << std::endl;
    probabilityGrid p;
    appendWorkerToProbGrid(p,wOne);

    TEST_CHECK_(p.totalGoodBoards == 30,
        "Expected %d, produced %ld in appending",50, p.totalGoodBoards);
    
    appendWorkerToProbGrid(p,wTwo);
    TEST_CHECK_(p.totalGoodBoards == 80,
        "Expected %d, produced %ld in appending",80, p.totalGoodBoards);
};

// test that all parts of the JSON file are saving and not returning "null" values
void test_jsonSaving(void){
    Json::Value jsonGameHistory;
    board randomBoard = rndBoard();
    playGame_fromStart(coordinateChooser::RND, randomBoard, jsonGameHistory);

    std::vector<std::string> jsonElements;
	jsonElements.push_back("infoGainGrid");
	jsonElements.push_back("probabilityGrid");
	jsonElements.push_back("shotRecord");
	jsonElements.push_back("turnsTaken");
    // These are all created in SaveGame and json data can't be retrived from that method
    // jsonElements.push_back("BOARD_SIZE");
	// jsonElements.push_back("FLEET");
	// jsonElements.push_back("FLEET_SIZE");
	// jsonElements.push_back("board");
	// jsonElements.push_back("shotMethod");
	// jsonElements.push_back("version");

    for(std::string nextString: jsonElements){
        std::stringstream buffer;
        buffer << jsonGameHistory[nextString];
        TEST_CHECK_("null" != buffer.str(), "JSON Value %s is null", nextString.c_str());
    }
};

TEST_LIST = {
    { "initBlankBoard", test_initBlankBoard },
    { "wipeBoard", test_wipeBoard },
    { "drawBoard", test_drawBoard },
    { "hitBoard", test_hitBoard },
    { "compareShipData", test_compareShipData },
    { "runThreads", test_runThreads },
    { "appendWorker", test_appendWorker },
    { "jsonSaving", test_jsonSaving },
    { NULL, NULL } // Must include at end of list
};