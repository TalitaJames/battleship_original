#ifndef RUNNER_H
#define RUNNER_H

#include <vector>


typedef int ship;

struct board;
struct shipPosition;
struct hitmask;
struct worker;

enum cellStatus{
  UNKNOWN,
  MISS,
  HIT,
  SUNK,
};


const int BOARD_SIZE = 5;
const int BOARD_DEFAULT = -1;

const ship FLEET[] = {2,3};//3,4,5};
const short FLEET_SIZE = sizeof(FLEET)/sizeof(FLEET[0]);

int threadCount = 6;

void drawBoard(board &, shipPosition*);
void wipeBoard(board &);
board initBlankBoard(void);

void hitBoard(board, hitmask &, int, int);
bool checkCompatible(board,hitmask);

shipPosition randShipPos();
std::random_device dev;
std::mt19937 rng(dev());

int compareShipPositions(shipPosition, shipPosition);
int compareShipArray(shipPosition *, shipPosition *);

int shipPosToInt(shipPosition); 
int shipArrayToInt(shipPosition *); 
void intToShipPos(int,shipPosition &); 
void intToShipArray(int, shipPosition *); 

void nextShipPosition(shipPosition &);
void nextShipPosition(shipPosition &, ship);
void nextShipPosArray(shipPosition *, ship const);

bool isStartPos(shipPosition);
bool isStartArray(shipPosition *);
void setEndArray(shipPosition *);
void setStartArray(shipPosition *);

void printBoard(board);
void printHitmask(hitmask);
void printWorkers(std::vector<worker>);

void checkBoards(worker &,char);
void dividePositions(int, std::vector<worker>&);

#endif // RUNNER_H