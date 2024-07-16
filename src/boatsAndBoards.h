#ifndef BOATSANDBOARDS_H
#define BOATSANDBOARDS_H

#include <algorithm>
#include <iostream>
#include <fstream>
#include <limits>
#include <chrono>
#include <vector>
#include <random>
#include <string>
#include <cstring>
#include <thread>
#include <mutex>

#include "json/json.h"

typedef int ship;

enum cellStatus{
  UNKNOWN,
  MISS,
  HIT,
  SUNK,
};

#define BOARD_SIZE 5
#define BOARD_DEFAULT -1

const ship FLEET[] = {2,3,3};
const short FLEET_SIZE = sizeof(FLEET)/sizeof(FLEET[0]);
extern int threadCount;
extern bool verbose;
extern std::string codeVersion;
 
// -- Structs

struct shipPosition{
    unsigned short x=0;
    unsigned short y=0;
    bool dir=0; // 1 is horizontal (X)
};

struct hitmask{
    cellStatus hitmask[BOARD_SIZE][BOARD_SIZE] {UNKNOWN};
    bool shipSunk[FLEET_SIZE] {false};
};

struct probabilityGrid{
    unsigned long totalGoodBoards = 0;
    unsigned long shipGrid[BOARD_SIZE][BOARD_SIZE] {0}; // how many ships could be in this spot (from each possible good board)? 
    double shipProb[BOARD_SIZE][BOARD_SIZE] {0}; // shipGrid % scaled to total board count (probability of a ship, p)
    double pChange[BOARD_SIZE][BOARD_SIZE] {0}; // p^2+(1-p)^2 (formerly infoGain)
    double infoGain[BOARD_SIZE][BOARD_SIZE] {0}; // Calcualated only when `coordinate_infoGain()` is called. the "Real" info gain
};

struct board{
    int board[BOARD_SIZE][BOARD_SIZE] {BOARD_DEFAULT}; // [x][y] array of the board
    bool isEmpty = true;
    bool isValid = false;
    int shipPositionsInt = 0; // Int representing the ship position array, aka arangment of boats on the board
};

struct worker{
    shipPosition start[FLEET_SIZE] = {0,0,0};
    shipPosition end[FLEET_SIZE] = {0,0,0};

    // unsigned long goodBoards = 0;
    probabilityGrid sub_probGrid;
};

struct runWorkerState{
    bool saveFileBool = false;
    std::string saveFilename = "ERROR";

    bool readFileBool = false;
    std::string readFilename = "ERROR";

};

// -- Board drawing and manipulation

board initBlankBoard(void);
void wipeBoard(board &);
void drawBoard(board &, shipPosition*);
void hitBoard(board, hitmask &, int, int);
bool checkCompatible(board,hitmask);
bool operator==(const struct hitmask&, const struct hitmask&);

// -- Random functions

shipPosition rndShipPos(ship);
board rndBoard(void);

// -- Ship Position Manipulation

int compareShipPositions(shipPosition, shipPosition);
int compareShipArray(shipPosition *, shipPosition *);

// -- Ship Position <-> numbers

unsigned long shipPosToInt(shipPosition); 
unsigned long shipArrayToInt(shipPosition *); 
void intToShipPos(unsigned long, shipPosition &); 
void intToShipArray(unsigned long, shipPosition *); 
board intToBoard(unsigned long);

// -- Itterate positions

void nextShipPosition(shipPosition &);
void nextShipPosition(shipPosition &, ship);
void nextShipPosArray(shipPosition *, ship const);

// -- Checking & setting array values

bool isStartPos(shipPosition);
bool isStartArray(shipPosition *);
void setEndArray(shipPosition *);
void setStartArray(shipPosition *);
bool isHitmaskSolved(hitmask); // have all the ship positions been hit?
bool isHit(hitmask, int, int);

// -- Output functions

Json::Value jsonArrayAdder(long unsigned int inputArray[][BOARD_SIZE]);
Json::Value jsonArrayAdder(int inputArray[][BOARD_SIZE]);
Json::Value jsonArrayAdder(double inputArray[][BOARD_SIZE]);
Json::Value jsonArrayAdder(int inputArray[], size_t size);
void jsonFileoutput(std::string filename, Json::Value jsonOut);

std::ostream& operator<<(std::ostream&, board&);
std::ostream& operator<<(std::ostream&, hitmask&);
std::ostream& operator<<(std::ostream&, probabilityGrid&);
std::ostream& operator<<(std::ostream&, std::vector<worker>&);
std::ostream& operator<<(std::ostream&, worker&);
std::ostream& operator<<(std::ostream&, runWorkerState&);
std::istream& operator>>(std::istream&, worker&);
worker inputWorker(std::string);


// -- probabilityGrid functions

void gatherProbabilityFromWorkers(probabilityGrid &, std::vector<worker>);
void appendWorkerToProbGrid(probabilityGrid &, worker);
void calcProbabilityGrid(probabilityGrid &);
void flattenBoardToProbabilityGrid(board, probabilityGrid &);


// -- Thread and bulk bits

void checkBoards(worker &, hitmask, int);
void checkBoardsSaveFile(worker &, hitmask, int, runWorkerState, std::ofstream &);
void dividePositions(int, std::vector<worker>&);
void runThreads(hitmask, probabilityGrid &, int);
void runThreads(hitmask, probabilityGrid &, int, runWorkerState);
void runThreadsRead(hitmask, probabilityGrid &, int, runWorkerState);


// -- Game Play (and position deciding)

enum coordinateChooser{
    USER_INPUT,
    RND,
    RND_W_PROB,
    P_MAX,
    P_RND,
    INFOGAIN,
    DIAGONAL,
    FLEXI
};

unsigned int playGame(coordinateChooser, board);
unsigned int playGame(coordinateChooser, board, Json::Value &);
unsigned int playGame(coordinateChooser, board, Json::Value &, int);

void takeTurn(coordinateChooser, board , hitmask &, probabilityGrid &, runWorkerState,Json::Value &);

unsigned int saveGame(coordinateChooser, board, int);
unsigned int saveGame(coordinateChooser, board, int, std::string);

void repeatGames(std::vector<coordinateChooser>, int, bool);
void repeatGames(std::vector<coordinateChooser>, int, bool, board);
void repeatIGRange(std::vector<board>);


// -- Coordinate choosing

void coordinate_userInput(int &, int &);
void coordinate_rnd(int &, int &, hitmask);
void coordinate_rndWProb(int &, int &, probabilityGrid, hitmask); // random, but with probability weighting
void coordinate_pMax(int &, int &, probabilityGrid, hitmask);
void coordinate_pRnd(int &, int &, probabilityGrid, hitmask);
double coordinate_infoGain(int &, int &, probabilityGrid &, hitmask);
void coordinate_diagonal(int &, int &, probabilityGrid, hitmask);



#endif // BOATSANDBOARDS_H
