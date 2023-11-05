#ifndef RUNNER_H
#define RUNNER_H

typedef int ship;

struct board;
struct shipPosition;
struct hitmask;

enum cellStatus{
  UNKNOWN,
  MISS,
  HIT,
  SUNK,
};


const int BOARD_SIZE = 5; //FIXME: I want this to be a command line argument but still a const
// int BOARD_SIZE=2;
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
void nextShipPosition(shipPosition &, ship);
void nextShipPosArray(shipPosition *, ship const);

bool isStartPos(shipPosition);
bool isStartArray(shipPosition *);

void printBoard(board);
void printHitmask(hitmask);


#endif // RUNNER_H