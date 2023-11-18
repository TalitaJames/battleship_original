#ifndef RUNNER_H
#define RUNNER_H

#include <vector>


typedef int ship;

struct shipPosition;
struct hitmask;
struct board;
struct worker;

enum cellStatus{
  UNKNOWN,
  MISS,
  HIT,
  SUNK,
};


const int BOARD_SIZE = 6;
const int BOARD_DEFAULT = -1;

const ship FLEET[] = {2,3,3,4,5};
const short FLEET_SIZE = sizeof(FLEET)/sizeof(FLEET[0]);

int threadCount = 4;

// -- Board drawing and manipulation
board initBlankBoard(void);
void wipeBoard(board &);
void drawBoard(board &, shipPosition*);
void hitBoard(board, hitmask &, int, int);
bool checkCompatible(board,hitmask);

shipPosition randShipPos(ship);
std::random_device dev;
std::mt19937 rng(dev());

// -- Ship Position Manipulation
int compareShipPositions(shipPosition, shipPosition);
int compareShipArray(shipPosition *, shipPosition *);

// -- Ship Position <-> numbers
unsigned long shipPosToInt(shipPosition); 
unsigned long shipArrayToInt(shipPosition *); 
void intToShipPos(unsigned long, shipPosition &); 
void intToShipArray(unsigned long, shipPosition *); 

// -- Itterate positions
void nextShipPosition(shipPosition &);
void nextShipPosition(shipPosition &, ship);
void nextShipPosArray(shipPosition *, ship const);

// -- Checking & setting array values
bool isStartPos(shipPosition);
bool isStartArray(shipPosition *);
void setEndArray(shipPosition *);
void setStartArray(shipPosition *);

// -- Output functions
void printBoard(board);
void printHitmask(hitmask);
void printWorkers(std::vector<worker>);

// -- Thread and bulk bits
void checkBoards(worker &,char);
void dividePositions(int, std::vector<worker>&);
void runThreads(bool);

#endif // RUNNER_H