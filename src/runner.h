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

void initSystem(int, char *[], bool);

board makeBoard(shipPosition*);
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
void nextShipPosition(shipPosition &, ship);
void nextShipPosArray(shipPosition *, ship const);

bool isStartPos(shipPosition);
bool isStartArray(shipPosition *);
void setEndArray(shipPosition &);

void printBoard(board);
void printHitmask(hitmask);

void dividePositions(int);
void checkBoards(worker &);

#endif // RUNNER_H