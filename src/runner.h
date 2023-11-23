#ifndef RUNNER_H
#define RUNNER_H

#include <vector>


typedef int ship;

struct shipPosition;
struct hitmask;
struct probabilityGrid;
struct board;
struct worker;

enum cellStatus{
  UNKNOWN,
  MISS,
  HIT,
  SUNK,
};


const int BOARD_SIZE = 8;
const int BOARD_DEFAULT = -1;

const ship FLEET[] = {2,3,3};
const short FLEET_SIZE = sizeof(FLEET)/sizeof(FLEET[0]);
int fleetPositionCount = 0;

int threadCount = 8;

bool verbose;

// -- Board drawing and manipulation
board initBlankBoard(void);
void wipeBoard(board &);
void drawBoard(board &, shipPosition*);
void hitBoard(board, hitmask &, int, int);
bool checkCompatible(board,hitmask);

// -- Random functions
std::random_device rdDev;
std::mt19937 rng(rdDev());
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
void printBoard(board);
void printHitmask(hitmask);
void printProbabilityGrid(probabilityGrid);
void printWorkers(std::vector<worker>);

// -- probabilityGrid functions
void gatherProbabilityFromWorkers(probabilityGrid &, hitmask, std::vector<worker>);
void calcProbabilityGrid(probabilityGrid &, hitmask);
void flattenBoardToProbabilityGrid(board, probabilityGrid &);

// -- Thread and bulk bits
void checkBoards(worker &, hitmask, char);
void dividePositions(int, std::vector<worker>&);
void runThreads(int threadCount, hitmask hitM, probabilityGrid &probGrid);

// -- Game Play (and position deciding)
enum coordinateChooser{
  USER_INPUT,
  RND,
  P_MAX,
  P_RND,
  KL_MAX,
  KL_RND
};

void playGame(coordinateChooser);
void coordinate_userInput(int &, int &);
void coordinate_rnd(int &, int &);
void coordinate_pMax(int &, int &, probabilityGrid, hitmask);
void coordinate_pRnd(int &, int &, probabilityGrid, hitmask);
void coordinate_klMax(int &, int &, probabilityGrid, hitmask);
void coordinate_klRnd(int &, int &, probabilityGrid, hitmask);




#endif // RUNNER_H
